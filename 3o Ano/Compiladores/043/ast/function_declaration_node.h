#pragma once

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include <ast/block_node.h>

namespace p6 {

    /**
   * Class for function declarations.
   */
  class function_declaration_node : public cdk::typed_node {
    int _qualifier;

    // node->type() gives us the function type as a basic_type and node->fun_type() gives us the function type as a functional_type (they both include the arguments' types too)
    // we keep _fun_type as an attribute because we might need to use functional_type specific methods across multiple visitors
    // (like the output() method to get the output type of the function specifically)
    // so by calling node->fun_type() we don't need to constantly do dynamic pointer casts, we can just access its methods directly
    std::shared_ptr<cdk::functional_type> _fun_type;
    
    std::string _base;

  public:
    // arguments are included in ret_type
    function_declaration_node(int lineno, int qualifier, std::shared_ptr<cdk::functional_type> fun_type,
      const std::string &base) :
        cdk::typed_node(lineno), _qualifier(qualifier), _fun_type(fun_type), _base(base)
    {
          type(fun_type);
    }

    int qualifier() { return _qualifier; }
    std::shared_ptr<cdk::functional_type> fun_type() { return _fun_type; };
    const std::string &base() const { return _base; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_function_declaration_node(this, level); }
  };

} // p6
