#pragma once

#include <cdk/ast/expression_node.h>

namespace p6 {

    /**
   * Class for return intruction (return [ expressão ]).
   */
    class return_node : public cdk::basic_node {
        cdk::expression_node *_expression;

    public:
        return_node(int lineno, cdk::expression_node *expression) :
            cdk::basic_node(lineno), _expression(expression) {
        }

        cdk::expression_node *expression() { return _expression; }

        void accept(basic_ast_visitor *sp, int level) override { sp->do_return_node(this, level); }
    };

} // p6