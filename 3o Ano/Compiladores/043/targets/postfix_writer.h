#pragma once

#include "targets/basic_ast_visitor.h"

#include <sstream>
#include <cdk/emitters/basic_postfix_emitter.h>
#include <set>
#include <string>

namespace p6 {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    cdk::symbol_table<p6::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;

    std::set<std::string> _functions_to_declare;

    std::vector<int> _whileIni, _whileEnd; // for stop/next (needs to be vector to keep track of several iterations)
    bool _inFunction;           // are we inside a function body
    bool _inMain;
    int _currentBodyRetLabel;   // label to jump to on return
    int _localsize;             // bytes needed for local variables
    std::shared_ptr<p6::symbol> _current_function; // current function symbol

  private:
      std::vector<int> _loop_cond_labels;
      std::vector<int> _loop_end_labels;
      void call_balanced3_unary(cdk::expression_node * const arg, const std::string &fname, int lvl);

      void call_balanced3_binary(cdk::expression_node * const left, cdk::expression_node * const right, const std::string &fname, int lvl);

      void call_takum3_binary(cdk::expression_node * const left, cdk::expression_node * const right, const std::string &fname, int lvl);

      void push_as_takum3(cdk::expression_node * const expr, int lvl);

      void push_as_double_from_numeric(cdk::expression_node * const expr, int lvl);
      void bool_to_balanced3();

  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<p6::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0),
        _inFunction(false), _currentBodyRetLabel(0), _localsize(0), _current_function(nullptr) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return oss.str();
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // p6

