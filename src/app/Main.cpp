/*
 * This file is part of the WarheadApp Project. See AUTHORS file for Copyright information
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

#include "Config.h"
#include "StopWatch.h"
#include "GitRevision.h"
#include "IoContext.h"
#include "Log.h"
#include "Logo.h"
#include "DiscordConfig.h"
#include "ClientSocketMgr.h"
#include <boost/version.hpp>

#ifndef _WARHEAD_DISCORD_CONFIG
#define _WARHEAD_DISCORD_CONFIG "WarheadDiscordClient.conf"
#endif

int main(int argc, char** argv)
{
    // Command line parsing to get the configuration file name
    std::string configFile = sConfigMgr->GetConfigPath() + std::string(_WARHEAD_DISCORD_CONFIG);
    int count = 1;

    while (count < argc)
    {
        if (strcmp(argv[count], "-c") == 0)
        {
            if (++count >= argc)
            {
                printf("Runtime-Error: -c option requires an input argument\n");
                return 1;
            }
            else
                configFile = argv[count];
        }
        ++count;
    }

    if (!sConfigMgr->LoadAppConfigs(configFile))
        return 1;

    // Init logging
    sLog->Initialize();

    Warhead::Logo::Show("discordclient",
        [](std::string_view text)
        {
            LOG_INFO("server", text);
        },
        []()
        {
            LOG_INFO("server", "> Using configuration file:       {}", sConfigMgr->GetFilename());
            //LOG_INFO("server", "> Using SSL version:              {} (library: {})", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
            LOG_INFO("server", "> Using Boost version:            {}.{}.{}", BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100);
        }
    );

    std::shared_ptr<Warhead::Asio::IoContext> ioContext = std::make_shared<Warhead::Asio::IoContext>();

    sClientSocketMgr->Initialize(*ioContext);

    std::shared_ptr<void> sDiscordHandle(nullptr, [](void*)
    {
        LOG_INFO("server.authserver", "Stop socket...");
        sClientSocketMgr->Disconnect();
    });

    // Start the io service worker loop
    ioContext->run();

    LOG_INFO("server.authserver", "Halting process...");

    return 0;
}

