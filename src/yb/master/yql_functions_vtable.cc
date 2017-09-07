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

#include "yb/master/master_defaults.h"
#include "yb/master/yql_functions_vtable.h"

namespace yb {
namespace master {

YQLFunctionsVTable::YQLFunctionsVTable(const Master* const master)
    : YQLEmptyVTable(master::kSystemSchemaFunctionsTableName, master, CreateSchema()) {
}

Schema YQLFunctionsVTable::CreateSchema() const {
  SchemaBuilder builder;
  CHECK_OK(builder.AddHashKeyColumn("keyspace_name", DataType::STRING));
  CHECK_OK(builder.AddKeyColumn("function_name", DataType::STRING));
  // TODO: argument_types should be part of the primary key, but since we don't support the CQL
  // 'frozen' type, we can't have collections in our primary key.
  CHECK_OK(builder.AddColumn("argument_types", YQLType::CreateTypeList(DataType::STRING)));
  // TODO: argument_names should be a frozen list.
  CHECK_OK(builder.AddColumn("argument_names", YQLType::CreateTypeList(DataType::STRING)));
  CHECK_OK(builder.AddColumn("called_on_null_input", YQLType::Create(DataType::BOOL)));
  CHECK_OK(builder.AddColumn("language", YQLType::Create(DataType::STRING)));
  CHECK_OK(builder.AddColumn("return_type", YQLType::Create(DataType::STRING)));
  return builder.Build();
}

}  // namespace master
}  // namespace yb
