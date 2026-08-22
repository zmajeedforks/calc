// calc2_parser.gtest.cpp

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

#include "lexer/calc2_lexer.h"
#include "calc2_parser.bison.h"

#include <sstream>
#include <string>
#include <queue>

#include <gtest/gtest.h>

using namespace std;
using namespace ::testing;

namespace {
using namespace calc2;

struct Calc2_Bison: Test {

  static int parse(const string& s, BisonDriver& driver) {

    stringstream strm{s};
    BisonParam bisonParam{driver};
    Calc2Lexer lexer(strm);
    LexParam lexParam;

    Calc2Parser parser([&lexer](LexParam& param) -> Calc2Parser::symbol_type {
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

namespace calc2::testing {

TEST_F(Calc2_Bison, test_00) {
  BisonDriver driver;

  ASSERT_EQ(parse("2 + 3", driver), 0);
  EXPECT_EQ(driver.expr, 5);
}

TEST_F(Calc2_Bison, test_01) {
  BisonDriver driver;

  ASSERT_EQ(parse("2 - 7", driver), 0);
  EXPECT_EQ(driver.expr, -5);
}

TEST_F(Calc2_Bison, test_02) {
  BisonDriver driver;

  ASSERT_EQ(parse("2 + - 7", driver), 0);
  EXPECT_EQ(driver.expr, -5);
}

TEST_F(Calc2_Bison, test_03) {
  BisonDriver driver;

  ASSERT_EQ(parse("a = b = c = 10", driver), 0);
  EXPECT_EQ(driver.expr, 10);
  EXPECT_EQ(driver.symtab["a"], 10);
  EXPECT_EQ(driver.symtab["b"], 10);
  EXPECT_EQ(driver.symtab["c"], 10);
}

TEST_F(Calc2_Bison, test_04) {
  string s = R"%(
a = 3; b = 5;
c=7;
x = a + b * c;
)%";
  BisonDriver driver;

  ASSERT_EQ(parse(s, driver), 0);
  EXPECT_EQ(driver.expr, 38);
  EXPECT_EQ(driver.symtab["x"], 38);
  EXPECT_EQ(driver.symtab["a"], 3);
  EXPECT_EQ(driver.symtab["b"], 5);
  EXPECT_EQ(driver.symtab["c"], 7);
}

TEST_F(Calc2_Bison, test_05) {
  string s = R"%(
a = 1; b = 2; c = 9; d = 4; e = 2; f = 3;
((a + b) * (c - d)) / (e + f)
)%";
  BisonDriver driver;

  ASSERT_EQ(parse(s, driver), 0);
  EXPECT_EQ(driver.expr, 3);
}

TEST_F(Calc2_Bison, test_06) {
  string s = R"%(
100 / 10 / 2
)%";
  BisonDriver driver;

  ASSERT_EQ(parse(s, driver), 0);
  EXPECT_EQ(driver.expr, 5);
}

TEST_F(Calc2_Bison, test_07) {
  string s = R"%(
(a) = 5;
)%";
  BisonDriver driver;

  EXPECT_NE(parse(s, driver), 0);
}

TEST_F(Calc2_Bison, test_08) {
  string s = R"%(
a = b * / c
)%";
  BisonDriver driver;

  EXPECT_NE(parse(s, driver), 0);
}

TEST_F(Calc2_Bison, test_09) {
  string s = R"%(
a + b = c
)%";
  BisonDriver driver;

  EXPECT_NE(parse(s, driver), 0);
}

TEST_F(Calc2_Bison, test_10) {
  string s = R"%(
a = 7; b = 3; c = -5;
a + (b = c)
)%";
  BisonDriver driver;

  ASSERT_EQ(parse(s, driver), 0);
  EXPECT_EQ(driver.expr, 2);
  EXPECT_EQ(driver.symtab["b"], -5);
}

TEST_F(Calc2_Bison, test_11) {
  string s = R"%(
a = 7; b = 3; c = -5;
a = -b * -c
)%";
  BisonDriver driver;

  ASSERT_EQ(parse(s, driver), 0);
  EXPECT_EQ(driver.expr, -15);
  EXPECT_EQ(driver.symtab["a"], -15);
}

TEST_F(Calc2_Bison, test_12) {
  string s = R"%(
c = 3;
a = (b = c + 5) * 2
)%";
  BisonDriver driver;

  ASSERT_EQ(parse(s, driver), 0);
  EXPECT_EQ(driver.expr, 16);
  EXPECT_EQ(driver.symtab["a"], 16);
  EXPECT_EQ(driver.symtab["b"], 8);
}

TEST_F(Calc2_Bison, test_13) {
  BisonDriver driver;

  ASSERT_EQ(parse( "(x = 5) * 2", driver), 0);
  EXPECT_EQ(driver.expr, 10);
  EXPECT_EQ(driver.symtab["x"], 5);
}

TEST_F(Calc2_Bison, test_14) {
  BisonDriver driver;

  ASSERT_EQ(parse( "x = 5; (y = 3) * 2", driver), 0);
  EXPECT_EQ(driver.expr, 6);
  EXPECT_EQ(driver.symtab["x"], 5);
  EXPECT_EQ(driver.symtab["y"], 3);
}

TEST_F(Calc2_Bison, test_15) {
  BisonDriver driver;

  EXPECT_NE(parse("(x = 5; y = 3) * 2", driver), 0);
}

}

