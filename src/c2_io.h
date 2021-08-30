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
#pragma once

#include "c2_status.h"

namespace c2 {

class WritableFile;

class IO {
 public:
  IO() {}
  virtual ~IO();

  // Return an IO implementation that performs sequential IO operations using
  // standard OS calls such as open(), read(), write(), fsync(), and close(),
  // and random read operations using pread(). Result of this call belongs to
  // C2 and should not be deleted.
  static IO* GetUnBuffered();

  // Return the default IO implementation. Current default performs sequential
  // IO using C buffered IO functions such as fopen(), fread(), fwrite(),
  // fflush(), and fclose(), and random read operations using regular pread().
  // Sophisticated clients may devise their own IO implementations rather than
  // relying on this default. Result of this call belongs to C2 and should not
  // be deleted.
  static IO* Default();

  // Create a WritableFile object that writes to a new file with the specified
  // name. Deletes any existing file with the same name and creates a new file.
  // On success, stores a pointer to the new file in *r and returns OK. On
  // failure stores NULL in *r and returns non-OK. The returned file can only
  // be accessed by one thread at a time.
  virtual Status NewWritableFile(const char* f, WritableFile** r) = 0;

  // Create the specified directory.
  virtual Status CreateDir(const char* dir) = 0;

  // Set *path to a temporary directory that can be used for testing. It may
  // or many not have just been created. The directory may or may not differ
  // between runs of the same process, but subsequent calls will return the
  // same directory.
  virtual Status GetTestDirectory(std::string* result) = 0;

 private:
  // No copying allowed
  void operator=(const IO& io);
  IO(const IO&);
};

// A file abstraction for sequential writing by a single thread. Callers may
// append small fragments at a time to the file.
class WritableFile {
 public:
  WritableFile() {}
  virtual ~WritableFile();

  virtual Status Append(const Slice& data) = 0;
  virtual Status Flush() = 0;
  virtual Status Sync() = 0;
  virtual Status Close() = 0;

 private:
  // No copying allowed
  void operator=(const WritableFile& file);
  WritableFile(const WritableFile&);
};

// An implementation of IO that forwards all calls to another IO implementation.
// This implementation is useful to clients who wish to override only part of
// the functionality of the other implementation.
class IOWrapper : public IO {
 public:
  explicit IOWrapper(IO* t) : target_(t) {}
  virtual ~IOWrapper();
  // Return the target to which this Env forwards all calls
  IO* target() const { return target_; }

  virtual Status NewWritableFile(const char* f, WritableFile** r) {
    return target_->NewWritableFile(f, r);
  }

  virtual Status CreateDir(const char* d) { return target_->CreateDir(d); }

  virtual Status GetTestDirectory(std::string* path) {
    return target_->GetTestDirectory(path);
  }

 private:
  IO* target_;
};

}  // namespace c2
