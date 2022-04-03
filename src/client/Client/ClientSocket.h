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

#ifndef _CLIENT_SOCKET_H_
#define _CLIENT_SOCKET_H_

#include "Socket.h"
#include "PacketQueue.h"
#include "DiscordPacket.h"
#include "DiscordSharedDefines.h"
#include <boost/asio/io_context.hpp>
#include <mutex>

namespace Warhead::Asio
{
    class DeadlineTimer;
}

class WorldSocket;

/// Manages all sockets connected to peers and network threads
class WH_CLIENT_API ClientSocket : public Socket<ClientSocket>
{
    using BaseSocket = Socket<ClientSocket>;

public:
    ClientSocket(tcp::socket&& socket, boost::asio::io_context& ioContext);

    void Start() override;
    void Stop();
    bool Update() override;

    inline bool IsStoped() { return _stop; }
    inline bool IsAuthed() { return _authed; }

    inline void SetAccountName(std::string_view name) { _accountName = std::string(_accountName); }
    
    void AddPacketToQueue(DiscordPacket const& packet);

protected:
    void OnClose() override;
    void ReadHandler() override;

private:
    enum class ReadDataHandlerResult
    {
        Ok = 0,
        Error = 1,
        WaitingForQuery = 2
    };

    ReadDataHandlerResult ReadDataHandler();
    bool ReadHeaderHandler();

    void HandleAuthResponce(DiscordPacket& packet);
    void LogOpcode(DiscordCode opcode);

    void SendPacket(DiscordPacket const* packet);

    std::mutex _sessionLock;
    MessageBuffer _headerBuffer;
    MessageBuffer _packetBuffer;
    bool _authed{ false };
    std::string _accountName;

    std::shared_ptr<WorldSocket> _worldSocket;

    PacketQueue<DiscordPacket> _bufferQueue;

    std::unique_ptr<Warhead::Asio::DeadlineTimer> _updateTimer;
    std::atomic<bool> _stop{ false };
};

#endif
