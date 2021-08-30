/*
 * Copyright (c) 2021 Triad National Security, LLC, as operator of Los Alamos
 * National Laboratory. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of TRIAD, Los Alamos National Laboratory, LANL, the
 *    U.S. Government, nor the names of its contributors may be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 2011 The LevelDB Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found at https://github.com/google/leveldb.
 */
#pragma once

#include "c2_slice.h"

namespace c2 {
// A Status encapsulates the result of an operation. It may indicate success,
// or it may indicate an error with an associated error message.
//
// Multiple threads can invoke const methods on a Status without external
// synchronization, but if any of the threads may call a non-const method, all
// threads accessing the same Status must use external synchronization.
class Status {
 public:
  // Create a status that indicates a success.
  Status() : state_(NULL) {}
  ~Status() { delete[] state_; }

  Status(const Status& s);  // Copy from the specified status.
  void operator=(const Status& s);

  // Return a success status.
  static Status OK() { return Status(); }

  // Returns true iff the status indicates success.
  bool ok() const { return (state_ == NULL); }

#define DEF_ERR(Err)                                                        \
  static inline Status Err(const Slice& msg, const Slice& msg2 = Slice()) { \
    return Status(k##Err, msg, msg2);                                       \
  }                                                                         \
  bool Is##Err() const { return code() == k##Err; }

  DEF_ERR(NotSupported);
  DEF_ERR(InvalidArgument);
  DEF_ERR(NotFound);
  DEF_ERR(AlreadyExists);
  DEF_ERR(AssertionFailed);
  DEF_ERR(AccessDenied);
  DEF_ERR(Corruption);
  DEF_ERR(IOError);

#undef DEF_ERR

  int err_code() const { return static_cast<int>(code()); }
  // Return a string representation of this status suitable for printing.
  // Returns the string "OK" for success.
  std::string ToString() const;

  enum Code {
    kOk = 0,
    kNotFound = 1,
    kAlreadyExists = 2,
    kCorruption = 3,
    kNotSupported = 4,
    kInvalidArgument = 5,
    kIOError = 6,
    kAssertionFailed = 7,
    kAccessDenied = 8,
    kUnknownError = 9
  };

  static const int kMaxCode = kUnknownError - 1;

 private:
  // OK status has a NULL state_.  Otherwise, state_ is a new[] array
  // of the following form:
  //    state_[0..3] == length of message
  //    state_[4]    == code
  //    state_[5..]  == message
  const char* state_;
  Status(Code code, const Slice& msg, const Slice& msg2);
  static const char* CopyState(const char* s);
  Code code() const {
    return (state_ == NULL) ? kOk : static_cast<Code>(state_[4]);
  }
};

inline Status::Status(const Status& s) {
  state_ = (s.state_ == NULL) ? NULL : CopyState(s.state_);
}

inline void Status::operator=(const Status& s) {
  // The following condition catches both aliasing (when this == &s),
  // and the common case where both s and *this are ok.
  if (state_ != s.state_) {
    delete[] state_;
    if (s.state_ != NULL) {
      state_ = CopyState(s.state_);
    } else {
      state_ = NULL;
    }
  }
}

}  // namespace c2
