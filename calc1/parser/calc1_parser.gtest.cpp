// calc1_parser.gtest.cpp

/*
MIT License

Copyright (c) 2024-2026 Zartaj Majeed

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "lexer/calc1_lexer.h"
#include "calc1_parser.bison.h"

#include <sstream>
#include <string>
#include <queue>

#include <gtest/gtest.h>

using namespace std;
using namespace ::testing;

namespace {
using namespace calc1;

struct Calc1_Bison: Test {

  static int parse(const string& s, BisonParam& bisonParam) {

    stringstream strm{s};
    Calc1Lexer lexer(strm);
    LexParam lexParam;

    Calc1Parser parser([&lexer](LexParam& param) -> Calc1Parser::symbol_type {
      auto& loc = param.loc;
      loc.step();
      loc.columns();

      return lexer.yylex(param);
    },
    bisonParam,
    lexParam);

    return parser();
  }

};

}

namespace calc1::testing {

TEST_F(Calc1_Bison, test_00) {
  BisonParam bisonParam;

  ASSERT_EQ(parse("2 + 3", bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, 5);
}

TEST_F(Calc1_Bison, test_01) {
  BisonParam bisonParam;

  ASSERT_EQ(parse("2 - 7", bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, -5);
}

TEST_F(Calc1_Bison, test_02) {
  BisonParam bisonParam;

  ASSERT_EQ(parse("2 + - 7", bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, -5);
}

TEST_F(Calc1_Bison, test_03) {
  BisonParam bisonParam;

  ASSERT_EQ(parse("a = b = c = 10", bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, 10);
  EXPECT_EQ(bisonParam.symtab["a"], 10);
  EXPECT_EQ(bisonParam.symtab["b"], 10);
  EXPECT_EQ(bisonParam.symtab["c"], 10);
}

TEST_F(Calc1_Bison, test_04) {
  string s = R"%(
a = 3; b = 5;
c=7;
x = a + b * c;
)%";
  BisonParam bisonParam;

  ASSERT_EQ(parse(s, bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, 38);
  EXPECT_EQ(bisonParam.symtab["x"], 38);
  EXPECT_EQ(bisonParam.symtab["a"], 3);
  EXPECT_EQ(bisonParam.symtab["b"], 5);
  EXPECT_EQ(bisonParam.symtab["c"], 7);
}

TEST_F(Calc1_Bison, test_05) {
  string s = R"%(
a = 1; b = 2; c = 9; d = 4; e = 2; f = 3;
((a + b) * (c - d)) / (e + f)
)%";
  BisonParam bisonParam;

  ASSERT_EQ(parse(s, bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, 3);
}

TEST_F(Calc1_Bison, test_06) {
  string s = R"%(
100 / 10 / 2
)%";
  BisonParam bisonParam;

  ASSERT_EQ(parse(s, bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, 5);
}

TEST_F(Calc1_Bison, test_07) {
  string s = R"%(
(a) = 5;
)%";
  BisonParam bisonParam;

  EXPECT_NE(parse(s, bisonParam), 0);
}

TEST_F(Calc1_Bison, test_08) {
  string s = R"%(
a = b * / c
)%";
  BisonParam bisonParam;

  EXPECT_NE(parse(s, bisonParam), 0);
}

TEST_F(Calc1_Bison, test_09) {
  string s = R"%(
a + b = c
)%";
  BisonParam bisonParam;

  EXPECT_NE(parse(s, bisonParam), 0);
}

TEST_F(Calc1_Bison, test_10) {
  string s = R"%(
a = 7; b = 3; c = -5;
a + (b = c)
)%";
  BisonParam bisonParam;

  ASSERT_EQ(parse(s, bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, 2);
  EXPECT_EQ(bisonParam.symtab["b"], -5);
}

TEST_F(Calc1_Bison, test_11) {
  string s = R"%(
a = 7; b = 3; c = -5;
a = -b * -c
)%";
  BisonParam bisonParam;

  ASSERT_EQ(parse(s, bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, -15);
  EXPECT_EQ(bisonParam.symtab["a"], -15);
}

TEST_F(Calc1_Bison, test_12) {
  string s = R"%(
c = 3;
a = (b = c + 5) * 2
)%";
  BisonParam bisonParam;

  ASSERT_EQ(parse(s, bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, 16);
  EXPECT_EQ(bisonParam.symtab["a"], 16);
  EXPECT_EQ(bisonParam.symtab["b"], 8);
}

TEST_F(Calc1_Bison, test_13) {
  BisonParam bisonParam;

  ASSERT_EQ(parse( "(x = 5) * 2", bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, 10);
  EXPECT_EQ(bisonParam.symtab["x"], 5);
}

TEST_F(Calc1_Bison, test_14) {
  BisonParam bisonParam;

  ASSERT_EQ(parse( "x = 5; (y = 3) * 2", bisonParam), 0);
  EXPECT_EQ(bisonParam.expr, 6);
  EXPECT_EQ(bisonParam.symtab["x"], 5);
  EXPECT_EQ(bisonParam.symtab["y"], 3);
}

TEST_F(Calc1_Bison, test_15) {
  BisonParam bisonParam;

  EXPECT_NE(parse("(x = 5; y = 3) * 2", bisonParam), 0);
}

}

