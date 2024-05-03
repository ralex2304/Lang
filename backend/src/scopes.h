#ifndef VAR_TABLE_H_
#define VAR_TABLE_H_

#include <assert.h>
#include <stdlib.h>

#include "utils/vector.h"

#include "ir_gen/ir_gen_objects.h"

enum class VarType {
    NONE  = 0,
    NUM   = 1,
    ARRAY = 2,
};

struct Var {
    size_t var_num = 0;

    VarType type = VarType::NONE;
    size_t size = 1;

    bool is_const = true;
    size_t addr_offset = 0;
};

enum class ScopeType {
    NONE    = -1,
    GLOBAL  =  1,
    NEUTRAL =  2,
    LOOP    =  3,
};

struct ScopeData {
    bool is_initialised = false;

    ScopeType type = ScopeType::NONE;

    IRScopeData ir_scope_data = {};

    Vector vars = {};

    size_t size = 0;

    inline bool ctor(ScopeType type_) {
        static size_t counter = 0;
        ir_scope_data.scope_num = counter++;

        assert(type_ != ScopeType::NONE);

        if (!vars.ctor(sizeof(Var)))
            return false;

        if (!ir_scope_data.ctor())
            return false;

        type = type_;

        is_initialised = true;
        return true;
    };

    inline void dtor() {
        vars.dtor();
        ir_scope_data.dtor();
        is_initialised = false;
        type = ScopeType::NONE;
        ir_scope_data.scope_num = 0;
    };

    inline bool reset_vars() {
        vars.dtor();

        if (!vars.ctor(sizeof(Var)))
            return false;

        return true;
    };

    inline Var* find_var(size_t var_num) {
        for (ssize_t i = 0; i < vars.size(); i++)
            if (((Var*)vars[(size_t)i])->var_num == var_num)
                return (Var*)vars[(size_t)i];

        return nullptr;
    };
};

struct Func {
    size_t func_num = 0;
    size_t arg_num = 0;

    ssize_t addr = -1;

    Vector addr_fixups = {};

    bool ctor() {
        return addr_fixups.ctor(sizeof(size_t));
    };

    void dtor() {
        addr_fixups.dtor();
    };

    bool set_addr_or_add_to_fixups(size_t* func_addr, const size_t fixup_addr) {
        assert(func_addr);
        *func_addr = (size_t)addr;

        if (addr < 0)
            return addr_fixups.push_back(&fixup_addr);

        return true;
    };
};

struct FuncTable {
    bool is_initialised = false;

    Vector funcs = {};

    inline bool ctor() {
        if (!funcs.ctor(sizeof(Func)))
            return false;

        for (ssize_t i = 0; i < funcs.size(); i++)
            if (!((Func*)funcs[(size_t)i])->ctor())
                return false;

        is_initialised = true;
        return true;
    };

    inline void dtor() {
        for (ssize_t i = 0; i < funcs.size(); i++)
            ((Func*)funcs[(size_t)i])->dtor();

        funcs.dtor();
        is_initialised = false;
    }

    inline Func* find_func(size_t func_num) {
        for (ssize_t i = 0; i < funcs.size(); i++)
            if (((Func*)funcs[(size_t)i])->func_num == func_num)
                return (Func*)funcs[(size_t)i];

        return nullptr;
    };
};

#endif //< #ifndef VAR_TABLE_H_
