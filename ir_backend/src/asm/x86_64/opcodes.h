#ifndef x86_64_OPCODES_H_
#define x86_64_OPCODES_H_

#include <stdlib.h>

#include "utils/statuses.h"
#include "ir_objects.h"
#include "x86_64_utils.h"
#include "elf/elf_gen.h"

struct Operand {
    char str[STR_MAXLEN + 1] = {};

    struct {
        unsigned char mod:2;
        unsigned char reg:3;
        unsigned char rm: 3;
    } modrm = {};

    bool is_sib_used = false;

    struct {
        unsigned char scale:2;
        unsigned char index:3;
        unsigned char base: 3;
    } sib = {};

    bool is_disp32_used = false;
    uint32_t disp32     = 0;
};

#define WRAPPER(...)            \
            do {                \
                __VA_ARGS__;    \
            } while (0)

#define BUF_PUSH(val__, size_)                                      \
            WRAPPER(size_t val_ = (size_t)val__;                    \
                    if (!elf->buf.push_back_several(&val_, size_))  \
                        return Status::FILE_ERROR)

#define BUF_PUSH_ZEROES(num_) VECTOR_CHECK(elf->buf.push_zero_elems(num_));

#define HEX1(byte_)   BUF_PUSH(byte_,  1)
#define HEX2(bytes_)  BUF_PUSH(bytes_, 2)
#define HEX3(bytes_)  BUF_PUSH(bytes_, 3)
#define HEX4(bytes_)  BUF_PUSH(bytes_, 4)
#define HEX8(bytes_)  BUF_PUSH(bytes_, 8)

#define CUR_OFFS() ((size_t)elf->buf.size() - (size_t)elf->phdr_text()->p_offset)

#define EPSILON_CONST       elf->globals.eps_vaddr
#define DOUBLE_NEG_CONST    elf->globals.double_neg_const_vaddr
#define GLOBAL_SECTION      elf->globals.global_section_vaddr

#define VECTOR_CHECK(cmd_)  WRAPPER(if (!cmd_) return Status::MEMORY_EXCEED)

#define ALIGN(align_) VECTOR_CHECK(elf->buf.align(align_))

inline void fixup(ElfData* elf, size_t addr) {
    if (addr == 0)
        return;

    size_t offs = addr + elf->phdr_text()->p_offset;

    int32_t val = 0;
    memcpy(&val, elf->buf[offs], sizeof(int32_t));
    val += (int32_t)CUR_OFFS();

    elf->buf.fixup_several(offs, &val, sizeof(int32_t));
}

#define FIXUP(addr_) fixup(elf, addr_)

inline Status::Statuses solve_fixups(ElfData* elf, IRNode* block) {
    assert(elf);
    assert(block);

    block->bin_offset = (ssize_t)CUR_OFFS();
    if (block->addr_fixups.is_initialised()) {
        for (ssize_t i = 0; i < block->addr_fixups.size(); i++)
            FIXUP(*(size_t*)block->addr_fixups[(size_t)i]);

        block->addr_fixups.dtor();
    }

    return Status::NORMAL_WORK;
}

inline Status::Statuses add_offs_fixup(ElfData* elf, IRNode* dest_block, size_t offs) {
    assert(elf);
    assert(dest_block);
    assert(offs);

    if (!dest_block->addr_fixups.is_initialised())
        VECTOR_CHECK(dest_block->addr_fixups.ctor(sizeof(size_t)));

    VECTOR_CHECK(dest_block->addr_fixups.push_back(&offs));

    return Status::NORMAL_WORK;
}

#define ADD_OFFS_FIXUP(dest_block_, offs_) STATUS_CHECK(add_offs_fixup(elf, dest_block_, offs_))

#define SOLVE_FIXUPS()  STATUS_CHECK(solve_fixups(elf, block))

#define REL_ADDR_CMD(cmd_, dest_block_index_)                                       \
            WRAPPER(IRNode* dest_block_ = &data->ir.arr[dest_block_index_].elem;    \
                    if (dest_block_->bin_offset == IRNode::NO_OFFSET) {             \
                        cmd_(0);                                                    \
                        ADD_OFFS_FIXUP(dest_block_, CUR_OFFS() - sizeof(uint32_t)); \
                    } else                                                          \
                        cmd_(dest_block_->bin_offset))

#define REX_CONST                0b01000000
#define REX_W       (REX_CONST | 0b00001000)
#define REX_R       (REX_CONST | 0b00000100)
#define REX_X       (REX_CONST | 0b00000010)
#define REX_B       (REX_CONST | 0b00000001)

#define MODRM_MOD(val_) ((val_) << 6)
#define MODRM_RM(val_)   (val_)
#define MODRM_REG(val_) ((val_) << 3)

#define MODRM(mod_, rm_, reg_) (MODRM_MOD(mod_) | MODRM_RM(rm_) | MODRM_REG(reg_))

#define MODRM_REG_REG(reg1_, reg2_)             MODRM(3, reg1_, reg2_)
#define MODRM_REG_EXT(reg_, ext_)               MODRM(3, reg_, ext_)
#define MODRM_REG_MEM(reg_)                     MODRM(0, 4, reg_)
#define MODRM_REG_REG_MEM_OFFS(reg1_, reg2_)    MODRM(2, reg2_, reg1_)

#define SIB_SCALE(val_)             ((val_) << 6)
#define SIB_INDEX(val_)             ((val_) << 3)
#define SIB_BASE(val_)               (val_)
#define SIB(scale_, index_, base_)  (SIB_SCALE(scale_) | SIB_INDEX(index_) | SIB_BASE(base_))

#define SIB_STK()                   SIB(0, 4, RSP)
#define SIB_ABS()                   SIB(0, 4, 5)

#define REG_RMOP(reg_, rmop_)       WRAPPER((rmop_).modrm.reg = reg_; RMOP(rmop_))
#define RMOP(rmop_)                 STATUS_CHECK(write_rmop(elf, rmop_))

inline Status::Statuses write_rmop(ElfData* elf, Operand op) {
    HEX1(MODRM(op.modrm.mod, op.modrm.rm, op.modrm.reg));

    if (op.is_sib_used)
        HEX1(SIB(op.sib.scale, op.sib.index, op.sib.base));

    if (op.is_disp32_used)
        HEX4(op.disp32);

    return Status::NORMAL_WORK;
};

#define SET_RMOP_STK_OFFS(op_, offs_)                                               \
            do {                                                                    \
                STR_VAR((op_)->str, "[rsp + %zd]", (ssize_t)offs_);                 \
                (op_)->modrm          = {.mod = 2, .rm = 4};                        \
                (op_)->is_sib_used    = true;                                       \
                (op_)->sib            = {.scale = 0, .index = 4, .base = RSP};      \
                (op_)->is_disp32_used = true;                                       \
                (op_)->disp32         = (uint32_t)offs_;                            \
            } while (0)

#define RMOP_STK()      {.modrm = {.mod = 0, .rm = 4}, .is_sib_used = true, \
                         .sib   = {.scale = 0, .index = 4, .base = RSP}}

#define RMOP_REG(reg_)  {.modrm = {.mod = 3, .rm = (unsigned char)(reg_)}}

// commands
//     |             name              |  prefix   |  opcode   |  arguments

#define REL_CALL_FIXUP_OFFS                            1
#define REL_CALL(abs_addr_)     WRAPPER(            HEX1(0xe8); HEX4(abs_addr_ - (CUR_OFFS() + 4))      )

#define REL_JMP_FIXUP_OFFS                             1
#define REL_JMP(abs_addr_)      WRAPPER(            HEX1(0xe9); HEX4(abs_addr_ - (CUR_OFFS() + 4))      )

#define REL_JC_FIXUP_OFFS                              2
#define REL_JC(abs_addr_)       WRAPPER(            HEX2(0x820f); HEX4(abs_addr_ - (CUR_OFFS() + 4))    )

#define REL_JNC_FIXUP_OFFS                             2
#define REL_JNC(abs_addr_)      WRAPPER(            HEX2(0x830f); HEX4(abs_addr_ - (CUR_OFFS() + 4))    )

#define ENTER_IMM16_0(imm_)     WRAPPER(            HEX1(0xc8); HEX2((uint16_t)imm_); HEX1(0x00)        )
#define LEAVE()                 WRAPPER(            HEX1(0xc9);                                         )
#define RET()                   WRAPPER(            HEX1(0xc3);                                         )
#define SYSCALL()               WRAPPER(            HEX2(0x050f);                                       )
#define SYSCALL_EXIT_CODE       0x3c

#define LEA_MODRM_OFFS(modrm_, offs_)                                                                   \
                                WRAPPER(            HEX1(REX_W); HEX1(0x8d);                            \
                                                                HEX1(modrm_); HEX4(offs_)               )

#define LEA_MODRM_ABS(modrm_, sib_, offs_)                                                              \
                                WRAPPER(            HEX1(REX_W); HEX1(0x8d);                            \
                                                                HEX1(modrm_); HEX1(sib_); HEX4(offs_)   )

#define ADD_REG_IMM(reg_, imm_) WRAPPER(            HEX1(REX_W); HEX1(0x81);                            \
                                                                HEX1(MODRM_REG_EXT(reg_, 0)); HEX4(imm_))

#define ADD_REG_REG(reg1_, reg2_)                                                                       \
                                WRAPPER(            HEX1(REX_W); HEX1(0x01);                            \
                                                                HEX1(MODRM_REG_REG(reg1_, reg2_))       )

#define SUB_REG_IMM(reg_, imm_) WRAPPER(            HEX1(REX_W); HEX1(0x81);                            \
                                                                HEX1(MODRM_REG_EXT(reg_, 5)); HEX4(imm_))

#define SUB_REG_REG(reg1_, reg2_)                                                                       \
                                WRAPPER(            HEX1(REX_W); HEX1(0x29);                            \
                                                                HEX1(MODRM_REG_REG(reg1_, reg2_))       )

#define MATH_REG_RMOP(code_, reg_, rmop_)                                                               \
                                WRAPPER(            HEX2(0x0ff2); HEX1(code_);                          \
                                                    REG_RMOP(reg_, rmop_)                               )

#define BITW_RMOP_REG(code_, rmop_, reg_)                                                               \
                                WRAPPER(            HEX1(REX_W); HEX1(code_);                           \
                                                                REG_RMOP(reg_, rmop_)                   )

#define PXOR_REG_REG(reg1_, reg2_)                                                                      \
                                WRAPPER(            HEX3(0xef0f66);                                     \
                                                                HEX1(MODRM_REG_REG(reg2_, reg1_))       )

#define SHL_REG_IMM(reg_, imm_) WRAPPER(            HEX1(REX_W); HEX1(0xc1);                            \
                                                                HEX1(MODRM_REG_EXT(reg_, 4)); HEX1(imm_))

#define AND_REG_REG(reg1_, reg2_)                                                                       \
                                WRAPPER(            HEX1(REX_W); HEX1(0x21);                            \
                                                                HEX1(MODRM_REG_REG(reg1_, reg2_))       )

#define CVTSD2SI_REG_REG(reg1_, reg2_)                                                                  \
                                WRAPPER(            HEX1(0xf2); HEX1(REX_W); HEX2(0x2d0f);              \
                                                                HEX1(MODRM_REG_REG(reg2_, reg1_))       )

#define CVTSD2SI_REG_STK(reg_)                                                                          \
                                WRAPPER(            HEX1(0xf2); HEX1(REX_W); HEX2(0x2d0f);              \
                                                                HEX1(MODRM_REG_MEM(reg_)); HEX1(SIB_STK()))

#define CVTTSD2SI_REG_REG(reg1_, reg2_)                                                                 \
                                WRAPPER(            HEX1(0xf2); HEX1(REX_W); HEX2(0x2c0f);              \
                                                                HEX1(MODRM_REG_REG(reg2_, reg1_))       )

#define ANDPD_REG_RODATA(reg_, rodata_)                                                                 \
                                WRAPPER(            HEX3(0x540f66);                                     \
                                                                HEX1(MODRM_REG_MEM(reg_)); HEX1(SIB_ABS()); HEX4(rodata_))

#define COMISD_REG_RODATA(reg_, rodata_)                                                                \
                                WRAPPER(            HEX3(0x2f0f66);                                     \
                                                                HEX1(MODRM_REG_MEM(reg_)); HEX1(SIB_ABS()); HEX4(rodata_))

#define COMISD_REG_REG(reg1_, reg2_)                                                                    \
                                WRAPPER(            HEX3(0x2f0f66);                                     \
                                                                HEX1(MODRM_REG_REG(reg2_, reg1_))       )

#define MOV_REG_IMM64(reg_, imm_)                                                                       \
                                WRAPPER(            HEX1(REX_W); HEX1(0xb8 + reg_); HEX8(imm_)          )

#define MOV_RMOP_REG(rmop_, reg_)                                                                       \
                                WRAPPER(            HEX1(REX_W); HEX1(0x89);                            \
                                                                REG_RMOP(reg_, rmop_)                   )
#define MOV_REG_RMOP(reg_, rmop_)                                                                       \
                                WRAPPER(            HEX1(REX_W); HEX1(0x8b);                            \
                                                                REG_RMOP(reg_, rmop_)                   )

#define MOVQ_XMM_RMOP(xmm_, rmop_)                                                                      \
                                WRAPPER(            HEX3(0x7e0ff3);                                     \
                                                                REG_RMOP((unsigned char)(xmm_), rmop_)  )

#define MOVQ_RMOP_XMM(rmop_, xmm_)                                                                      \
                                WRAPPER(            HEX3(0xd60f66);                                     \
                                                                REG_RMOP((unsigned char)(xmm_), rmop_)  )

#define MOVQ_XMM_REGRMOP(reg_, rmop_)                                                                   \
                                WRAPPER(            HEX1(0x66); HEX1(REX_W); HEX2(0x6e0f);              \
                                                                REG_RMOP((unsigned char)(reg_), rmop_)  )

#define MOVQ_REGRMOP_XMM(rmop_, reg_)                                                                   \
                                WRAPPER(            HEX1(0x66); HEX1(REX_W); HEX2(0x7e0f);              \
                                                                REG_RMOP((unsigned char)(reg_), rmop_)  )

#define MOVQ_RMOP_RMOP(rmop1_, rmop2_)                                  \
            WRAPPER(                                                    \
                if ((rmop1_).modrm.mod == 3)                            \
                    MOVQ_XMM_RMOP((rmop1_).modrm.rm, rmop2_);           \
                else if ((rmop2_).modrm.mod == 3)                       \
                    MOVQ_RMOP_XMM(rmop1_, (rmop2_).modrm.rm);           \
                else {                                                  \
                    assert(0 && "One of the rmops must be register");   \
                    return Status::LIST_ERROR;                          \
                })

#define MOVQ_XMM_XMM(xmm1_, xmm2_)  \
                WRAPPER(            \
                    Operand op_xmm1_ = {.modrm = {.mod = 3, .rm = (unsigned char)(xmm1_)}};  \
                    MOVQ_RMOP_XMM(op_xmm1_, xmm2_);                \
                )

#define MOVQ_XMM_REG(xmm_, reg_)                                                                \
                WRAPPER(                                                                        \
                    Operand op_reg_ = {.modrm = {.mod = 3, .rm = (unsigned char)(reg_)}};       \
                    MOVQ_XMM_REGRMOP(xmm_, op_reg_);                                            \
                )

#define MOVQ_REG_XMM(reg_, xmm_)                                                                \
                WRAPPER(                                                                        \
                    Operand op_reg_ = {.modrm = {.mod = 3, .rm = (unsigned char)(reg_)}};       \
                    MOVQ_REGRMOP_XMM(op_reg_, xmm_);                                            \
                )
// Registers

#define XMM 0

enum Regs {
    RAX = 0,
    RCX = 1,
    RDX = 2,
    RBX = 3,
    RSP = 4,
    RBP = 5,
    RSI = 6,
    RDI = 7,
    // r8 - r15 are not supported
};

// Math and bitw codes

#define MATH_ADDSD_CODE  0x58
#define MATH_SUBSD_CODE  0x5c
#define MATH_MULSD_CODE  0x59
#define MATH_DIVSD_CODE  0x5e
#define MATH_SQRTSD_CODE 0x51
#define BITW_AND_CODE    0x21
#define BITW_XOR_CODE    0x31

#endif //< #ifndef x86_64_OPCODES_H_
