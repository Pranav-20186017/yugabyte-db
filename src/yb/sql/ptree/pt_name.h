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
// Tree node definitions for database object names such as table, column, or index names.
//--------------------------------------------------------------------------------------------------

#ifndef YB_SQL_PTREE_PT_NAME_H_
#define YB_SQL_PTREE_PT_NAME_H_

#include "yb/client/yb_table_name.h"
#include "yb/sql/ptree/tree_node.h"
#include "yb/sql/ptree/list_node.h"

namespace yb {
namespace sql {

// This class represents a name node.
class PTName : public TreeNode {
 public:
  //------------------------------------------------------------------------------------------------
  // Public types.
  typedef MCSharedPtr<PTName> SharedPtr;
  typedef MCSharedPtr<const PTName> SharedPtrConst;

  //------------------------------------------------------------------------------------------------
  // Constructor and destructor.
  explicit PTName(MemoryContext *memctx = nullptr,
                  YBLocation::SharedPtr loc = nullptr,
                  const MCSharedPtr<MCString>& name = nullptr);
  virtual ~PTName();

  template<typename... TypeArgs>
  inline static PTName::SharedPtr MakeShared(MemoryContext *memctx, TypeArgs&&... args) {
    return MCMakeShared<PTName>(memctx, std::forward<TypeArgs>(args)...);
  }

  CHECKED_STATUS SetupPrimaryKey(SemContext *sem_context);
  CHECKED_STATUS SetupHashAndPrimaryKey(SemContext *sem_context);

  const MCString& name() const {
    return *name_;
  }

  const MCSharedPtr<MCString>& name_ptr() {
    return name_;
  }

 private:
  MCSharedPtr<MCString> name_;
};

// This class represents "*" (i.e. all fields) in SQL statement.
class PTNameAll : public PTName {
 public:
  //------------------------------------------------------------------------------------------------
  // Public types.
  typedef MCSharedPtr<PTNameAll> SharedPtr;
  typedef MCSharedPtr<const PTNameAll> SharedPtrConst;

  //------------------------------------------------------------------------------------------------
  // Constructor and destructor.
  PTNameAll(MemoryContext *memctx, YBLocation::SharedPtr loc);
  virtual ~PTNameAll();

  template<typename... TypeArgs>
  inline static PTNameAll::SharedPtr MakeShared(MemoryContext *memctx, TypeArgs&&... args) {
    return MCMakeShared<PTNameAll>(memctx, std::forward<TypeArgs>(args)...);
  }
};

// This class represents a qualified name (e.g. "a.m.t").
class PTQualifiedName : public PTName {
 public:
  //------------------------------------------------------------------------------------------------
  // Public types.
  typedef MCSharedPtr<PTQualifiedName> SharedPtr;
  typedef MCSharedPtr<const PTQualifiedName> SharedPtrConst;

  //------------------------------------------------------------------------------------------------
  // Constructor and destructor.
  PTQualifiedName(MemoryContext *mctx,
                  YBLocation::SharedPtr loc,
                  const PTName::SharedPtr& ptname);
  PTQualifiedName(MemoryContext *mctx,
                  YBLocation::SharedPtr loc,
                  const MCSharedPtr<MCString>& name);
  virtual ~PTQualifiedName();

  template<typename... TypeArgs>
  inline static PTQualifiedName::SharedPtr MakeShared(MemoryContext *memctx, TypeArgs&&... args) {
    return MCMakeShared<PTQualifiedName>(memctx, std::forward<TypeArgs>(args)...);
  }

  // Forming qualified name by appending.
  void Append(const PTName::SharedPtr& ptname);

  // Forming qualified name by prepending.
  void Prepend(const PTName::SharedPtr& ptname);

  // Node semantics analysis.
  virtual CHECKED_STATUS Analyze(SemContext *sem_context) override;

  const MCString& first_name() const {
    return ptnames_.front()->name();
  }

  const MCString& last_name() const {
    return ptnames_.back()->name();
  }

  bool IsSimpleName() const {
    return ptnames_.size() == 1;
  }

  // Construct bind variable name from this name.
  const MCSharedPtr<MCString>& bindvar_name() {
    return ptnames_.back()->name_ptr();
  }

  client::YBTableName ToTableName() const {
    // We support only names with 1 or 2 sub-names.
    // See Analyze() implementation.
    return (ptnames_.size() >= 2 ? client::YBTableName(first_name().c_str(), last_name().c_str())
        : client::YBTableName(last_name().c_str()));
  }

 private:
  MCList<PTName::SharedPtr> ptnames_;
};

using PTQualifiedNameListNode = TreeListNode<PTQualifiedName>;

}  // namespace sql
}  // namespace yb

#endif  // YB_SQL_PTREE_PT_NAME_H_
