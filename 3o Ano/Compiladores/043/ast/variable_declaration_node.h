#pragma once

#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/types/basic_type.h>

namespace p6 {

  /**
   * Class for declaring variables.
  */


   // declaração  -> 	[ qualificador ] tipo identificador [ = expressão ] ;
   //  	          ->  [ qualificador ] [ auto ] identificador = expressão ;
   
  class variable_declaration_node : public cdk::typed_node {
    // qualifier is optional
    int _qualifier;
    std::string _base;
    // initializer is also optional
    cdk::expression_node *_initializer;

  public:
    variable_declaration_node(int lineno, int qualifier, std::shared_ptr <cdk::basic_type> varType,
      const std::string &base, cdk::expression_node *initializer) :
      cdk::typed_node(lineno), _qualifier(qualifier), _base(base), _initializer(initializer) {
      type(varType);
    }

    bool constant() {return false;}    

    int qualifier() {return _qualifier;}

    const std::string &base() const {return _base;}
    
    cdk::expression_node *initializer() {return _initializer;}

    void initializer(cdk::expression_node *initializer) {_initializer = initializer;}

    void accept(basic_ast_visitor *sp, int level) override {sp->do_variable_declaration_node(this, level);}

  };

} // p6
