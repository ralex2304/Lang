#ifndef DEF_TERMINAL
static_assert(0 && "DEF_TERMINAL is not defined");
#endif //< #ifndef DEF_TERMINAL


//          NUM | ENUM NAME | NAMES

DEF_TERMINAL(1,  CMD_SEPARATOR,     {"\n"})
DEF_TERMINAL(2,  VAR,               {"var",      "мой",            "контора"})
DEF_TERMINAL(3,  CONST,             {"const",    "грубый",         "пидорасов"})
DEF_TERMINAL(4,  ASSIGNMENT,        {"=",        nullptr,          "ано"})

DEF_TERMINAL(10, RETURN,            {"return",   "свергаю кумира", "заебал"})

DEF_TERMINAL(11, OPEN_SCOPE,        {"{"})
DEF_TERMINAL(12, CLOSE_SCOPE,       {"}"})
DEF_TERMINAL(13, OPEN_BRACE,        {"("})
DEF_TERMINAL(14, CLOSE_BRACE,       {")"})
DEF_TERMINAL(15, OPEN_INDEX_BRACE,  {"["})
DEF_TERMINAL(16, CLOSE_INDEX_BRACE, {"]"})
DEF_TERMINAL(17, VAR_SEPARATOR,     {",",        nullptr,          "ещё"})

DEF_TERMINAL(20, MATH_ADD,          {"+",        nullptr,          "курва"})
DEF_TERMINAL(21, MATH_SUB,          {"-",        nullptr,          "дурка"})
DEF_TERMINAL(22, MATH_MUL,          {"*",        nullptr,          "пердольно"})
DEF_TERMINAL(23, MATH_DIV,          {"/",        nullptr,          "стл.стл"})
DEF_TERMINAL(24, MATH_POW,          {"^",        nullptr,          "у нас тут физика своя"})
DEF_TERMINAL(25, MATH_SQRT,         {"sqrt",     nullptr,          "япония прекрасна"})
DEF_TERMINAL(26, MATH_SIN,          {"sin",      nullptr,          "купил говна"})
DEF_TERMINAL(27, MATH_COS,          {"cos",      nullptr,          "печатаешь говна"})
DEF_TERMINAL(28, MATH_LN,           {"ln",       nullptr,          "красивое"})
DEF_TERMINAL(29, MATH_DIFF,         {"diff",     "золото святое",  "поебаца тебе не завернуть?"})
DEF_TERMINAL(30, MATH_DOUBLE_ADD,   {"++",       nullptr,          "пизда"})
DEF_TERMINAL(31, MATH_DOUBLE_SUB,   {"--",       nullptr,          "да"})

DEF_TERMINAL(40, LOGIC_GREATER,     {">",        nullptr,          "по эскобару"})
DEF_TERMINAL(41, LOGIC_LOWER,       {"<",        nullptr,          "не по эскобару"})
DEF_TERMINAL(42, LOGIC_NOT,         {"!",        nullptr,          "ано не"})

DEF_TERMINAL(50, WHILE,             {"while",    "омывает тысячи рек",
                                     "суши пластик, сухой? нихуя, суши ещё, суши блять пластик"})

DEF_TERMINAL(51, DO_WHILE_DO,       {"do while", "омывает",        "суши пластик"})
DEF_TERMINAL(52, DO_WHILE_WHILE,    {"while do", "тысячи рек",
                                     "сухой? нихуя, суши ещё, суши блять пластик"})

DEF_TERMINAL(53, IF,                {"if",       "что это такое",  "ано?"})
DEF_TERMINAL(54, DO_IF_DO,          {"do if",    "что",            "до среды лягушонок"})
DEF_TERMINAL(55, DO_IF_IF,          {"if do",    "это такое",      "ну всё габела"})
DEF_TERMINAL(56, ELSE,              {"else",     "никто не знает", "ано блять, ано"})
DEF_TERMINAL(57, BREAK,             {"break",    "тяжело",         "гоношно"})
DEF_TERMINAL(58, CONTINUE,          {"continue", "амням",          "не гоношно"})

DEF_TERMINAL(70, IN,                {"in",       "слушай",         "печатник хуйня?"})
DEF_TERMINAL(71, OUT,               {"out",      "серенада",       "печатают хуйню и то хуёво"})
DEF_TERMINAL(72, SHOW,              {"show",     nullptr,          "срали мазали лепили"})
DEF_TERMINAL(73, SET_FPS,           {"set fps",  nullptr,          "заверните"})

