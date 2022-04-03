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

#include "ClientSocketMgr.h"
#include "ClientSocket.h"
#include "IoContext.h"
#include "DeadlineTimer.h"
#include "DiscordConfig.h"
#include "Resolver.h"

/*static*/ ClientSocketMgr* ClientSocketMgr::instance()
{
    static ClientSocketMgr instance;
    return &instance;
}

void ClientSocketMgr::Disconnect()
{
    _stopped = true;
    _updateTimer->cancel();

    if (_clientSocket && !_clientSocket->IsStoped())
        _clientSocket->Stop();

    _clientSocket.reset();
}

void ClientSocketMgr::Update()
{
    if (_stopped)
        return;

    _updateTimer->expires_from_now(boost::posix_time::milliseconds(1));
    _updateTimer->async_wait([this](boost::system::error_code const&) { Update(); });

    if (!_clientSocket)
        return;

    if (_clientSocket->IsStoped())
    {
        _clientSocket->Stop();
        _clientSocket.reset();
    }

    if (_bufferQueue.IsEmpty())
        return;

    DiscordPacket* queuedPacket{ nullptr };

    while (_bufferQueue.GetNextPacket(queuedPacket))
    {
        SendPacket(*queuedPacket);
        delete queuedPacket;
    }
}

void ClientSocketMgr::Initialize(Warhead::Asio::IoContext& ioContext)
{
    if (!CONF_GET_BOOL("Discord.Server.Enable"))
    {
        LOG_WARN("discord.client", "> Discord is disable");
        return;
    }

    _updateTimer = std::make_unique<Warhead::Asio::DeadlineTimer>(ioContext);

    Warhead::Asio::Resolver resolver(ioContext);
    auto const& hostName = CONF_GET_STR("Discord.Server.Host");

    auto address = resolver.Resolve(boost::asio::ip::tcp::v4(), hostName, "");
    if (!address)
    {
        LOG_ERROR("discord.client", "Could not resolve address {}", hostName);
        return;
    }

    _address = std::make_unique<boost::asio::ip::address>(address->address());

    ConnectToServer();
}

void ClientSocketMgr::AddPacketToQueue(DiscordPacket const& packet)
{
    LOG_TRACE("discord.client", "Client->Server: {}", packet.GetOpcode());
    _bufferQueue.AddPacket(new DiscordPacket(packet));
}

void ClientSocketMgr::SendPacket(DiscordPacket const& packet)
{
    if (!_clientSocket)
    {
        LOG_ERROR("discord.client", "{}: Not found client socket. Skip send packet.", __FUNCTION__);
        return;
    }

    _clientSocket->AddPacketToQueue(packet);
}

void ClientSocketMgr::ConnectToServer()
{
    _updateTimer->expires_from_now(boost::posix_time::milliseconds(1));
    _updateTimer->async_wait([this](boost::system::error_code const&)
    {
        if (_clientSocket && _clientSocket->IsOpen())
        {
            LOG_ERROR("discord.client", "> Connection is already exist");
            return;
        }

        if (!_address)
        {
            LOG_ERROR("discord.client", "> Could not resolve address. Skip connect");
            return;
        }

        LOG_INFO("discord.client", "> Start connect to discord server...");

        try
        {
            tcp::socket clientSocket(Warhead::Asio::get_io_context(*_updateTimer));
            clientSocket.connect(tcp::endpoint(*_address, sDiscordConfig->GetOption<uint16>("Discord.Server.Port")));

            _clientSocket = std::make_shared<ClientSocket>(std::move(clientSocket), Warhead::Asio::get_io_context(*_updateTimer));
            _clientSocket->Start();
        }
        catch (boost::system::system_error const& err)
        {
            LOG_WARN("node", "Failed connet. Error {}", err.what());
            return;
        }

        _updateTimer->expires_from_now(boost::posix_time::milliseconds(1));
        _updateTimer->async_wait([this](boost::system::error_code const&) { Update(); });
    });
}
