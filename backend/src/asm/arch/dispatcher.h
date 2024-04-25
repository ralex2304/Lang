#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include "utils/statuses.h"
#include "objects.h"
#include "../asm_objects.h"


#define FUNC_DEF(name_, ...) \
            Status::Statuses (*name_)(AsmData* asm_d, ## __VA_ARGS__) = nullptr;

struct ArchDispatcher {

    Status::Statuses fill_table(const Arches arch);

#include "dispatcher_declarations.h"

};

#undef FUNC_DEF
#undef STATIC_FUNC_DEF

#endif //< #ifndef DISPATCHER_H_
