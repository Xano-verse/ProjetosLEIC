#pragma once

#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/types/primitive_type.h>
#include "ast/block_node.h"
#include "ast/function_definition_node.h"

namespace p6 {

  /**
   * Class for describing program nodes.
   */
  class program_node : public p6::function_definition_node {

    public:
      program_node(int lineno, p6::block_node *body) : p6::function_definition_node(
        lineno, 3, cdk::primitive_type::create(4, cdk::TYPE_INT), "_main", new cdk::sequence_node(lineno), body) {
      }
      
      void accept(basic_ast_visitor *sp, int level) { sp->do_program_node(this, level); }
  };

} // p6

