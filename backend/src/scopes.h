#ifndef VAR_TABLE_H_
#define VAR_TABLE_H_

#include <assert.h>
#include <stdlib.h>

#include "objects.h"
#include "utils/vector.h"
#include "utils/statuses.h"

struct Var {
    size_t var_num = 0;

    bool is_const = true;
    size_t addr_offset = 0;
};

enum class ScopeType {
    NONE    = -1,
    GLOBAL  = 1,
    NEUTRAL = 2,
    LOOP    = 3,
};

struct ScopeData {
    bool is_initialised = false;

    ScopeType type = ScopeType::NONE;
    size_t scope_num = 0;

    Vector vars = {};

    inline bool ctor(ScopeType type_) {
        static size_t counter = 0;
        scope_num = counter++;

        assert(type_ != ScopeType::NONE);

        if (!vars.ctor(sizeof(Var)))
            return false;

        type = type_;

        is_initialised = true;
        return true;
    };

    inline void dtor() {
        vars.dtor();
        is_initialised = false;
        type = ScopeType::NONE;
        scope_num = 0;
    };

    inline Var* find_var(size_t var_num) {
        for (ssize_t i = 0; i < vars.size(); i++)
            if (((Var*)vars[i])->var_num == var_num)
                return (Var*)vars[i];

        return nullptr;
    };
};

struct Func {
    size_t func_num = 0;
    size_t arg_num = 0;
};

struct FuncTable {
    bool is_initialised = false;

    Vector funcs = {};

    inline bool ctor() {
        if (!funcs.ctor(sizeof(Func)))
            return false;

        is_initialised = true;
        return true;
    };

    inline void dtor() {
        funcs.dtor();
        is_initialised = false;
    }

    inline Func* find_func(size_t func_num) {
        for (ssize_t i = 0; i < funcs.size(); i++)
            if (((Func*)funcs[i])->func_num == func_num)
                return (Func*)funcs[i];

        return nullptr;
    };
};

#endif //< #ifndef VAR_TABLE_H_
