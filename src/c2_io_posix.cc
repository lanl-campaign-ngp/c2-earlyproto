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
#include "c2_io_posix.h"

#include "port.h"

#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>

namespace c2 {
#if __cplusplus >= 201103L
#define OVERRIDE override
#else
#define OVERRIDE
#endif
class PosixIO : public IO {
 public:
  PosixIO() {}
  virtual ~PosixIO() {}

  virtual Status NewWritableFile(const char* fname, WritableFile** r) OVERRIDE {
    int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd != -1) {
      *r = new PosixWritableFile(fname, fd);
      return Status::OK();
    } else {
      *r = NULL;
      return PosixError(fname, errno);
    }
  }

  virtual Status CreateDir(const char* dirname) OVERRIDE {
    Status result;
    if (mkdir(dirname, 0755) != 0) {
      result = PosixError(dirname, errno);
    }
    return result;
  }
};

class PosixLibcBufferedIOWrapper : public IOWrapper {
 public:
  explicit PosixLibcBufferedIOWrapper(IO* base) : IOWrapper(base) {}
  virtual ~PosixLibcBufferedIOWrapper() {}

  virtual Status NewWritableFile(const char* fname, WritableFile** r) OVERRIDE {
    FILE* f = fopen(fname, "w");
    if (f != NULL) {
      *r = new PosixBufferedWritableFile(fname, f);
      return Status::OK();
    } else {
      *r = NULL;
      return PosixError(fname, errno);
    }
  }
};

// Map integer errors to C2 status objects.
Status PosixError(const Slice& err_context, int err_number) {
  switch (err_number) {
    case EEXIST:
      return Status::AlreadyExists(err_context);
    case ENOENT:
      return Status::NotFound(err_context);
    case EACCES:
      return Status::AccessDenied(err_context);
    default:
      return Status::IOError(err_context, strerror(err_number));
  }
}

namespace {
pthread_once_t once = PTHREAD_ONCE_INIT;
IO* posix_io_buf = NULL;
IO* posix_io = NULL;

void OpenPosixIO() {
  IO* const base = new PosixIO;
  posix_io_buf = new PosixLibcBufferedIOWrapper(base);
  posix_io = base;
}

IO* PosixGetBaseImplementation() {
  port::PthreadCall("pthread_once", pthread_once(&once, OpenPosixIO));
  return posix_io;
}

IO* PosixGetBuffered() {
  port::PthreadCall("pthread_once", pthread_once(&once, OpenPosixIO));
  return posix_io_buf;
}

}  // namespace

// Return the base IO implementation that uses standard IO calls
// provided by the OS. The returned result belongs to c2.
IO* IO::GetUnBuffered() {
  IO* result = PosixGetBaseImplementation();
  return result;
}

// Return buffered IO. The returned result belongs to c2.
IO* IO::Default() {
  IO* result = PosixGetBuffered();
  return result;
}

}  // namespace c2
