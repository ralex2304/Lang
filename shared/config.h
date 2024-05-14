#ifndef CONFIG_H_
#define CONFIG_H_

#ifndef NDEBUG

#define DEBUG

#endif //< #ifndef NDEBUG

#ifdef DEBUG

#define TREE_INCLUDE "TreeDebug/TreeDebug.h"

#else //< #ifndef DEBUG

#define TREE_INCLUDE "Tree/Tree.h"

#endif //< #ifdef DEBUG

// 8 * 2
#define ASM_TAB "    ""    ""    ""    "

const double ASM_EPSILON = 0.000001;

#endif //< #ifndef CONFIG_H_
