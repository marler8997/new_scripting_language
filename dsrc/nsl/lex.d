module nsl.lex;

enum TokenType : ubyte
{
    id,
    leftParen,
    rightParen,
    string_,
    eof,
};
string[] tokenTypeStrings = [
    "id",
    "(",
    ")",
    "string",
    "end-of-file",
];

struct Token
{
    string source;
    enum TokenType type;
    union
    {
        /*
        struct {
        } id;
        */
        private struct String
	{
            string escaped;
        }
	String string_;
    };
};

void setTokenToID(Token *token, string source)
{
  token.source = source;
  token.type = TokenType.id;
}
void setTokenToEof(Token *token, const char *eof_ptr)
{
  token.source = make_string(eof_ptr, 0);
  token.type = EOF;
}

enum keyword {
  KEYWORD_CALL,
  KEYWORD_RUN,
};
bool try_as_keyword(string source, enum keyword *result);

struct parser
{
  struct script_file *file;
  const char *next;
  unsigned line_number;
  Token token; // the last token that was parsed
};

static void report_unexpected_token(struct parser *parser, TokenType expected, Token *actual)
{
  errorf("%s(line %d) expected '%s' but got '%s'", parser.file.filename, parser.line_number,
         tokenTypeStrings[expected], tokenTypeStrings[actual.type]);
}
inline static err_t unexpected_token(struct parser *parser, TokenType expected, Token *actual)
{
  report_unexpected_token(parser, expected, actual);
  return err_fail;
}

/*
Either returns the last value in parser.token or reads a new value into it.
Use this function when you're not sure which token will be next and you're not
sure if you are going to consume it right away.
 */
err_t read_token(struct parser *parser);

/*
Consume the token in parser.token.  This will force read_token to parse a new value next time.
 */
static inline err_t consume_token(struct parser *parser)
{
  if (!parser.token.source.ptr) {
    errorf("codebug: consume_token was called but the current token is null");
    return err_fail;
  }
  parser.token.source = make_string(NULL, 0);
  return err_pass;
}

/*
Read a token, return it to the given 'token' and consume it.
Use this function when you're not sure which token will be next but you know you
are going to consume it right away.
 */
static inline err_t read_and_consume_token(struct parser *parser, Token *token)
{
  if (read_token(parser))
    return err_fail;
  *token = parser.token;
  return consume_token(parser);
}

/*
Read an expected token and consume it, but don't return it.
Use this function when you know what token you are expecting and you don't need to see it.
 */
static inline err_t expect_and_ignore_token(struct parser *parser, TokenType expected_type)
{
  if (read_token(parser))
    return err_fail;
  if (parser.token.type != expected_type)
      return unexpected_token(parser, expected_type, &parser.token);
  return consume_token(parser);
}
/*
Read an expected token, consume it and return it.
Use this function when you know what kind of token you're expecting and you want to see it.
 */
static inline err_t expect_and_return_token(struct parser *parser, TokenType expected_type, Token *token)
{
  if (read_token(parser))
    return err_fail;
  if (parser.token.type != expected_type)
      return unexpected_token(parser, expected_type, &parser.token);
  *token = parser.token;
  return consume_token(parser);
}

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "bool.h"
#include "err_t.h"
#include "array.h"
#include "lenstring.h"
#include "vector.h"

#include "log.h"
#include "ast.h"
#include "scriptfile.h"
#include "lex.h"

bool try_as_keyword(struct string source, enum keyword *result)
{
  // TODO: should we use a HASH? Check performance.
  switch(source.length) {
  case 3:
    if (0 == memcmp(source.ptr, "run", source.length)) {
      *result = KEYWORD_RUN;
      return true;
    }
  case 4:
    if (0 == memcmp(source.ptr, "call", source.length)) {
      *result = KEYWORD_CALL;
      return true;
    }
    break;
  }
  return false; // not a keyword
}

// skip whitespace and comments
static void skip_trivial(struct parser *parser)
{
  for (;;) {
    char c = parser.next[0];
    if (c == ' ' || c == '\t' || c == '\r') {
      parser.next++;
    } else if (c == '\n') {
      parser.line_number++;
      parser.next++;
    } else if (c == '#') {
      for (;;) {
        parser.next++;
        c = parser.next[0];
        if (c == '\n') {
          parser.next++;
          parser.line_number++;
          break;
        }
        if (c == '\0')
          return;
      }
    } else {
      return;
    }
  }
}

static bool is_identifier_start(char c)
{
  if (c >= 'a') return c <= 'z';
  if (c >= 'A') return c <= 'Z' || c == '_';
  return false;
}
static bool is_identifier_mid(char c)
{
  if (is_identifier_start(c))
    return true;
  if (c >= '0') return c <= '9';
  return false;
}
// Assumption: parser is at the start of the identifier
//             is_identifier_start(parser.next[0]) was already checked to be true
static struct string peel_identifier(struct parser *parser)
{
  const char *start = parser.next;
  for (;;) {
    parser.next++;
    char c = parser.next[0];
    if (!is_identifier_mid(c))
      break;
  }
  return make_string(start, parser.next - start);
}

// assumption: paser points to the opening quote of the string
static err_t parse_quoted_string(struct parser *parser, Token *token)
{
  if (parser.next[0] != '"') {
    errorf("code bug: parse_quoted_string was called but parser does not point to a double-quote character");
    return err_fail;
  }
  const char *start = parser.next;
  for (;;) {
    parser.next++;
    char c = parser.next[0];
    if (c == '"') {
      if ((parser.next -1)[0] == '\\') {
        errorf("escape sequences not implemented");
        return err_fail;
      }
      parser.next++;
      token.source = make_string(start, parser.next - start);
      token.type = TOKEN_TYPE_STRING;
      // TODO: actually escape the string
      token.string.escaped = make_string(start + 1, parser.next - start - 2);
      return err_pass;
    }
    if (c == '\n' || c == '\0') {
      // TODO: print file location
      errorf("unterminated double-quoted string");
      return err_fail;
    }
  }
}

static struct {
  char c;
  enum token_type type;
} one_character_tokens[] = {
  {'(', TOKEN_TYPE_LEFT_PAREN},
  {')', TOKEN_TYPE_RIGHT_PAREN},
  {'\0', 0},
};

err_t read_token(struct parser *parser)
{
  if (parser.token.source.ptr)
    return err_pass;

  skip_trivial(parser);
  char c = parser.next[0];
  if (is_identifier_start(c)) {
    struct string id = peel_identifier(parser);
    setTokenToID(&parser.token, id);
    return err_pass;
  }
  for(unsigned i = 0; one_character_tokens[i].c != '\0'; i++) {
    if (c == one_character_tokens[i].c) {
      parser.token.source = make_string(parser.next, 1);
      parser.next++;
      parser.token.type = one_character_tokens[i].type;
      return err_pass;
    }
  }
  if (c == '"')
    return parse_quoted_string(parser, &parser.token);

  if (c == '\0') {
    setTokenToEof(&parser.token, parser.next);
    return err_pass;
  }
  errorf("%s(%d): could not interpret the following as a token: %s",
         // TODO: don't print the rest of the file
         parser.file.filename, parser.line_number, parser.next);
  return err_fail;
}
