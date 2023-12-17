#ifndef CONFIG_H_
#define CONFIG_H_

#define DEBUG

#ifdef DEBUG

#define TREE_INCLUDE "TreeDebug/TreeDebug.h"

#else //< #ifndef DEBUG

#define TREE_INCLUDE "Tree/Tree.h"

#endif //< #ifdef DEBUG

#endif //< #ifndef CONFIG_H_
