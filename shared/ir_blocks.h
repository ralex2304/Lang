// MEM:      STK | LOCAL_VAR | GLOBAL_VAR | ARR_VAR | ARG_VAR
// VAR_TYPE: LOCAL_VAR | GLOBAL_VAR
// ALL:      MEM | REG | CONST

//                                      |              x86-64               |  |                SPU                |
//                                      |  src[0]   |  src[1]   |   dest    |  |  src[0]   |  src[1]   |   dest    |
IR_BLOCK( 0, NONE,                      {NONE,       NONE,       NONE,      }, {NONE,       NONE,       NONE       })
IR_BLOCK( 1, START,                     {NONE,       NONE,       NONE,      }, {NONE,       NONE,       NONE       })
IR_BLOCK( 2, END,                       {NONE,       NONE,       NONE,      }, {NONE,       NONE,       NONE       })
IR_BLOCK( 3, BEGIN_FUNC_DEF,            {INT_CONST,  NONE,       NONE,      }, {NONE,       NONE,       NONE       })
IR_BLOCK( 4, END_FUNC_DEF,              {NONE,       NONE,       NONE,      }, {NONE,       NONE,       NONE       })
IR_BLOCK( 5, CALL_FUNC,                 {NONE,       NONE,       ADDR,      }, {INT_CONST,  NONE,       ADDR       })
IR_BLOCK( 6, RET,                       {NONE,       NONE,       NONE,      }, {NONE,       NONE,       NONE       })
IR_BLOCK( 7, INIT_MEM_FOR_GLOBALS,      {INT_CONST,  NONE,       NONE,      }, {NONE,       NONE,       NONE       })
IR_BLOCK( 8, COUNT_ARR_ELEM_ADDR_CONST, {VAR_TYPE,   NONE,       NONE,      }, {VAR_TYPE,   NONE,       NONE       })
IR_BLOCK( 9, ARR_ELEM_ADDR_ADD_INDEX,   {STK | REG,  VAR_TYPE,   NONE,      }, {STK,        VAR_TYPE,   NONE       })
IR_BLOCK(10, MOV,                       {ALL,        NONE,       MEM | REG, }, {ALL,        NONE,       MEM | REG  })
IR_BLOCK(11, SWAP,                      {MEM | REG,  MEM | REG,  NONE,      }, {STK,        STK,        NONE       })
IR_BLOCK(12, STORE_CMP_RES,             {ALL,        ALL,        MEM | REG, }, {STK,        STK,        STK        })
IR_BLOCK(13, SET_FLAGS_CMP_WITH_ZERO,   {MEM | REG,  NONE,       NONE,      }, {STK,        STK,        NONE       })
IR_BLOCK(14, MATH_OPER,                 {MEM | REG,  MEM | REG,  MEM | REG, }, {STK,        STK,        STK        })
IR_BLOCK(15, JUMP,                      {NONE,       NONE,       NONE,      }, {NONE,       NONE,       NONE       })
IR_BLOCK(16, READ_DOUBLE,               {NONE,       NONE,       MEM | REG, }, {NONE,       NONE,       STK        })
IR_BLOCK(17, PRINT_DOUBLE,              {ALL,        NONE,       NONE,      }, {STK,        NONE,       NONE       })
IR_BLOCK(18, SET_FPS,                   {NONE,       NONE,       NONE,      }, {INT_CONST,  NONE,       NONE       })
IR_BLOCK(19, SHOW_VIDEO_FRAME,          {NONE,       NONE,       NONE,      }, {NONE,       NONE,       NONE       })
