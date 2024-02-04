#ifndef TEXT_PARSER_H_
#define TEXT_PARSER_H_

#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "utils/statuses.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h"
#include "../frontend_objects.h"
#include "utils/vector.h"
#include "error_printer/error_printer.h"

Status::Statuses parse_text(ParseData* data);

namespace TextParser {
    Status::Statuses Main(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses CH_DefFunc(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses CH_DefVar(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses FuncArgsDef(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses CH_Commands(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses CH_FuncCall(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses FuncArgs(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses Command(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses SimpleCommand(ParseData* data, size_t* const pos, TreeNode** dest,
                                   size_t* const size);

    Status::Statuses CH_ComplexCommand(ParseData* data, size_t* const pos, TreeNode** dest,
                                       size_t* const size);

    Status::Statuses Clause(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses ClauseAction(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses CH_SimpleClause(ParseData* data, size_t* const pos, TreeNode** dest,
                                     size_t* const size);

    Status::Statuses CH_PostClause(ParseData* data, size_t* const pos, TreeNode** dest,
                                   size_t* const size);

    Status::Statuses CH_CommandWithArg(ParseData* data, size_t* const pos, TreeNode** dest,
                                       size_t* const size);

    Status::Statuses CH_CommandWithConstArg(ParseData* data, size_t* const pos, TreeNode** dest,
                                            size_t* const size);

    Status::Statuses Expr(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses MathLvl1(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses MathLvl2(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses MathLvl3(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses MathLvl4(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses MathLvl5(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses CH_Binary(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses CH_Unary(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses Primary(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses Prefix(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses Postfix(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses Double(ParseData* data, size_t* const pos, TreeNode** dest, size_t* const size);

    Status::Statuses new_oper_node(TreeNode** dest, OperNum oper, DebugInfo debug_info,
                                   TreeNode* l_child, TreeNode* r_child);

    Status::Statuses new_num_node(TreeNode** dest, double val, DebugInfo debug_info);

    Status::Statuses new_var_node(TreeNode** dest, size_t var_num, DebugInfo debug_info);
};

#endif //< #ifndef TEXT_PARSER_H_
