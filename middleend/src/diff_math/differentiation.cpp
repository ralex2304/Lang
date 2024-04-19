#include "differentiation.h"

#include "dsl.h"
#include "../midend_dsl.h"

static Status::Statuses diff_do_diff_find_diff_operators_(MidData* data, TreeNode** node);

static Status::Statuses diff_do_diff_oper_node_(MidData* data, TreeNode** node);

static Status::Statuses diff_do_diff_addition_(MidData* data, TreeNode** node);

static Status::Statuses diff_do_diff_multiplication_(MidData* data, TreeNode** node);

static Status::Statuses diff_do_diff_vdu_udv_copy_(MidData* data, TreeNode** node, OperNum oper,
                                                   size_t* subtree_size = nullptr);

static Status::Statuses diff_do_diff_copy_and_replace_with_mul_(MidData* data, TreeNode** original,
                                                                TreeNode** copy_dest, size_t* copy_size);

static Status::Statuses diff_do_diff_division_(MidData* data, TreeNode** node);

static Status::Statuses diff_do_diff_division_create_pow_node_(MidData* data, TreeNode** dest,
                                                               TreeNode* parent, TreeNode* src);

static Status::Statuses diff_do_diff_pow_simple_v_(MidData* data, TreeNode** node,
                                TreeNode* u_original, TreeNode* u_copy, const size_t u_size,
                                TreeNode* v_original, TreeNode* v_copy, const size_t v_size);

static Status::Statuses diff_do_diff_pow_not_simple_(MidData* data, TreeNode** node,
                                TreeNode* u_original, TreeNode* u_copy, const size_t u_size,
                                TreeNode* v_original, TreeNode* v_copy, const size_t v_size);

static Status::Statuses diff_do_diff_pow_create_u_pow_v_(MidData* data, TreeNode** node,
                                                           TreeNode* u_original, const size_t u_size,
                                                           TreeNode* v_original, const size_t v_size);

static Status::Statuses diff_do_diff_ln_(MidData* data, TreeNode** node);

static Status::Statuses diff_do_diff_sqrt_(MidData* data, TreeNode** node);

static Status::Statuses diff_do_diff_sin_(MidData* data, TreeNode** node);

static Status::Statuses diff_do_diff_cos_(MidData* data, TreeNode** node);

static Status::Statuses diff_do_diff_complex_func_with_coeff_(MidData* data, TreeNode* node,
                                                              OperNum main_oper, const double coeff,
                                                              OperNum diffed_oper);

static Status::Statuses diff_do_diff_pow_(MidData* data, TreeNode** node);

static Status::Statuses diff_do_diff_prepost_(MidData* data, TreeNode** node);


Status::Statuses diff_do_diff(MidData* data) {
    assert(data);

    STATUS_CHECK(diff_do_diff_find_diff_operators_(data, &data->tree.root));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_find_diff_operators_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);

    if (*node == nullptr)
        return Status::NORMAL_WORK;

    if (!NODE_IS_OPER(*node, OperNum::MATH_DIFF)) {
        STATUS_CHECK(diff_do_diff_find_diff_operators_(data, L(*node)));
        STATUS_CHECK(diff_do_diff_find_diff_operators_(data, R(*node)));

        return Status::NORMAL_WORK;
    }

    ssize_t tmp_arg_var_num = *ARG_VAR_NUM;

    if (!TYPE_IS_VAR(*R(*node)))
        return DAMAGED_TREE("Diff operator must have right var child");

    *ARG_VAR_NUM = *VAR_NUM(*R(*node));

    DO_DIFF(L(*node));

    RECONNECT(node, *L(*node));

    *ARG_VAR_NUM = tmp_arg_var_num;

    return Status::NORMAL_WORK;
}

Status::Statuses diff_do_diff_traversal(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);

    if (TYPE_IS_OPER(*node)) {
        STATUS_CHECK(diff_do_diff_oper_node_(data, node));
        return Status::NORMAL_WORK;
    }

    if (TYPE_IS_VAR(*node) && (ssize_t)*VAR_NUM(*node) == *ARG_VAR_NUM) {
        *NODE_TYPE(*node) = TreeElemType::NUM;
        *NUM_VAL(*node) = 1;
        return Status::NORMAL_WORK;
    }

    *NODE_TYPE(*node) = TreeElemType::NUM;
    *NUM_VAL(*node) = 0;

    return Status::NORMAL_WORK;
}

#define OPER_CASE_(oper_, ...)  case OperNum::oper_:            \
                                    STATUS_CHECK(__VA_ARGS__);  \
                                    break

static Status::Statuses diff_do_diff_oper_node_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(TYPE_IS_OPER(*node));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (*OPER_NUM(*node)) {
        OPER_CASE_(MATH_ADD,      diff_do_diff_addition_(data, node));
        OPER_CASE_(MATH_SUB,      diff_do_diff_addition_(data, node));
        OPER_CASE_(MATH_MUL,      diff_do_diff_multiplication_(data, node));
        OPER_CASE_(MATH_DIV,      diff_do_diff_division_(data, node));
        OPER_CASE_(MATH_POW,      diff_do_diff_pow_(data, node));

        OPER_CASE_(MATH_SQRT,     diff_do_diff_sqrt_(data, node));
        OPER_CASE_(MATH_SIN,      diff_do_diff_sin_(data, node));
        OPER_CASE_(MATH_COS,      diff_do_diff_cos_(data, node));
        OPER_CASE_(MATH_LN,       diff_do_diff_ln_(data, node));

        OPER_CASE_(MATH_NEGATIVE, diff_do_diff_traversal(data, R(*node)));

        OPER_CASE_(PREFIX_ADD,    diff_do_diff_prepost_(data, node));
        OPER_CASE_(PREFIX_SUB,    diff_do_diff_prepost_(data, node));
        OPER_CASE_(POSTFIX_ADD,   diff_do_diff_prepost_(data, node));
        OPER_CASE_(POSTFIX_SUB,   diff_do_diff_prepost_(data, node));

        OPER_CASE_(MATH_DIFF,     diff_do_diff_find_diff_operators_(data, node));

        case OperNum::NONE:
        default:
            assert(0 && "Invalid DiffOperNum given");
            return Status::TREE_ERROR;
    }
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}
#undef OPER_CASE_

static Status::Statuses diff_do_diff_addition_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(*OPER_NUM(*node) == OperNum::MATH_ADD ||
           *OPER_NUM(*node) == OperNum::MATH_SUB);

    DO_DIFF(L(*node));
    DO_DIFF(R(*node));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_multiplication_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(*OPER_NUM(*node) == OperNum::MATH_MUL);

    VdU_UdV_BUILD(OperNum::MATH_ADD);

    DO_DIFF(dU(*node));
    DO_DIFF(dV(*node));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_vdu_udv_copy_(MidData* data, TreeNode** node, OperNum oper,
                                                   size_t* subtree_size) {
    assert(data);
    assert(node);
    assert(*node);

    *OPER_NUM(*node) = oper;

    COPY_AND_REPLACE_WITH_MUL(VdU(*node), u_copy, u_size);

    COPY_AND_REPLACE_WITH_MUL(UdV(*node), v_copy, v_size);

    *V(*node) = INSERT_SUBTREE(*VdU(*node), v_copy, v_size);

    *U(*node) = INSERT_SUBTREE(*UdV(*node), u_copy, u_size);

    if (subtree_size != nullptr)
        // size(u) + size(dv) + size(v) + size(du) + mul_node + mul_node + parent
        *subtree_size = v_size * 2 + u_size * 2 + 2 + 1;

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_copy_and_replace_with_mul_(MidData* data, TreeNode** original,
                                                                TreeNode** copy_dest, size_t* copy_size) {
    assert(data);
    assert(original);
    assert(*original);
    assert(copy_dest);
    assert(*copy_dest == nullptr);
    assert(copy_size);

    COPY_SUBTREE(*original, copy_dest, copy_size);

    TreeNode* original_tmp = UNTIE_SUBTREE(*original, *copy_size);

    TREE_INSERT(original, original_tmp->parent, OPER_ELEM(OperNum::MATH_MUL, *DEBUG_INFO(original_tmp)));

    *L(*original) = INSERT_SUBTREE(*original, original_tmp, *copy_size);

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_division_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(*OPER_NUM(*node) == OperNum::MATH_DIV);

    size_t subtree_size = 0;
    VdU_UdV_BUILD_AND_COUNT_SIZE(OperNum::MATH_SUB, &subtree_size);

    TreeNode* udv_vdu_node = UNTIE_SUBTREE(*node, subtree_size);

    TREE_INSERT(node, udv_vdu_node->parent, OPER_ELEM(OperNum::MATH_DIV, *DEBUG_INFO(udv_vdu_node)));

    *L(*node) = INSERT_SUBTREE(*node, udv_vdu_node, subtree_size);


    STATUS_CHECK(diff_do_diff_division_create_pow_node_(data, R(*node), *node,
                                                        *V(udv_vdu_node)));

    DO_DIFF(dU(*L(*node)));
    DO_DIFF(dV(*L(*node)));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_division_create_pow_node_(MidData* data, TreeNode** dest,
                                                               TreeNode* parent, TreeNode* src) {
    assert(data);
    assert(dest);
    assert(*dest == nullptr);
    assert(parent);
    assert(src);

    TREE_INSERT(dest, parent, OPER_ELEM(OperNum::MATH_POW, *DEBUG_INFO(parent)));

    TREE_INSERT(R(*dest), *dest, NUM_ELEM(2, *DEBUG_INFO(parent)));

    TreeNode* copy = nullptr;
    size_t copy_size = 0;
    COPY_SUBTREE(src, &copy, &copy_size);

    *L(*dest) = INSERT_SUBTREE(*dest, copy, copy_size);

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_pow_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(*OPER_NUM(*node) == OperNum::MATH_POW);

    DebugInfo debug_info = *DEBUG_INFO(*node);

    // u^v

    TreeNode* u_copy = nullptr;
    size_t u_size = 0;
    bool u_is_simple = false;
    COPY_SUBTREE_AND_CHECK_SIMPLICITY(*L(*node), &u_copy, &u_size, &u_is_simple);
    TreeNode* u_original = UNTIE_SUBTREE(*L(*node), u_size);


    TreeNode* v_copy = nullptr;
    size_t v_size = 0;
    bool v_is_simple = false;
    COPY_SUBTREE_AND_CHECK_SIMPLICITY(*R(*node), &v_copy, &v_size, &v_is_simple);
    TreeNode* v_original = UNTIE_SUBTREE(*R(*node), v_size);

    if (u_is_simple && v_is_simple) { //< n^n
        DELETE_UNTIED_SUBTREE(&u_copy, u_size);
        DELETE_UNTIED_SUBTREE(&v_copy, v_size);
        DELETE_UNTIED_SUBTREE(&u_original, u_size); //< size value turns to 0 there,
        DELETE_UNTIED_SUBTREE(&v_original, v_size); //< so value isn't important

        TREE_REPLACE_SUBTREE_WITH_NUM(node, 0, debug_info);

        return Status::NORMAL_WORK;
    }

    *OPER_NUM(*node) = OperNum::MATH_MUL;

    if (v_is_simple) { //< x^n
        STATUS_CHECK(diff_do_diff_pow_simple_v_(data, node, u_original, u_copy, u_size,
                                                                 v_original, v_copy, v_size));
        return Status::NORMAL_WORK;
    }

    // n^x or x^x
    STATUS_CHECK(diff_do_diff_pow_not_simple_(data, node, u_original, u_copy, u_size,
                                                               v_original, v_copy, v_size));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_pow_simple_v_(MidData* data, TreeNode** node,
                                TreeNode* u_original, TreeNode* u_copy, const size_t u_size,
                                TreeNode* v_original, TreeNode* v_copy, const size_t v_size) {
    assert(data);
    assert(node);
    assert(*node);
    assert(u_original);
    assert(u_copy);
    assert(v_original);
    assert(v_copy);
    // x^n

    *L(*node) = INSERT_SUBTREE(*node, v_copy, v_size);

    TREE_INSERT(R(*node), *node, OPER_ELEM(OperNum::MATH_MUL, *DEBUG_INFO(*node)));
    // result: v * (*)

    *R(*R(*node)) = INSERT_SUBTREE(*R(*node), u_copy, u_size);

    TreeNode**  pow_node = L(*R(*node));
    TREE_INSERT(pow_node, *R(*node), OPER_ELEM(OperNum::MATH_POW, *DEBUG_INFO(*node)));
    // result: v * ((^) * u')

    *L(*pow_node) = INSERT_SUBTREE(*pow_node, u_original, u_size);

    TREE_INSERT(R(*pow_node), *pow_node, OPER_ELEM(OperNum::MATH_SUB, *DEBUG_INFO(*node)));
    // result: v * ((u ^ (-)) * u')

    *L(*R(*pow_node)) = INSERT_SUBTREE(*R(*pow_node), v_original, v_size);

    TREE_INSERT(R(*R(*pow_node)), *R(*pow_node), NUM_ELEM(1, *DEBUG_INFO(*node)));
    // result: v * ((u ^ (v - 1)) * u'

    DO_DIFF(R(*R(*node)));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_pow_not_simple_(MidData* data, TreeNode** node,
                                TreeNode* u_original, TreeNode* u_copy, const size_t u_size,
                                TreeNode* v_original, TreeNode* v_copy, const size_t v_size) {
    assert(data);
    assert(node);
    assert(*node);
    assert(u_original);
    assert(u_copy);
    assert(v_original);
    assert(v_copy);
    // n^x or x^x

    STATUS_CHECK(diff_do_diff_pow_create_u_pow_v_(data, node, u_original, u_size,
                                                              v_original, v_size));

    TreeNode** r_node = R(*node);

    TREE_INSERT(r_node, *node, OPER_ELEM(OperNum::MATH_MUL, *DEBUG_INFO(*node)));

    *L(*r_node) = INSERT_SUBTREE(*r_node, v_copy, v_size);


    TreeNode** ln_node = R(*r_node);

    TREE_INSERT(ln_node, *r_node, OPER_ELEM(OperNum::MATH_LN, *DEBUG_INFO(*node)));

    *R(*ln_node) = INSERT_SUBTREE(*ln_node, u_copy, u_size);;

    DO_DIFF(R(*node));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_pow_create_u_pow_v_(MidData* data, TreeNode** node,
                                                           TreeNode* u_original, const size_t u_size,
                                                           TreeNode* v_original, const size_t v_size) {
    assert(data);
    assert(node);
    assert(*node);
    assert(u_original);
    assert(v_original);

    TREE_INSERT(L(*node), *node, OPER_ELEM(OperNum::MATH_POW, *DEBUG_INFO(*node)));

    *L(*L(*node)) = INSERT_SUBTREE(*L(*node), u_original, u_size);

    *R(*L(*node)) = INSERT_SUBTREE(*L(*node), v_original, v_size);

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_ln_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(*OPER_NUM(*node) == OperNum::MATH_LN);

    *OPER_NUM(*node) = OperNum::MATH_DIV;

    TreeNode* copy = nullptr;
    size_t copy_size = 0;
    COPY_SUBTREE(*R(*node), &copy, &copy_size);

    *L(*node) = INSERT_SUBTREE(*node, copy, copy_size);

    DO_DIFF(L(*node));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_sqrt_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(*OPER_NUM(*node) == OperNum::MATH_SQRT);

    STATUS_CHECK(diff_do_diff_complex_func_with_coeff_(data, *node, OperNum::MATH_DIV, 2,
                                                                    OperNum::MATH_SQRT));

    DO_DIFF(L(*node));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_sin_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(*OPER_NUM(*node) == OperNum::MATH_SIN);

    *OPER_NUM(*node) = OperNum::MATH_MUL;

    TreeNode* copy = nullptr;
    size_t size = 0;
    COPY_SUBTREE(*R(*node), &copy, &size);

    TreeNode* original = UNTIE_SUBTREE(*R(*node), size);
    *L(*node) = INSERT_SUBTREE(*node, copy, size);

    TREE_INSERT(R(*node), *node, OPER_ELEM(OperNum::MATH_COS, *DEBUG_INFO(*node)));

    *R(*R(*node)) = INSERT_SUBTREE(*R(*node), original, size);

    DO_DIFF(L(*node));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_cos_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(NODE_IS_OPER(*node, OperNum::MATH_COS));

    STATUS_CHECK(diff_do_diff_complex_func_with_coeff_(data, *node, OperNum::MATH_MUL, -1,
                                                                    OperNum::MATH_SIN));

    DO_DIFF(L(*node));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_prepost_(MidData* data, TreeNode** node) {
    assert(data);
    assert(node);
    assert(*node);
    assert(NODE_IS_OPER(*node, OperNum::PREFIX_ADD)  || NODE_IS_OPER(*node, OperNum::PREFIX_SUB) ||
           NODE_IS_OPER(*node, OperNum::POSTFIX_ADD) || NODE_IS_OPER(*node, OperNum::POSTFIX_ADD));

    double result = NAN;

    assert(TYPE_IS_VAR(*L(*node)));
    if ((ssize_t)*VAR_NUM(*L(*node)) == *ARG_VAR_NUM) result = 1;
    else                                              result = 0;

    TREE_REPLACE_SUBTREE_WITH_NUM(node, result, *DEBUG_INFO(*node));

    return Status::NORMAL_WORK;
}

static Status::Statuses diff_do_diff_complex_func_with_coeff_(MidData* data, TreeNode* node,
                                                              OperNum main_oper, const double coeff,
                                                              OperNum diffed_oper) {
    assert(data);
    assert(node);

    *OPER_NUM(node) = main_oper;


    TreeNode* copy = nullptr;
    size_t size = 0;
    COPY_SUBTREE(*R(node), &copy, &size);

    TreeNode* original = UNTIE_SUBTREE(*R(node), size);

    *L(node) = INSERT_SUBTREE(node, copy, size);

    TREE_INSERT(R(node), node, OPER_ELEM(OperNum::MATH_MUL, *DEBUG_INFO(node)));


    TREE_INSERT(L(*R(node)), *R(node), NUM_ELEM(coeff, *DEBUG_INFO(node)));

    TREE_INSERT(R(*R(node)), *R(node), OPER_ELEM(diffed_oper, *DEBUG_INFO(node)));

    *R(*R(*R(node))) = INSERT_SUBTREE(*R(*R(node)), original, size);

    return Status::NORMAL_WORK;
}

