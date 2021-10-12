/*
 * Copyright (c) 2021 Triad National Security, LLC, as operator of Los Alamos
 * National Laboratory with the U.S. Department of Energy/National Nuclear
 * Security Administration. All Rights Reserved.
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
 *
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
#include "c2_env.h"
#include "c2_index_builder.h"
#include "c2_random.h"

#include <fastbit/part.h>
#include <stdio.h>
#include <vector>

static int FLAGS_n = 10000000;  // Number of keys

static int FLAGS_value_min = 100;

static int FLAGS_value_max = 1000;

namespace c2 {

class IndexBench {
 public:
  IndexBuilderOptions opts;
  IndexBuilder* bu;
  std::string tmpdir;

  IndexBench() : bu(NULL) {
    ibis::gVerbose = 5;
    IO::Default()->GetTestDirectory(&tmpdir);
    tmpdir += "/index_builder_test";
    IO::Default()->CreateDir(tmpdir.c_str());
    opts.ibis_col =
        new ibis::column(new ibis::part(tmpdir.c_str(), NULL, false),
                         ibis::FLOAT, "var0", "test_var");
    bu = new IndexBuilder(opts, NULL);
  }

  ~IndexBench() {
    delete opts.ibis_col->partition();
    delete opts.ibis_col;
    delete bu;
  }

  void Build(const std::vector<float>& inputdata) {
    uint64_t begin = env::CurrentMicros();
    bu->TEST_BuildIndexes(inputdata);
    bu->print(std::cerr);
    uint64_t d = env::CurrentMicros() - begin;
    fprintf(stderr, "== Index built in %.3f s\n", double(d) / 1000.0 / 1000.0);
  }

  void LessThan(float a) {
    ibis::bitvector results;
    ibis::qContinuousRange r("var0", ibis::qExpr::OP_LT, a);
    uint64_t begin = env::CurrentMicros();
    bu->evaluate(r, results);
    uint64_t d = env::CurrentMicros() - begin;
    fprintf(stderr, "== Query evaluated in %.3f s\n",
            double(d) / 1000.0 / 1000.0);
  }
};

void BM_Main(int* const argc, char*** const argv) {
  c2::IndexBench b;
  c2::Random r(301);
  std::vector<float> inputdata;
  for (int i = 1; i < *argc; i++) {
    int a;
    char junk;
    if (sscanf((*argv)[i], "--n=%d%c", &a, &junk) == 1 ||
        sscanf((*argv)[i], "-n=%d%c", &a, &junk) == 1) {
      FLAGS_n = a;
    }
  }
  inputdata.reserve(FLAGS_n);
  for (int i = 0; i < FLAGS_n; i++) {
    inputdata.push_back(
        float(FLAGS_value_min +
            double(FLAGS_value_max - FLAGS_value_min) * r.Next() / INT32_MAX));
  }
  b.Build(inputdata);
  for (int i = 1; i < 10; i++) {
    b.LessThan(float(FLAGS_value_min +
                     double(FLAGS_value_max - FLAGS_value_min) * i / 10));
  }
}

}  // namespace c2

int main(int argc, char* argv[]) {
  c2::BM_Main(&argc, &argv);
  return 0;
}
