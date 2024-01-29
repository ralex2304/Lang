#ifndef DIFF_MATH_EVAL_H_
#define DIFF_MATH_EVAL_H_

#include <assert.h>

#include "utils/statuses.h"
#include "objects.h"
#include "../midend_objects.h"
#include "config.h"
#include TREE_INCLUDE

Status::Statuses diff_eval(MidData* diff_data, TreeNode** node, double* result);

Status::Statuses diff_eval_calc(const double left_val, const double right_val,
                                OperNum oper, double* result);

#endif //< #ifndef DIFF_MATH_EVAL_H_
