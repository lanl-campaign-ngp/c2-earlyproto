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

#include "c2_status.h"

#include <sstream>
#include <stdio.h>
#include <stdlib.h>

namespace c2 {
namespace test {

// Run all or a subset of the tests registered by the TEST() macro. If the
// environment variable "TESTS" is not set, runs all tests. Otherwise, runs only
// the tests whose name contains the value of "TESTS" as a substring.
//
// E.g., suppose the tests are:
//    TEST(Foo, Hello) { ... }
//    TEST(Foo, World) { ... }
//
// TESTS=Hello will run the first test
// TESTS=o     will run both tests
// TESTS=Junk  will run no tests
//
// Returns 0 if all tests pass.
// Dies or returns a non-zero value if some test fails.
extern int RunAllTests(int* argc, char*** argv);

// Return the directory to use for temporary storage.
extern std::string TmpDir();

// Return a randomization seed for this run. Typically returns the
// same number on repeated invocations of this binary, but automated
// runs may be able to vary the seed.
extern int RandomSeed();

// An instance of Tester is allocated to hold temporary state during
// the execution of an assertion.
class Tester {
 private:
  bool ok_;
  const char* fname_;
  int line_;
  std::stringstream ss_;

 public:
  Tester(const char* f, int l) : ok_(true), fname_(f), line_(l) {}

  ~Tester() {
    if (!ok_) {
      fprintf(stderr, "%s:%d:%s\n", fname_, line_, ss_.str().c_str());
      exit(1);
    }
  }

  Tester& Is(bool b, const char* msg) {
    if (!b) {
      ss_ << " Assertion failure: " << msg;
      ok_ = false;
    }
    return *this;
  }

  Tester& IsOk(const Status& s) {
    if (!s.ok()) {
      ss_ << " " << s.ToString();
      ok_ = false;
    }
    return *this;
  }

#define BINARY_OP(name, op)                          \
  template <class X, class Y>                        \
  Tester& name(const X& x, const Y& y) {             \
    if (!(x op y)) {                                 \
      ss_ << " failed: " << x << (" " #op " ") << y; \
      ok_ = false;                                   \
    }                                                \
    return *this;                                    \
  }

  BINARY_OP(IsEq, ==)
  BINARY_OP(IsNe, !=)
  BINARY_OP(IsGe, >=)
  BINARY_OP(IsGt, >)
  BINARY_OP(IsLe, <=)
  BINARY_OP(IsLt, <)
#undef BINARY_OP

  // Attach the specified value to the error message if an error has occurred
  template <class V>
  Tester& operator<<(const V& value) {
    if (!ok_) {
      ss_ << " " << value;
    }
    return *this;
  }
};

#define ASSERT_TRUE(c) ::c2::test::Tester(__FILE__, __LINE__).Is((c), #c)
#define ASSERT_OK(s) ::c2::test::Tester(__FILE__, __LINE__).IsOk((s))
#define ASSERT_EQ(a, b) ::c2::test::Tester(__FILE__, __LINE__).IsEq((a), (b))
#define ASSERT_NE(a, b) ::c2::test::Tester(__FILE__, __LINE__).IsNe((a), (b))
#define ASSERT_GE(a, b) ::c2::test::Tester(__FILE__, __LINE__).IsGe((a), (b))
#define ASSERT_GT(a, b) ::c2::test::Tester(__FILE__, __LINE__).IsGt((a), (b))
#define ASSERT_LE(a, b) ::c2::test::Tester(__FILE__, __LINE__).IsLe((a), (b))
#define ASSERT_LT(a, b) ::c2::test::Tester(__FILE__, __LINE__).IsLt((a), (b))

#define TCONCAT(a, b) TCONCAT1(a, b)
#define TCONCAT1(a, b) a##b
#define TEST(base, name)                                                      \
  class TCONCAT(_Test_, name) : public base {                                 \
   public:                                                                    \
    void _Run();                                                              \
    static void _RunIt() {                                                    \
      TCONCAT(_Test_, name) t;                                                \
      t._Run();                                                               \
    }                                                                         \
  };                                                                          \
  bool TCONCAT(_Test_ignored_, name) =                                        \
      ::c2::test::RegisterTest(#base, #name, &TCONCAT(_Test_, name)::_RunIt); \
  void TCONCAT(_Test_, name)::_Run()

// Register the specified test. Typically not used directly, but invoked
// via the macro expansion of TEST.
extern bool RegisterTest(const char* base, const char* name, void (*func)());

}  // namespace test
}  // namespace c2
