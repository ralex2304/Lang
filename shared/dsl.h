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

#define LAST_VAR_TABLE (data->scopes.data[data->scopes.size - 1])

//=====================================codegen dsl==================================================

#define ASSIGNMENT_WITH_ACTION(cmd_)                            \
            EVAL_SUBTREE_GET_VAL(*L(node)); /* get var value*/  \
            EVAL_SUBTREE_GET_VAL(*R(node));                     \
            ASM_PRINT_COMMAND(0, cmd_ "\n")

#define LOGIC(jump_)                \
            EVAL_SUBTREE_GET_VAL(*L(node)); \
            EVAL_SUBTREE_GET_VAL(*R(node)); \
            LOGIC_COMPARE_(jump_)

#define LOGIC_COMPARE_(jump_)       \
            STATUS_CHECK(asm_logic_compare(file, jump_))

#define EVAL_SUBTREE_GET_VAL(node_)         \
            STATUS_CHECK(asm_command_traversal(data, file, node_, true))

#define EVAL_SUBTREE_NO_VAL(node_)          \
            STATUS_CHECK(asm_command_traversal(data, file, node_, false))

#define EVAL_SUBTREE(node_, is_val_needed_) \
            STATUS_CHECK(asm_command_traversal(data, file, node_, is_val_needed_))

#define ADD_VAR(node_)              \
            STATUS_CHECK(asm_add_var_(data, node_, false))

#define ADD_CONST_VAR(node_)         \
            STATUS_CHECK(asm_add_var_(data, node_, true))

#define ASSIGN_VAR_VAL(node_)       \
            STATUS_CHECK(asm_assign_var(data, file, node_))

#define CHECK_VAR_FOR_ASSIGNMENT(node_) \
            STATUS_CHECK(asm_check_var_for_assign_(data, node_))

#define ASM_PRINT_COMMAND(lvl_change_, ...)     \
            STATUS_CHECK(asm_print_command(lvl_change_, file, __VA_ARGS__))

#define ASM_PRINT_COMMAND_NO_TAB(...)  \
            STATUS_CHECK(asm_print_command_no_tab(file, __VA_ARGS__))

#define BINARY_MATH(cmd_)                   \
            STATUS_CHECK(asm_binary_math_(data, file, node, cmd_, is_val_needed))

#define UNARY_MATH(cmd_)                    \
            STATUS_CHECK(asm_unary_math_(data, file, node, cmd_, is_val_needed))

#define DAMAGED_TREE(err_msg_)                      \
            tree_is_damaged(&data->tree, err_msg_); \
            return Status::TREE_ERROR

#define ENTER_SCOPE(num_)                                   \
            if (!asm_create_scope(&data->scopes, num_)) \
                return Status::STACK_ERROR

#define ENTER_LOOP_SCOPE(num_)                                      \
            if (!asm_create_scope(&data->scopes, num_, true))   \
                return Status::STACK_ERROR

#define EXIT_SCOPE()    \
            STATUS_CHECK(asm_pop_var_table(&data->scopes))

#define EVAL_FUNC_ARGS(node_, offset_, arg_count_)                                      \
            STATUS_CHECK(asm_eval_func_args_(data, file, node_, offset_, arg_count_,    \
                                             &ELEM(node)->debug_info))

#define FIND_FUNC(num_) \
            (data->func_table.find_func(num_))

#define PROVIDE_FUNC_CALL()    \
            STATUS_CHECK(asm_provide_func_call_(data, file, node, is_val_needed))

#define ASM_SET_FPS(value_node_)    \
            STATUS_CHECK(asm_make_set_fps_(data, file, value_node_))

#define ASM_MAKE_IF(node_)  \
            STATUS_CHECK(asm_make_if_(data, file, node_))

#define ASM_MAKE_IF_ELSE(node_)     \
            STATUS_CHECK(asm_make_if_else_(data, file, node_))

#define ASM_MAKE_WHILE(node_)   \
            STATUS_CHECK(asm_make_while_(data, file, node_))

#define ASM_MAKE_WHILE_ELSE(node_)   \
            STATUS_CHECK(asm_make_while_else_(data, file, node_))

#endif //< #ifndef LANG_DSL_H_
