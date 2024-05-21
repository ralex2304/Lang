#ifndef LANG_DSL_H_
#define LANG_DSL_H_

#include "config.h"
#include TREE_INCLUDE

#include <math.h>

#define NODE_IS_OPER(node_, oper_)  (node_ != nullptr && TYPE_IS_OPER(node_)    \
                                     && NODE_DATA(node_)->oper == oper_)

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

#define DEBUG_INFO(node_) (&ELEM(node_)->debug_info)

inline bool dsl_is_double_equal(const double a, const double b) {
    static const double EPSILON = 0.000001;

    return abs(a - b) < EPSILON;
}

#define NUM_ELEM(val_,     debug_)  {.type = TreeElemType::NUM,  .data = {.num  = val_},    .debug_info = debug_}
#define OPER_ELEM(oper_,   debug_)  {.type = TreeElemType::OPER, .data = {.oper = oper_},   .debug_info = debug_}
#define VAR_ELEM(var_num_, debug_)  {.type = TreeElemType::VAR,  .data = {.var = var_num_}, .debug_info = debug_}

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

#define TREE_INSERT(dest_, parent_, elem_)                                              \
            do {                                                                        \
                TreeElem new_elem_ = elem_;                                             \
                if (tree_insert(&data->tree, dest_, parent_, &new_elem_) != Tree::OK)   \
                    return Status::TREE_ERROR;                                          \
            } while(0)

#define TREE_DELETE_NODE(node_)                                         \
            do {                                                        \
                if (tree_delete(&data->tree, node_, false) != Tree::OK) \
                    return Status::TREE_ERROR;                          \
            } while(0)

#define TREE_DELETE_SUBTREE(node_)                                      \
            do {                                                        \
                if (tree_delete(&data->tree, node_, true) != Tree::OK)  \
                    return Status::TREE_ERROR;                          \
            } while(0)

#define SYNTAX_ERROR(...) syntax_error(*DEBUG_INFO(node), __VA_ARGS__)

//=====================================codegen dsl==================================================

#define ASSIGNMENT_WITH_ACTION(cmd_)                            \
            EVAL_SUBTREE_GET_VAL(*R(node));                     \
            EVAL_SUBTREE_GET_VAL(*L(node)); /* get var value*/  \
            GEN_SWAP_LAST_STK_VALS();                           \
            GEN_MATH_OPERATOR(cmd_)

#define GEN_SWAP_LAST_STK_VALS()    \
            STATUS_CHECK(ir_block_swap_last_stk_vals(&data->ir_d))

#define VAR_DEFINITION_ASSIGNMENT(node_)            \
            do {                                    \
                if (*R(node_) == nullptr)           \
                    break;                          \
                                                    \
                EVAL_SUBTREE_GET_VAL(*R(node_));    \
                                                    \
                ASSIGN_VAR_VAL(*L(node_));          \
            } while (0)

#define ARRAY_DEFINITION_ASSIGNMENT(node_)          \
            if (*R(node_) != nullptr)               \
                STATUS_CHECK(array_definition_assignment_(data, node_))

#define LOGIC(jump_)                \
            EVAL_SUBTREE_GET_VAL(*L(node)); \
            EVAL_SUBTREE_GET_VAL(*R(node)); \
            LOGIC_COMPARE_(NODE_DATA(node)->oper)

#define LOGIC_COMPARE_(jump_)       \
            STATUS_CHECK(ir_block_logic_compare(&data->ir_d, jump_))

#define EVAL_SUBTREE_GET_VAL(node_)         \
            STATUS_CHECK(ir_command_traversal(data, node_, true))

#define EVAL_SUBTREE_NO_VAL(node_)          \
            STATUS_CHECK(ir_command_traversal(data, node_, false))

#define EVAL_SUBTREE(node_, is_val_needed_) \
            STATUS_CHECK(ir_command_traversal(data, node_, is_val_needed_))

#define ADD_NUM_VAR(node_)          \
            STATUS_CHECK(add_num_var_(data, node_, false))

#define ADD_NUM_CONST_VAR(node_)    \
            STATUS_CHECK(add_num_var_(data, node_, true))

#define ADD_ARRAY(node_)            \
            STATUS_CHECK(add_array_(data, node_, false))

#define ADD_CONST_ARRAY(node_)      \
            STATUS_CHECK(add_array_(data, node_, true))

#define ASSIGN_VAR_VAL(node_)                                               \
            do {                                                            \
                if (NODE_IS_OPER(node_, OperNum::ARRAY_ELEM)) {             \
                    EVAL_SUBTREE_GET_VAL(*R(node_));                        \
                    STATUS_CHECK(local_assign_arr_elem(data, node_));       \
                } else                                                      \
                    STATUS_CHECK(local_assign_var(data, node_));            \
            } while (0)

#define ASSIGN_VAR_VAL_SAME(node_)                                                      \
            do {                                                                        \
                if (NODE_IS_OPER(node_, OperNum::ARRAY_ELEM))                           \
                    STATUS_CHECK(ir_block_pop_arr_elem_value_the_same(&data->ir_d));   \
                else                                                                    \
                    STATUS_CHECK(local_assign_var(data, node_));                        \
            } while (0)

#define CHECK_VAR_FOR_ASSIGNMENT(node_) \
            STATUS_CHECK(check_var_for_assign_(data, node_))

#define BINARY_MATH()   \
            STATUS_CHECK(binary_math_(data, node, NODE_DATA(node)->oper, is_val_needed))

#define UNARY_MATH()    \
            STATUS_CHECK(unary_math_(data, node, NODE_DATA(node)->oper, is_val_needed))

#define GEN_MATH_OPERATOR(oper_)    \
            STATUS_CHECK(ir_block_math_operator(&data->ir_d, oper_));

#define GEN_READ_DOUBLE()   \
            STATUS_CHECK(ir_block_read_double(&data->ir_d, is_val_needed))

#define GEN_PRINT_DOUBLE()  \
            STATUS_CHECK(ir_block_print_double(&data->ir_d))

#define GEN_RET()   \
            STATUS_CHECK(ir_block_ret(&data->ir_d))

#define GEN_WRITE_RETURNED_VALUE()  \
            STATUS_CHECK(ir_block_write_returned_value(&data->ir_d))

#define DAMAGED_TREE(err_msg_)  \
            tree_is_damaged(&data->tree, err_msg_);

#define ENTER_SCOPE(num_)                                   \
            if (!local_create_scope(&data->scopes, num_))   \
                return Status::STACK_ERROR

#define ENTER_LOOP_SCOPE(num_)                                  \
            if (!local_create_scope(&data->scopes, num_, true)) \
                return Status::STACK_ERROR

#define EXIT_SCOPE()    \
            STATUS_CHECK(local_pop_var_table(&data->scopes))

#define SCOPE_RESET_VARS()                                              \
            if (!data->scopes.data[data->scopes.size - 1].reset_vars()) \
                return Status::MEMORY_EXCEED

#define SCOPE_SET_TYPE(type_)    \
            data->scopes.data[data->scopes.size - 1].type = type_

#define EVAL_FUNC_ARGS(node_, offset_, arg_count_)  \
            STATUS_CHECK(eval_func_args_(data, node_, offset_, arg_count_, DEBUG_INFO(node_)))

#define FIND_FUNC(num_) \
            (data->func_table.find_func(num_))

#define PROVIDE_FUNC_CALL()    \
            STATUS_CHECK(provide_func_call_(data, node, is_val_needed))

#define GET_ARR_ELEM_VAL()      \
            STATUS_CHECK(get_arr_elem_val_(data, node, is_val_needed))

#define GEN_SET_FPS(value_node_)    \
            STATUS_CHECK(make_set_fps_(data, value_node_))

#define GEN_VIDEO_SHOW_FRAME()  \
            STATUS_CHECK(ir_block_video_show_frame(&data->ir_d));

#define GEN_MAKE_IF(node_)  \
            STATUS_CHECK(make_if_(data, node_))

#define GEN_MAKE_IF_ELSE(node_)     \
            STATUS_CHECK(make_if_else_(data, node_))

#define GEN_MAKE_WHILE(node_)   \
            STATUS_CHECK(make_while_(data, node_))

#define GEN_MAKE_WHILE_ELSE(node_)   \
            STATUS_CHECK(make_while_else_(data, node_))

#define GEN_MAKE_BREAK(node_)       \
            STATUS_CHECK(make_break_(data, node_))

#define GEN_MAKE_CONTINUE(node_)    \
            STATUS_CHECK(make_continue_(data, node_))

#define GEN_MAKE_DO_WHILE(node_)    \
            STATUS_CHECK(make_do_while_(data, node_))

#define GEN_MAKE_DO_IF(node_)   \
            STATUS_CHECK(make_do_if_(data, node_))

#define PREFIX_OPER(oper_)  \
            STATUS_CHECK(make_prefix_oper_(data, node, oper_, is_val_needed))

#define POSTFIX_OPER(oper_) \
            STATUS_CHECK(make_postfix_oper_(data, node, oper_, is_val_needed))

#endif //< #ifndef LANG_DSL_H_
