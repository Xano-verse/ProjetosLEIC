#pragma once

#include <cdk/ast/expression_node.h>

namespace p6 {

    /**
   * Class for read intruction.
   */
    class read_node : public cdk::expression_node {
    public:
        read_node(int lineno) : cdk::expression_node(lineno) {}

        void accept(basic_ast_visitor *sp, int level) override { sp->do_read_node(this, level); }
    };

} // p6
