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

#ifndef YB_CLIENT_YQL_DML_TEST_BASE_H
#define YB_CLIENT_YQL_DML_TEST_BASE_H

#include <algorithm>
#include <functional>
#include <vector>

#include <gtest/gtest.h>

#include "yb/client/client.h"
#include "yb/client/yb_op.h"
#include "yb/client/callbacks.h"
#include "yb/common/yql_protocol.pb.h"
#include "yb/common/yql_rowblock.h"
#include "yb/integration-tests/mini_cluster.h"
#include "yb/integration-tests/yb_mini_cluster_test_base.h"
#include "yb/master/mini_master.h"
#include "yb/util/async_util.h"
#include "yb/util/test_util.h"

namespace yb {
namespace client {

using std::string;
using std::vector;
using std::shared_ptr;
using std::unique_ptr;

extern const client::YBTableName kTableName;

class TableHandle {
 public:
  void Create(const YBTableName& table_name,
              YBClient* client,
              YBSchemaBuilder* builder);

  std::shared_ptr<YBqlWriteOp> NewWriteOp(YQLWriteRequestPB::YQLStmtType type);

  std::shared_ptr<YBqlReadOp> NewReadOp();

  int32_t ColumnId(const string &column_name) {
    return column_ids_[column_name];
  }

  void SetInt32ColumnValue(
      YQLColumnValuePB *column_value, const std::string &column_name, const int32_t value,
      YBPartialRow *prow = nullptr, int prow_index = -1);
  void SetStringColumnValue(
      YQLColumnValuePB *column_value, const std::string &column_name, const std::string &value,
      YBPartialRow *prow = nullptr, int prow_index = -1);

  // Set a column id without value - for DELETE
  void SetColumn(YQLColumnValuePB *column_value, const string &column_name);

  // Set a int32 column value comparison.
  // E.g. <column-id> = <int32-value>
  void SetInt32Condition(
      YQLConditionPB *const condition, const string &column_name, const YQLOperator op,
      const int32_t value);

  // Set a string column value comparison.
  // E.g. <column-id> = <string-value>
  void SetStringCondition(
      YQLConditionPB *const condition, const string &column_name, const YQLOperator op,
      const string &value);

  // Add a int32 column value comparison under a logical comparison condition.
  // E.g. Add <column-id> = <int32-value> under "... AND <column-id> = <int32-value>".
  void AddInt32Condition(
      YQLConditionPB *const condition, const string &column_name, const YQLOperator op,
      const int32_t value);

  // Add a string column value comparison under a logical comparison condition.
  // E.g. Add <column-id> = <string-value> under "... AND <column-id> = <string-value>".
  void AddStringCondition(
      YQLConditionPB *const condition, const string &column_name, const YQLOperator op,
      const string &value);

  // Add a simple comparison operation under a logical comparison condition.
  // E.g. Add <EXISTS> under "... AND <EXISTS>".
  void AddCondition(YQLConditionPB *const condition, const YQLOperator op);

  void AddColumns(const std::vector<std::string>& columns, YQLReadRequestPB* req);

  const YBTableName& name() const {
    return table_->name();
  }

  const YBSchema& schema() const {
    return table_->schema();
  }

 private:
  typedef std::unordered_map<std::string, yb::ColumnId> ColumnIdsMap;

  YBTablePtr table_;
  ColumnIdsMap column_ids_;
};

Status FlushSession(YBSession *session);

class YqlDmlTestBase: public YBMiniClusterTestBase<MiniCluster> {
 public:
  void SetUp() override;
  void DoTearDown() override;

 protected:
  shared_ptr<YBClient> client_;
};

}  // namespace client
}  // namespace yb

#endif // YB_CLIENT_YQL_DML_TEST_BASE_H
