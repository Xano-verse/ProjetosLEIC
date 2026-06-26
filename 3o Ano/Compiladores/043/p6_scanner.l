%option c++ prefix="p6_scanner_" outfile="p6_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <climits>
#include <cerrno>
#include <cstdlib>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include <cmath>
#include <cdk/types/balanced3_type.h>
#include <cdk/types/takum3_type.h>
#include "p6_parser.tab.h"

// don't change this
#define yyerror LexerError

enum qualifier_t {
  QUAL_NONE,
  QUAL_EXTERN,
  QUAL_FORWARD,
  QUAL_PUBLIC,
  QUAL_AUTO
};

int comment_depth = 0;
long long balanced3_intlit = 0;
int balanced3_digits = 0;
bool balanced3_overflow = false;
static constexpr long long B3_MAX = 6078832729528464400LL;

static void add_balanced3_digit(int digit) {
  if (balanced3_intlit > (B3_MAX - digit) / 3 ||
      balanced3_intlit < (-B3_MAX - digit) / 3) {
    balanced3_overflow = true;
    return;
  }

  balanced3_intlit = balanced3_intlit * 3 + digit;
  balanced3_digits++;
}

static cdk::balanced3_type::value_type *balanced3_from_decimal(const char *text, bool *overflow){
  errno = 0;
  char *end = nullptr;

  long long value = strtoll(text, &end, 10);

  *overflow =
      (errno == ERANGE) ||
      (*end != '\0') ||
      (value < 0) ||
      (value > B3_MAX);

  return new cdk::balanced3_type::value_type(*overflow ? 0 : value);
}

static cdk::balanced3_type::value_type *balanced3_from_base9(const char *text, bool *overflow){
  long long value = 0;
  *overflow = false;

  for (int i = 0; text[i] != '\0'; i++) {
    int digit = text[i] - '0';

    if (digit < 0 || digit > 8) {
      *overflow = true;
      return new cdk::balanced3_type::value_type(0);
    }

    if (value > (B3_MAX - digit) / 9) {
      *overflow = true;
      return new cdk::balanced3_type::value_type(0);
    }

    value = value * 9 + digit;
  }

  return new cdk::balanced3_type::value_type(value);
}

%}
%x X_STRING
%x X_STRING_SKIP
%x X_INTLIT_BALANCED3

%x X_COMMENT

%%
  yydebug=0; set_debug(1);

"//".*                 ; /* ignore comments */

">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE;
"!!"                   return tPRINTLINE;
"!"                    return tPRINT;
"&&"                   return tAND;
"||"                   return tOR;
"~"                    return tNOT;

"->"                   return tARROW;

"while"                return tWHILE; 
"if"                   return tIF;
"elif"                 return tELIF; 
"else"                 return tELSE; 
"input"                return tREAD; 
"begin"                return tBEGIN; 
"end"                  return tEND;
"sizeof"               return tSIZEOF;
"null"                 return tNULL;
"stop"                 return tSTOP;
"next"                 return tNEXT;
"return"               return tRETURN;

"int"                return tINT_TYPE;
"real"               return tREAL_TYPE;
"string"             return tSTRING_TYPE;
"void"               return tVOID_TYPE;

"extern"             return tEXTERN_DECL;
"forward"            return tFORWARD_DECL;
"public"             return tPUBLIC_DECL;
"auto"               return tAUTO_DECL;


"0"[0-8]+                         {
                                    bool overflow;
                                    yylval.b3 = balanced3_from_base9(yytext, &overflow);
                                    if (overflow) yyerror("integer literal overflow");
                                    return tINTEGER;
                                  }

"0"[0-9]+                         {
                                    yylval.b3 = new cdk::balanced3_type::value_type(0);
                                    yyerror("invalid base9 integer literal");
                                    return tINTEGER;
                                  }

[0-9]+                            {
                                    bool overflow;
                                    yylval.b3 = balanced3_from_decimal(yytext, &overflow);
                                    if (overflow) yyerror("integer literal overflow");
                                    return tINTEGER;
                                  }

([0-9]+\.[0-9]*|\.[0-9]+)([Ee][+-]?[0-9]+)?  {
                                                yylval.t3 = new cdk::takum3_type::value_type(yytext);
                                                return tREAL;
                                              }


[0-9]+[Ee][+-]?[0-9]+                        {
                                                yylval.t3 = new cdk::takum3_type::value_type(yytext);
                                                return tREAL;
                                              }

"0t"                              { yy_push_state(X_INTLIT_BALANCED3); balanced3_intlit = 0; balanced3_digits = 0; balanced3_overflow = false; }
<X_INTLIT_BALANCED3>"-"          { add_balanced3_digit(-1); }
<X_INTLIT_BALANCED3>"0"          { add_balanced3_digit(0); }
<X_INTLIT_BALANCED3>"+"          { add_balanced3_digit(1); }
<X_INTLIT_BALANCED3>.            {
                                    if (balanced3_digits == 0) {
                                      yyerror("invalid balanced base3 literal");
                                    }
                                    if (balanced3_overflow) {
                                      yyerror("integer overflow in balanced base3 literal");
                                    }
                                    yylval.b3 = new cdk::balanced3_type::value_type(balanced3_intlit);
                                    yy_pop_state();
                                    yyless(0);            // puts back the character that was read in the <X_INTLIT_BALANCEDB3>.
                                    return tINTEGER;
                                  }



[A-Za-z][A-Za-z0-9]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

\"                       yy_push_state(X_STRING); yylval.s = new std::string("");

<X_STRING>\"             yy_pop_state(); return tSTRING;
<X_STRING>\\0            yy_push_state(X_STRING_SKIP);
<X_STRING>\\t            *yylval.s += '\t';
<X_STRING>\\n            *yylval.s += '\n';
<X_STRING>\\r            *yylval.s += '\r';
<X_STRING>\\\"           *yylval.s += '"';
<X_STRING>\\\\           *yylval.s += '\\';
<X_STRING>\\[0-7]{1,3}   *yylval.s += (char)strtol(yytext + 1, nullptr, 8);
<X_STRING>\n             yyerror("newline in string");
<X_STRING>.              *yylval.s += yytext[0];

<X_STRING_SKIP>\"        yy_pop_state(); yy_pop_state(); return tSTRING;
<X_STRING_SKIP>\n        yyerror("newline in string");
<X_STRING_SKIP>.         ;


"/*"                    { comment_depth = 1; yy_push_state(X_COMMENT); }

<X_COMMENT>"/*"         { comment_depth++; }
<X_COMMENT>"*/"         { if (--comment_depth == 0) yy_pop_state(); }
<X_COMMENT>.|\n         ; /* ignore content */

[-()<>=+*/%;{}\[\],'?]      return *yytext;

[ \t\n]+               ; /* ignore whitespace */

.                      yyerror("Unknown character");

%%
