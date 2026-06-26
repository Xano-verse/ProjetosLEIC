#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include ".auto/all_nodes.h"  // all_nodes.h is automatically generated

enum qualifier_t {
  QUAL_NONE,
  QUAL_EXTERN,
  QUAL_FORWARD,
  QUAL_PUBLIC,
  QUAL_AUTO
};

static size_t local_size(cdk::basic_node *node) {
  if (node == nullptr) return 0;

  if (auto seq = dynamic_cast<cdk::sequence_node*>(node)) {
    size_t size = 0;
    for (size_t i = 0; i < seq->size(); i++)
      size += local_size(seq->node(i));
    return size;
  }

  if (auto block = dynamic_cast<p6::block_node*>(node))
    return local_size(block->declarations()) + local_size(block->instructions());

  if (auto decl = dynamic_cast<p6::variable_declaration_node*>(node))
    return decl->type()->size();

  if (auto w = dynamic_cast<p6::while_node*>(node))
    return local_size(w->block());

  if (auto i = dynamic_cast<p6::if_node*>(node))
    return local_size(i->block());

  if (auto i = dynamic_cast<p6::if_else_node*>(node))
    return local_size(i->thenblock()) + local_size(i->elseblock());

  return 0;
}


// Helper functions


// Push a takum3 to the stack, if its a balanced3 convert it to takum3
void p6::postfix_writer::push_as_takum3(cdk::expression_node * const expr, int lvl) {
  expr->accept(this, lvl);

  if (expr->is_typed(cdk::TYPE_BALANCED3)) {
    _pf.B2T();
  }

}

void p6::postfix_writer::call_balanced3_unary(cdk::expression_node * const arg, const std::string &fname, int lvl) {
  arg->accept(this, lvl);

  _functions_to_declare.insert(fname);
  _pf.CALL(fname);
  _pf.TRASH(8);
  _pf.LDFVAL64I();
}

void p6::postfix_writer::call_balanced3_binary(cdk::expression_node * const left, cdk::expression_node * const right, const std::string &fname, int lvl) {

  // comparison functions need inverted stack order of arguments
  right->accept(this, lvl);
  left->accept(this, lvl);

  _functions_to_declare.insert(fname);
  _pf.CALL(fname);
  _pf.TRASH(16);
  _pf.LDFVAL64I();
}

void p6::postfix_writer::call_takum3_binary(cdk::expression_node * const left, cdk::expression_node * const right, const std::string &fname, int lvl) {

  right->accept(this, lvl);
  if (right->is_typed(cdk::TYPE_BALANCED3)) {
    _pf.B2T();
  }

  left->accept(this, lvl);
  if (left->is_typed(cdk::TYPE_BALANCED3)) {
    _pf.B2T();
  }

  _functions_to_declare.insert(fname);
  _pf.CALL(fname);
  _pf.TRASH(32);
  _pf.LDFVAL64I();            // comparison functions of takum3 return balanced3 so its a 64I to load
}


// Convert a boolean truth value to a kleene's logic balanced3 truth value
void p6::postfix_writer::bool_to_balanced3() {
  int lbl_true = ++_lbl;
  int lbl_end = ++_lbl;

  _pf.JNZ(mklbl(lbl_true));

  _pf.BALANCED3(cdk::balanced3_type::value_type(-1));
  _pf.JMP(mklbl(lbl_end));

  _pf.LABEL(mklbl(lbl_true));
  _pf.BALANCED3(cdk::balanced3_type::value_type(1));

  _pf.LABEL(mklbl(lbl_end));
}


//---------------------------------------------------------------------------

void p6::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void p6::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void p6::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  // EMPTY
}
void p6::postfix_writer::do_balanced3_node(cdk::balanced3_node * const node, int lvl) {
  _pf.BALANCED3(node->value());
}
void p6::postfix_writer::do_posit3_node(cdk::posit3_node * const node, int lvl) {
  // EMPTY
}
void p6::postfix_writer::do_takum3_node(cdk::takum3_node * const node, int lvl) {
  _pf.TAKUM3(node->value());
}
void p6::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  call_balanced3_unary(node->argument(), "balanced3_kleene_not", lvl);
}

void p6::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  int lbl_eval_right = ++_lbl;
  int lbl_end = ++_lbl;

  // Only evaluate right side of AND if the left side is true
  node->left()->accept(this, lvl);

  _pf.DUP64();
  _pf.B2I();

  _pf.INT(0);
  _pf.LT();

  // If left side is true, jump to evaluate right side
  _pf.JZ(mklbl(lbl_eval_right));

  // If left side is false, we know the AND is false,
  // so we push false and jump to the end, skipping the right side evaluation
  _pf.TRASH(8);
  _pf.BALANCED3(cdk::balanced3_type::value_type(-1));
  _pf.JMP(mklbl(lbl_end));

  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_eval_right));

  // Evaluate right side
  node->right()->accept(this, lvl);

  _functions_to_declare.insert("balanced3_kleene_and");
  _pf.CALL("balanced3_kleene_and");
  _pf.TRASH(16);
  _pf.LDFVAL64I();

  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_end));
}

void p6::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  int lbl_eval_right = ++_lbl;
  int lbl_end = ++_lbl;

  // Only evaluate right side of OR if the left side is false
  node->left()->accept(this, lvl);

  _pf.DUP64();
  _pf.B2I();

  _pf.INT(0);
  _pf.GT();

  // If left side is false, jump to evaluate right side
  _pf.JZ(mklbl(lbl_eval_right));
  
  // If left side is true, we know the OR is true,
  // so we push true and jump to the end, skipping the right side evaluation
  _pf.TRASH(8);
  _pf.BALANCED3(cdk::balanced3_type::value_type(1));
  _pf.JMP(mklbl(lbl_end));

  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_eval_right));

  // Evaluate right side
  node->right()->accept(this, lvl);

  _functions_to_declare.insert("balanced3_kleene_or");
  _pf.CALL("balanced3_kleene_or");
  _pf.TRASH(16);
  _pf.LDFVAL64I();

  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_end));
}

//---------------------------------------------------------------------------

// Just visit every node of the sequence
void p6::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    if (node->node(i)) node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  _pf.BALANCED3(cdk::balanced3_type::value_type(node->value()));
}

void p6::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters

  /* leave the address on the stack */
  _pf.TEXT(); // return to the TEXT segment
  _pf.ADDR(mklbl(lbl1)); // the string to be printed
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_unary_minus_node(cdk::unary_minus_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Pushes a 0, then the argument, then subtracts 0 - argument, giving us the symmetric
  if (node->is_typed(cdk::TYPE_BALANCED3)) {
    _pf.BALANCED3(cdk::balanced3_type::value_type(0));
    node->argument()->accept(this, lvl);
    _pf.BSUB();

  } else if (node->is_typed(cdk::TYPE_TAKUM3)) {
    _pf.TAKUM3(cdk::takum3_type::value_type(0));
    push_as_takum3(node->argument(), lvl);
    _pf.TSUB();

  } else {
    node->argument()->accept(this, lvl);
    _pf.NEG();
  }
}

void p6::postfix_writer::do_unary_plus_node(cdk::unary_plus_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
}

//---------------------------------------------------------------------------


void p6::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->is_typed(cdk::TYPE_BALANCED3)) {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.BADD();

  } else if (node->is_typed(cdk::TYPE_TAKUM3)) {
    // push_as_takum3 already convert balanced3 to takum3 if needed
    push_as_takum3(node->left(), lvl);
    push_as_takum3(node->right(), lvl);
    _pf.TADD();

  } else if (node->is_typed(cdk::TYPE_POINTER)) {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);

    // if we want to add an integer to a pointer, we must convert the integer from balanced3
    // "Note-se que a aritmética de ponteiros é possível apenas com inteiros binários, mas não com inteiros ternários (estes devem ser convertidos para binário, caso seja necessário). "
    if (node->right()->is_typed(cdk::TYPE_BALANCED3)) {
      _pf.B2I();
    }
  
    // We need to scale the integer according to the referenced type's size
    auto ptr = std::dynamic_pointer_cast<cdk::reference_type>(node->left()->type());

    _pf.INT(ptr->referenced()->size());             // push the size of the referenced type onto the stack (which is the type of the node)
    _pf.MUL();                                      // multiply the size of the referenced type by the provided integer, giving us an offset
    _pf.ADD();                                      // add the offset to the base


  } else {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.ADD();
  }
}


void p6::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->is_typed(cdk::TYPE_BALANCED3)) {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.BSUB();

  } else if (node->is_typed(cdk::TYPE_TAKUM3)) {
    // push_as_takum3 already convert balanced3 to takum3 if needed
    push_as_takum3(node->left(), lvl);
    push_as_takum3(node->right(), lvl);
    _pf.TSUB();

  } else {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.SUB();
  }
}

void p6::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->is_typed(cdk::TYPE_BALANCED3)) {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.BMUL();

  } else if (node->is_typed(cdk::TYPE_TAKUM3)) {
    // push_as_takum3 already convert balanced3 to takum3 if needed
    push_as_takum3(node->left(), lvl);
    push_as_takum3(node->right(), lvl);
    _pf.TMUL();

  } else {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.MUL();
  }
}

void p6::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->is_typed(cdk::TYPE_BALANCED3)) {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.BDIV();

  } else if (node->is_typed(cdk::TYPE_TAKUM3)) {
    // push_as_takum3 already convert balanced3 to takum3 if needed
    push_as_takum3(node->left(), lvl);
    push_as_takum3(node->right(), lvl);
    _pf.TDIV();

  } else {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.DIV();
  }
}

// mod operation, gives remainder of integer division
void p6::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->is_typed(cdk::TYPE_BALANCED3)) {
    // Temporary label used to store the divisor
    int lbl_tmp = ++_lbl;
    // Label used to skip the correction step when the result is already non-negative
    int lbl_done = ++_lbl;

    // Reserve 8 bytes in DATA to temporarily store the BALANCED3 divisor
    _pf.DATA();
    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl_tmp));
    _pf.SALLOC(8);

    // Return to TEXT before generating executable code
    _pf.TEXT();

    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);

    // Keep a copy of the divisor, because BMOD consumes the operands
    _pf.DUP64();
    _pf.ADDR(mklbl(lbl_tmp));
    _pf.STBALANCED3();

    // Compute left mod right using BALANCED3 modulo
    _pf.BMOD();

    // Check whether the modulo result is negative
    _pf.DUP64();
    _pf.B2I();
    _pf.INT(0);
    _pf.LT();

    // If the result is not negative, skip the correction step
    _pf.JZ(mklbl(lbl_done));

    // If the result is negative, add the divisor to make the result positive
    _pf.ADDR(mklbl(lbl_tmp));
    _pf.LDBALANCED3();
    _pf.BADD();

    // End of BALANCED3 modulo correction
    _pf.ALIGN();
    _pf.LABEL(mklbl(lbl_done));

  } else {
    // Default modulo handling for regular integer-like values
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.MOD();
  }
}


// Comparison functions


void p6::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->left()->is_typed(cdk::TYPE_TAKUM3) ||
      node->right()->is_typed(cdk::TYPE_TAKUM3)) {
    call_takum3_binary(node->left(), node->right(), "takum3_lt", lvl);

  } else if (node->left()->is_typed(cdk::TYPE_BALANCED3) ||
             node->right()->is_typed(cdk::TYPE_BALANCED3)) {
    call_balanced3_binary(node->left(), node->right(), "balanced3_lt", lvl);

  } else {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.LT();
    bool_to_balanced3();
  }
}
void p6::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->left()->is_typed(cdk::TYPE_TAKUM3) ||
      node->right()->is_typed(cdk::TYPE_TAKUM3)) {
    call_takum3_binary(node->left(), node->right(), "takum3_le", lvl);

  } else if (node->left()->is_typed(cdk::TYPE_BALANCED3) ||
             node->right()->is_typed(cdk::TYPE_BALANCED3)) {
    call_balanced3_binary(node->left(), node->right(), "balanced3_le", lvl);

  } else {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.LE();
    bool_to_balanced3();
  }
}
void p6::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->left()->is_typed(cdk::TYPE_TAKUM3) ||
      node->right()->is_typed(cdk::TYPE_TAKUM3)) {
    call_takum3_binary(node->left(), node->right(), "takum3_ge", lvl);

  } else if (node->left()->is_typed(cdk::TYPE_BALANCED3) ||
             node->right()->is_typed(cdk::TYPE_BALANCED3)) {
    call_balanced3_binary(node->left(), node->right(), "balanced3_ge", lvl);

  } else {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.GE();
    bool_to_balanced3();
  }
}
void p6::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->left()->is_typed(cdk::TYPE_TAKUM3) ||
      node->right()->is_typed(cdk::TYPE_TAKUM3)) {
    call_takum3_binary(node->left(), node->right(), "takum3_gt", lvl);

  } else if (node->left()->is_typed(cdk::TYPE_BALANCED3) ||
             node->right()->is_typed(cdk::TYPE_BALANCED3)) {
    call_balanced3_binary(node->left(), node->right(), "balanced3_gt", lvl);

  } else {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.GT();
    bool_to_balanced3();
  }
}
void p6::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->left()->is_typed(cdk::TYPE_TAKUM3) ||
      node->right()->is_typed(cdk::TYPE_TAKUM3)) {
    call_takum3_binary(node->left(), node->right(), "takum3_ne", lvl);

  } else if (node->left()->is_typed(cdk::TYPE_BALANCED3) ||
             node->right()->is_typed(cdk::TYPE_BALANCED3)) {
    call_balanced3_binary(node->left(), node->right(), "balanced3_ne", lvl);

  } else {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.NE();
    bool_to_balanced3();
  }
}
void p6::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  if (node->left()->is_typed(cdk::TYPE_TAKUM3) ||
      node->right()->is_typed(cdk::TYPE_TAKUM3)) {
    call_takum3_binary(node->left(), node->right(), "takum3_eq", lvl);

  } else if (node->left()->is_typed(cdk::TYPE_BALANCED3) ||
             node->right()->is_typed(cdk::TYPE_BALANCED3)) {
    call_balanced3_binary(node->left(), node->right(), "balanced3_eq", lvl);

  } else {
    node->left()->accept(this, lvl);
    node->right()->accept(this, lvl);
    _pf.EQ();
    bool_to_balanced3();
  }
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  auto symbol = _symtab.find(node->name());

  if (symbol) {
    int offset = symbol->value();

    if (offset < 0 || offset >= 8) {
      _pf.LOCAL(offset);
      return;
    }
  }

  _pf.ADDR(node->name());
}

void p6::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_BALANCED3) {
    _pf.LDBALANCED3();
  } else if (node->type()->name() == cdk::TYPE_TAKUM3) {
    _pf.LDTAKUM3();
  } else {
    // strings and pointers occupy 4 bytes
    _pf.LDINT(); // depends on type size
  }
}

void p6::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  auto l = node->lvalue();
  auto r = node->rvalue();

  r->accept(this, lvl); // determine the new value
  if (node->is_typed(cdk::TYPE_TAKUM3)) {
    if (r->type()->name() == cdk::TYPE_BALANCED3) {
      _pf.B2T();                                  // we allow conversion from balanced3 (int) to takum3 (double) -> the b3 rvalue is currently at the stack
    }
    _pf.DUP128();
  } else if (node->is_typed(cdk::TYPE_BALANCED3)) {
    _pf.DUP64();
  } else {              // for strings and pointers (4 bytes)
    _pf.DUP32();            
  }

  // in case the variable is already declared or we're dealing with a local variable of a function
  if (new_symbol() == nullptr || _inFunction) {
    reset_new_symbol();
    l->accept(this, lvl); // where to store the value

  } else {
    _pf.DATA(); // variables are all global and live in DATA
    _pf.ALIGN(); // make sure we are aligned
    _pf.LABEL(new_symbol()->name()); // name variable location
    reset_new_symbol();

    // initialize it to 0 according to type
    if (node->is_typed(cdk::TYPE_TAKUM3)) {
      _pf.STAKUM3(cdk::takum3_type::value_type(0));
    } else if (node->is_typed(cdk::TYPE_BALANCED3)) {
      _pf.SBALANCED3(cdk::balanced3_type::value_type(0));
    } else {
      _pf.SINT(0);
    }

    _pf.TEXT(); // return to the TEXT segment
    l->accept(this, lvl);  
  }
  
  // store the value at address according to type
  if (node->is_typed(cdk::TYPE_TAKUM3)) {
      _pf.STTAKUM3();
  } else if (node->is_typed(cdk::TYPE_BALANCED3)) {
      _pf.STBALANCED3();
  } else {
    _pf.STINT(); 
  }
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_program_node(p6::program_node * const node, int lvl) {

  _inMain = true;

  do_function_definition_node(node, lvl);

  _inMain = false;
  
  // declare external functions
  for (std::string s : _functions_to_declare) {
      _pf.EXTERN(s);
  }
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_evaluation_node(p6::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl);                // determine the value
  _pf.TRASH(node->argument()->type()->size());        // remove the value from the stack
}


void p6::postfix_writer::do_print_node(p6::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // print all arguments
  for (size_t ix = 0; ix < node->argument()->size(); ix++) {
    auto arg = dynamic_cast<cdk::expression_node*>(node->argument()->node(ix));

    std::shared_ptr<cdk::basic_type> argtype = arg->type();
    arg->accept(this, lvl);         // evaluate expression to print

    if (argtype->name() == cdk::TYPE_BALANCED3) {
      _functions_to_declare.insert("balanced3_print");
      _pf.CALL("balanced3_print");
      _pf.TRASH(8);                 // remove the expression to print from the stack (balanced3 are 8 bytes)
    } else if (argtype->name() == cdk::TYPE_TAKUM3) {
      _functions_to_declare.insert("takum3_print");
      _pf.CALL("takum3_print");
      _pf.TRASH(16);               // remove the expression to print from the stack (takum3 are 16 bytes)
    } else if (argtype->name() == cdk::TYPE_STRING) {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      _pf.TRASH(4);
    } else {
      std::cerr << "cannot print expression of unknown type" << std::endl;
      return;
    }
  }

  // check if we should pritn a new line and do it if so
  if (node->newline()) {
    _functions_to_declare.insert("println");
    _pf.CALL("println");
  }
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_read_node(p6::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // the type of the read_node is set in the type_checker
  if (node->is_typed(cdk::TYPE_BALANCED3)) {
    _functions_to_declare.insert("balanced3_read");
    _pf.CALL("balanced3_read");
    _pf.LDFVAL64I();

  } else if (node->is_typed(cdk::TYPE_TAKUM3)) {
    _functions_to_declare.insert("takum3_read");
    _pf.CALL("takum3_read");
    _pf.LDFVAL64F();

  } else {
    std::cerr << "FATAL: " << node->lineno() << ": cannot read type" << std::endl;
    return;
  }

  // value is now psuhed onto the stack, the caller has the responsability of consuming it, not the read_node
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_while_node(p6::while_node * const node, int lvl) {

  // Define useful labels 
  int lbl_cond = ++_lbl;
  int lbl_end = ++_lbl;

  _loop_cond_labels.push_back(lbl_cond);
  _loop_end_labels.push_back(lbl_end);

  // Create the actual assembly label
 _pf.LABEL(mklbl(lbl_cond));

  // Evaluate condition
  node->condition()->accept(this, lvl + 2);

  // Jump to the end if zero or less (ternary logic)
  _pf.B2I();                            // converts the value on the stack to binary integer
  _pf.INT(0);                           // push a zero to compare
  _pf.JLE(mklbl(lbl_end));               // jump if less or equal to 0

  // Execute instruction block
  node->block()->accept(this, lvl + 2);

  // Jump to while condition to test it again
  _pf.JMP(mklbl(lbl_cond));

  // Create the actual assembly label
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl_end));

  _loop_cond_labels.pop_back();
  _loop_end_labels.pop_back();

}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_if_node(p6::if_node * const node, int lvl) {
  int lblEnd = ++_lbl;

  // Evaluate the condition
  node->condition()->accept(this, lvl);

  // Jump to the end if zero or less (ternary logic)
  _pf.B2I();                            // converts the value on the stack to binary integer
  _pf.INT(0);                           // push a zero to compare
  _pf.JLE(mklbl(lblEnd));               // jump if less or equal to 0

  // Execute instruction block
  node->block()->accept(this, lvl + 2);

  // Create the end label
  _pf.ALIGN();
  _pf.LABEL(mklbl(lblEnd));
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_if_else_node(p6::if_else_node * const node, int lvl) {
  int lblElse = ++_lbl;
  int lblEnd = ++_lbl; 

  // Evaluate the condition
  node->condition()->accept(this, lvl);

  // Jump to the else if zero or less (ternary logic)
  _pf.B2I();                            // converts the value on the stack to binary integer
  _pf.INT(0);                            // push a zero to compare
  _pf.JLE(mklbl(lblElse));               // jump if less or equal to 0

  // Execute normal instruction block
  node->thenblock()->accept(this, lvl + 2);

  // Jump to end, skipping the else block
  _pf.JMP(mklbl(lblEnd));

  // Create the else label
  _pf.ALIGN();
  _pf.LABEL(mklbl(lblElse));

  // Execute else instruction block
  node->elseblock()->accept(this, lvl + 2);

  // Create the end label
  _pf.LABEL(mklbl(lblEnd));
}

//----------------------------------------------------------------- New Functions -----------------------------------------------------------------

void p6::postfix_writer::do_address_of_node(p6::address_of_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_block_node(p6::block_node * const node, int lvl) {
  _symtab.push();
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->instructions()) node->instructions()->accept(this, lvl + 2);
  _symtab.pop();
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_function_call_node(p6::function_call_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Keep track of how many argument bytes must be removed from the stack after the call
  size_t argsSize = 0;

  auto symbol = _symtab.find(node->base());
  auto ftype = symbol ? cdk::functional_type::cast(symbol->type()) : nullptr;
  auto params = ftype ? ftype->input() : nullptr;

  // Label used to reserve temporary storage for TAKUM3 return values
  int retlbl = 0;
  
  // TAKUM3 return values with hidden argument
  if (node->is_typed(cdk::TYPE_TAKUM3)) {
    retlbl = ++_lbl;

    _pf.DATA();
    _pf.ALIGN();
    _pf.LABEL(mklbl(retlbl));

    // For TAKUM3 we need to allocate 16 bytes because the return logic uses hidden pointer
    _pf.SALLOC(16);
    _pf.TEXT();
  }

  // Add every function argument to the stack before calling
  for (int i = (int)node->arguments()->size() - 1; i >= 0; i--) {
    auto arg = node->argument(i);

    arg->accept(this, lvl + 2);

    if (params) {
      auto param_type = params->component(i);

      // If the function expects TAKUM3 but receives BALANCED3
      if (param_type->name() == cdk::TYPE_TAKUM3 && arg->is_typed(cdk::TYPE_BALANCED3)) {
        _pf.B2T();
        argsSize += 16;
      } else {
        argsSize += arg->type()->size();
      }
    } else {
      argsSize += arg->type()->size();
    }
  }

  // If the return value is TAKUM3 we must add 4 more bytes for the hidden pointer
  if (node->is_typed(cdk::TYPE_TAKUM3)) {
    _pf.ADDR(mklbl(retlbl));
    argsSize += 4;
  }

  // Call the function
  _pf.CALL(node->base());

  // Clear the arguments from the stack
  if (argsSize > 0)
    _pf.TRASH(argsSize);

  // Load the value from the return register according to the type
  if (node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_POINTER)) {
    _pf.LDFVAL32I();

  } else if (node->is_typed(cdk::TYPE_BALANCED3)) {
    _pf.LDFVAL64I();

  // Here we load from the hidden pointer, the return register only has a dummy 0
  } else if (node->is_typed(cdk::TYPE_TAKUM3)) {
    _pf.ADDR(mklbl(retlbl));
    _pf.LDTAKUM3();
  }
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_function_declaration_node(p6::function_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Emit an EXTERN directive so calls can be resolved at link time
  if (node->qualifier() == QUAL_EXTERN)
    _pf.EXTERN(node->base());
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_function_definition_node(p6::function_definition_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Create new scope
  _symtab.push();

  // Save the current state so it can be restored after generating this function
  bool oldMain = _inMain;
  bool oldInFunction = _inFunction;
  auto oldFunction = _current_function;
  int oldRetLabel = _currentBodyRetLabel;
  int oldLocalSize = _localsize;

  _inFunction = true;

  // Store the current function symbol so return statements can access its values
  _current_function = std::make_shared<p6::symbol>(
      node->type(),
      node->base(),
      node->qualifier()
  );
  reset_new_symbol();

   // Create a unique label used as the common return of this function
  _currentBodyRetLabel = ++_lbl;

  // Reset local variable size counting for this function's stack
  _localsize = 0;
  
  // Function's code is alligned 
  _pf.TEXT();
  _pf.ALIGN();

  // If this function is not the implicit main function, clear the main flag
  if (node->base() != "_main") {
    _inMain = false;
  }

  // Public functions must be exported
  if (node->qualifier() == QUAL_PUBLIC) {
    _pf.GLOBAL(node->base(), _pf.FUNC());
  }

  // Emit the function label
  _pf.LABEL(node->base());
  
  // Reserve stack space for all local variables in the function body
  size_t frameSize = local_size(node->body());
  _pf.ENTER(frameSize);

  auto ftype = cdk::functional_type::cast(node->type());

   // Function arguments start at offset 8 from the frame pointer
  int argOffset = 8;

  // For TAKUM3 we need additional 4 bytes to store the hidden pointer that will point to the TAKUM3 return value
  if (!_inMain && ftype && ftype->output(0)->name() == cdk::TYPE_TAKUM3) {
    argOffset = 12;
  }
  
  // Register each function argument in the current symbol table scope
  for (size_t i = 0; i < node->arguments()->size(); i++) {
    auto arg = dynamic_cast<p6::variable_declaration_node *>(node->arguments()->node(i));

    auto symbol = std::make_shared<p6::symbol>(
        arg->type(),
        arg->base(),
        QUAL_NONE
    );

    symbol->value(argOffset);
    _symtab.insert(arg->base(), symbol);

    // Move to the next argument position
    argOffset += arg->type()->size();
  }

  if (node->body()) {
    node->body()->accept(this, lvl + 2);
  }

  // Sets default return value if the function doesn't provide one (if it does, the return node will jump to the retLabel and skip these ifs)
  if (_inMain) {
    _pf.INT(0);
    _pf.STFVAL32I();

  } else if (ftype && ftype->output(0)->name() == cdk::TYPE_BALANCED3) {
    _pf.BALANCED3(cdk::balanced3_type::value_type(0));
    _pf.STFVAL64I();

  } else if (ftype && ftype->output(0)->name() == cdk::TYPE_TAKUM3) {
    
    // return of TAKUM3 is done with hidden pointer logic -> see do_return_node for explanation
    _pf.TAKUM3(cdk::takum3_type::value_type(0));

    _pf.LOCAL(8);
    _pf.LDINT();
    _pf.STTAKUM3();

    _pf.INT(0);
    _pf.STFVAL32I();
  }
  // Restore the caller's stack frame and return from the function
  _pf.LABEL(mklbl(_currentBodyRetLabel));
  _pf.LEAVE();
  _pf.RET();

  _inMain = oldMain;
  _localsize = oldLocalSize;
  _currentBodyRetLabel = oldRetLabel;
  _current_function = oldFunction;
  _inFunction = oldInFunction;

  _symtab.pop();
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_index_node(p6::index_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  // Push both values onto the stack
  node->base()->accept(this, lvl);
  node->index()->accept(this, lvl);

  _pf.B2I();                            // the index is balanced3, we need to convert it to int

  // We want to multiply the index by the size of the reference type
  _pf.INT(node->type()->size());                  // push the size of the referenced type onto the stack (which is the type of the node)
  _pf.MUL();                                      // multiply the size of the referenced type by the index, giving us an offset
  _pf.ADD();                                      // add the offset to the base

  // value stays on the stack, the caller is the one to take it out
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_next_node(p6::next_node * const node, int lvl) {
  // Get how many nested loops the next statement should countinue
  int level = node->level();

  // The level must pe possible with the existing loop
  if (level <= 0 || level > (int)_loop_cond_labels.size())
    throw std::string("invalid next level");

  // Select the condition label of the target loop.
  // level 1 means the innermost loop, level 2 the next outer loop, etc
  int index = _loop_cond_labels.size() - level;

  // Jump to the selected loop's condition label
  _pf.JMP(mklbl(_loop_cond_labels[index]));
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_null_node(p6::null_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  if (_inFunction) {
    _pf.BALANCED3(cdk::balanced3_type::value_type(0));           // push a 0 to the stack (a null pointer is represented as 0 internally)
  } else {
    _pf.SBALANCED3(cdk::balanced3_type::value_type(0));          // push a static 0 
  }
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_return_node(p6::return_node * const node, int lvl) {
  // If we're in main, we must return 0 on success
  // "O valor de retorno da função principal é devolvido ao ambiente que invocou o programa."
  // "Este valor de retorno segue as seguintes regras (sistema operativo): 0 (zero), execução sem erros; 1 (um), argumentos inválidos (em número ou valor); 2 (dois), erro de execução."
  if (_inMain) {
    if (node->expression()) {
      node->expression()->accept(this, lvl + 2);
      _pf.TRASH(node->expression()->type()->size());
    }

    _pf.INT(0);
    _pf.STFVAL32I();
    _pf.JMP(mklbl(_currentBodyRetLabel));
    return;
  }

  if (node->expression()) {
    // Pushes return value to the stack
    node->expression()->accept(this, lvl + 2);

    auto ftype = _current_function
      ? cdk::functional_type::cast(_current_function->type())
      : nullptr;

    auto ret_type = ftype ? ftype->output(0) : node->expression()->type();

    if (ret_type->name() == cdk::TYPE_TAKUM3) {
      if (node->expression()->is_typed(cdk::TYPE_BALANCED3)) {
        _pf.B2T();
      }

      // For returning TAKUM3 we're using hidden pointers
      // So we push the address of the hidden pointer to the stack (the CALLER stored it with an offset of 8 from us)
      _pf.LOCAL(8);

      // Then we load the value of that address aka we push the actual pointer to the stack (the address of where we'll store the TAKUM3)
      _pf.LDINT();

      // We store the TAKUM3 (the CALLER already allocated enough space)
      _pf.STTAKUM3();

      // Return a 0 just to indicate success
      _pf.INT(0);
      _pf.STFVAL32I();

    } else if (ret_type->name() == cdk::TYPE_BALANCED3) {
      // balanced3 is 8 bytes
      _pf.STFVAL64I();

    } else {
      // strings and pointers are 4 bytes
      _pf.STFVAL32I();
    }
  }

  _pf.JMP(mklbl(_currentBodyRetLabel));
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_sizeof_node(p6::sizeof_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.BALANCED3(cdk::balanced3_type::value_type((long long) node->expression()->type()->size()));
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_stack_alloc_node(p6::stack_alloc_node * const node, int lvl) {
  // We cant have ASSERT_SAFE_EXPRESSIONS because it would re-run the stack_alloc_node in isolation
  // but its the assignment node that sets the stack_alloc_node's type by inference, so we need stack_alloc_node to only be run with assignment node
  node->argument()->accept(this, lvl + 2);
  
  _pf.B2I();                       // convert to int so we can multiply
  
  // stack_alloc_node has the type that the variable we're alocating points to
  auto ptr = std::dynamic_pointer_cast<cdk::reference_type>(node->type());
  _pf.INT(ptr->referenced()->size());  

  _pf.MUL();                      // will multiply the size of the type that we're allocating by the number provided in the node argument
  _pf.ALLOC();                    // allocs as many bytes as indicated by the top of the stack
  _pf.SP();

}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_stop_node(p6::stop_node * const node, int lvl) {
  // Get how many nested loops the stop statement should break out of
  int level = node->level();
  
  // The level must pe possible with the existing loop
  if (level <= 0 || level > (int)_loop_end_labels.size())
    throw std::string("invalid stop level");

  // Select the end label of the target loop.
  int index = _loop_end_labels.size() - level;

  // Jump to the selected loop's end label
  _pf.JMP(mklbl(_loop_end_labels[index]));
}

//---------------------------------------------------------------------------

void p6::postfix_writer::do_variable_declaration_node(p6::variable_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // Get the symbol created by the type checker for this variable declaration
  auto symbol = new_symbol();

  // For global variable declarations
  if (!_inFunction) {

    // Extern variables are declared but not defined
    if (node->qualifier() == QUAL_EXTERN) {
      _pf.EXTERN(node->base());
      reset_new_symbol();
      return;
    }

    // Export public variables so other modules can acess it
    if (node->qualifier() == QUAL_PUBLIC)
      _pf.GLOBAL(node->base(), _pf.OBJ());

    // Global variables with inicializer
    if (node->initializer()) {
      // Inicializer for BALANCED3
      if (auto i = dynamic_cast<cdk::balanced3_node*>(node->initializer())) {
        _pf.DATA();
        _pf.ALIGN();
        _pf.LABEL(node->base());
        _pf.SBALANCED3(i->value());
      // Inicializer for TAKUM3
      } else if (auto t = dynamic_cast<cdk::takum3_node*>(node->initializer())) {
        _pf.DATA();
        _pf.ALIGN();
        _pf.LABEL(node->base());
        _pf.STAKUM3(t->value());
      // Inicializer for string
      } else if (auto s = dynamic_cast<cdk::string_node*>(node->initializer())) {
        int lbl = ++_lbl;
        _pf.RODATA();
        _pf.ALIGN();
        _pf.LABEL(mklbl(lbl));
        _pf.SSTRING(s->value());
        _pf.DATA();
        _pf.ALIGN();
        _pf.LABEL(node->base());
        _pf.SADDR(mklbl(lbl));
      // Non-literal global initializer
     } else {
        // complex initializer — emit via accept
        _pf.DATA();
        _pf.ALIGN();
        _pf.LABEL(node->base());
        _pf.SINT(0);  // placeholder, will be overwritten at runtime
      }
    // Global variable without initializer
    // Reserve enough space according to the variable type
    } else {
      _pf.DATA();
      _pf.ALIGN();
      _pf.LABEL(node->base());
      _pf.SALLOC(node->type()->size());
    }

    reset_new_symbol();
    return;
  }

  // Local variable declaration
  // Increase the function's local stack size and assign a negative offset
  _localsize += node->type()->size();
  int offset = -_localsize;

  // Store the local variable offset in its symbol table entry
  if (symbol) {
    symbol->value(offset);
    reset_new_symbol();
  }

  // Local variable declaration with initializer
  if (node->initializer()) {
    node->initializer()->accept(this, lvl + 2);
    // Convert BALANCED3 initializer to TAKUM3 if needed
    if (node->is_typed(cdk::TYPE_TAKUM3) &&
        node->initializer()->is_typed(cdk::TYPE_BALANCED3)) {
      _pf.B2T();
    }
    // Push the address of the local variable
    _pf.LOCAL(offset);
    // Store initializer for TAKUM3
    if (node->is_typed(cdk::TYPE_TAKUM3)) {
      _pf.STTAKUM3();
    // Store initializer for BALANCED3
    } else if (node->is_typed(cdk::TYPE_BALANCED3)) {
      _pf.STBALANCED3();
    } else {
      // Used for pointer/string-like values
      _pf.STINT();
    }
  }
}