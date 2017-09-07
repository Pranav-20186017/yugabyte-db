// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
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

// Portions Copyright (c) YugaByte, Inc.

#ifndef YB_COMMON_HYBRID_TIME_H_
#define YB_COMMON_HYBRID_TIME_H_

#include <inttypes.h>

#include <string>
#include <limits>

#include "yb/util/status.h"

namespace yb {

// This is used to represent the physical component of a HybridTime (microseconds since the UNIX
// epoch).
using MicrosTime = uint64_t;

// An alias for the raw in-memory representation of a HybridTime.
using HybridTimeRepr = uint64_t;

// An alias for the in-memory representation of the logical component of a HybridTime.
using LogicalTimeComponent = uint32_t;

constexpr HybridTimeRepr kMinHybridTimeValue =
    std::numeric_limits<HybridTimeRepr>::min();
constexpr HybridTimeRepr kMaxHybridTimeValue =
    std::numeric_limits<HybridTimeRepr>::max();
constexpr HybridTimeRepr kInitialHybridTimeValue = kMinHybridTimeValue + 1;
constexpr HybridTimeRepr kInvalidHybridTimeValue = kMaxHybridTimeValue - 1;

// A transaction hybrid time generated by a Clock.
class HybridTime {
 public:
  // TODO: replace all mentions of this with HybridTimeRepr itself and deprecate val_type.
  using val_type = HybridTimeRepr;

  // Left shifting 12 bits gives us 12 bits for the logical value and should still keep accurate
  // microseconds time until 2100+
  static constexpr int kBitsForLogicalComponent = 12;

  // This mask gives us back the logical bits.
  static constexpr HybridTimeRepr kLogicalBitMask = (1 << kBitsForLogicalComponent) - 1;

  // An initial transaction hybrid time, higher than min so that we can have
  // a hybrid time guaranteed to be lower than all generated hybrid times.
  static const HybridTime kInitialHybridTime;

  // An invalid transaction hybrid time -- HybridTime types initialize to this variable.
  static const HybridTime kInvalidHybridTime;

  // The maximum hybrid time.
  static const HybridTime kMax;

  // The minimum hybrid time.
  static const HybridTime kMin;

  // Hybrid times are converted to debug strings as <this_string_constant>(<hybrid_time_value>).
  static const char* const kHybridTimeDebugStrPrefix;

  // ----------------------------------------------------------------------------------------------
  // Constructors / static factories

  HybridTime() noexcept : v(kInvalidHybridTimeValue) {}

  HybridTime(MicrosTime micros, LogicalTimeComponent logical_value) {
    v = (micros << kBitsForLogicalComponent) + logical_value;
  }

  static inline HybridTime FromMicrosecondsAndLogicalValue(
      MicrosTime micros, LogicalTimeComponent logical_value) {
    return HybridTime(micros, logical_value);
  }

  static inline HybridTime FromMicros(MicrosTime micros) {
    return HybridTime(micros, 0);
  }

  explicit HybridTime(uint64_t val) : v(val) {}

  bool operator ==(const HybridTime &other) const {
    return v == other.v;
  }
  bool operator !=(const HybridTime &other) const {
    return v != other.v;
  }

  // Decode a hybrid time from the given input slice.
  // Mutates the slice to point after the decoded hybrid time.
  // Returns true upon success.
  bool DecodeFrom(Slice *input);

  // Encode the hybrid time to the given buffer.
  void EncodeTo(faststring *dst) const;

  int CompareTo(const HybridTime &other) const;

  std::string ToString() const;

  std::string ToDebugString() const;

  // Returns this hybrid time as an uint64_t
  uint64_t ToUint64() const;

  // Return the highest value of a HybridTime that is lower than this. If called on kMin, returns
  // kMin itself, because it cannot be decremented.
  HybridTime Decremented() const {
    if (v == 0) return *this;
    return HybridTime(v - 1);
  }

  // Sets this hybrid time from 'value'
  CHECKED_STATUS FromUint64(uint64_t value);

  HybridTimeRepr value() const { return v; }

  bool operator <(const HybridTime& other) const {
    return CompareTo(other) < 0;
  }

  bool operator >(const HybridTime& other) const {
    return CompareTo(other) > 0;
  }

  bool operator <=(const HybridTime& other) const {
    return CompareTo(other) <= 0;
  }

  bool operator >=(const HybridTime& other) const {
    return CompareTo(other) >= 0;
  }

  // Returns the physical value embedded in this HybridTime, in microseconds.
  inline HybridTimeRepr GetPhysicalValueMicros() const {
    return v >> kBitsForLogicalComponent;
  }

  inline LogicalTimeComponent GetLogicalValue() const {
    return v & kLogicalBitMask;
  }

  inline bool is_valid() const { return v != kInvalidHybridTimeValue; }

 private:

  HybridTimeRepr v;
};
class faststring;

class Slice;

inline int HybridTime::CompareTo(const HybridTime &other) const {
  if (v < other.v) {
    return -1;
  } else if (v > other.v) {
    return 1;
  }
  return 0;
}

inline std::ostream &operator <<(std::ostream &o, const HybridTime &hybridTime) {
  return o << hybridTime.ToString();
}

}  // namespace yb

#endif  // YB_COMMON_HYBRID_TIME_H_
