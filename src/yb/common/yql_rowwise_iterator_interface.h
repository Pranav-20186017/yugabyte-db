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

#ifndef YB_COMMON_YQL_ROWWISE_ITERATOR_INTERFACE_H
#define YB_COMMON_YQL_ROWWISE_ITERATOR_INTERFACE_H

#include "yb/common/iterator.h"
#include "yb/common/yql_rowblock.h"
#include "yb/common/yql_scanspec.h"

namespace yb {
namespace common {

class YQLRowwiseIteratorIf : public RowwiseIterator {
 public:
  virtual ~YQLRowwiseIteratorIf() {}

  virtual CHECKED_STATUS Init(ScanSpec *spec) override = 0;

  // Init YQL read scan.
  virtual CHECKED_STATUS Init(const YQLScanSpec& spec) = 0;

  // This may return one row at a time in the initial implementation, even though Kudu's scanning
  // interface supports returning multiple rows at a time.
  virtual CHECKED_STATUS NextBlock(RowBlock *dst) override = 0;

  // Is the next row column to read a static column?
  virtual bool IsNextStaticColumn() const = 0;

  // Read next row using the specified projection.
  virtual CHECKED_STATUS NextRow(const Schema& projection, YQLTableRow* table_row) = 0;

  // Skip the current row.
  virtual void SkipRow() = 0;

  // Checks whether we have processed enough rows for a page and sets the appropriate paging
  // state in the response object.
  virtual CHECKED_STATUS SetPagingStateIfNecessary(const YQLReadRequestPB& request,
                                                   const YQLRowBlock& rowblock,
                                                   const size_t row_count_limit,
                                                   YQLResponsePB* response) const = 0;
};

}  // namespace common
}  // namespace yb
#endif // YB_COMMON_YQL_ROWWISE_ITERATOR_INTERFACE_H
