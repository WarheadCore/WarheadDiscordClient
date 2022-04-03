/*
 * This file is part of the WarheadCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ClientSocket.h"
#include "DiscordPacketHeader.h"
#include "DeadlineTimer.h"
#include "IoContext.h"
#include "Errors.h"
#include "Log.h"
#include "DiscordConfig.h"

ClientSocket::ClientSocket(tcp::socket&& socket, boost::asio::io_context& ioContext) :
    Socket(std::move(socket))
{
    _headerBuffer.Resize(sizeof(DiscordClientPktHeader));
    _updateTimer = std::make_unique<Warhead::Asio::DeadlineTimer>(ioContext);

    SetNoDelay(true);

    _accountName = CONF_GET_STR("Discord.Server.AccountName");
}

void ClientSocket::Start()
{
    LOG_DEBUG("node", "Start process auth from server. Account name '{}'", _accountName);

    DiscordPacket packet(CLIENT_AUTH_SESSION, 1);
    packet << _accountName;
    SendPacket(&packet);

    _updateTimer->expires_from_now(boost::posix_time::milliseconds(1));
    _updateTimer->async_wait([this](boost::system::error_code const&) { Update(); });

    AsyncRead();
}

void ClientSocket::Stop()
{
    _updateTimer->cancel();
    _stop = true;
    BaseSocket::CloseSocket();
}

void ClientSocket::OnClose()
{
    _stop = true;
    LOG_DEBUG("server", "> Disconnected from server");
}

bool ClientSocket::Update()
{
    if (_stop)
        return false;

    _updateTimer->expires_from_now(boost::posix_time::milliseconds(1));
    _updateTimer->async_wait([this](boost::system::error_code const&) { Update(); });

    if (_authed && IsOpen())
    {
        DiscordPacket* queuedPacket{ nullptr };

        while (_bufferQueue.GetNextPacket(queuedPacket))
        {
            SendPacket(queuedPacket);
            delete queuedPacket;
        }
    }

    if (!BaseSocket::Update())
        return false;

    return true;
}

void ClientSocket::ReadHandler()
{
    MessageBuffer& packet = GetReadBuffer();

    while (packet.GetActiveSize() > 0)
    {
        if (_headerBuffer.GetRemainingSpace() > 0)
        {
            // need to receive the header
            std::size_t readHeaderSize = std::min(packet.GetActiveSize(), _headerBuffer.GetRemainingSpace());
            _headerBuffer.Write(packet.GetReadPointer(), readHeaderSize);
            packet.ReadCompleted(readHeaderSize);

            if (_headerBuffer.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole header this time.
                ASSERT(packet.GetActiveSize() == 0);
                break;
            }

            // We just received nice new header
            if (!ReadHeaderHandler())
            {
                CloseSocket();
                return;
            }
        }

        // We have full read header, now check the data payload
        if (_packetBuffer.GetRemainingSpace() > 0)
        {
            // need more data in the payload
            std::size_t readDataSize = std::min(packet.GetActiveSize(), _packetBuffer.GetRemainingSpace());
            _packetBuffer.Write(packet.GetReadPointer(), readDataSize);
            packet.ReadCompleted(readDataSize);

            if (_packetBuffer.GetRemainingSpace() > 0)
            {
                // Couldn't receive the whole data this time.
                ASSERT(packet.GetActiveSize() == 0);
                break;
            }
        }

        // just received fresh new payload
        ReadDataHandlerResult result = ReadDataHandler();
        _headerBuffer.Reset();
        if (result != ReadDataHandlerResult::Ok)
        {
            if (result != ReadDataHandlerResult::WaitingForQuery)
                CloseSocket();

            return;
        }
    }

    AsyncRead();
}

bool ClientSocket::ReadHeaderHandler()
{
    ASSERT(_headerBuffer.GetActiveSize() == sizeof(DiscordClientPktHeader));

    DiscordClientPktHeader* header = reinterpret_cast<DiscordClientPktHeader*>(_headerBuffer.GetReadPointer());
    EndianConvertReverse(header->size);
    EndianConvert(header->cmd);

    if (!header->IsValidSize() || !header->IsValidOpcode())
    {
        LOG_ERROR("node", "ClientSocket::ReadHeaderHandler(): node sent malformed packet (size: {}, cmd: {})", header->size, header->cmd);
        return false;
    }

    header->size -= sizeof(header->cmd);
    _packetBuffer.Resize(header->size);
    return true;
}

ClientSocket::ReadDataHandlerResult ClientSocket::ReadDataHandler()
{
    DiscordClientPktHeader* header = reinterpret_cast<DiscordClientPktHeader*>(_headerBuffer.GetReadPointer());
    DiscordCode opcode = static_cast<DiscordCode>(header->cmd);

    DiscordPacket packet(opcode, std::move(_packetBuffer));

    std::unique_lock<std::mutex> sessionGuard(_sessionLock, std::defer_lock);

    LogOpcode(opcode);

    switch (opcode)
    {
        case SERVER_SEND_AUTH_RESPONSE:
        {
            if (_authed)
            {
                // locking just to safely log offending user is probably overkill but we are disconnecting him anyway
                if (sessionGuard.try_lock())
                    LOG_ERROR("node", "{}: received duplicate SERVER_SEND_AUTH_RESPONSE", __FUNCTION__);

                return ReadDataHandlerResult::Error;
            }

            HandleAuthResponce(packet);
            return ReadDataHandlerResult::Ok;
        }
        default:
            return ReadDataHandlerResult::Error;
    }
}

void ClientSocket::LogOpcode(DiscordCode opcode)
{
    LOG_TRACE("network.opcode", "Client->Server: {}", opcode);
}

void ClientSocket::SendPacket(DiscordPacket const* packet)
{
    if (!IsOpen())
    {
        LOG_ERROR("node", "{}: Not open socket!", __FUNCTION__);
        return;
    }

    if (_stop)
    {
        LOG_ERROR("node", "{}: Updating is stopped!", __FUNCTION__);
        return;
    }

    MessageBuffer buffer;

    DiscordServerPktHeader header(packet->size() + sizeof(packet->GetOpcode()), packet->GetOpcode());
    if (buffer.GetRemainingSpace() < packet->size() + header.GetHeaderLength())
    {
        QueuePacket(std::move(buffer));
        //buffer.Resize(4096);
    }

    if (buffer.GetRemainingSpace() >= packet->size() + header.GetHeaderLength())
    {
        buffer.Write(header.header, header.GetHeaderLength());
        if (!packet->empty())
            buffer.Write(packet->contents(), packet->size());
    }
    else // single packet larger than 4096 bytes
    {
        MessageBuffer packetBuffer(packet->size() + header.GetHeaderLength());
        packetBuffer.Write(header.header, header.GetHeaderLength());
        if (!packet->empty())
            packetBuffer.Write(packet->contents(), packet->size());

        QueuePacket(std::move(packetBuffer));
    }

    if (buffer.GetActiveSize() > 0)
        QueuePacket(std::move(buffer));
}

void ClientSocket::AddPacketToQueue(DiscordPacket const& packet)
{
    _bufferQueue.AddPacket(new DiscordPacket(packet));
}

void ClientSocket::HandleAuthResponce(DiscordPacket& packet)
{
    uint8 code;
    packet >> code;

    DiscordAuthResponseCodes responseCode = static_cast<DiscordAuthResponseCodes>(code);

    if (responseCode == DiscordAuthResponseCodes::Ok)
    {
        LOG_INFO("server", "Auth correct");
        _authed = true;
    }
    else
        LOG_INFO("server", "Auth incorrect. Code {}", code);
}
