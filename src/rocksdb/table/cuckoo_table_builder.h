//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.
//
// The following only applies to changes made to this file as part of YugaByte development.
//
// Portions Copyright (c) YugaByte, Inc.
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

#ifndef ROCKSDB_TABLE_CUCKOO_TABLE_BUILDER_H
#define ROCKSDB_TABLE_CUCKOO_TABLE_BUILDER_H

#ifndef ROCKSDB_LITE
#include <stdint.h>
#include <limits>
#include <string>
#include <utility>
#include <vector>
#include "port/port.h"
#include "rocksdb/status.h"
#include "table/table_builder.h"
#include "rocksdb/table.h"
#include "rocksdb/table_properties.h"
#include "util/autovector.h"

namespace rocksdb {

class CuckooTableBuilder: public TableBuilder {
 public:
  CuckooTableBuilder(WritableFileWriter* file, double max_hash_table_ratio,
                     uint32_t max_num_hash_func, uint32_t max_search_depth,
                     const Comparator* user_comparator,
                     uint32_t cuckoo_block_size, bool use_module_hash,
                     bool identity_as_first_hash,
                     uint64_t (*get_slice_hash)(const Slice&, uint32_t,
                                                uint64_t));

  // REQUIRES: Either Finish() or Abandon() has been called.
  ~CuckooTableBuilder() {}

  // Add key,value to the table being constructed.
  // REQUIRES: key is after any previously added key according to comparator.
  // REQUIRES: Finish(), Abandon() have not been called
  void Add(const Slice& key, const Slice& value) override;

  // Return non-ok iff some error has been detected.
  Status status() const override { return status_; }

  // Finish building the table.  Stops using the file passed to the
  // constructor after this function returns.
  // REQUIRES: Finish(), Abandon() have not been called
  Status Finish() override;

  // Indicate that the contents of this builder should be abandoned.  Stops
  // using the file passed to the constructor after this function returns.
  // If the caller is not going to call Finish(), it must call Abandon()
  // before destroying this builder.
  // REQUIRES: Finish(), Abandon() have not been called
  void Abandon() override;

  // Number of calls to Add() so far.
  uint64_t NumEntries() const override;

  // Size of the file generated so far.  If invoked after a successful
  // Finish() call, returns the size of the final generated file.
  uint64_t TotalFileSize() const override;

  // For cuckoo table SST file format - the same as TotalFileSize, since it uses the only base file
  // for both data and metadata.
  uint64_t BaseFileSize() const override;

  TableProperties GetTableProperties() const override { return properties_; }

 private:
  struct CuckooBucket {
    CuckooBucket()
      : vector_idx(kMaxVectorIdx), make_space_for_key_call_id(0) {}
    uint32_t vector_idx;
    // This number will not exceed kvs_.size() + max_num_hash_func_.
    // We assume number of items is <= 2^32.
    uint32_t make_space_for_key_call_id;
  };
  static const uint32_t kMaxVectorIdx = port::kMaxInt32;

  bool MakeSpaceForKey(const autovector<uint64_t>& hash_vals,
                       const uint32_t call_id,
                       std::vector<CuckooBucket>* buckets, uint64_t* bucket_id);
  Status MakeHashTable(std::vector<CuckooBucket>* buckets);

  inline bool IsDeletedKey(uint64_t idx) const;
  inline Slice GetKey(uint64_t idx) const;
  inline Slice GetUserKey(uint64_t idx) const;
  inline Slice GetValue(uint64_t idx) const;

  uint32_t num_hash_func_;
  WritableFileWriter* file_;
  const double max_hash_table_ratio_;
  const uint32_t max_num_hash_func_;
  const uint32_t max_search_depth_;
  const uint32_t cuckoo_block_size_;
  uint64_t hash_table_size_;
  bool is_last_level_file_;
  bool has_seen_first_key_;
  bool has_seen_first_value_;
  uint64_t key_size_;
  uint64_t value_size_;
  // A list of fixed-size key-value pairs concatenating into a string.
  // Use GetKey(), GetUserKey(), and GetValue() to retrieve a specific
  // key / value given an index
  std::string kvs_;
  std::string deleted_keys_;
  // Number of key-value pairs stored in kvs_ + number of deleted keys
  uint64_t num_entries_;
  // Number of keys that contain value (non-deletion op)
  uint64_t num_values_;
  Status status_;
  TableProperties properties_;
  const Comparator* ucomp_;
  bool use_module_hash_;
  bool identity_as_first_hash_;
  uint64_t (*get_slice_hash_)(const Slice& s, uint32_t index,
    uint64_t max_num_buckets);
  std::string largest_user_key_ = "";
  std::string smallest_user_key_ = "";

  bool closed_;  // Either Finish() or Abandon() has been called.

  // No copying allowed
  CuckooTableBuilder(const CuckooTableBuilder&) = delete;
  void operator=(const CuckooTableBuilder&) = delete;
};

}  // namespace rocksdb

#endif  // ROCKSDB_LITE

#endif  // ROCKSDB_TABLE_CUCKOO_TABLE_BUILDER_H
