#ifndef LANG_DSL_H_
#define LANG_DSL_H_

#include "objects.h"
#include "config.h"
#include TREE_INCLUDE

#include <math.h>

#define NODE_IS_OPER(node_, oper_)                                                          \
            ((NODE_TYPE(node_) == TreeElemType::OPER) && NODE_DATA(node_).oper == oper_)

#define L(node_) (&(node_)->left)
#define R(node_) (&(node_)->right)

#define ELEM(node_)  ((TreeElem*)((node_)->elem))

#define NODE_DATA(node_) (ELEM(node_)->data)
#define NODE_TYPE(node_) (ELEM(node_)->type)

inline bool dsl_is_double_equal(const double a, const double b) {
    static const double EPSILON = 0.000001;

    return abs(a - b) < EPSILON;
}

#define IS_DOUBLE_EQ(a_, b_) dsl_is_double_equal(a_, b_)

#define LAST_VAR_TABLE (data->var_tables.data[data->var_tables.size - 1])

//=====================================codegen dsl==================================================

#define ASSIGNMENT_WITH_ACTION(cmd_)                    \
            EVAL_SUBTREE(*L(node)); /* get var value*/  \
            EVAL_SUBTREE(*R(node));                     \
            ASM_PRINT_COMMAND(cmd_)

#define LOGIC(jump_)                \
            EVAL_SUBTREE(*L(node)); \
            EVAL_SUBTREE(*R(node)); \
            LOGIC_COMPARE_(jump_)

#define LOGIC_COMPARE_(jump_)       \
            STATUS_CHECK(asm_logic_compare(file, jump_))

#define EVAL_SUBTREE(node_)         \
            STATUS_CHECK(asm_command_traversal(data, file, node_))

#define ADD_VAR(node_)              \
            STATUS_CHECK(asm_add_var_(data, node_, false))

#define ADD_CONST_VAR(node_)         \
            STATUS_CHECK(asm_add_var_(data, node_, true))

#define ASSIGN_VAR_VAL(node_)       \
            do {                                                                                        \
                bool is_global_ = false;                                                                \
                Var* var_ = (asm_search_var_(&data->var_tables, NODE_DATA(node_).var, &is_global_));    \
                assert(var_);                                                                           \
                STATUS_CHECK(asm_pop_var_value(file, var_->addr_offset, is_global_));                   \
            } while (0)

#define CHECK_VAR_FOR_ASSIGNMENT(node_) \
            STATUS_CHECK(asm_check_var_for_assign_(data, node_))

#define ASM_PRINT_COMMAND(cmd_)     \
            STATUS_CHECK(asm_print_command(file, cmd_))

#define BINARY_MATH(cmd_)           \
            EVAL_SUBTREE(*L(node)); \
            EVAL_SUBTREE(*R(node)); \
            ASM_PRINT_COMMAND(cmd_)

#define UNARY_MATH(cmd_)            \
            EVAL_SUBTREE(*R(node)); \
            ASM_PRINT_COMMAND(cmd_)

#define DAMAGED_TREE()                      \
            tree_is_damaged(&data->tree);   \
            return Status::TREE_ERROR

#define ENTER_SCOPE()                                       \
            if (!asm_create_var_table(&data->var_tables))  \
                return Status::STACK_ERROR

#define EXIT_SCOPE()    \
            STATUS_CHECK(asm_pop_var_table(&data->var_tables))

#define EVAL_FUNC_ARGS(node_, offset_, arg_count_)                                      \
            STATUS_CHECK(asm_eval_func_args_(data, file, node_, offset_, arg_count_,    \
                                             &ELEM(node)->debug_info))

#define FIND_FUNC(num_) \
            (data->func_table.find_func(num_))

#define PROVIDE_FUNC_CALL()    \
            STATUS_CHECK(asm_provide_func_call_(data, file, node))

#define ASM_MAKE_IF(node_)  \
            STATUS_CHECK(asm_make_if_(data, file, node_))

#define ASM_MAKE_IF_ELSE(node_)     \
            STATUS_CHECK(asm_make_if_else_(data, file, node_))

#endif //< #ifndef LANG_DSL_H_
