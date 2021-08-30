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
#include "c2_status.h"

#include <stdint.h>

namespace c2 {

const char* Status::CopyState(const char* state) {
  uint32_t size;
  memcpy(&size, state, sizeof(size));
  char* const result = new char[size + 5];
  memcpy(result, state, size + 5);
  return result;
}

Status::Status(Code code, const Slice& msg1, const Slice& msg2) {
  assert(code != kOk);
  const uint32_t len1 = msg1.size();
  const uint32_t len2 = msg2.size();
  const uint32_t size = len1 + (len2 ? (2 + len2) : 0);
  char* const result = new char[size + 5];
  memcpy(result, &size, sizeof(size));
  result[4] = static_cast<char>(code);
  memcpy(result + 5, msg1.data(), len1);
  if (len2) {
    result[5 + len1] = ':';
    result[6 + len1] = ' ';
    memcpy(result + 7 + len1, msg2.data(), len2);
  }
  state_ = result;
}

namespace {
/* clang-format off */
static const char* kCodeString[] = {
  /* kOk */ "OK",                                             /* 0 */
  /* kNotFound */ "Not found",                                /* 1 */
  /* kAlreadyExists */ "Already exists",                      /* 2 */
  /* kCorruption */ "Corruption",                             /* 3 */
  /* kNotSupported */ "Not implemented",                      /* 4 */
  /* kInvalidArgument */ "Invalid argument",                  /* 5 */
  /* kIOError */ "IO error",                                  /* 6 */
  /* kAssertionFailed */ "Assertion failed",                  /* 7 */
  /* kAccessDenied */ "Permission denied",                    /* 8 */
};
/* clang-format on */
}  // namespace

std::string Status::ToString() const {
  if (state_ == NULL) {
    return "OK";
  } else {
    const char* const type = kCodeString[err_code()];
    uint32_t length;
    memcpy(&length, state_, sizeof(length));
    if (length == 0) {
      return type;
    } else {
      std::string result(type);
      result.append(": ");
      result.append(state_ + 5, length);
      return result;
    }
  }
}

}  // namespace c2
