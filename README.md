# Компилятор

Компиляция программ на своём эзотерическом языке для [собственного `SPU`](https://github.com/ralex2304/Processor) и `x86-64`.

## Содержание

// TODO содержание

## Процесс компиляции

![Compilation process](img/compilation_sequence.svg)

- [Frontend](#frontend) - лексический разбор текста программы и создание абстрактного синтаксического дерева (AST);
- [Middleend](#middleend) - оптимизации и преобразования AST:
    - свёртка констант;
    - удаление мёртвого кода;
    - дифференцирование математических выражений;
- [Backend](#backend) - генерация линейного архитектурно-независимого промежуточного представления (IR);
- [IR Backend](#ir-backend) - обработка IR:
    - оптимизации:
        - удаление избыточных парных перемещений данных;
        - свёртка операций со стеком;
    - создание бинарного исполняемого файла программы, генерация ассемблерного листинга.

### Frontend

#### Лексер

Сначала текст программы разбирается на лексемы. Такой подход позволил облегчить дальнейшую обработку конструкций языка, ввести синонимичные ключевые слова, а также игнорировать комментарии.

<img rel="Ключевые слова" src="img/terminals.png" width="75%">

[Полный список ключевых слов](frontend/src/terminals.h). **Осторожно, встречается ненормативная лексика.**

#### Парсер

Массив лексем преобразуется в абстрактное синтаксическое дерево (AST) при помощи алгоритма рекурсивного спуска. Так как реализация алгоритма тяжело читаема после написания, для верификации и дальнейшей поддержки был создан файл с формальным описанием синтаксических конструкций и приоритета операций языка.

Условные обозначения:
- `<expr name> := <expr>` - определение нового типа выражения
- `'<keyword>'` - ключевое слово
- `{<expr>}` - скобки для обозначения приоритета
- `!<expr>` - выражение не должно встретиться
- `<expr>?` - необязательное выражение
- `<expr>*` - выражение может не встретиться или встретиться несколько раз
- `<expr1> | <expr2>` - выражение 1 или выражение 2

```
// 'CH_' - means that function exits without error (gives choice)

Main := {'CMD_SEPARATOR'? {CH_DefFunc | CH_DefVar} 'CMD_SEPARATOR'?}* 'TERMINATOR'

CH_DefFunc := VarName {'VAR' | 'CONST'} 'OPEN_BRACE' FuncArgsDef 'CLOSE_BRACE' 'CMD_SEPARATOR'? CH_Commands

CH_DefVar := VarName {'OPEN_INDEX_BRACE' Expr 'CLOSE_INDEX_BRACE'}? 'VAR' 'CONST'? {'ASSIGNMENT' Expr {'VAR_SEPARATOR' Expr}* }?

CH_Commands := 'OPEN_SCOPE' 'CMD_SEPARATOR'? {Command {'CMD_SEPARATOR' Command}*} 'CMD_SEPARATOR'? 'CLOSE_SCOPE'

Command := {CH_Commands | CH_DefVar | CH_CommandWithArg | CH_ComplexCommand | CH_CommandWithConstArg | SimpleCommand}

// ---------------------------------------------MATHS------------------------------------------------

Expr := {VarName {'OPEN_INDEX_BRACE' Expr 'CLOSE_INDEX_BRACE'}? {{'ASSIGNMENT' !'ASSIGNMENT'} | {{'MATH_ADD' | 'MATH_SUB' | 'MATH_MUL' | 'MATH_DIV'} 'ASSIGNMENT'}} Expr} | MathLvl1

MathLvl1 := MathLvl2 {{{{'ASSIGNMENT' | 'LOGIC_NOT' | 'LOGIC_LOWER' | 'LOGIC_GREATER'} 'ASSIGNMENT'} | {'LOGIC_LOWER' | 'LOGIC_GREATER'}} MathLvl2}*

MathLvl2 := MathLvl3 {{'MATH_ADD' | 'MATH_SUB'} MathLvl3}*

MathLvl3 := MathLvl4 {{'MATH_MUL' | 'MATH_DIV'} MathLvl4}*

MathLvl4 := MathLvl5 {'MATH_POW' MathLvl5}*

MathLvl5 := {{'MATH_SUB' MathLvl5} | {{'OPEN_BRACE' Expr 'CLOSE_BRACE'} | CH_Binary | CH_Unary | Primary}}

```

[Актуальная версия синтаксиса](Programs/syntax_examples/syntax.txt).

#### Обработка синтаксических ошибок

При выявлении синтаксической ошибки выводится `gcc`-подобное сообщение.

<img rel="Синтаксическая ошибка" src="img/syntax_error.png" width="75%">

### Middleend

Данная программа использует наработки из [проекта математического дифференциатора](https://github.com/ralex2304/differentiator).

Возможности:
- свёртка константных выражений;
- упрощение математических выражений:
    - удаление нейтральных элементов (`x + 0`, `x * 1`);
    - свёртка выражений независимым результатом (`x * 0`);
    - удаление парных постфиксных и префиксных операторов (`x++--`);
- удаление мёртвого кода;
- математическое дифференцирование по произвольной переменной.

Так, например, выявляются константные условные выражения и выдаются предупреждения о недостижимости участка кода:

<img rel="Предупреждение о константном условии" src="img/const_clause_warning.png" width="75%">

### Backend

// TODO описание бекенда

### IR Backend

// TODO описание IR бекенда

## Зависимости

### Зависимости сборки

1. [`GNU make`](https://www.gnu.org/software/make/) - система сборки
2. [`clang`](https://clang.llvm.org/) - компилятор - можно заменить на `gcc`, изменив четыре `Makefile`
3. [`bear`](https://github.com/rizsotto/Bear) - *необязательно (`make build nobear=1`)* - утилита для создания файла `compile_commands.json`

#### Библиотеки

1. `Tree` и/или `TreeDebug` - [релизы](https://github.com/ralex2304/Tree/releases) - библиотека для работы с бинарными деревьями

##### Установка

Заменить символические ссылки в директории `./lib` на папки с файлами `.h` и `.a`. Скачать их можно в разделе `Releases` соответствующих библиотек

### Зависимости компиляции

1. [`iconv`](https://ru.wikipedia.org/wiki/Iconv) - используется скриптом `compiler.sh` для отображения русских символов в консоли
2. [`graphviz dot`](https://graphviz.org/docs/layouts/dot/) - *необязательно* - графический дамп внутренних структур программ при ошибках (только в Debug версии)

## Использование

### Сборка

```
make build <release=1> <nobear=1>
```

#### Release версия

- опция компиляции `-O2`.

```
make build release=1
```
#### Debug версия

- верификаторы структур данных;
- логирование внутренних ошибок;
- sanitizer;
- опция компиляции `-Og`;
- assert.

```
make build
```

### Компиляция программы

По умолчанию выбрана архитектура `x86-64`

```
./compiler.sh <file>
```

#### Опции компиляции

| Опция              | Описание |
|:-------------------|:---------|
| `--help`           | Вывести опции компиляции
| `-o <file>`        | Задать имя выходного файла
| `-m={spu\|x86_64}` | Выбрать архитектуру
| `-l=<file>`        | Задать имя объектного файла библиотеки ввода/вывода
| `-S`               | Включить генерацию текстового ассемблерного листинга
