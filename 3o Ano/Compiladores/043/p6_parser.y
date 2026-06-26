%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#include <cdk/ast/takum3_node.h>
#include <cdk/types/takum3_type.h>
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!

#define EMPTY (new cdk::sequence_node(LINE))

enum qualifier_t {
  QUAL_NONE,
  QUAL_EXTERN,
  QUAL_FORWARD,
  QUAL_PUBLIC,
  QUAL_AUTO
};

%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;          /* integer value */
  cdk::takum3_type::value_type *t3;
  std::string          *s;          /* symbol name or string literal */
  cdk::balanced3_type::value_type *b3;
  cdk::basic_node      *node;       /* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  p6::block_node       *block;
  std::vector<std::shared_ptr<cdk::basic_type>> *types;
  std::shared_ptr<cdk::functional_type> fdtype;
};

%token <b3> tINTEGER
%token <t3> tREAL
%token <s> tIDENTIFIER tSTRING 
%token tIF tELIF tELSE tWHILE tSTOP tNEXT tRETURN tBEGIN tEND tARROW 
%token tPRINT tPRINTLINE tREAD tSIZEOF tNULL
%token tINT_TYPE tREAL_TYPE tSTRING_TYPE tVOID_TYPE
%token tEXTERN_DECL tFORWARD_DECL tPUBLIC_DECL tAUTO_DECL
%token tAND tOR tNOT tNE tLE tGE tEQ 

%nonassoc tIFX
%nonassoc tELSE
%nonassoc tELIF

%right '='
%left tOR
%left tAND
%left tEQ tNE
%left '>' tGE '<' tLE 
%left '+' '-'
%left '*' '/' '%'
%nonassoc tNOT
%nonassoc tUNARY
%nonassoc '[' ']'

%type <node> stmt program fun_decl fun_def var_decl param declaration iffalse
%type <sequence> file stmts exprs declarations params var_decls 
%type <expression> expr
%type <lvalue> lval
%type <type> type
%type <types> types
%type <fdtype> fundecl_type
%type <block> block
%type <s> string

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

file : declarations                               { compiler->ast($$ = $1);}
     | declarations program                       { compiler->ast($$ = new cdk::sequence_node(LINE, $2, $1));}
     | program                                    { compiler->ast($$ = new cdk::sequence_node(LINE, $1));}
     | /* empty */                                { compiler->ast($$ = EMPTY);}
     ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

program : tBEGIN var_decls stmts tEND             { $$ = new p6::program_node(LINE, new p6::block_node(LINE, $2, $3)); }
        | tBEGIN var_decls tEND                   { $$ = new p6::program_node(LINE, new p6::block_node(LINE, $2, EMPTY)); }
        | tBEGIN stmts tEND                       { $$ = new p6::program_node(LINE, new p6::block_node(LINE, EMPTY, $2)); }
        | tBEGIN tEND                             { $$ = new p6::program_node(LINE, new p6::block_node(LINE, EMPTY, EMPTY)); }
        ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

declarations :              declaration           { $$ = new cdk::sequence_node(LINE, $1);     }
             | declarations declaration           { $$ = new cdk::sequence_node(LINE, $2, $1); }
             ;

declaration  : var_decl                           { $$ = $1; }
             | fun_def                            { $$ = $1; }
             | fun_decl                           { $$ = $1; }
             ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

fun_decl : tPUBLIC_DECL fundecl_type tIDENTIFIER ';'                                 { $$ = new p6::function_declaration_node(LINE, QUAL_PUBLIC, $2, *$3); delete $3; }
         | tPUBLIC_DECL tAUTO_DECL tIDENTIFIER ';'                                   { $$ = new p6::function_declaration_node(LINE, QUAL_PUBLIC, nullptr, *$3); delete $3; }
         | fundecl_type tIDENTIFIER ';'                                              { $$ = new p6::function_declaration_node(LINE, QUAL_NONE, $1, *$2); delete $2; }
         | tAUTO_DECL tIDENTIFIER ';'                                                { $$ = new p6::function_declaration_node(LINE, QUAL_AUTO, nullptr, *$2); delete $2; }

         | tFORWARD_DECL fundecl_type tIDENTIFIER ';'                                { $$ = new p6::function_declaration_node(LINE, QUAL_FORWARD, $2, *$3); delete $3; }
         | tEXTERN_DECL fundecl_type tIDENTIFIER ';'                                 { $$ = new p6::function_declaration_node(LINE, QUAL_EXTERN, $2, *$3); delete $3; }
         ;

fun_def  : tPUBLIC_DECL tIDENTIFIER '(' params ')' tARROW type block                 { $$ = new p6::function_definition_node(LINE, QUAL_PUBLIC, $7, *$2, $4, $8); delete $2; }
         | tPUBLIC_DECL tIDENTIFIER '(' ')' tARROW type block                        { $$ = new p6::function_definition_node(LINE, QUAL_PUBLIC, $6, *$2, EMPTY, $7); delete $2; }
         | tIDENTIFIER '(' params ')' tARROW type block                              { $$ = new p6::function_definition_node(LINE, QUAL_NONE, $6, *$1, $3, $7); delete $1; }
         | tIDENTIFIER '(' ')' tARROW type block                                     { $$ = new p6::function_definition_node(LINE, QUAL_NONE, $5, *$1, EMPTY, $6); delete $1; }
          
         | tFORWARD_DECL tIDENTIFIER '(' params ')' tARROW type block                { $$ = new p6::function_definition_node(LINE, QUAL_FORWARD, $7, *$2, $4, $8); delete $2; }
         | tEXTERN_DECL tIDENTIFIER '(' params ')' tARROW type block                 { $$ = new p6::function_definition_node(LINE, QUAL_EXTERN, $7, *$2, $4, $8); delete $2; }
         ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

params    : param                                 { $$ = new cdk::sequence_node(LINE, $1); }
          | params ',' param                      { $$ = new cdk::sequence_node(LINE, $3, $1); }
          ;

param     : type tIDENTIFIER                      { $$ = new p6::variable_declaration_node(LINE, QUAL_NONE, $1, *$2, nullptr); delete $2; }
          ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

var_decls    : var_decls var_decl                 { $$ = new cdk::sequence_node(LINE, $2, $1); }
             | var_decl                           { $$ = new cdk::sequence_node(LINE, $1, nullptr); }
             ;      

var_decl  : type tIDENTIFIER ';'                                                     { $$ = new p6::variable_declaration_node(LINE, QUAL_NONE, $1, *$2, nullptr); delete $2; }
          | type tIDENTIFIER '=' expr ';'                                            { $$ = new p6::variable_declaration_node(LINE, QUAL_NONE, $1, *$2, $4); delete $2; }
          | tPUBLIC_DECL type tIDENTIFIER ';'                                        { $$ = new p6::variable_declaration_node(LINE, QUAL_PUBLIC, $2, *$3, nullptr); delete $3; }
          | tPUBLIC_DECL type tIDENTIFIER '=' expr ';'                               { $$ = new p6::variable_declaration_node(LINE, QUAL_PUBLIC, $2, *$3, $5); delete $3; }
          | tPUBLIC_DECL tIDENTIFIER '=' expr ';'                                    { $$ = new p6::variable_declaration_node(LINE, QUAL_PUBLIC, nullptr, *$2, $4); delete $2; }
          | tPUBLIC_DECL tAUTO_DECL tIDENTIFIER '=' expr ';'                         { $$ = new p6::variable_declaration_node(LINE, QUAL_PUBLIC, nullptr, *$3, $5); delete $3; }

          | tFORWARD_DECL type tIDENTIFIER ';'                                       { $$ = new p6::variable_declaration_node(LINE, QUAL_FORWARD, $2, *$3, nullptr); delete $3; }
          | tEXTERN_DECL type tIDENTIFIER ';'                                        { $$ = new p6::variable_declaration_node(LINE, QUAL_EXTERN, $2, *$3, nullptr); delete $3; }
          | tAUTO_DECL tIDENTIFIER '=' expr ';'                                      { $$ = new p6::variable_declaration_node(LINE, QUAL_AUTO, nullptr, *$2, $4); delete $2; }
          ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

stmts : stmt                                      { $$ = new cdk::sequence_node(LINE, $1); }
      | stmts stmt                                { $$ = new cdk::sequence_node(LINE, $2, $1); }
      ;

stmt : expr ';'                                   { $$ = new p6::evaluation_node(LINE, $1); }
     | exprs tPRINT                               { $$ = new p6::print_node(LINE, $1, false); }
     | exprs tPRINTLINE                           { $$ = new p6::print_node(LINE, $1, true); }
     | tWHILE '(' expr ')' stmt                   { $$ = new p6::while_node(LINE, $3, $5); }
     | tIF '(' expr ')' stmt %prec tIFX           { $$ = new p6::if_node(LINE, $3, $5); }
     | tIF '(' expr ')' stmt iffalse              { $$ = new p6::if_else_node(LINE, $3, $5, $6); }
     | tSTOP tINTEGER ';'                         { $$ = new p6::stop_node(LINE, $2->to_int()); delete $2; }
     | tSTOP ';'                                  { $$ = new p6::stop_node(LINE); }
     | tNEXT tINTEGER ';'                         { $$ = new p6::next_node(LINE, $2->to_int()); delete $2; }
     | tNEXT ';'                                  { $$ = new p6::next_node(LINE); }
     | tRETURN expr ';'                           { $$ = new p6::return_node(LINE, $2); }
     | tRETURN ';'                                { $$ = new p6::return_node(LINE, nullptr); }
     | block                                      { $$ = $1; }
     ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

iffalse : tELSE stmt                              { $$ = $2; }
        | tELIF '(' expr ')' stmt iffalse         { $$ = new p6::if_else_node(LINE, $3, $5, $6); }
        | tELIF '(' expr ')' stmt %prec tIFX      { $$ = new p6::if_node(LINE, $3, $5); }
        ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

// stmts ARE the instructions
block : '{' var_decls stmts'}'                    { $$ = new p6::block_node(LINE, $2, $3); }
      | '{' stmts '}'                             { $$ = new p6::block_node(LINE, EMPTY, $2); }
      ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

exprs     : expr                                  { $$ = new cdk::sequence_node(LINE, $1); }
          | exprs ',' expr                        { $$ = new cdk::sequence_node(LINE, $3, $1); }
          ;

expr : tINTEGER                                   { $$ = new cdk::balanced3_node(LINE, *$1); delete $1; }
     | tREAL                                      { $$ = new cdk::takum3_node(LINE, *$1); delete $1; }
     | string                                     { $$ = new cdk::string_node(LINE, $1);}
     | '-' expr %prec tUNARY                      { $$ = new cdk::unary_minus_node(LINE, $2); }
     | '+' expr %prec tUNARY                      { $$ = new cdk::unary_plus_node(LINE, $2); }
     | expr '+' expr                              { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr                              { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr                              { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr                              { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr                              { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '<' expr                              { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr                              { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr                              { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr                              { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr                              { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr                              { $$ = new cdk::eq_node(LINE, $1, $3); }
     | expr tAND expr                             { $$ = new cdk::and_node(LINE, $1, $3);}
     | expr tOR expr                              { $$ = new cdk::or_node(LINE, $1, $3);}
     | tNOT expr                                  { $$ = new cdk::not_node(LINE, $2);}
     | '(' expr ')'                               { $$ = $2; }
     | lval                                       { $$ = new cdk::rvalue_node(LINE, $1); }
     | lval '=' expr                              { $$ = new cdk::assignment_node(LINE, $1, $3); }
     | tREAD                                      { $$ = new p6::read_node(LINE); }
     | tSIZEOF  '(' expr ')'                      { $$ = new p6::sizeof_node(LINE, $3); }
     | tNULL                                      { $$ = new p6::null_node(LINE); }
     | lval '?' %prec tUNARY                      { $$ = new p6::address_of_node(LINE, $1); }
     | tIDENTIFIER '(' ')'                        { $$ = new p6::function_call_node(LINE, *$1); delete $1; }
     | tIDENTIFIER '(' exprs ')'                  { $$ = new p6::function_call_node(LINE, *$1, $3); delete $1; }
     | '[' expr ']'                               { $$ = new p6::stack_alloc_node(LINE, $2); }
     ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

string : tSTRING                                  { $$ = $1; }
       | string tSTRING                           { *$1 += *$2; delete $2; $$ = $1; }
       ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

lval : tIDENTIFIER                                { $$ = new cdk::variable_node(LINE, *$1); delete $1; }
     | expr '[' expr ']'                          { $$ = new p6::index_node(LINE, $1 , $3); }
     ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

types     : type                                  { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>({ $1 }); }
          | types ',' type                        { $1->push_back($3); $$ = $1; }
          | /* empty */                           { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); }
          ;


type : tINT_TYPE                                  { $$ = cdk::primitive_type::create(8, cdk::TYPE_BALANCED3); }
     | tREAL_TYPE                                 { $$ = cdk::primitive_type::create(16, cdk::TYPE_TAKUM3); }
     | tSTRING_TYPE                               { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
     | tVOID_TYPE                                 { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
     | '[' type ']'                               { $$ = cdk::reference_type::create(4, $2); }
     ;
     
// this exists to tell the parser specifically that we'll never pass anything other than a functional_type into a function_declaration_node
fundecl_type : type '<' types '>'                         { $$ = cdk::functional_type::create(*$3, $1); delete $3;}
             ;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------

%%
