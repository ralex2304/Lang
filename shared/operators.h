#ifndef DEF_OPER
static_assert(0 && "DEF_OPER is not defined");
#endif //< #ifndef DEF_OPER

//   | NUM |      NAME      | TYPE | MATH_TYPE | ASM_COMMAND

DEF_OPER(1,  CMD_SEPARATOR,   LIST,   MATH_L_R, {
    EVAL_SUBTREE_NO_VAL(*L(node));

    if (*R(node) != nullptr) {
        EVAL_SUBTREE_NO_VAL(*R(node));
    }
})


DEF_OPER(2,  VAR_DEFINITION,  BINARY, MATH_R,   {
    ADD_VAR(*L(node));

    EVAL_SUBTREE_GET_VAL(*R(node));

    ASSIGN_VAR_VAL(*L(node));
})
DEF_OPER(3,  CONST_VAR_DEF,   UNARY,  MATH_R,   {
    ADD_CONST_VAR(*L(*R(node)));

    EVAL_SUBTREE_GET_VAL(*R(*R(node)));

    ASSIGN_VAR_VAL(*L(*R(node)));
})

DEF_OPER(4,  FUNC_DEFINITION, BINARY, MATH_R,   {
    DAMAGED_TREE("Unexpected FUNC_DEFINITION (not in global scope)");
})

DEF_OPER(5,  ASSIGNMENT,      BINARY, MATH_R,   {
    CHECK_VAR_FOR_ASSIGNMENT(*L(node));

    EVAL_SUBTREE_GET_VAL(*R(node));

    ASSIGN_VAR_VAL(*L(node));
})
DEF_OPER(6,  ASSIGNMENT_ADD,  BINARY, MATH_R,   {
    CHECK_VAR_FOR_ASSIGNMENT(*L(node));

    ASSIGNMENT_WITH_ACTION("add");

    ASSIGN_VAR_VAL(*L(node));
})
DEF_OPER(7,  ASSIGNMENT_SUB,  BINARY, MATH_R,   {
    CHECK_VAR_FOR_ASSIGNMENT(*L(node));

    ASSIGNMENT_WITH_ACTION("sub");

    ASSIGN_VAR_VAL(*L(node));
})
DEF_OPER(8,  ASSIGNMENT_MUL,  BINARY, MATH_R,   {
    CHECK_VAR_FOR_ASSIGNMENT(*L(node));

    ASSIGNMENT_WITH_ACTION("mul");

    ASSIGN_VAR_VAL(*L(node));
})
DEF_OPER(9,  ASSIGNMENT_DIV,  BINARY, MATH_R,   {
    CHECK_VAR_FOR_ASSIGNMENT(*L(node));

    ASSIGNMENT_WITH_ACTION("div");

    ASSIGN_VAR_VAL(*L(node));
})

DEF_OPER(10, RETURN,          UNARY,  MATH_R,   {
    EVAL_SUBTREE_GET_VAL(*R(node));

    ASM_PRINT_COMMAND(0, "pop rax\n\n");
    ASM_PRINT_COMMAND(0, "ret\n\n");
})

DEF_OPER(15, VAR_SEPARATOR,   LIST,   MATH_L_R, { DAMAGED_TREE("unexpected VAR_SEPARATOR"); })

DEF_OPER(16, FUNC_CALL,       BINARY, MATH_R,   { PROVIDE_FUNC_CALL(); })

DEF_OPER(20, MATH_ADD,        BINARY, MATH,     { BINARY_MATH("add"); })
DEF_OPER(21, MATH_SUB,        BINARY, MATH,     { BINARY_MATH("sub"); })
DEF_OPER(22, MATH_MUL,        BINARY, MATH,     { BINARY_MATH("mul"); })
DEF_OPER(23, MATH_DIV,        BINARY, MATH,     { BINARY_MATH("div"); })
DEF_OPER(24, MATH_POW,        BINARY, MATH,     { BINARY_MATH("pow"); })
DEF_OPER(25, MATH_SQRT,       UNARY,  MATH,     { UNARY_MATH("sqrt"); })
DEF_OPER(26, MATH_SIN,        UNARY,  MATH,     { UNARY_MATH("sin");  })
DEF_OPER(27, MATH_COS,        UNARY,  MATH,     { UNARY_MATH("cos");  })
DEF_OPER(28, MATH_LN,         UNARY,  MATH,     { UNARY_MATH("ln");   })

DEF_OPER(29, MATH_NEGATIVE,   UNARY,  MATH,     {
    ASM_PRINT_COMMAND(0, "push -1\n");
    UNARY_MATH(          "mul");
})

DEF_OPER(30, MATH_DIFF,       BINARY, MATH_L,   { DAMAGED_TREE("unexpected MATH_DIFF"); })

DEF_OPER(40, LOGIC_GREAT,     BINARY, MATH_L_R, { LOGIC("ja");  })
DEF_OPER(41, LOGIC_LOWER,     BINARY, MATH_L_R, { LOGIC("jb");  })
DEF_OPER(42, LOGIC_NOT_EQUAL, BINARY, MATH_L_R, { LOGIC("jne"); })
DEF_OPER(43, LOGIC_EQUAL,     BINARY, MATH_L_R, { LOGIC("je");  })
DEF_OPER(44, LOGIC_GREAT_EQ,  BINARY, MATH_L_R, { LOGIC("jae"); })
DEF_OPER(45, LOGIC_LOWER_EQ,  BINARY, MATH_L_R, { LOGIC("jbe"); })

DEF_OPER(50, PREFIX_ADD,      BINARY, MATH,     { PREFIX_OPER ("add"); })
DEF_OPER(51, PREFIX_SUB,      BINARY, MATH,     { PREFIX_OPER ("sub"); })
DEF_OPER(52, POSTFIX_ADD,     BINARY, MATH,     { POSTFIX_OPER("add"); })
DEF_OPER(53, POSTFIX_SUB,     BINARY, MATH,     { POSTFIX_OPER("sub"); })

DEF_OPER(60, WHILE,           BINARY, MATH_L_R, {
    if (NODE_IS_OPER(*R(node), OperNum::ELSE)) {
        ASM_MAKE_WHILE_ELSE(node);
    } else {
        ASM_MAKE_WHILE(node);
    }
})

DEF_OPER(61, DO_WHILE,        BINARY, MATH_L_R, { ASM_MAKE_DO_WHILE(node); })

DEF_OPER(63, IF,              BINARY, MATH_L_R, {
    EVAL_SUBTREE_GET_VAL(*L(node));

    if (NODE_IS_OPER(*R(node), OperNum::ELSE)) {
        ASM_MAKE_IF_ELSE(*R(node));
    } else {
        ASM_MAKE_IF(*R(node));
    }
})

DEF_OPER(64, DO_IF,           BINARY, MATH_L_R, { ASM_MAKE_DO_IF(node); })

DEF_OPER(66, ELSE,            BINARY, MATH_L_R, { DAMAGED_TREE("unexpected ELSE node"); })

DEF_OPER(67, BREAK,           LEAF,   NO_MATH,  { ASM_MAKE_BREAK(node); })
DEF_OPER(68, CONTINUE,        LEAF,   NO_MATH,  { ASM_MAKE_CONTINUE(node); })

DEF_OPER(70, IN,              LEAF,   NO_MATH,  { ASM_PRINT_COMMAND(0, "in\n"); })

DEF_OPER(71, OUT,             UNARY,  MATH_R,   {
    EVAL_SUBTREE_GET_VAL(*R(node));
    ASM_PRINT_COMMAND(0, "out\n\n");
})

DEF_OPER(72, SHOW,            LEAF,   NO_MATH,  { ASM_PRINT_COMMAND(0, "shw\n\n"); })

DEF_OPER(73, SET_FPS,         UNARY,  MATH_R,   { ASM_SET_FPS(*R(node)); })
