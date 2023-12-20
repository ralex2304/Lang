#include "tree_output.h"

static Status::Statuses write_tree_(ParseData* data, FILE* file);

static Status::Statuses write_tree_print_debug_data_(FILE* file, DebugInfo* info);

static Status::Statuses write_tree_traversal_(ParseData* data, FILE* file, TreeNode* node);

static Status::Statuses write_vars_(ParseData* data, FILE* file);



#define PRINT_(...)     do {                                        \
                            if (!file_printf(file, __VA_ARGS__))    \
                                return Status::OUTPUT_ERROR;        \
                        } while (0)

Status::Statuses tree_output_write(ParseData* data, const char* filename) {
    assert(data);
    assert(filename);

    FILE* file = {};
    if (!file_open(&file, filename, "wb"))
        return Status::OUTPUT_ERROR;

    STATUS_CHECK(write_tree_(data, file), file_close(file));

    STATUS_CHECK(write_vars_(data, file), file_close(file));

    if (!file_close(file))
        return Status::OUTPUT_ERROR;

    return Status::NORMAL_WORK;
}

static Status::Statuses write_vars_(ParseData* data, FILE* file) {
    assert(data);
    assert(file);

    for (ssize_t i = 0; i < data->vars.size(); i++) {
        String* str = (String*)(data->vars[i]);

        PRINT_("%.*s\n", (int)str->len, str->s);
    }

    return Status::NORMAL_WORK;
}

static Status::Statuses write_tree_(ParseData* data, FILE* file) {
    assert(data);
    assert(file);

    STATUS_CHECK(write_tree_traversal_(data, file, data->tree.root));

    PRINT_("\n");

    return Status::NORMAL_WORK;
}

static Status::Statuses write_tree_traversal_(ParseData* data, FILE* file, TreeNode* node) {
    assert(data);
    assert(file);

    if (node == nullptr) {
        PRINT_("(nil)");

        return Status::NORMAL_WORK;
    }

    PRINT_("(");

    STATUS_CHECK(write_tree_print_debug_data_(file, &((TreeElem*)(node->elem))->debug_info));

    PRINT_(", %d, ", (int)(((TreeElem*)(node->elem))->type));

    switch ((((TreeElem*)(node->elem))->type)) {
        case TreeElemType::OPER:
            PRINT_("%d", (int)(((TreeElem*)(node->elem))->data.oper));
            break;
        case TreeElemType::NUM:
            PRINT_("%lg", ((TreeElem*)(node->elem))->data.num);
            break;
        case TreeElemType::VAR:
            PRINT_("%zu", ((TreeElem*)(node->elem))->data.var);
            break;

        case TreeElemType::NONE:
        default:
            PRINT_("ERR");
            break;
    }

    PRINT_(", ");

    STATUS_CHECK(write_tree_traversal_(data, file, node->left));

    PRINT_(" ");

    STATUS_CHECK(write_tree_traversal_(data, file, node->right));

    PRINT_(")");

    return Status::NORMAL_WORK;
}

static Status::Statuses write_tree_print_debug_data_(FILE* file, DebugInfo* info) {
    assert(file);
    assert(info);

    PRINT_("{\"%s\", %zu, %zu, %zu}", info->filename, info->line, info->symbol, info->line_position);

    return Status::NORMAL_WORK;
}
