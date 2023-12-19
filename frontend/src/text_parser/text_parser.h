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
#include "syntax_error/syntax_error.h"

struct Subtree {
    TreeNode* parent = nullptr;
    TreeNode* node = nullptr;

    size_t size = 0;
};


namespace TextParser {

}

#endif //< #ifndef TEXT_PARSER_H_
