#pragma once

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include <ast/block_node.h>

namespace p6 {

    /**
   * Class for function definitions.
   */
  class function_definition_node : public cdk::typed_node {
    int _qualifier;
    std::string _base;
    cdk::sequence_node *_arguments;
    p6::block_node *_body;

  public:
    function_definition_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> ret_type,
      const std::string &base, cdk::sequence_node *arguments, p6::block_node *body) :
        cdk::typed_node(lineno), _qualifier(qualifier), _base(base), _arguments(arguments), _body(body) {

      std::vector<std::shared_ptr<cdk::basic_type>> inputs;

      for (size_t i = 0; i < _arguments->size(); i++)
        inputs.push_back(dynamic_cast<cdk::typed_node *>(_arguments->node(i))->type());
      type(cdk::functional_type::create(inputs, ret_type));
    }

    int qualifier() { return _qualifier; }
    const std::string &base() const { return _base; }

    cdk::sequence_node *arguments() { return _arguments; }
    cdk::typed_node *argument(size_t ax) {
      return dynamic_cast<cdk::typed_node *>(_arguments->node(ax));
    }
    p6::block_node *body() { return _body; }

    void accept(basic_ast_visitor *sp, int level) override { sp->do_function_definition_node(this, level); }
  };

} // p6