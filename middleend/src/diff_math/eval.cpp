#include "eval.h"

#include "dsl.h"
#include "../midend_dsl.h"

static Status::Statuses diff_eval_oper_(MidData* data, TreeNode** node, double* result);


Status::Statuses diff_eval(MidData* data, TreeNode** node, double* result) {
    assert(data);
    assert(node);
    assert(*node);
    assert(result);

    switch (*NODE_TYPE(*node)) {
        case TreeElemType::NUM:
            *result = *NUM_VAL(*node);
            break;

        case TreeElemType::OPER:
            STATUS_CHECK(diff_eval_oper_(data, node, result));
            break;

        case TreeElemType::VAR:
        case TreeElemType::NONE:
        default:
            assert(0 && "Invalid DiffElemType given");
            return Status::TREE_ERROR;
    }
    return Status::NORMAL_WORK;
}

static Status::Statuses diff_eval_oper_(MidData* data, TreeNode** node, double* result) {
    assert(data);
    assert(node);
    assert(*node);
    assert(TYPE_IS_OPER(*node));
    assert(result);

    double  left_res = NAN;
    double right_res = NAN;

    if (IS_L_EXIST(*node))
        STATUS_CHECK(diff_eval(data, L(*node), &left_res));

    if (IS_R_EXIST(*node))
        STATUS_CHECK(diff_eval(data, R(*node), &right_res));

    STATUS_CHECK(diff_eval_calc(left_res, right_res, *OPER_NUM(*node), result));

    return Status::NORMAL_WORK;
}

#define OPER_CASE_(oper_, result_)  case OperNum::oper_:    \
                                        *result = result_;  \
                                        break;

Status::Statuses diff_eval_calc(const double left_val, const double right_val,
                                OperNum oper, double* result) {
    assert(result);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
    switch (oper) {

        OPER_CASE_(MATH_ADD,            left_val + right_val);
        OPER_CASE_(MATH_SUB,            left_val - right_val);
        OPER_CASE_(MATH_MUL,            left_val * right_val);
        OPER_CASE_(MATH_DIV,            left_val / right_val);
        OPER_CASE_(MATH_POW,        pow(left_val,  right_val));

        OPER_CASE_(MATH_SQRT,       sqrt(right_val));
        OPER_CASE_(MATH_SIN,        sin( right_val));
        OPER_CASE_(MATH_COS,        cos( right_val));
        OPER_CASE_(MATH_LN,         log( right_val));
        OPER_CASE_(MATH_NEGATIVE,       -right_val);

        OPER_CASE_(LOGIC_GREAT,     (double)((left_val > right_val) && !IS_DOUBLE_EQ(left_val, right_val)));
        OPER_CASE_(LOGIC_LOWER,     (double)((left_val < right_val) && !IS_DOUBLE_EQ(left_val, right_val)));
        OPER_CASE_(LOGIC_GREAT_EQ,  (double)((left_val > right_val) ||  IS_DOUBLE_EQ(left_val, right_val)));
        OPER_CASE_(LOGIC_LOWER_EQ,  (double)((left_val < right_val) ||  IS_DOUBLE_EQ(left_val, right_val)));
        OPER_CASE_(LOGIC_EQUAL,     (double)( IS_DOUBLE_EQ(left_val, right_val)));
        OPER_CASE_(LOGIC_NOT_EQUAL, (double)(!IS_DOUBLE_EQ(left_val, right_val)));

        case OperNum::NONE:
            assert(0 && "Invalid DiffOperNum given");
            return Status::TREE_ERROR;

        default:
            *result = NAN;
            break;
    }
#pragma GCC diagnostic pop

    return Status::NORMAL_WORK;
}
#undef OPER_CASE_
