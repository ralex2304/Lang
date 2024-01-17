#include "TreeAddonDump.h"

#include "utils/html.h"
#include "utils/ptr_valid.h"

// They are inside tree lib
#include "log/log.h"
#include "log/graph_log.h"

extern LogFileData log_file;

static const char* get_oper_str_(OperNum oper);

static const char* get_num_str_(double num);

static Status::Statuses tree_addon_dot_node_val_color_(Vector* vars, TreeNode* node,
                                                       const char** str_val, const char** color);

static Status::Statuses tree_addon_dot_dump_traversal_(Vector* vars, Tree* tree, TreeNode* node,
                                                       FILE* file, size_t* nil_num = nullptr);

static Status::Statuses tree_addon_dot_dump_traversal_next_(Vector* vars, Tree* tree, TreeNode* parent,
                                                            TreeNode* node, FILE* file);

#define FPRINTF_(...) if (fprintf(file, __VA_ARGS__) == 0) return Status::OUTPUT_ERROR

#define BACKGROUND_COLOR "\"#1f1f1f\""
#define FONT_COLOR       "\"#000000\""
#define NODE_PREFIX      "elem_"
#define NODE_PARAMS      "shape=\"rect\", style=\"filled\", fillcolor=\"#6e7681\""
#define ZERO_NODE_PARAMS "shape=\"rect\", style=\"filled\", fillcolor=\"#6e7681\", color=yellow"

static Status::Statuses tree_addon_dot_dump_traversal_(Vector* vars, Tree* tree, TreeNode* node,
                                                       FILE* file, size_t* nil_num) {
    assert(vars);
    assert(tree);
    // node can be nullptr
    assert(file);
    // nil_num can be nullptr

    const char* str_val = nullptr;
    const char* color   = nullptr;

    STATUS_CHECK(tree_addon_dot_node_val_color_(vars, node, &str_val, &color));

    if (node == nullptr) {
        static size_t nil_node_cnt = 0;

        if (nil_num != nullptr)
            *nil_num = nil_node_cnt;

        FPRINTF_(NODE_PREFIX "nil_%zu [label=\"%s\", fillcolor=\"%s\"]\n" ,
                                nil_node_cnt++, str_val,         color);
        return Status::NORMAL_WORK;
    }

    FPRINTF_(NODE_PREFIX "%p [label=\"%s\", fillcolor=\"%s\"]\n" , node, str_val, color);

    STATUS_CHECK(tree_addon_dot_dump_traversal_next_(vars, tree, node, node->left, file));

    STATUS_CHECK(tree_addon_dot_dump_traversal_next_(vars, tree, node, node->right, file));

    return Status::NORMAL_WORK;
}

static Status::Statuses tree_addon_dot_dump_traversal_next_(Vector* vars, Tree* tree, TreeNode* parent,
                                                            TreeNode* node, FILE* file) {
    assert(vars);
    assert(tree);
    assert(parent);
    // node can be nullptr
    assert(file);

    size_t nil_num = 0;
    STATUS_CHECK(tree_addon_dot_dump_traversal_(vars, tree, node, file, &nil_num));

    if (node == nullptr) {
        FPRINTF_(NODE_PREFIX "%p->" NODE_PREFIX "nil_%zu[color=green]\n", parent, nil_num);
        return Status::NORMAL_WORK;
    }

    FPRINTF_(NODE_PREFIX "%p->" NODE_PREFIX "%p[color=green]\n", parent, node);

    return Status::NORMAL_WORK;
}

#define ELEM(node_) ((TreeElem*)(node_->elem))

static Status::Statuses tree_addon_dot_node_val_color_(Vector* vars, TreeNode* node,
                                                       const char** str_val, const char** color) {
    assert(vars);
    // node can be nullptr
    assert(str_val);
    assert(*str_val == nullptr);
    assert(color);
    assert(*color == nullptr);

    if (node == nullptr) {
        *str_val = "nil";
        *color   = "#6e7681";
        return Status::NORMAL_WORK;
    }

    switch (ELEM(node)->type) {
        case TreeElemType::OPER:
            *str_val = get_oper_str_(ELEM(node)->data.oper);
            *color = "#296dff";
            break;
        case TreeElemType::VAR:
            *str_val = *(const char**)((*vars)[ELEM(node)->data.var]);
            *color = "#e20f0f";
            break;

        case TreeElemType::NUM:
            *str_val = get_num_str_(ELEM(node)->data.num);
            *color = "#dcbb1b";
            break;

        case TreeElemType::NONE:
        default:
            assert(0 && "Wrong TreeElemType passed");
            return Status::TREE_ERROR;
    }

    return Status::NORMAL_WORK;
}

static const char* get_oper_str_(OperNum oper) {

    switch (oper) {

#define DEF_OPER(num_, name_, ...)   \
            case OperNum::name_:    \
                return #name_;

#include "operators.h"

#undef DEF_OPER

        case OperNum::NONE:
        default:
            assert(0 && "Wrong oper given");
            return nullptr;
    }

    assert(0 && "Something really strange happened. This must be unreachable");
    return nullptr;
}

static const char* get_num_str_(const double num) {
    static char buf[64] = {};

    if (snprintf(buf, 64, "%lg", num) > 0)
        return buf;

    return nullptr;
}

Status::Statuses tree_addon_dump_dot(Vector* vars, Tree* tree) {
    assert(tree);

    static size_t dot_number = 0;

    if (!log_create_timestamp_dir(&log_file))
        return Status::FILE_ERROR;

    char dot_filename[log_file.MAX_FILENAME_LEN] = {};
    char img_filename[log_file.MAX_FILENAME_LEN] = {};

    size_t str_len = strncat_len(dot_filename, log_file.timestamp_dir, log_file.MAX_FILENAME_LEN);
    snprintf(dot_filename + str_len, log_file.MAX_FILENAME_LEN - str_len,
             "%zd", dot_number);
    str_len = strncat_len(dot_filename, ".dot", log_file.MAX_FILENAME_LEN);

    FILE* file = fopen(dot_filename, "wb");
    if (file == nullptr)
        return Status::FILE_ERROR;

    FPRINTF_("digraph List{\n"
             "    graph [bgcolor=" BACKGROUND_COLOR "];\n"
             "    node[shape=rect, color=white, fontcolor=" FONT_COLOR ", fontsize=14, "
             "fontname=\"verdana\", style=\"filled\", fillcolor=\"#6e7681\"];\n\n");

    STATUS_CHECK(tree_addon_dot_dump_traversal_(vars, tree, tree->root, file));

    FPRINTF_("}\n");

    if (fclose(file) != 0) {
        perror("Error closing file");
        return Status::FILE_ERROR;
    }

    str_len = strncat_len(img_filename, log_file.timestamp_dir, log_file.MAX_FILENAME_LEN);
    snprintf(img_filename + str_len, log_file.MAX_FILENAME_LEN - str_len, "%zd", dot_number++);
    str_len = strncat_len(img_filename, ".svg", log_file.MAX_FILENAME_LEN);

    if (!convert_to_utf8(dot_filename))
        return Status::FILE_ERROR;

    if (!create_img(dot_filename, img_filename)) {
        fprintf(stderr, "Error creating dot graph\n");
        return Status::FILE_ERROR;
    }

    return Status::NORMAL_WORK;
}
#undef FPRINTF_

