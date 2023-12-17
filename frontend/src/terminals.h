#ifndef DEF_TERMINAL
static_assert(0 && "DEF_TERMINAL is not defined");
#endif //< #ifndef DEF_TERMINAL


//          NUM | ENUM NAME | NAME_IS_TEXT | NAME

DEF_TERMINAL(1,  CMD_SEPARATOR,  false, "\n")               //< ;
DEF_TERMINAL(2,  VAR,            true,  "мой")              //< var
DEF_TERMINAL(3,  CONST,          true,  "грубый")           //< const
DEF_TERMINAL(4,  ASSIGNMENT,     false, "=")
DEF_TERMINAL(5,  RETURN,         true,  "свергаю_кумира")   //< return

DEF_TERMINAL(6,  OPEN_SCOPE,     false, "{")
DEF_TERMINAL(7,  CLOSE_SCOPE,    false, "}")
DEF_TERMINAL(8,  OPEN_BRACE,     false, "(")
DEF_TERMINAL(9,  CLOSE_BRACE,    false, ")")
DEF_TERMINAL(10, VAR_SEPARATOR,  false, ",")

DEF_TERMINAL(20, MATH_ADD,       false, "+")
DEF_TERMINAL(21, MATH_SUB,       false, "-")
DEF_TERMINAL(22, MATH_MUL,       false, "*")
DEF_TERMINAL(23, MATH_DIV,       false, "/")
DEF_TERMINAL(24, MATH_SQRT,      true,  "sqrt")
DEF_TERMINAL(25, MATH_SIN,       true,  "sin")
DEF_TERMINAL(26, MATH_COS,       true,  "cos")

DEF_TERMINAL(30, LOGIC_GREATER,  false, ">")
DEF_TERMINAL(31, LOGIC_LOWER,    false, "<")
DEF_TERMINAL(32, LOGIC_NOT,      false, "!")

DEF_TERMINAL(50, WHILE,          true,  "омывает_тысячи_рек")  //< while
DEF_TERMINAL(51, DO_WHILE_DO,    true,  "омывает")             //< do (while)
DEF_TERMINAL(52, DO_WHILE_WHILE, true,  "тысячи_рек")          //< (do) while
DEF_TERMINAL(53, IF,             true,  "что_это_такое")       //< if
DEF_TERMINAL(54, DO_IF_DO,       true,  "что")                 //< do (if)
DEF_TERMINAL(55, DO_IF_IF,       true,  "это_такое")           //< (do) if
DEF_TERMINAL(56, BREAK,          true,  "тяжело")              //< break
DEF_TERMINAL(57, CONTINUE,       true,  "амням")               //< continue

DEF_TERMINAL(70, IN,             true,  "слушай")
DEF_TERMINAL(71, OUT,            true,  "серенада")
DEF_TERMINAL(72, SHOW,           true,  "мелькнуло")
DEF_TERMINAL(73, SET_FPS,        true,  "мелькнуло_столько_раз")
