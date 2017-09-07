// Copyright (c) YugaByte, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.
//
//
// This file contains the CQLServerOptions class that defines the CQL server options.

#ifndef YB_CQLSERVER_CQL_SERVER_OPTIONS_H
#define YB_CQLSERVER_CQL_SERVER_OPTIONS_H

#include "yb/server/server_base_options.h"

namespace yb {
namespace cqlserver {

// Options for constructing a CQL server.
class CQLServerOptions : public yb::server::ServerBaseOptions {
 public:
  CQLServerOptions();
  std::string broadcast_rpc_address;

  ~CQLServerOptions() {}
};

} // namespace cqlserver
} // namespace yb
#endif /* YB_CQLSERVER_CQL_SERVER_OPTIONS_H */
