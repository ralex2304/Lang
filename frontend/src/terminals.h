#ifndef DEF_TERMINAL
static_assert(0 && "DEF_TERMINAL is not defined");
#endif //< #ifndef DEF_TERMINAL


//          NUM | ENUM NAME | NAMES

DEF_TERMINAL(1,  CMD_SEPARATOR,     {"\n"})
DEF_TERMINAL(2,  VAR,               {"var",      "���",            "�������"})
DEF_TERMINAL(3,  CONST,             {"const",    "������",         "���������"})
DEF_TERMINAL(4,  ASSIGNMENT,        {"=",        nullptr,          "���"})

DEF_TERMINAL(10, RETURN,            {"return",   "������� ������", "������"})

DEF_TERMINAL(11, OPEN_SCOPE,        {"{"})
DEF_TERMINAL(12, CLOSE_SCOPE,       {"}"})
DEF_TERMINAL(13, OPEN_BRACE,        {"("})
DEF_TERMINAL(14, CLOSE_BRACE,       {")"})
DEF_TERMINAL(15, OPEN_INDEX_BRACE,  {"["})
DEF_TERMINAL(16, CLOSE_INDEX_BRACE, {"]"})
DEF_TERMINAL(17, VAR_SEPARATOR,     {",",        nullptr,          "���"})

DEF_TERMINAL(20, MATH_ADD,          {"+",        nullptr,          "�����"})
DEF_TERMINAL(21, MATH_SUB,          {"-",        nullptr,          "�����"})
DEF_TERMINAL(22, MATH_MUL,          {"*",        nullptr,          "���������"})
DEF_TERMINAL(23, MATH_DIV,          {"/",        nullptr,          "���.���"})
DEF_TERMINAL(24, MATH_POW,          {"^",        nullptr,          "� ��� ��� ������ ����"})
DEF_TERMINAL(25, MATH_SQRT,         {"sqrt",     nullptr,          "������ ���������"})
DEF_TERMINAL(26, MATH_SIN,          {"sin",      nullptr,          "����� �����"})
DEF_TERMINAL(27, MATH_COS,          {"cos",      nullptr,          "��������� �����"})
DEF_TERMINAL(28, MATH_LN,           {"ln",       nullptr,          "��������"})
DEF_TERMINAL(29, MATH_DIFF,         {"diff",     "������ ������",  "������� ���� �� ���������?"})
DEF_TERMINAL(30, MATH_DOUBLE_ADD,   {"++",       nullptr,          "�����"})
DEF_TERMINAL(31, MATH_DOUBLE_SUB,   {"--",       nullptr,          "��"})

DEF_TERMINAL(40, LOGIC_GREATER,     {">",        nullptr,          "�� ��������"})
DEF_TERMINAL(41, LOGIC_LOWER,       {"<",        nullptr,          "�� �� ��������"})
DEF_TERMINAL(42, LOGIC_NOT,         {"!",        nullptr,          "��� ��"})

DEF_TERMINAL(50, WHILE,             {"while",    "������� ������ ���",
                                     "���� �������, �����? �����, ���� ���, ���� ����� �������"})

DEF_TERMINAL(51, DO_WHILE_DO,       {"do while", "�������",        "���� �������"})
DEF_TERMINAL(52, DO_WHILE_WHILE,    {"while do", "������ ���",
                                     "�����? �����, ���� ���, ���� ����� �������"})

DEF_TERMINAL(53, IF,                {"if",       "��� ��� �����",  "���?"})
DEF_TERMINAL(54, DO_IF_DO,          {"do if",    "���",            "�� ����� ���������"})
DEF_TERMINAL(55, DO_IF_IF,          {"if do",    "��� �����",      "�� �� ������"})
DEF_TERMINAL(56, ELSE,              {"else",     "����� �� �����", "��� �����, ���"})
DEF_TERMINAL(57, BREAK,             {"break",    "������",         "�������"})
DEF_TERMINAL(58, CONTINUE,          {"continue", "�����",          "�� �������"})

DEF_TERMINAL(70, IN,                {"in",       "������",         "�������� �����?"})
DEF_TERMINAL(71, OUT,               {"out",      "��������",       "�������� ����� � �� ����"})
DEF_TERMINAL(72, SHOW,              {"show",     nullptr,          "����� ������ ������"})
DEF_TERMINAL(73, SET_FPS,           {"set fps",  nullptr,          "���������"})

