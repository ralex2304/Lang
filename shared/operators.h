#ifndef DEF_OPER
static_assert(0 && "DEF_OPER is not defined");
#endif //< #ifndef DEF_OPER

//   | NUM |      NAME      | TYPE |

DEF_OPER(1,  CMD_SEPARATOR,   LIST,   {
    EVAL_SUBTREE_NO_VAL(*L(node));

    if (*R(node) != nullptr) {
        EVAL_SUBTREE_NO_VAL(*R(node));
    }
})


DEF_OPER(2,  VAR_DEFINITION,  BINARY, {
    ADD_VAR(*L(node));

    EVAL_SUBTREE_GET_VAL(*R(node));

    ASSIGN_VAR_VAL(*L(node));
})
DEF_OPER(3,  CONST_VAR_DEF,   UNARY,  {
    ADD_CONST_VAR(*L(*R(node)));

    EVAL_SUBTREE_GET_VAL(*R(*R(node)));

    ASSIGN_VAR_VAL(*L(*R(node)));
})

DEF_OPER(4,  FUNC_DEFINITION, BINARY, { DAMAGED_TREE("Unexpected FUNC_DEFINITION (not in global scope)"); })

DEF_OPER(5,  ASSIGNMENT,      BINARY, {
    CHECK_VAR_FOR_ASSIGNMENT(*L(node));

    EVAL_SUBTREE_GET_VAL(*R(node));

    ASSIGN_VAR_VAL(*L(node));
})
DEF_OPER(6,  ASSIGNMENT_ADD,  BINARY, {
    CHECK_VAR_FOR_ASSIGNMENT(*L(node));

    ASSIGNMENT_WITH_ACTION("add");

    ASSIGN_VAR_VAL(*L(node));
})
DEF_OPER(7,  ASSIGNMENT_SUB,  BINARY, {
    CHECK_VAR_FOR_ASSIGNMENT(*L(node));

    ASSIGNMENT_WITH_ACTION("sub");

    ASSIGN_VAR_VAL(*L(node));
})
DEF_OPER(8,  ASSIGNMENT_MUL,  BINARY, {
    CHECK_VAR_FOR_ASSIGNMENT(*L(node));

    ASSIGNMENT_WITH_ACTION("mul");

    ASSIGN_VAR_VAL(*L(node));
})
DEF_OPER(9,  ASSIGNMENT_DIV,  BINARY, {
    CHECK_VAR_FOR_ASSIGNMENT(*L(node));

    ASSIGNMENT_WITH_ACTION("div");

    ASSIGN_VAR_VAL(*L(node));
})

DEF_OPER(10, RETURN,          UNARY,  {
    EVAL_SUBTREE_GET_VAL(*R(node));

    ASM_PRINT_COMMAND(0, "pop rax\n\n");
    ASM_PRINT_COMMAND(0, "ret\n\n");
})

DEF_OPER(15, VAR_SEPARATOR,   LIST,   { DAMAGED_TREE("unexpected VAR_SEPARATOR"); })

DEF_OPER(16, FUNC_CALL,       BINARY, { PROVIDE_FUNC_CALL(); })

DEF_OPER(20, MATH_ADD,        BINARY, { BINARY_MATH("add"); })
DEF_OPER(21, MATH_SUB,        BINARY, { BINARY_MATH("sub"); })
DEF_OPER(22, MATH_MUL,        BINARY, { BINARY_MATH("mul"); })
DEF_OPER(23, MATH_DIV,        BINARY, { BINARY_MATH("div"); })
DEF_OPER(24, MATH_SQRT,       UNARY,  { UNARY_MATH("sqrt"); })
DEF_OPER(25, MATH_SIN,        UNARY,  { UNARY_MATH("sin");  })
DEF_OPER(26, MATH_COS,        UNARY,  { UNARY_MATH("cos");  })

DEF_OPER(27, MATH_NEGATIVE,   UNARY,  {
    ASM_PRINT_COMMAND(0, "push -1\n");
    UNARY_MATH(          "mul");
})

DEF_OPER(28, MATH_DIFF,       BINARY, { DAMAGED_TREE("unexpected MATH_DIFF"); })

DEF_OPER(30, LOGIC_GREAT,     BINARY, { LOGIC("ja");  })
DEF_OPER(31, LOGIC_LOWER,     BINARY, { LOGIC("jb");  })
DEF_OPER(32, LOGIC_NOT_EQUAL, BINARY, { LOGIC("jne"); })
DEF_OPER(33, LOGIC_EQUAL,     BINARY, { LOGIC("je");  })
DEF_OPER(34, LOGIC_GREAT_EQ,  BINARY, { LOGIC("jae"); })
DEF_OPER(35, LOGIC_LOWER_EQ,  BINARY, { LOGIC("jbe"); })

DEF_OPER(40, PREFIX_ADD,      BINARY, { PREFIX_OPER ("add"); })
DEF_OPER(41, PREFIX_SUB,      BINARY, { PREFIX_OPER ("sub"); })
DEF_OPER(42, POSTFIX_ADD,     BINARY, { POSTFIX_OPER("add"); })
DEF_OPER(43, POSTFIX_SUB,     BINARY, { POSTFIX_OPER("sub"); })

DEF_OPER(50, WHILE,           BINARY, {
    if (NODE_IS_OPER(*R(node), OperNum::ELSE)) {
        ASM_MAKE_WHILE_ELSE(node);
    } else {
        ASM_MAKE_WHILE(node);
    }
})

DEF_OPER(51, DO_WHILE,        BINARY, { ASM_MAKE_DO_WHILE(node); })

DEF_OPER(53, IF,              BINARY, {
    EVAL_SUBTREE_GET_VAL(*L(node));

    if (NODE_IS_OPER(*R(node), OperNum::ELSE)) {
        ASM_MAKE_IF_ELSE(*R(node));
    } else {
        ASM_MAKE_IF(*R(node));
    }
})

DEF_OPER(54, DO_IF,           BINARY, { ASM_MAKE_DO_IF(node); })

DEF_OPER(56, ELSE,            BINARY, { DAMAGED_TREE("unexpected ELSE node"); })

DEF_OPER(57, BREAK,           LEAF,   { ASM_MAKE_BREAK(node); })
DEF_OPER(58, CONTINUE,        LEAF,   { ASM_MAKE_CONTINUE(node); })

DEF_OPER(70, IN,              LEAF,   { ASM_PRINT_COMMAND(0, "in\n"); })

DEF_OPER(71, OUT,             UNARY,  {
    EVAL_SUBTREE_GET_VAL(*R(node));
    ASM_PRINT_COMMAND(0, "out\n\n");
})

DEF_OPER(72, SHOW,            LEAF,   { ASM_PRINT_COMMAND(0, "shw\n\n"); })

DEF_OPER(73, SET_FPS,         UNARY,  { ASM_SET_FPS(*R(node)); })
