#ifndef LANG_DSL_H_
#define LANG_DSL_H_

#include "objects.h"
#include "config.h"
#include TREE_INCLUDE

#include <math.h>

#define NODE_IS_OPER(node_, oper_)  (TYPE_IS_OPER(node_) && NODE_DATA(node_)->oper == oper_)

#define L(node_) (&(node_)->left)
#define R(node_) (&(node_)->right)

#define IS_L_EXIST(node_) (*L(node_) != nullptr)
#define IS_R_EXIST(node_) (*R(node_) != nullptr)

#define ELEM(node_)  ((TreeElem*)((node_)->elem))

#define NODE_DATA(node_) (&ELEM(node_)->data)
#define NODE_TYPE(node_) (&ELEM(node_)->type)

#define NUM_VAL(node_)      (&NODE_DATA(node_)->num)
#define VAR_NUM(node_)      (&NODE_DATA(node_)->var)
#define OPER_NUM(node_)     (&NODE_DATA(node_)->oper)

#define TYPE_IS_NUM(node_)  (*NODE_TYPE(node_) == TreeElemType::NUM)
#define TYPE_IS_VAR(node_)  (*NODE_TYPE(node_) == TreeElemType::VAR)
#define TYPE_IS_OPER(node_) (*NODE_TYPE(node_) == TreeElemType::OPER)

inline bool dsl_is_double_equal(const double a, const double b) {
    static const double EPSILON = 0.000001;

    return abs(a - b) < EPSILON;
}

#define IS_DOUBLE_EQ(a_, b_) dsl_is_double_equal(a_, b_)

#define NODE_VAL_EQUALS(node_, val_) (TYPE_IS_NUM(node_) && IS_DOUBLE_EQ(*NUM_VAL(node_), val_))

#define LAST_VAR_TABLE (data->scopes.data[data->scopes.size - 1])

#define TREE_CHECK(action_, ...)    do {                                \
                                        if (action_ != Tree::OK) {      \
                                            __VA_ARGS__;                \
                                            return Status::TREE_ERROR;  \
                                        }                               \
                                    } while(0)

#define DSL_TREE_CTOR(tree_, ...) TREE_CHECK(TREE_CTOR(tree_, sizeof(TreeElem), &tree_elem_dtor, \
                                                  &tree_elem_verify, &tree_elem_str_val), __VA_ARGS__)

#define DSL_TREE_DTOR(tree_, ...) TREE_CHECK(tree_dtor(tree_), __VA_ARGS__)

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

#define ASM_PRINT_COMMAND(lvl_change_, ...)                                     \
            do {                                                                \
                if (AsmPrint::asm_printf(lvl_change_, file, __VA_ARGS__) < 0)   \
                    return Status::OUTPUT_ERROR;                                \
            } while (0)

#define ASM_PRINT_COMMAND_NO_TAB(...)                                           \
            do {                                                                \
                if (AsmPrint::asm_printf_with_tab(0, file, __VA_ARGS__) < 0)    \
                    return Status::OUTPUT_ERROR;                                \
            } while (0)

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

#define ASM_MAKE_BREAK(node_)       \
            STATUS_CHECK(asm_make_break_(data, file, node_))

#define ASM_MAKE_CONTINUE(node_)    \
            STATUS_CHECK(asm_make_continue_(data, file, node_))

#define ASM_MAKE_DO_WHILE(node_)    \
            STATUS_CHECK(asm_make_do_while_(data, file, node_))

#define ASM_MAKE_DO_IF(node_)   \
            STATUS_CHECK(asm_make_do_if_(data, file, node_))

#define PREFIX_OPER(oper_)  \
            STATUS_CHECK(asm_make_prefix_oper_(data, file, node, oper_, is_val_needed))

#define POSTFIX_OPER(oper_) \
            STATUS_CHECK(asm_make_postfix_oper_(data, file, node, oper_, is_val_needed))

#endif //< #ifndef LANG_DSL_H_
