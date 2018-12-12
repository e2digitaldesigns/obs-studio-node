/*
 * Modern effects for a modern Streamer
 * Copyright (C) 2017 Michael Fabian Dirks
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#pragma once
#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>

#ifndef _DEBUG
#include "client/crash_report_database.h"
#include "client/crashpad_client.h"
#include "client/settings.h"
#include <crow.hpp>
#endif

namespace util
{
	class CrashManager
	{
		public:

		enum class OperationType
		{
			Unknow,
			Crashpad,
			Sentry
		};

#ifndef _DEBUG

		struct CrashHandlerInfo
		{
			base::FilePath                                 handler;
			base::FilePath                                 db;
			std::string                                    url;
			std::vector<std::string>                       arguments;
			crashpad::CrashpadClient                       client;
			std::unique_ptr<crashpad::CrashReportDatabase> database;
			std::unique_ptr<nlohmann::crow>                sentry;
		};

#else

		struct CrashHandlerInfo
		{};

#endif

		~CrashManager();
		bool Initialize(OperationType _operationType);
		void Configure();
		void OpenConsole();

		private:
		bool        SetupCrashpad();
		bool        SetupSentry();
		static bool TryHandleCrash(std::string _format, std::string _crashMessage);
		static void HandleExit() noexcept;
		static void HandleCrash(std::string _crashInfo, bool _callAbort = true) noexcept;
		static void InvokeReport(std::string _crashInfo, std::vector<std::string> _callStack);
	};

}; // namespace util