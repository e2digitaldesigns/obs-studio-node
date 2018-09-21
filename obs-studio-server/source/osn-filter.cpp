// Client module for the OBS Studio node module.
// Copyright(C) 2017 Streamlabs (General Workings Inc)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110 - 1301, USA.

#include "osn-Filter.hpp"
#include <ipc-server.hpp>
#include <memory>
#include <obs.h>
#include "error.hpp"
#include "osn-source.hpp"
#include "shared.hpp"

void osn::Filter::Register(ipc::server& srv)
{
	std::shared_ptr<ipc::collection> cls = std::make_shared<ipc::collection>("Filter");
	cls->register_function(std::make_shared<ipc::function>("Types", std::vector<ipc::type>{}, Types));
	cls->register_function(std::make_shared<ipc::function>(
	    "Create", std::vector<ipc::type>{ipc::type::String, ipc::type::String}, Create));
	cls->register_function(std::make_shared<ipc::function>(
	    "Create", std::vector<ipc::type>{ipc::type::String, ipc::type::String, ipc::type::String}, Create));
	srv.register_collection(cls);
}

void osn::Filter::Types(
    void*                          data,
    const int64_t                  id,
    const std::vector<ipc::value>& args,
    std::vector<ipc::value>&       rval)
{
	rval.push_back(ipc::value((uint64_t)ErrorCode::Ok));
	const char* typeId = nullptr;
	for (size_t idx = 0; obs_enum_filter_types(idx, &typeId); idx++) {
		rval.push_back(ipc::value(typeId ? typeId : ""));
	}
	AUTO_DEBUG;
}

void osn::Filter::Create(
    void*                          data,
    const int64_t                  id,
    const std::vector<ipc::value>& args,
    std::vector<ipc::value>&       rval)
{
	std::string sourceId, name;
	obs_data_t* settings = nullptr;

	switch (args.size()) {
	case 3:
		settings = obs_data_create_from_json(args[2].value_str.c_str());
	case 2:
		name     = args[1].value_str;
		sourceId = args[0].value_str;
		break;
	}

	obs_source_t* source = obs_source_create_private(sourceId.c_str(), name.c_str(), settings);
	if (!source) {
		rval.push_back(ipc::value((uint64_t)ErrorCode::Error));
		rval.push_back(ipc::value("Failed to create filter."));
		AUTO_DEBUG;
		return;
	}

	uint64_t uid = osn::Source::Manager::GetInstance().allocate(source);
	if (uid == UINT64_MAX) {
		// No further Ids left, leak somewhere.
		rval.push_back(ipc::value((uint64_t)ErrorCode::CriticalError));
		rval.push_back(ipc::value("Index list is full."));
		AUTO_DEBUG;
		return;
	}
	osn::Source::attach_source_signals(source);

	rval.push_back(ipc::value((uint64_t)ErrorCode::Ok));
	rval.push_back(ipc::value(uid));
	AUTO_DEBUG;
}
