struct script_file
{
  const char *filename;
  struct string text;
  struct ast_node_vector nodes;
};
