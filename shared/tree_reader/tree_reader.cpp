#include "tree_reader.h"

static Status::Statuses read_tree_traversal_(Tree* tree, char* text, size_t* const pos,
                                             TreeNode** node, TreeNode* parent);

static Status::Statuses read_tree_(Tree* tree, char* text, size_t* const pos);

static Status::Statuses read_vars_(Vector* vars, char* text, size_t* const pos);



Status::Statuses read_tree(Tree* tree, Vector* vars, char** text, const char* filename) {
    assert(tree);
    assert(vars);
    assert(filename);

    STATUS_CHECK(file_open_read_close(filename, text));
    assert(text);

    size_t pos = 0;
    STATUS_CHECK(read_tree_(tree, *text, &pos));

    STATUS_CHECK(read_vars_(vars, *text, &pos));

    return Status::NORMAL_WORK;
}

#define ERR_(...)   do {                                        \
                        fprintf(stderr, "Error. " __VA_ARGS__); \
                        return Status::INPUT_ERROR;             \
                    } while (0)

static Status::Statuses read_vars_(Vector* vars, char* text, size_t* const pos) {
    assert(vars);
    assert(text);
    assert(pos);

    while (text[*pos]) {
        char* line_end = strchr(text + *pos, '\n');
        if (line_end == nullptr)
            break;

        *line_end = '\0';
        String var = {.s = text + *pos, .len = (size_t)(line_end - (text + *pos))};
        if (!vars->push_back(&var))
            return Status::MEMORY_EXCEED;

        *pos += line_end - var.s + 1;
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses read_tree_(Tree* tree, char* text, size_t* const pos) {
    assert(tree);
    assert(text);
    assert(pos);

    STATUS_CHECK(read_tree_traversal_(tree, text, pos, &tree->root, nullptr));

    if (text[*pos] != '\n')
        ERR_("Expected '\\n'\n");
    (*pos)++;

    return Status::NORMAL_WORK;
}



static Status::Statuses read_tree_traversal_(Tree* tree, char* text, size_t* const pos,
                                             TreeNode** node, TreeNode* parent) {
    assert(tree);
    assert(text);
    assert(pos);
    assert(node);
    assert(*node == nullptr);

    if (strncmp("(nil)", text + *pos, 5) == 0) {
        *pos += 5;
        return Status::NORMAL_WORK;
    }

    if (text[*pos] != '(')
        ERR_("'(' expected instead of \"%.10s\"\n", text + *pos);

    (*pos)++;

    TreeElem elem = {};

    if (strncmp("{\"", text + *pos, 2) == 0) {
        *pos += 2;
        char* filename_end = strchr(text + *pos, '"');
        if (filename_end == nullptr)
            ERR_("'\"' not found\n");

        *filename_end = '\0';
        elem.debug_info.filename = text + *pos;

        *pos += filename_end - text - *pos + 1;

        int len = -1;
        if (sscanf(text + *pos, ", %zu, %zu, %zu %n", &elem.debug_info.line, &elem.debug_info.symbol,
                                                    &elem.debug_info.line_position, &len) != 3)
            ERR_("Expected debug info\n");
        assert(len > 0);

        *pos += len;

        if (strncmp("}, ", text + *pos, 3) != 0)
            ERR_("Expected \"}, \" instead of \"%.10s\"\n", text + *pos);
        *pos += 3;
    }

    int len = -1;
    int type = -1;
    if (sscanf(text + *pos, "%d, %n", &type, &len) != 1)
        ERR_("Expected type\n");
    assert(len > 0);

    elem.type = (TreeElemType)type;
    *pos += len;

    if (text[*pos] == ' ') (*pos)++;

    int oper = -1;

    switch (elem.type) {
        case TreeElemType::OPER:
            oper = -1;
            if (sscanf(text + *pos, "%d %n", &oper, &len) != 1)
                ERR_("Expected oper num\n");
            elem.data.oper = (OperNum)oper;
            break;
        case TreeElemType::VAR:
            if (sscanf(text + *pos, "%zu %n", &elem.data.var, &len) != 1)
                ERR_("Expected var num\n");
            break;
        case TreeElemType::NUM:
            if (sscanf(text + *pos, "%lg %n", &elem.data.num, &len) != 1)
                ERR_("Expected number\n");
            break;

        case TreeElemType::NONE:
        default:
            ERR_("Unknown type \"%d\"\n", (int)elem.type);
            break;
    }
    *pos += len;

    if (strncmp(", ", text + *pos, 2) != 0)
        ERR_("Expected \", \"\n");
    *pos += 2;

    if (tree_insert(tree, node, parent, &elem) != Tree::OK)
        return Status::TREE_ERROR;

    STATUS_CHECK(read_tree_traversal_(tree, text, pos, &(*node)->left, *node));

    if (text[*pos] == ' ')
        (*pos)++;

    STATUS_CHECK(read_tree_traversal_(tree, text, pos, &(*node)->right, *node));

    if (text[*pos] != ')')
        ERR_("Expected ')'\n");
    (*pos)++;

    return Status::NORMAL_WORK;
}
