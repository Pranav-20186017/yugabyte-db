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
#ifndef YB_UTIL_PSTACK_WATCHER_H
#define YB_UTIL_PSTACK_WATCHER_H

#include <string>
#include <vector>

#include "yb/util/condition_variable.h"
#include "yb/util/monotime.h"
#include "yb/util/mutex.h"
#include "yb/util/status.h"
#include "yb/util/thread.h"

namespace yb {

// PstackWatcher is an object which will pstack the current process and print
// the results to stdout.  It does this after a certain timeout has occured.
class PstackWatcher {
 public:

  enum Flags {
    NO_FLAGS = 0,

    // Run 'thread apply all bt full', which is very verbose output
    DUMP_FULL = 1
  };

  // Static method to collect and write stack dump output to stdout of the current
  // process.
  static CHECKED_STATUS DumpStacks(int flags = NO_FLAGS);

  // Like the above but for any process, not just the current one.
  static CHECKED_STATUS DumpPidStacks(pid_t pid, int flags = NO_FLAGS);

  // Instantiate a watcher that writes a pstack to stdout after the given
  // timeout expires.
  explicit PstackWatcher(MonoDelta timeout);

  ~PstackWatcher();

  // Shut down the watcher and do not log a pstack.
  // This method is not thread-safe.
  void Shutdown();

  // Test whether the watcher is still running or has shut down.
  // Thread-safe.
  bool IsRunning() const;

  // Wait until the timeout expires and the watcher logs a pstack.
  // Thread-safe.
  void Wait() const;

 private:
  // Test for the existence of the given program in the system path.
  static CHECKED_STATUS HasProgram(const char* progname);

  // Get a stack dump using GDB directly.
  static CHECKED_STATUS RunGdbStackDump(pid_t pid, int flags);

  // Get a stack dump using the pstack or gstack program.
  static CHECKED_STATUS RunPstack(const std::string& progname, pid_t pid);

  // Invoke and wait for the stack dump program.
  static CHECKED_STATUS RunStackDump(const std::string& prog, const std::vector<std::string>& argv);

  // Run the thread that waits for the specified duration before logging a
  // pstack.
  void Run();

  const MonoDelta timeout_;
  bool running_;
  scoped_refptr<Thread> thread_;
  mutable Mutex lock_;
  mutable ConditionVariable cond_;
};

} // namespace yb
#endif
