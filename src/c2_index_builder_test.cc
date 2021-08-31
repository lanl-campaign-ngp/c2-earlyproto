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
#include "c2_index_builder.h"

#include "testharness.h"

#include <fastbit/part.h>
#include <vector>

namespace c2 {
class IndexBuilderTest {
 public:
  IndexBuilderOptions opts;
  IndexBuilder* bu;
  std::vector<float> inputdata;
  std::string tmpdir;

  IndexBuilderTest() : bu(NULL) {
    ibis::gVerbose = 5;
    tmpdir = test::TmpDir() + "/index_builder_test";
    IO::Default()->CreateDir(tmpdir.c_str());
    opts.ibis_col =
        new ibis::column(new ibis::part(tmpdir.c_str(), NULL, false),
                         ibis::FLOAT, "var0", "test_var");
    bu = new IndexBuilder(opts, NULL);
  }

  ~IndexBuilderTest() {
    delete opts.ibis_col->partition();
    delete opts.ibis_col;
    delete bu;
  }
};

TEST(IndexBuilderTest, Try) {
  inputdata.push_back(1.0);
  inputdata.push_back(1.7);
  inputdata.push_back(1.3);
  bu->TEST_BuildIndexes(inputdata);
  bu->print(std::cerr);
}

}  // namespace c2

int main(int argc, char* argv[]) {
  return ::c2::test::RunAllTests(&argc, &argv);
}
