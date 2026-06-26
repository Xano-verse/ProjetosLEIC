#pragma once

#include <cdk/ast/sequence_node.h>

namespace p6 {

    /**
   * Class for calling functions.
   */

    class function_call_node : public cdk::expression_node {
        std::string _base;
        cdk::sequence_node *_arguments;

    public:
        function_call_node(int lineno, const std::string &base) :
            cdk::expression_node(lineno), _base(base), _arguments(new cdk::sequence_node(lineno)) {
        }

        function_call_node(int lineno, const std::string &base, cdk::sequence_node *arguments) :
            cdk::expression_node(lineno), _base(base), _arguments(arguments) {
        }

        const std::string &base() { return _base; }
        cdk::sequence_node *arguments() { return _arguments; }
        cdk::expression_node *argument(size_t ix) {
        return dynamic_cast<cdk::expression_node *>(_arguments->node(ix));
        }

        void accept(basic_ast_visitor *sp, int level) override { sp->do_function_call_node(this, level); }
    };

} // p6