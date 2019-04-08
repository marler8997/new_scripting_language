
enum ast_node_type {
  AST_NODE_STRING,
  AST_NODE_CALL,
  AST_NODE_RUN,
};
extern const char *ast_node_type_strings[];

DEFINE_TYPED_VECTOR(ast_node, struct ast_node);

struct ast_node
{
  //char *fileptr;
  enum ast_node_type type;
  union
  {
    struct {
      struct string escaped;
    } string;
    struct {
      struct string function;
      struct ast_node_vector args;
    } call;
    struct {
      struct ast_node_vector args;
    } run;
  };
};

static inline void init_ast_node(struct ast_node *node, enum ast_node_type type/*, char *fileptr*/)
{
  //node->fileptr = fileptr;
  node->type = type;
  //memset(&node->union_data, 0, sizeof(node->union_data));
}

void dump_nodes(struct ast_node_vector *nodes);
