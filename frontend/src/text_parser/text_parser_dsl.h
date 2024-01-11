#ifndef TEXT_PARSER_DSL_H_
#define TEXT_PARSER_DSL_H_


#define IS_TOKEN_TYPE(token_, type_) ((token_).type == type_)

#define IS_TOKEN_TERM_EQ(token_, term_) (IS_TOKEN_TYPE(token_, TokenType::TERM) && (token_).data.term == term_)

#define CUR_TOKEN (*(Token*)(data->tokens[*pos]))

#define NEXT_TOKEN (*(Token*)(data->tokens[*pos + 1]))

#define CUR_TOKEN_DEBUG_INFO CUR_TOKEN.debug_info

#define TREE_INSERT(node_, parent_, elem_)                                      \
            if (tree_insert(&data->tree, node_, parent_, elem_) != Tree::OK)    \
                return Status::TREE_ERROR

#define L(node_) (&(node_)->left)
#define R(node_) (&(node_)->right)

#define ELEM(node_)  ((TreeElem*)((node_)->elem))

#define NODE_DATA(node_) (ELEM(node_)->data)
#define NODE_TYPE(node_) (ELEM(node_)->type)

#define NUM_ELEM(val_, debug_)      {.type = TreeElemType::NUM,  .data = {.num  = val_},    \
                                     .debug_info = debug_}
#define OPER_ELEM(oper_, debug_)    {.type = TreeElemType::OPER, .data = {.oper = oper_},   \
                                     .debug_info = debug_}
#define VAR_ELEM(var_num_, debug_)  {.type = TreeElemType::VAR,  .data = {.var = var_num_}, \
                                     .debug_info = debug_}


#endif //< #ifndef TEXT_PARSER_DSL_H_
