#ifndef CALC2_API_H
#define CALC2_API_H
// calc2_api.h

#include <stdint.h>

#include <string>
#include <expected>
#include <chrono>
#include <ranges>
#include <flat_map>
#include <istream>
#include <fstream>
#include <sstream>
#include <iostream>

#include "lexer/calc2_lexer.h"
#include "calc2_parser.bison.h"

namespace calc2 {
using namespace std;
using namespace chrono;

struct ParseConfig {
  bool debug = false;
  string filename{};
};

struct Stats {
  duration<double> parseSec;
  duration<double> lexSec;
  time_point<steady_clock> parseStart;
  time_point<steady_clock> parseEnd;
};

using Symtab = flat_map<string, int64_t>;

struct Calc2 {

  int64_t exprResult;
  int errCode;
  Error errInfo;
  Symtab symtab;
  Stats stats{};

  static Calc2 parse(istream& in, const ParseConfig& config) {

    string filename = config.filename;
    if(filename.empty()) {
      filename = "stream";
    }
    auto inputName = make_unique<string>(filename);

    Calc2Lexer lexer{in};

    BisonDriver driver;
    BisonParam bisonParam{driver};
    LexParam lexParam{.loc = location(inputName.get())};

    duration<double> lexSec{};

    Calc2Parser parser(
      [&lexer, &lexSec](LexParam& lexParam) -> Calc2Parser::symbol_type {
        time_point<steady_clock> start = steady_clock::now();
        auto token = lexer.yylex(lexParam);
        time_point<steady_clock> end = steady_clock::now();
        lexSec += end - start;
        return token;
      },
      bisonParam,
      lexParam
    );

    lexer.set_debug(config.debug);
    parser.set_debug_level(config.debug);

    time_point<steady_clock> parseStart = steady_clock::now();
    int ev = parser();
    time_point<steady_clock> parseEnd = steady_clock::now();

    return Calc2{
      .exprResult = driver.expr,
      .errCode = ev,
      .errInfo = move(driver.error),
      .symtab = move(driver.symtab),
      .stats = {
        parseEnd - parseStart,
        lexSec,
        parseStart,
        parseEnd,
      }
    };
  }

  static Calc2 parse(istream& in = cin, const ParseConfig&& config = { .filename = "stdin" }) {
    return parse(in, config);
  }

  static Calc2 parseString(const string& str, const ParseConfig&& config = { .filename = "string" }) {
    istringstream in{str};
    return parse(in, config);
  }

  static Calc2 parseFile(const string& file, ParseConfig&& config = {}) {
    if(config.filename.empty()) {
      config.filename = file;
    }

    if(file == "-") {
      return parse(cin, config);
    }
    ifstream filestrm;
    if(filestrm.open(file); !filestrm) {
      return Calc2{
        .exprResult = 0,
        .errCode = 1,
        .errInfo = {
          "cannot open file",
          0,
          0,
          file,
        },
        .symtab = {},
        .stats = {}
      };
    }
    return parse(filestrm, config);
  }

  expected<int64_t, int> eval() {
    if(errCode != 0) {
      return unexpected{errCode};
    }
    return exprResult;
  }

  bool hasError() {
    return errCode != 0;
  }

  int errorCode() {
    return errCode;
  }

  Error errorInfo() {
    return errInfo;
  }

  string errorStr() {
    const auto& [msg, line, col, file] = errInfo;
    return format("{}:{}.{}: {}", file, line, col, msg);
  }

};



}

#endif
