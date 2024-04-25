#ifndef MIDEND_DSL_H_
#define MIDEND_DSL_H_

#include "dsl.h"

#define VdU(node_) L(node_)
#define UdV(node_) R(node_)

#define V(node_)  R(*VdU(node_))
#define U(node_)  R(*UdV(node_))
#define dU(node_) L(*VdU(node_))
#define dV(node_) L(*UdV(node_))

#define VdU_UdV_BUILD(type_) STATUS_CHECK(diff_do_diff_vdu_udv_copy_(data, node, type_))

#define VdU_UdV_BUILD_AND_COUNT_SIZE(type_, size_ptr_)                                  \
            STATUS_CHECK(diff_do_diff_vdu_udv_copy_(data, node, type_, size_ptr_))

#define DO_DIFF(node_) STATUS_CHECK(diff_do_diff_traversal(data, node_))

#define NODE_VAL(node_) (TYPE_IS_NUM(node_) ? NUM_VAL(node_) : (TYPE_IS_VAR(node_) ? VAR_VAL(node_) : nullptr))

#define ROOT (&data->tree.root)

#define NODE_IS_ROOT(node_) (node_ == *ROOT)

#define PARENT(node_) ((node_)->parent)

#define IS_L_CHILD(node_) (PARENT(node_) != nullptr && *L(PARENT(node_)) == node_)
#define IS_R_CHILD(node_) (PARENT(node_) != nullptr && *R(PARENT(node_)) == node_)

#define IS_ADD_OR_SUB(node_) (TYPE_IS_OPER(node_) && OPER_NUM_ADD_OR_SUB(*OPER_NUM(node_)))

#define IS_L_CHILD_OF_POW(node_) (!NODE_IS_ROOT(node_) && *OPER_NUM(PARENT(node_)) == DiffOperNum::POW && IS_L_CHILD(node_))

#define ARG_VAR_NUM &data->argument_var_num


#define COPY_AND_REPLACE_WITH_MUL(target_, copy_, size_)                                            \
            TreeNode* copy_ = nullptr;                                                              \
            size_t size_ = 0;                                                                       \
                                                                                                    \
            STATUS_CHECK(diff_do_diff_copy_and_replace_with_mul_(data, target_, &copy_, &size_))

#define INSERT_SUBTREE(parent_, src_, size_)    \
            /* = */ src_;                       \
            data->tree.size += size_;           \
            (src_)->parent = parent_

#define COPY_SUBTREE(src_, dest_, size_)                        \
            STATUS_CHECK(tree_copy_subtree(src_, dest_, size_))

#define COPY_SUBTREE_AND_CHECK_SIMPLICITY(src_, dest_, size_, is_simple_)   \
            STATUS_CHECK(tree_copy_subtree(src_, dest_, size_, is_simple_))

#define UNTIE_SUBTREE(node_, size_)     \
            /* = */ node_;              \
            data->tree.size -= size_;   \
            node_ = nullptr

#define DELETE_UNTIED_SUBTREE(copy_, size_)                     \
            do {                                                \
                STATUS_CHECK(tree_dtor_untied_subtree(copy_));  \
                size_ = 0;                                      \
            } while(0)


#define RECONNECT(dest_, src_)  STATUS_CHECK(tree_reconnect_node(&data->tree, dest_, src_))

#define SIMPLIFY_EVAL(node_)    STATUS_CHECK(diff_simplify_eval_subtree_(data, node_))

#define TREE_REPLACE_SUBTREE_WITH_NUM(node_, val_, debug_)                  \
            do {                                                            \
                TreeNode* parent_ = (*(node_))->parent;                     \
                DebugInfo debug_info_ = debug_;                             \
                                                                            \
                TREE_DELETE_SUBTREE(node_);                                 \
                TREE_INSERT(node_, parent_, NUM_ELEM(val_, debug_info_));   \
            } while(0)

#define TREE_CHECK(action_, ...)    do {                                \
                                        if (action_ != Tree::OK) {      \
                                            __VA_ARGS__;                \
                                            return Status::TREE_ERROR;  \
                                        }                               \
                                    } while(0)

#define CLEAR_TREE(tree_, ...)  do {                                    \
                                    DSL_TREE_DTOR(tree_, __VA_ARGS__);  \
                                    DSL_TREE_CTOR(tree_, __VA_ARGS__);  \
                                } while(0)

#endif //< #ifndef MIDEND_DSL_H_
