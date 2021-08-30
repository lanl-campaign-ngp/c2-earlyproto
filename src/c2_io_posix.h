/*
 * Copyright (c) 2021 Triad National Security, LLC, as operator of
 *     Los Alamos National Laboratory.
 * All rights reserved.
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
#pragma once

#include "c2_io.h"

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#if !defined(__linux__)
// Use regular buffered IO routines on non-Linux platforms
#define fwrite_unlocked fwrite
#define fflush_unlocked fflush
#define fdatasync fsync
#endif

namespace c2 {

// Map standard POSIX errors as C2 status objects.
extern Status PosixError(const Slice& err_context, int err_number);

// C2 wrapper of an opened FILE object.
class PosixBufferedWritableFile : public WritableFile {
 private:
  std::string filename_;
  FILE* file_;

 public:
  PosixBufferedWritableFile(const char* fname, FILE* f)
      : filename_(fname), file_(f) {}

  virtual ~PosixBufferedWritableFile() {
    if (file_ != NULL) {
      fclose(file_);  // Ignoring any potential errors
    }
  }

  virtual Status Append(const Slice& data) {
    Status result;
    size_t r = fwrite_unlocked(data.data(), 1, data.size(), file_);
    if (r != data.size()) {
      result = PosixError(filename_, errno);
    }
    return result;
  }

  virtual Status Close() {
    Status result;
    if (fclose(file_) != 0) {
      result = PosixError(filename_, errno);
    }
    file_ = NULL;
    return result;
  }

  virtual Status Flush() {
    Status result;
    if (fflush_unlocked(file_) != 0) {
      result = PosixError(filename_, errno);
    }
    return result;
  }

  virtual Status Sync() {
    Status result = Flush();
    if (!result.ok()) {
      return result;
    } else if (fdatasync(fileno(file_)) != 0) {
      result = PosixError(filename_, errno);
    }
    return result;
  }
};

// C2 wrapper of a plain OS file descriptor.
class PosixWritableFile : public WritableFile {
 private:
  std::string filename_;
  int fd_;

 public:
  PosixWritableFile(const char* fname, int fd) : filename_(fname), fd_(fd) {}

  virtual ~PosixWritableFile() {
    if (fd_ != -1) {
      close(fd_);
    }
  }

  virtual Status Append(const Slice& buf) {
    if (buf.empty()) return Status::OK();  // Avoiding empty writes
    ssize_t nw = write(fd_, buf.data(), buf.size());
    if (nw != buf.size()) {
      return PosixError(filename_, errno);
    } else {
      return Status::OK();
    }
  }

  virtual Status Close() {
    close(fd_);
    fd_ = -1;
    return Status::OK();
  }

  virtual Status Flush() {  // Do nothing; all writes are directly sent to OS
    return Status::OK();
  }

  virtual Status Sync() {
    int r = fdatasync(fd_);
    if (r != 0) {
      return PosixError(filename_, errno);
    }
    return Status::OK();
  }
};

}  // namespace c2
