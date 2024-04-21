#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils/statuses.h"
#include "objects.h"
#include "../../Stack/stack.h"
#include "config.h"
#include TREE_INCLUDE
#include "TreeAddon/TreeAddon.h"
#include "file/file.h"
#include "error_printer/error_printer.h"
#include "asm_print.h"

enum class Arches {
    NONE    = -1,
    SPU     =  0,
  //X86     =  1,
  //X86_BIN =  2,
};

#define FUNC_DEF(name_, ...) \
            Status::Statuses (*name_)(FILE* file, ## __VA_ARGS__) = nullptr;

struct ArchDispatcher {

    Status::Statuses fill_table(Arches arch);

#include "dispatcher_declarations.h"

};

#undef FUNC_DEF
#undef STATIC_FUNC_DEF

#endif //< #ifndef DISPATCHER_H_
