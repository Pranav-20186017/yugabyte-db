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

#ifndef YB_UTIL_TSAN_UTIL_H
#define YB_UTIL_TSAN_UTIL_H

namespace yb {

template <class T>
constexpr T NonTsanVsTsan(T value_not_in_tsan, T value_in_tsan) {
#if THREAD_SANITIZER
  return value_in_tsan;
#else
  return value_not_in_tsan;
#endif
}

}  // namespace yb

#endif  // YB_UTIL_TSAN_UTIL_H
