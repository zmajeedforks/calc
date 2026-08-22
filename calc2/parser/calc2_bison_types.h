#ifndef CALC2_BISON_TYPES_H
#define CALC2_BISON_TYPES_H
// calc2/parser/calc2_bison_types.h

#include <stdint.h>

#include <string>
#include <flat_map>

namespace calc2 {
using namespace std;

using Symtab = flat_map<string, int64_t>;

struct Error {
  string msg;
  uint64_t line;
  uint64_t col;
  string file;
};

struct BisonDriver {
  int64_t expr = -1;
  Symtab symtab;
  Error error;
};

}

#endif
