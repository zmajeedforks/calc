// calc1_parser.minimal.gtest.cpp

#include "calc1_parser.minimal.bison.h"

#include <sstream>
#include <string>
#include <vector>
#include <initializer_list>
#include <variant>
#include <span>

#include <gtest/gtest.h>

using namespace std;
using namespace ::testing;

namespace {
using namespace calc1min;
using tk = Calc1MinParser::token;
using Kind = Calc1MinParser::token_kind_type;
using Sym = Calc1MinParser::symbol_type;

// generic token with value
// store token kind type to allow visit lambdas to match on just value type with auto
template<Kind K, class V>
struct Token {
  V v;
  Kind kind = K;
};

// partial specializations on value type of token with value

// string tokens
template<Kind K>
using StrTok = Token<K, string>;

// int64_t tokens
template<Kind K>
using IntTok = Token<K, int64_t>;

// complete specializations for each distinct token kind
using IDENT = StrTok<tk::IDENT>;
using INT =   IntTok<tk::INT>;

// valueless tokens are original enum names and values
inline constexpr auto DIV =         tk::DIV;
inline constexpr auto EQUAL =       tk::EQUAL;
inline constexpr auto LEFT_PAREN =  tk::LEFT_PAREN;
inline constexpr auto MINUS =       tk::MINUS;
inline constexpr auto PLUS =        tk::PLUS;
inline constexpr auto RIGHT_PAREN = tk::RIGHT_PAREN;
inline constexpr auto SEMICOLON =   tk::SEMICOLON;
inline constexpr auto TIMES =       tk::TIMES;

// add each distinct type of token with value to variant
// valueless tokens are all covered by Kind
struct Tok: variant<Kind, IDENT, INT> {
  using variant::variant;

  Sym toSym(const location& loc) const {

    return visit(overload{

// valueless tokens
      [&](Kind k) -> Sym {
        return Sym(k, loc);
      },

// matches all other token types with values
      [&](const auto& t) -> Sym {
        return Sym(t.kind, t.v, loc);
      },

#if __cpp_lib_variant >= 202306L
    });
#else
    }, *this);
#endif

  }

private:

#if WIN32
  template<class... Ts> struct __declspec(empty_bases) overload: Ts... { using Ts::operator()...; };
#else
  template<class... Ts> struct overload: Ts... { using Ts::operator()...; };
#endif
  template<class... Ts> overload(Ts...) -> overload<Ts...>;
};


struct Calc1Min_BisonNoFlex: Test {

  static int parse(span<const Tok> toks) {

    LexParam lexParam;

    size_t i = 0;
    Calc1MinParser parser([&toks, &i](LexParam& param) -> Sym {
      auto& loc = param.loc;
      loc.step();
      loc.columns();

      if(i < toks.size()) {
        return toks[i++].toSym(loc);
      }
      return Calc1MinParser::make_YYEOF(loc);
    },
    lexParam);

    return parser();
  }

  static int parse(initializer_list<Tok> toks) {
    return parse(span<const Tok>{toks.begin(), toks.size()});
  }

};

}

namespace calc1min::testing {

TEST_F(Calc1Min_BisonNoFlex, test_01) {
// input: 3
  EXPECT_EQ(parse({ INT{3} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_02) {
// input: 3 + 5 * 7
  EXPECT_EQ(parse({ INT{3}, PLUS, INT{5}, TIMES, INT{7} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_03) {
// input: 3 - 5
  EXPECT_EQ(parse({ INT{3}, MINUS, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_04) {
// input: 3 - - 5
  EXPECT_EQ(parse({ INT{3}, MINUS, MINUS, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_05) {
// input: 3 - - - 5
  EXPECT_EQ(parse({ INT{3}, MINUS, MINUS, MINUS, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_06) {
// input: 3 - - + - 5
  EXPECT_EQ(parse({ INT{3}, MINUS, MINUS, PLUS, MINUS, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_07) {
// input: 3 - x * 7
  EXPECT_EQ(parse({ INT{3}, MINUS, IDENT{"x"}, TIMES, INT{7} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_08) {
// input: x = 7
  EXPECT_EQ(parse({ IDENT{"x"}, EQUAL, INT{7} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_09) {
// input: x =3 + 5 
  EXPECT_EQ(parse({ IDENT{"x"}, EQUAL, INT{3}, PLUS, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_10) {
// input: x = y = 7
  EXPECT_EQ(parse({ IDENT{"x"}, EQUAL, IDENT{"y"}, EQUAL, INT{7} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_11) {
// input: x = 7; y = x + 5
  EXPECT_EQ(parse({ IDENT{"x"}, EQUAL, INT{7}, SEMICOLON, IDENT{"y"}, EQUAL, IDENT{"x"}, PLUS, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_12) {
// input: (x = (3 + 5) * 2) + 9
  EXPECT_EQ(parse({ LEFT_PAREN, IDENT{"x"}, EQUAL, LEFT_PAREN, INT{3}, PLUS, INT{5}, RIGHT_PAREN, TIMES, INT{2}, RIGHT_PAREN, PLUS, INT{9} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_13) {
// input: 3 5
  EXPECT_NE(parse({ INT{3}, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_14) {
// input: 3 +
  EXPECT_NE(parse({ INT{3}, PLUS }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_15) {
// input: 3 + / 5
  EXPECT_NE(parse({ INT{3}, PLUS, DIV, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_16) {
// input: * 5
  EXPECT_NE(parse({ TIMES, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_17) {
// input: (5
  EXPECT_NE(parse({ LEFT_PAREN, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_18) {
// input: 5)
  EXPECT_NE(parse({ INT{5}, RIGHT_PAREN }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_19) {
// input: 3 = 5
  EXPECT_NE(parse({ INT{3}, EQUAL, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_20) {
// input: (x) = 5
  EXPECT_NE(parse({ LEFT_PAREN, IDENT{"x"}, RIGHT_PAREN, EQUAL, INT{5} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_21) {
// input: a = b * / c
  EXPECT_NE(parse({ IDENT{"a"}, EQUAL, IDENT{"b"}, TIMES, DIV, IDENT{"c"} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_22) {
// input: a + b = c
  EXPECT_NE(parse({ IDENT{"a"}, PLUS, IDENT{"b"}, EQUAL, IDENT{"c"} }), 0);
}

TEST_F(Calc1Min_BisonNoFlex, test_23) {
// input: (x = 5; y = 3) * 2
  vector<Tok> input = { LEFT_PAREN, IDENT{"x"}, EQUAL, INT{5}, SEMICOLON, IDENT{"y"}, EQUAL, INT{3}, RIGHT_PAREN, TIMES, INT{2} };

  EXPECT_NE(parse(input), 0);
}



}

