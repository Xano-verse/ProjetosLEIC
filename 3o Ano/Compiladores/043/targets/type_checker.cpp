#include <string>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated
#include <cdk/types/primitive_type.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

enum qualifier_t {
  QUAL_NONE,
  QUAL_EXTERN,
  QUAL_FORWARD,
  QUAL_PUBLIC,
  QUAL_AUTO
};

// Help functions

bool p6::type_checker::compatible(std::shared_ptr<cdk::basic_type> left, std::shared_ptr<cdk::basic_type> right) {
  if ((left->name() == cdk::TYPE_TAKUM3 && right->name() == cdk::TYPE_BALANCED3)) {
    return true;
  }

  // pointer arithmetic -> conversao de balanced3 para int terá que acontecer, será ao nivel de postfix
  // "Note-se que a aritmética de ponteiros é possível apenas com inteiros binários, mas não com inteiros ternários (estes devem ser convertidos para binário, caso seja necessário). "
  if (left->name() == cdk::TYPE_POINTER && right->name() == cdk::TYPE_BALANCED3) return true;
  if (left->name() == cdk::TYPE_BALANCED3 && right->name() == cdk::TYPE_POINTER) return true;
  
  if (left->name() == cdk::TYPE_POINTER && right->name() == cdk::TYPE_POINTER) {
    auto lp = cdk::reference_type::cast(left);
    auto rp = cdk::reference_type::cast(right);
    // null pointers are compatible with every type of pointer
    if (lp->referenced() == nullptr || rp->referenced() == nullptr) {
      return true;

    // pointeiro nulo != ponteiro genérico
    // pointeiro nulo é null, ponteiro genérico é algo do tipo void* p

    // "O tipo void apenas pode ser usado para indicar a ausência de retorno ou para declarar um ponteiro genérico."
    // "Um ponteiro deste tipo é compatível com todos os outros tipos de ponteiros"
    } else if (lp->referenced()->name() == cdk::TYPE_VOID || rp->referenced()->name() == cdk::TYPE_VOID) {
      return true;

    // two pointer types are compatible if the types they point to are compatible (we must allow conversion here because basic assignment uses pointers for example)
    } else if (compatible(lp->referenced(), rp->referenced())) {
      return true;
    } else {
      return false;
    }
  }
  
  if (left->name() == right->name()) return true;

  return false;
}


std::shared_ptr<cdk::basic_type> p6::type_checker::getCompatibleType(cdk::typed_node *left, cdk::typed_node *right) {
  // "Podem ser atribuídos valores inteiros a left-values reais (conversão automática). Nos outros casos, ambos os tipos têm de concordar"
  if ((left->is_typed(cdk::TYPE_TAKUM3) && right->is_typed(cdk::TYPE_BALANCED3))) {
    return cdk::primitive_type::create(16, cdk::TYPE_TAKUM3);

  } else if (left->is_typed(cdk::TYPE_TAKUM3) && right->is_typed(cdk::TYPE_TAKUM3)) {
    return cdk::primitive_type::create(16, cdk::TYPE_TAKUM3);

  } else if (left->is_typed(cdk::TYPE_BALANCED3) && right->is_typed(cdk::TYPE_BALANCED3)) {
    return cdk::primitive_type::create(8, cdk::TYPE_BALANCED3);

  } else if (left->is_typed(cdk::TYPE_STRING) && right->is_typed(cdk::TYPE_STRING)) {
    return cdk::primitive_type::create(4, cdk::TYPE_STRING);

  // pointer arithmetic -> conversao de balanced3 para int terá que acontecer, será ao nivel de postfix
  // "Note-se que a aritmética de ponteiros é possível apenas com inteiros binários, mas não com inteiros ternários (estes devem ser convertidos para binário, caso seja necessário). "
  } else if (left->is_typed(cdk::TYPE_POINTER) && right->is_typed(cdk::TYPE_BALANCED3)) {
    auto lp = cdk::reference_type::cast(left->type());
    return cdk::reference_type::create(4, lp->referenced());

  } else if (left->is_typed(cdk::TYPE_BALANCED3) && right->is_typed(cdk::TYPE_POINTER)) {
    auto rp = cdk::reference_type::cast(right->type());
    return cdk::reference_type::create(4, rp->referenced());

  // must check if pointers are compatible
  } else if (left->is_typed(cdk::TYPE_POINTER) && right->is_typed(cdk::TYPE_POINTER)) {
    auto lp = cdk::reference_type::cast(left->type());
    auto rp = cdk::reference_type::cast(right->type());

    // "O literal null é compatível com todos os tipos de ponteiros."
    // We set the type to the remaining side of the expression
    if (lp->referenced() == nullptr && rp->referenced() == nullptr) {
      return cdk::reference_type::create(4, nullptr);

    } else if (lp->referenced() == nullptr) {
      return cdk::reference_type::create(4, rp->referenced());

    } else if (rp->referenced() == nullptr) {
      return cdk::reference_type::create(4, lp->referenced());

    // two pointer types are compatible if the types they point to are compatible (we must allow conversion here because basic assignment uses pointers for example)
    } else if (compatible(lp->referenced(), rp->referenced())) {
      return cdk::reference_type::create(4, lp->referenced());          // must pass in lp in case there's a conversion from int to real, which only happens when left value is real

    } else {
      throw std::string("referenced types of left and right pointers of binary expression or assignment are not compatible");
    }

  } else {
    throw std::string("types of left and right arguments of binary expression or assignment do not match");
  }

}

//---------------------------------------------------------------------------

void p6::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void p6::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}
void p6::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}
void p6::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  // EMPTY
}
void p6::type_checker::do_balanced3_node(cdk::balanced3_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_BALANCED3));
}
void p6::type_checker::do_posit3_node(cdk::posit3_node *const node, int lvl) {
  // EMPTY
}
void p6::type_checker::do_takum3_node(cdk::takum3_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(16, cdk::TYPE_TAKUM3));
}
void p6::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->argument()->accept(this, lvl + 2);

  if (!node->argument()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("balanced3 expression expected in not expression");

  node->type(cdk::primitive_type::create(8, cdk::TYPE_BALANCED3));
}

void p6::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (!node->left()->is_typed(cdk::TYPE_BALANCED3) || !node->right()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("balanced3 expressions expected in and expression");

  node->type(cdk::primitive_type::create(8, cdk::TYPE_BALANCED3));
}

void p6::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (!node->left()->is_typed(cdk::TYPE_BALANCED3) || !node->right()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("balanced3 expressions expected in or expression");

  node->type(cdk::primitive_type::create(8, cdk::TYPE_BALANCED3));
}

//---------------------------------------------------------------------------

void p6::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_BALANCED3));
}

void p6::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------

// for '+'/'-' expressions 
void p6::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);

  if (node->argument()->is_typed(cdk::TYPE_BALANCED3)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_BALANCED3));
  } else if (node->argument()->is_typed(cdk::TYPE_TAKUM3)) {
    node->type(cdk::primitive_type::create(16, cdk::TYPE_TAKUM3));
  }
}

void p6::type_checker::do_unary_minus_node(cdk::unary_minus_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void p6::type_checker::do_unary_plus_node(cdk::unary_plus_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

// Aritmetic expressions "+,-,%,*"
void p6::type_checker::processBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  node->type(getCompatibleType(node->left(), node->right()));
}

// Comparissons like "<, >"
void p6::type_checker::processComparisonExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (!compatible(node->left()->type(), node->right()->type())) {
    throw std::string("types of left and right arguments of comparison expression do not match");
  }

  node->type(cdk::primitive_type::create(8, cdk::TYPE_BALANCED3));
}

void p6::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void p6::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void p6::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void p6::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}

void p6::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (!node->left()->is_typed(cdk::TYPE_BALANCED3) || !node->right()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("balanced3 expressions expected in mod expression");

  node->type(cdk::primitive_type::create(8, cdk::TYPE_BALANCED3));

}

void p6::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processComparisonExpression(node, lvl);
}
void p6::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processComparisonExpression(node, lvl);
}
void p6::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processComparisonExpression(node, lvl);
}
void p6::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processComparisonExpression(node, lvl);
}
void p6::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  processComparisonExpression(node, lvl);
}
void p6::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processComparisonExpression(node, lvl);
}

//---------------------------------------------------------------------------

void p6::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  // looks for variable in symbol table
  std::shared_ptr<p6::symbol> symbol = _symtab.find(id);
  
  // checks if symbol exists
  if (symbol != nullptr) {
    // assign the symbol type to this node's type
    node->type(symbol->type());
  } else {
    throw id;
  }
}

void p6::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
    // catches id trown from do_variable
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void p6::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl);
  node->rvalue()->accept(this, lvl + 2);
  
  node->type(getCompatibleType(node->lvalue(), node->rvalue()));
  
  // See if we're dealing with memory allocation so we can infer the stack_alloc_node's type
  auto alloc = dynamic_cast<p6::stack_alloc_node*>(node->rvalue());       // will be set to null if fails
  if (alloc) {
    // Get the type that the left-value points to
    auto lptr = std::dynamic_pointer_cast<cdk::reference_type>(node->lvalue()->type());
    if (lptr) {
      // Set the right node's type aka set the stack_alloc_node type (by inferring it from the left value)
      // We need to do this because in postfix we'll need to know which type we're alocating
      node->rvalue()->type(cdk::reference_type::create(4, lptr->referenced()));
    }
    
  }

}

//---------------------------------------------------------------------------

void p6::type_checker::do_program_node(p6::program_node *const node, int lvl) {
  // "_main" returns a BALANCED3 value and takes no arguments
  auto return_type = cdk::primitive_type::create(8, cdk::TYPE_BALANCED3);
  std::vector<std::shared_ptr<cdk::basic_type>> input_types;
  auto func_type = cdk::functional_type::create(input_types, return_type);

  node->type(func_type);

  // Store the current function context so return statements inside the program
  // can be checked against "_main"'s return type.
  _current_function = std::make_shared<p6::symbol>(func_type, "_main", 0);

  node->body()->accept(this, lvl);

  // Clear current function context after leaving program
  _current_function = nullptr;
}

void p6::type_checker::do_evaluation_node(p6::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void p6::type_checker::do_print_node(p6::print_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void p6::type_checker::do_read_node(p6::read_node *const node, int lvl) {
  // read_node is a leaf node, it has no children to accept
  node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------

void p6::type_checker::do_while_node(p6::while_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  // condition type must be a BALANCED3 because its how we represent integers but in postfix we convert it to binary ints
  if (!node->condition()->is_typed(cdk::TYPE_BALANCED3)) throw std::string("expected integer condition");

  node->block()->accept(this, lvl+4);
}

//---------------------------------------------------------------------------

void p6::type_checker::do_if_node(p6::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_BALANCED3)) throw std::string("expected integer condition");
  node->block()->accept(this, lvl + 4); 
}

void p6::type_checker::do_if_else_node(p6::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
  if (!node->condition()->is_typed(cdk::TYPE_BALANCED3)) throw std::string("expected integer condition");

  // blocks have no type of their own but their contents need type checking
  node->thenblock()->accept(this, lvl + 4);
  node->elseblock()->accept(this, lvl + 4);
}

//----------------------------------------------------------------- New Functions -----------------------------------------------------------------

void p6::type_checker::do_address_of_node(p6::address_of_node * const node, int lvl) {
  ASSERT_UNSPEC;

  node->lvalue()->accept(this, lvl + 2);

  auto base_type = node->lvalue()->type();

  if (!base_type)
    throw std::string("invalid operand in address-of expression");

  node->type(cdk::reference_type::create(4, base_type));
}

//---------------------------------------------------------------------------

void p6::type_checker::do_block_node(p6::block_node * const node, int lvl) {
  // Create new scope for block so local declarations dont leak
  _symtab.push();
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->instructions()) node->instructions()->accept(this, lvl + 2);
  _symtab.pop();
}

//---------------------------------------------------------------------------

void p6::type_checker::do_function_call_node(p6::function_call_node * const node, int lvl) {
  // Look up function identifier in symbol table
  auto symbol = _symtab.find(node->base());

  // Function must have been declared before calling
  if (!symbol)
    throw std::string("function '" + node->base() + "' not declared");

  // Identifier must refer to a function
  auto ftype = cdk::functional_type::cast(symbol->type());
  if (!ftype)
    throw std::string("'" + node->base() + "' is not a function");

  // Extract function parameters and arguments used in call
  auto params = ftype->input();
  auto args = node->arguments();

  // Must have the same number of parameters and arguments
  if (args->size() != params->length())
    throw std::string("wrong number of arguments in call to '" + node->base() + "'");

  // Each paramenter must have same type as corresponding argument
  for (size_t i = 0; i < args->size(); i++) {
    auto arg = dynamic_cast<cdk::expression_node*>(args->node(i));
    arg->accept(this, lvl + 2);

    if (!compatible(params->component(i), arg->type()))
      throw std::string("wrong type in argument " + std::to_string(i) + " of call to '" + node->base() + "'");
  }
  // Type is the same as the functions return type
  node->type(ftype->output(0));
}

//---------------------------------------------------------------------------

void p6::type_checker::do_function_declaration_node(p6::function_declaration_node * const node, int lvl) {
  // Create a symbol for the function that stores its type, name and qualifier
  auto symbol = std::make_shared<p6::symbol>(node->fun_type(), node->base(), node->qualifier());
  
  // Try to insert symbol to table
  if (!_symtab.insert(node->base(), symbol)) {

    // If it was already declared, only allow it if it is foward, and replace it
    auto existing = _symtab.find(node->base());
    if (existing->value() != QUAL_FORWARD)
      throw std::string("function '" + node->base() + "' already declared");
    _symtab.replace(node->base(), symbol);
  }

  // Notify parent that new symbol was declared
  _parent->set_new_symbol(symbol);
}

//---------------------------------------------------------------------------

void p6::type_checker::do_function_definition_node(p6::function_definition_node * const node, int lvl) {
  // Create a symbol for the function that stores its type, name and qualifier
  auto symbol = std::make_shared<p6::symbol>(node->type(), node->base(), node->qualifier());
  
  // Try to insert symbol to table
  if (!_symtab.insert(node->base(), symbol)) {

    // If it was already declared, only allow it if it is foward, and replace it
    auto existing = _symtab.find(node->base());
    if (existing->value() != QUAL_FORWARD)
      throw std::string("function '" + node->base() + "' already declared");
    _symtab.replace(node->base(), symbol);
  }

  // Create new scope for function
  _symtab.push();

  // register args by accepting them
  if (node->arguments()) {
    for (size_t i = 0; i < node->arguments()->size(); i++)
      node->arguments()->node(i)->accept(this, lvl + 2);
  }

  // Save previous function context before entering this one
  auto prev_function = _current_function;
  // Set new function context so return statements can check expected return type
  _current_function = symbol;

  if (node->body()) {
    node->body()->accept(this, lvl + 2);
  }

  // Restore previous function context
  _current_function = prev_function;

  _symtab.pop();

  // Notify parent that new symbol was declared
  _parent->set_new_symbol(symbol);
}

//---------------------------------------------------------------------------

void p6::type_checker::do_index_node(p6::index_node * const node, int lvl) {
  ASSERT_UNSPEC;

  // This will store the pointer type of the expression being indexed
  std::shared_ptr < cdk::reference_type > btype;

  node->base()->accept(this, lvl + 2);
  btype = cdk::reference_type::cast(node->base()->type());
  if (!node->base()->is_typed(cdk::TYPE_POINTER)) throw std::string("pointer expression expected in index left-value");

  // Enunciado states that "Não é possível indexar ponteiros que designem funções."
  if (btype->referenced()->name() == cdk::TYPE_FUNCTIONAL) throw std::string("cannot index functional types");

  node->index()->accept(this, lvl + 2);

  // The index must be an BALANCED3 expression
  if (!node->index()->is_typed(cdk::TYPE_BALANCED3)) throw std::string("integer expression expected in left-value index");

  // The type of p[i] is the type pointed to by p
  node->type(btype->referenced());
}

//---------------------------------------------------------------------------

void p6::type_checker::do_next_node(p6::next_node * const node, int lvl) {
  // nothing to do, parser already ensures the received n is a valid integer
  // EMPTY
}

//---------------------------------------------------------------------------

void p6::type_checker::do_null_node(p6::null_node * const node, int lvl) {
  // null_node should only be set to null if the type hasnt been determined by context yet
  ASSERT_UNSPEC;
  node->type(cdk::reference_type::create(4, nullptr));
}

//---------------------------------------------------------------------------

void p6::type_checker::do_return_node(p6::return_node * const node, int lvl) {

  // Find current function context
  auto function_symbol = _current_function;

  // A return statement is only valid inside a function context
  if (!function_symbol) {
    throw std::string("return statement outside of function");
  }

  // Return expression
  if (node->expression()) {

    node->expression()->accept(this, lvl + 2);

    // Get the type of the returned expression
    auto expr_type = node->expression()->type();

    auto func_type = function_symbol->type();
    auto ftype = cdk::functional_type::cast(func_type);
    // Expected return type of the current function
    auto return_type = ftype->output(0);

    // The returned expression type must be compatible with the function's declared return type
    if (!compatible(return_type, expr_type)) {
      throw std::string("wrong return type in function '" + function_symbol->name() + "'");
    }
  }

  // Return void
  else {
    auto func_type = function_symbol->type();
    auto ftype = cdk::functional_type::cast(func_type);

    auto return_type = ftype->output(0);

    // A return without a value is only valid in void functions
    if (return_type->name() != cdk::TYPE_VOID) {
      throw std::string("missing return value in non-void function '" + function_symbol->name() + "'");
    }
  }

}

//---------------------------------------------------------------------------

void p6::type_checker::do_sizeof_node(p6::sizeof_node * const node, int lvl) {
  node->expression()->accept(this, lvl + 2);

  if (!node->expression()->type())
    throw std::string("invalid argument to sizeof");

  // sizeof always returns a BALANCED3
  node->type(cdk::primitive_type::create(8, cdk::TYPE_BALANCED3));
}

//---------------------------------------------------------------------------

void p6::type_checker::do_stack_alloc_node(p6::stack_alloc_node * const node, int lvl) {

  node->argument()->accept(this, lvl + 2);

  // The allocation size must be a BALANCED3
  if (!node->argument()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("integer expression expected in memory reservation");

  // The result of stack allocation is a pointer
  node->type(cdk::reference_type::create(4, nullptr));
}

//---------------------------------------------------------------------------

void p6::type_checker::do_stop_node(p6::stop_node * const node, int lvl) {
  // nothing to do, parser already ensures the received n is a valid integer
  // EMPTY
}

//---------------------------------------------------------------------------

void p6::type_checker::do_variable_declaration_node(p6::variable_declaration_node * const node, int lvl) {
  // If its forward, it can't have initializer
  if (node->qualifier() == QUAL_FORWARD && node->initializer())
    throw std::string("forward declaration cannot have initializer");

  if (node->initializer()) {
    node->initializer()->accept(this, lvl + 2);
    
    // If auto, assign type of initializer
    if (!node->type() || node->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(node->initializer()->type());

    // Otherwise, verify that initializer is compatible
    } else {
      if (!compatible(node->type(), node->initializer()->type()))
        throw std::string("wrong type in variable initializer of '" + node->base() + "'");
    }
  }

  // Create a symbol for variable using its resolved type, name, and qualifier
  auto symbol = std::make_shared<p6::symbol>(node->type(), node->base(),  node->qualifier());
  
  // Try to insert the variable into the current scope
  if (!_symtab.insert(node->base(), symbol)) {

    // If it was already declared, only allow it if it is foward, and replace it
    auto existing = _symtab.find(node->base());
    if (existing->value() != QUAL_FORWARD)
      throw std::string("variable '" + node->base() + "' already declared");
    _symtab.replace(node->base(), symbol);
  }
  
  // Notify parent that new symbol was declared
  _parent->set_new_symbol(symbol);
}
