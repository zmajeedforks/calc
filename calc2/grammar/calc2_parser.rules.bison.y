// calc2_parser.rules.bison.y

// calc grammar without clutter of semantic actions
// bison --color=always -Wall -Wdangling-alias -Werror -Wcounterexamples --report counterexamples,lookaheads --report-file bisonreport.txt calc2_parser.rules.bison.y

%{
// %{ unnamed codeblock
// goes at top of .cpp file after %code top, before namespace and parser class

%}

%token DIV                  "/"
%token EQUAL                "="
%token LEFT_PAREN           "("
%token MINUS                "-"
%token PLUS                 "+"
%token RIGHT_PAREN          ")"
%token SEMICOLON            ";"
%token TIMES                "*"

%token IDENT                "ident"
%token INT                  "int"

%start expr

%%

expr: assign_exprs | assign_exprs ";"

assign_exprs: assign_expr | assign_exprs ";" assign_expr

assign_expr: IDENT "=" assign_expr | add_expr

add_expr: term | add_expr "+" term | add_expr "-" term

term: unary | term "*" unary | term "/" unary

unary: atom | "+" unary | "-" unary

atom: INT | IDENT | "(" assign_expr ")"



