/*
 * This file is part of the WarheadCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "DiscordSharedDefines.h"
#include "Define.h"
#include "SmartEnum.h"
#include <stdexcept>

namespace Warhead::Impl::EnumUtilsImpl
{

/**************************************************************************\
|* data for enum 'DiscordCode' in 'DiscordSharedDefines.h' auto-generated *|
\**************************************************************************/
template<>
WH_API_EXPORT EnumText EnumUtils<DiscordCode>::ToString(DiscordCode value)
{
    switch (value)
    {
        case DiscordCode::CLIENT_SEND_HELLO: return { "CLIENT_SEND_HELLO", "CLIENT_SEND_HELLO", "" };
        case DiscordCode::CLIENT_AUTH_SESSION: return { "CLIENT_AUTH_SESSION", "CLIENT_AUTH_SESSION", "" };
        case DiscordCode::CLIENT_SEND_MESSAGE: return { "CLIENT_SEND_MESSAGE", "CLIENT_SEND_MESSAGE", "" };
        case DiscordCode::CLIENT_SEND_MESSAGE_EMBED: return { "CLIENT_SEND_MESSAGE_EMBED", "CLIENT_SEND_MESSAGE_EMBED", "" };
        case DiscordCode::CLIENT_SEND_PING: return { "CLIENT_SEND_PING", "CLIENT_SEND_PING", "" };
        case DiscordCode::SERVER_SEND_AUTH_RESPONSE: return { "SERVER_SEND_AUTH_RESPONSE", "SERVER_SEND_AUTH_RESPONSE", "" };
        case DiscordCode::SERVER_SEND_PONG: return { "SERVER_SEND_PONG", "SERVER_SEND_PONG", "" };
        case DiscordCode::NUM_MSG_TYPES: return { "NUM_MSG_TYPES", "NUM_MSG_TYPES", "" };
        default: throw std::out_of_range("value");
    }
}

template<>
WH_API_EXPORT size_t EnumUtils<DiscordCode>::Count() { return 8; }

template<>
WH_API_EXPORT DiscordCode EnumUtils<DiscordCode>::FromIndex(size_t index)
{
    switch (index)
    {
        case 0: return DiscordCode::CLIENT_SEND_HELLO;
        case 1: return DiscordCode::CLIENT_AUTH_SESSION;
        case 2: return DiscordCode::CLIENT_SEND_MESSAGE;
        case 3: return DiscordCode::CLIENT_SEND_MESSAGE_EMBED;
        case 4: return DiscordCode::CLIENT_SEND_PING;
        case 5: return DiscordCode::SERVER_SEND_AUTH_RESPONSE;
        case 6: return DiscordCode::SERVER_SEND_PONG;
        case 7: return DiscordCode::NUM_MSG_TYPES;
        default: throw std::out_of_range("index");
    }
}

template<>
WH_API_EXPORT size_t EnumUtils<DiscordCode>::ToIndex(DiscordCode value)
{
    switch (value)
    {
        case DiscordCode::CLIENT_SEND_HELLO: return 0;
        case DiscordCode::CLIENT_AUTH_SESSION: return 1;
        case DiscordCode::CLIENT_SEND_MESSAGE: return 2;
        case DiscordCode::CLIENT_SEND_MESSAGE_EMBED: return 3;
        case DiscordCode::CLIENT_SEND_PING: return 4;
        case DiscordCode::SERVER_SEND_AUTH_RESPONSE: return 5;
        case DiscordCode::SERVER_SEND_PONG: return 6;
        case DiscordCode::NUM_MSG_TYPES: return 7;
        default: throw std::out_of_range("value");
    }
}

/***************************************************************************************\
|* data for enum 'DiscordAuthResponseCodes' in 'DiscordSharedDefines.h' auto-generated *|
\***************************************************************************************/
template<>
WH_API_EXPORT EnumText EnumUtils<DiscordAuthResponseCodes>::ToString(DiscordAuthResponseCodes value)
{
    switch (value)
    {
        case DiscordAuthResponseCodes::Ok: return { "Ok", "Ok", "" };
        case DiscordAuthResponseCodes::Failed: return { "Failed", "Failed", "" };
        case DiscordAuthResponseCodes::IncorrectKey: return { "IncorrectKey", "IncorrectKey", "" };
        case DiscordAuthResponseCodes::UnknownAccount: return { "UnknownAccount", "UnknownAccount", "" };
        case DiscordAuthResponseCodes::BannedAccount: return { "BannedAccount", "BannedAccount", "" };
        case DiscordAuthResponseCodes::BannedIP: return { "BannedIP", "BannedIP", "" };
        case DiscordAuthResponseCodes::BannedPermanentlyAccount: return { "BannedPermanentlyAccount", "BannedPermanentlyAccount", "" };
        case DiscordAuthResponseCodes::BannedPermanentlyIP: return { "BannedPermanentlyIP", "BannedPermanentlyIP", "" };
        case DiscordAuthResponseCodes::ServerOffline: return { "ServerOffline", "ServerOffline", "" };
        default: throw std::out_of_range("value");
    }
}

template<>
WH_API_EXPORT size_t EnumUtils<DiscordAuthResponseCodes>::Count() { return 9; }

template<>
WH_API_EXPORT DiscordAuthResponseCodes EnumUtils<DiscordAuthResponseCodes>::FromIndex(size_t index)
{
    switch (index)
    {
        case 0: return DiscordAuthResponseCodes::Ok;
        case 1: return DiscordAuthResponseCodes::Failed;
        case 2: return DiscordAuthResponseCodes::IncorrectKey;
        case 3: return DiscordAuthResponseCodes::UnknownAccount;
        case 4: return DiscordAuthResponseCodes::BannedAccount;
        case 5: return DiscordAuthResponseCodes::BannedIP;
        case 6: return DiscordAuthResponseCodes::BannedPermanentlyAccount;
        case 7: return DiscordAuthResponseCodes::BannedPermanentlyIP;
        case 8: return DiscordAuthResponseCodes::ServerOffline;
        default: throw std::out_of_range("index");
    }
}

template<>
WH_API_EXPORT size_t EnumUtils<DiscordAuthResponseCodes>::ToIndex(DiscordAuthResponseCodes value)
{
    switch (value)
    {
        case DiscordAuthResponseCodes::Ok: return 0;
        case DiscordAuthResponseCodes::Failed: return 1;
        case DiscordAuthResponseCodes::IncorrectKey: return 2;
        case DiscordAuthResponseCodes::UnknownAccount: return 3;
        case DiscordAuthResponseCodes::BannedAccount: return 4;
        case DiscordAuthResponseCodes::BannedIP: return 5;
        case DiscordAuthResponseCodes::BannedPermanentlyAccount: return 6;
        case DiscordAuthResponseCodes::BannedPermanentlyIP: return 7;
        case DiscordAuthResponseCodes::ServerOffline: return 8;
        default: throw std::out_of_range("value");
    }
}

/**********************************************************************************\
|* data for enum 'AccountCreateResult' in 'DiscordSharedDefines.h' auto-generated *|
\**********************************************************************************/
template<>
WH_API_EXPORT EnumText EnumUtils<AccountCreateResult>::ToString(AccountCreateResult value)
{
    switch (value)
    {
        case AccountCreateResult::Ok: return { "Ok", "Ok", "" };
        case AccountCreateResult::LongName: return { "LongName", "LongName", "" };
        case AccountCreateResult::LongKey: return { "LongKey", "LongKey", "" };
        case AccountCreateResult::NameAlreadyExist: return { "NameAlreadyExist", "NameAlreadyExist", "" };
        case AccountCreateResult::NameNotExist: return { "NameNotExist", "NameNotExist", "" };
        case AccountCreateResult::DBError: return { "DBError", "DBError", "" };
        default: throw std::out_of_range("value");
    }
}

template<>
WH_API_EXPORT size_t EnumUtils<AccountCreateResult>::Count() { return 6; }

template<>
WH_API_EXPORT AccountCreateResult EnumUtils<AccountCreateResult>::FromIndex(size_t index)
{
    switch (index)
    {
        case 0: return AccountCreateResult::Ok;
        case 1: return AccountCreateResult::LongName;
        case 2: return AccountCreateResult::LongKey;
        case 3: return AccountCreateResult::NameAlreadyExist;
        case 4: return AccountCreateResult::NameNotExist;
        case 5: return AccountCreateResult::DBError;
        default: throw std::out_of_range("index");
    }
}

template<>
WH_API_EXPORT size_t EnumUtils<AccountCreateResult>::ToIndex(AccountCreateResult value)
{
    switch (value)
    {
        case AccountCreateResult::Ok: return 0;
        case AccountCreateResult::LongName: return 1;
        case AccountCreateResult::LongKey: return 2;
        case AccountCreateResult::NameAlreadyExist: return 3;
        case AccountCreateResult::NameNotExist: return 4;
        case AccountCreateResult::DBError: return 5;
        default: throw std::out_of_range("value");
    }
}
}
