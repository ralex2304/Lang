#include "optimisations.h"

#include "List/list.h"
#include "../Stack/stack.h"

static Status::Statuses push_pop_optimisation_(IRBackData* data);

static Status::Statuses push_pop_opt_block_(IRBackData* data, ssize_t block_i, Stack* pushes_stk);

static Status::Statuses is_data_changing_between_ir_blocks_(IRBackData* data, ssize_t push_block_i,
                                                            ssize_t pop_block_i, bool* is_changing) ;

static void push_pop_opt_collapse_block_(IRNode* block);

static Status::Statuses push_pop_opt_change_blocks_(IRBackData* data, ssize_t push_block_i,
                                                    ssize_t pop_block_i, size_t pop_src_num);


Status::Statuses ir_optimise(IRBackData* data) {
    assert(data);

    STATUS_CHECK(push_pop_optimisation_(data));

    return Status::NORMAL_WORK;
}

static Status::Statuses push_pop_optimisation_(IRBackData* data) {
    assert(data);

    Stack pushes_stk = {};
    STK_CHECK(STK_CTOR(&pushes_stk));

    ssize_t phys_i = list_head(&data->ir);
    ssize_t log_i = 0;
    LIST_FOREACH(data->ir, phys_i, log_i)
        STATUS_CHECK(push_pop_opt_block_(data, phys_i, &pushes_stk),    stk_dtor(&pushes_stk));

    STK_CHECK(stk_dtor(&pushes_stk));

    return Status::NORMAL_WORK;
}

static Status::Statuses push_pop_opt_block_parse_src_(IRBackData* data, ssize_t pop_block_i,
                                                      Stack* pushes_stk, size_t src_num) {
    assert(data);
    assert(pushes_stk);
    assert(src_num < 2);

    if (data->ir.arr[pop_block_i].elem.src[src_num].type != IRVal::STK)
        return Status::NORMAL_WORK;

    ssize_t push_block_i = -1;
    STK_CHECK(stk_pop(pushes_stk, &push_block_i));

    bool is_changing = true;
    STATUS_CHECK(is_data_changing_between_ir_blocks_(data, push_block_i, pop_block_i, &is_changing));

    if (!is_changing)
        STATUS_CHECK(push_pop_opt_change_blocks_(data, push_block_i, pop_block_i, src_num));

    return Status::NORMAL_WORK;
}

static Status::Statuses push_pop_opt_block_(IRBackData* data, ssize_t block_i, Stack* pushes_stk) {
    assert(data);
    assert(pushes_stk);
    assert(0 <= block_i && block_i < data->ir.capacity);

    IRNode* block = &data->ir.arr[block_i].elem;

    if (block->type == IRNodeType::SWAP) //< swap is too complex for such simple optimisations
        return Status::NORMAL_WORK;

    STATUS_CHECK(push_pop_opt_block_parse_src_(data, block_i, pushes_stk, 1)); //< src[1]
    STATUS_CHECK(push_pop_opt_block_parse_src_(data, block_i, pushes_stk, 0)); //< src[0]

    if (block->dest.type == IRVal::STK)
        STK_CHECK(stk_push(pushes_stk, block_i));

    return Status::NORMAL_WORK;
}

inline bool is_irval_compatiable(const IRBackData* data, const IRNodeType block_type,
                                 const IRArgType arg_type, const IRVal* val) {
    assert(data);
    assert(block_type != IRNodeType::DEFAULT);
    assert((const size_t)block_type < sizeof(IR_BLOCKS_VALID_ARGS) / sizeof(*IR_BLOCKS_VALID_ARGS));
    assert(arg_type != IRArgType::NONE);
    assert((const size_t)arg_type < IRARGTYPE_NUM);
    assert((const size_t)data->arch < ARCHES_NUM);

    return IR_BLOCKS_VALID_ARGS[(size_t)block_type][(size_t)data->arch][(size_t)arg_type] & val->type;
}

static void push_pop_opt_collapse_block_(IRNode* block) {
    assert(block);

    block->type = IRNodeType::NONE;
    block->src[0] = {.type = IRVal::NONE, .num = {.offset = 0}};
    block->src[1] = {.type = IRVal::NONE, .num = {.offset = 0}};
    block->dest   = {.type = IRVal::NONE, .num = {.offset = 0}};
}

static Status::Statuses push_pop_opt_change_blocks_(IRBackData* data, ssize_t push_block_i,
                                                    ssize_t pop_block_i, size_t pop_src_num) {
    assert(data);
    assert(0 <= pop_block_i  && pop_block_i  < data->ir.capacity);
    assert(0 <= push_block_i && push_block_i < data->ir.capacity);
    assert(pop_src_num == 0 || pop_src_num == 1);

    IRNode* push_block = &data->ir.arr[push_block_i].elem;
    IRNode* pop_block  = &data->ir.arr[pop_block_i].elem;

    if (push_block->type == IRNodeType::MOV && pop_block->type == IRNodeType::MOV &&
        is_irval_equal(&push_block->src[0], &pop_block->dest)) {

        push_pop_opt_collapse_block_(push_block);
        push_pop_opt_collapse_block_(pop_block);

        return Status::NORMAL_WORK;
    }

    if (push_block->type == IRNodeType::MOV &&
        is_irval_compatiable(data, pop_block->type, pop_src_num == 0 ? IRArgType::SRC0 : IRArgType::SRC1,
                             &push_block->src[0])) {

        pop_block->src[pop_src_num] = push_block->src[0];
        push_pop_opt_collapse_block_(push_block);
        return Status::NORMAL_WORK;
    }

    if (pop_block->type == IRNodeType::MOV &&
        is_irval_compatiable(data, push_block->type, IRArgType::DEST, &pop_block->dest)) {

        push_block->dest = pop_block->dest;
        push_pop_opt_collapse_block_(pop_block);
        return Status::NORMAL_WORK;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses is_data_changing_between_ir_blocks_(IRBackData* data, ssize_t push_block_i,
                                                            ssize_t pop_block_i, bool* is_changing) {
    assert(data);
    assert(0 <= pop_block_i  && pop_block_i  < data->ir.capacity);
    assert(0 <= push_block_i && push_block_i < data->ir.capacity);
    assert(is_changing);

    ssize_t log_i = 0;
    ssize_t phys_i = data->ir.arr[push_block_i].next;
    LIST_FOREACH(data->ir, phys_i, log_i) {
        if (phys_i == pop_block_i) {
            *is_changing = false;
            return Status::NORMAL_WORK;
        }

        assert(data->ir.arr[phys_i].elem.type != IRNodeType::DEFAULT);

        IRNode elem = data->ir.arr[phys_i].elem;
        if (elem.type != IRNodeType::NONE) {
            *is_changing = true;
            return Status::NORMAL_WORK;
        }
    }

    assert(0 && "Unexpectedely reached end of the List");
    return Status::LIST_ERROR;
}
