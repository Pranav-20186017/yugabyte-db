//--------------------------------------------------------------------------------------------------
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
// Tree node definitions for USE KEYSPACE statement.
//--------------------------------------------------------------------------------------------------

#ifndef YB_SQL_PTREE_PT_USE_KEYSPACE_H_
#define YB_SQL_PTREE_PT_USE_KEYSPACE_H_

#include "yb/sql/ptree/tree_node.h"

namespace yb {
namespace sql {

//--------------------------------------------------------------------------------------------------
// USE KEYSPACE statement.

class PTUseKeyspace : public TreeNode {
 public:
  //------------------------------------------------------------------------------------------------
  // Public types.
  typedef MCSharedPtr<PTUseKeyspace> SharedPtr;
  typedef MCSharedPtr<const PTUseKeyspace> SharedPtrConst;

  //------------------------------------------------------------------------------------------------
  // Constructor and destructor.
  PTUseKeyspace(MemoryContext *memctx,
                YBLocation::SharedPtr loc,
                const MCSharedPtr<MCString>& name);
  virtual ~PTUseKeyspace();

  template<typename... TypeArgs>
  inline static PTUseKeyspace::SharedPtr MakeShared(MemoryContext *memctx, TypeArgs&&... args) {
    return MCMakeShared<PTUseKeyspace>(memctx, std::forward<TypeArgs>(args)...);
  }

  // Node type.
  virtual TreeNodeOpcode opcode() const override {
    return TreeNodeOpcode::kPTUseKeyspace;
  }

  // Node semantics analysis.
  virtual CHECKED_STATUS Analyze(SemContext *sem_context) override;
  void PrintSemanticAnalysisResult(SemContext *sem_context);

  const char* name() const {
    return name_->c_str();
  }

 private:
  MCSharedPtr<MCString> name_;
};

}  // namespace sql
}  // namespace yb

#endif  // YB_SQL_PTREE_PT_USE_KEYSPACE_H_
