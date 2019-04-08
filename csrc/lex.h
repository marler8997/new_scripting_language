enum token_type {
  TOKEN_TYPE_ID,
  TOKEN_TYPE_LEFT_PAREN,
  TOKEN_TYPE_RIGHT_PAREN,
  TOKEN_TYPE_STRING,
  TOKEN_TYPE_EOF,
};
extern const char *token_type_strings[];

struct token
{
  struct string source;
  enum token_type type;
  union {
    /*
    struct {
    } id;
    */
    struct {
      struct string escaped;
    } string;
  };
};

static inline void set_token_to_id(struct token *token, struct string source)
{
  token->source = source;
  token->type = TOKEN_TYPE_ID;
}
static inline void set_token_to_eof(struct token *token, const char *eof_ptr)
{
  token->source = make_string(eof_ptr, 0);
  token->type = TOKEN_TYPE_EOF;
}

enum keyword {
  KEYWORD_CALL,
  KEYWORD_RUN,
};
bool try_as_keyword(struct string source, enum keyword *result);

struct parser
{
  struct script_file *file;
  const char *next;
  unsigned line_number;
  struct token token; // the last token that was parsed
};

static void report_unexpected_token(struct parser *parser, enum token_type expected, struct token *actual)
{
  errorf("%s(line %d) expected '%s' but got '%s'", parser->file->filename, parser->line_number,
         token_type_strings[expected], token_type_strings[actual->type]);
}
inline static err_t unexpected_token(struct parser *parser, enum token_type expected, struct token *actual)
{
  report_unexpected_token(parser, expected, actual);
  return err_fail;
}

/*
Either returns the last value in parser->token or reads a new value into it.
Use this function when you're not sure which token will be next and you're not
sure if you are going to consume it right away.
 */
err_t read_token(struct parser *parser);

/*
Consume the token in parser->token.  This will force read_token to parse a new value next time.
 */
static inline err_t consume_token(struct parser *parser)
{
  if (!parser->token.source.ptr) {
    errorf("codebug: consume_token was called but the current token is null");
    return err_fail;
  }
  parser->token.source = make_string(NULL, 0);
  return err_pass;
}

/*
Read a token, return it to the given 'token' and consume it.
Use this function when you're not sure which token will be next but you know you
are going to consume it right away.
 */
static inline err_t read_and_consume_token(struct parser *parser, struct token *token)
{
  if (read_token(parser))
    return err_fail;
  *token = parser->token;
  return consume_token(parser);
}

/*
Read an expected token and consume it, but don't return it.
Use this function when you know what token you are expecting and you don't need to see it.
 */
static inline err_t expect_and_ignore_token(struct parser *parser, enum token_type expected_type)
{
  if (read_token(parser))
    return err_fail;
  if (parser->token.type != expected_type)
      return unexpected_token(parser, expected_type, &parser->token);
  return consume_token(parser);
}
/*
Read an expected token, consume it and return it.
Use this function when you know what kind of token you're expecting and you want to see it.
 */
static inline err_t expect_and_return_token(struct parser *parser, enum token_type expected_type, struct token *token)
{
  if (read_token(parser))
    return err_fail;
  if (parser->token.type != expected_type)
      return unexpected_token(parser, expected_type, &parser->token);
  *token = parser->token;
  return consume_token(parser);
}
