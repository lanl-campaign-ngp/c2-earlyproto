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

/*
 * Copyright (c) 2011 The LevelDB Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found at https://github.com/google/leveldb.
 */
#pragma once

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string>

namespace c2 {

// Slice is a simple structure containing a pointer into some external
// storage and a size. A user of a Slice must ensure that the slice is not used
// after the corresponding external storage has been deallocated.
//
// Multiple threads can invoke const methods on a Slice without external
// synchronization, but if any of the threads may call a non-const method, all
// threads accessing the same Slice must use external synchronization.
class Slice {
 public:
  // Create an empty slice.
  Slice() : data_(""), size_(0) {}

  // Create a slice that refers to d[0,n-1].
  Slice(const char* d, size_t n) : data_(d), size_(n) {}

  // Create a slice that refers to the contents of "s"
  Slice(const std::string& s) : data_(s.data()), size_(s.size()) {}

  // Create a slice that refers to s[0,strlen(s)-1]
  Slice(const char* s) : data_(s), size_(strlen(s)) {}

  // Return a pointer to the beginning of the referenced data
  const char* data() const { return data_; }

  // Return the length (in bytes) of the referenced data
  size_t size() const { return size_; }

  // Return true iff the length of the referenced data is zero
  bool empty() const { return size_ == 0; }

  // Return the ith byte in the referenced data.
  // REQUIRES: -size() <= n < size()
  const char& operator[](ssize_t n) const {
    if (n < 0) n += size_;
    assert(n >= 0 && n < size_);
    return data_[n];
  }

  // Change this slice to refer to an empty array
  void clear() {
    data_ = "";
    size_ = 0;
  }

  // Drop the first "n" bytes from this slice.
  void remove_prefix(size_t n) {
    assert(n <= size());
    data_ += n;
    size_ -= n;
  }

  // Drop the last "n" bytes from this slice.
  void remove_suffix(size_t n) {
    assert(n <= size());
    size_ -= n;
  }

  // Return a string that contains the copy of the referenced data.
  std::string ToString() const {
    if (size_ != 0) {
      return std::string(data_, size_);
    } else {
      return std::string();
    }
  }

  const char* c_str() const {
    assert(data_[size_] == 0);
    return data_;
  }

  // Return a string that contains the copy of a suffix of the referenced data.
  std::string substr(size_t start) const {
    assert(start <= size());
    return std::string(data_ + start, size_ - start);
  }

  // Three-way comparison.  Returns value:
  //   <  0 iff "*this" <  "b",
  //   == 0 iff "*this" == "b",
  //   >  0 iff "*this" >  "b"
  int compare(const Slice& b) const;

  // Return true iff "x" is a prefix of "*this"
  bool starts_with(const Slice& x) const {
    return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
  }

  // Return true iff "x" is a suffix of "*this"
  bool ends_with(const Slice& x) const {
    if (size_ < x.size_) {
      return false;
    } else {
      return memcmp(data_ + size_ - x.size_, x.data_, x.size_) == 0;
    }
  }

 private:
  const char* data_;
  size_t size_;

  // Intentionally copyable
};

inline bool operator<(const Slice& x, const Slice& y) {
  return x.compare(y) < 0;
}

inline bool operator>(const Slice& x, const Slice& y) {
  return x.compare(y) > 0;
}

inline bool operator<=(const Slice& x, const Slice& y) {
  return x.compare(y) <= 0;
}

inline bool operator>=(const Slice& x, const Slice& y) {
  return x.compare(y) >= 0;
}

inline bool operator==(const Slice& x, const Slice& y) {
  return (x.size() == y.size()) && (memcmp(x.data(), y.data(), y.size()) == 0);
}

inline bool operator!=(const Slice& x, const Slice& y) { return !(x == y); }

}  // namespace c2
