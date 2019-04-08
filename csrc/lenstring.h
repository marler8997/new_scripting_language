DEFINE_ARRAY(string, const char)
DEFINE_ARRAY(mutable_string, char)

static inline struct string as_string(struct mutable_string str)
{
  return *(struct string*)&str;
}

#define strlit(lit) make_string(lit, sizeof(lit) - 1)

static inline struct string cstr_to_str(const char *cstr)
{
  return make_string(cstr, strlen(cstr));
}

static inline char *malloc_str_to_cstr(struct string str)
{
  char *buffer = malloc(str.length + 1);
  if (buffer) {
    memcpy(buffer, str.ptr, str.length);
    buffer[str.length] = '\0';
  }
  return buffer;
}

static inline char string_equals(struct string left, struct string right)
{
  return left.length == right.length &&
    0 == memcmp(left.ptr, right.ptr, left.length);
}
#define string_equals_literal(str, lit)                         \
     string_equals(str, make_string(lit, sizeof(lit) - 1))

bool str_ends_with(struct string left, struct string right);
bool str_starts_with(struct string left, struct string right);
//size_t str_diff_index(struct string left, struct string right);

/*
THIS FUNCTION DOESN'T WORK ON ARM
struct mutable_string concat_impl(struct string first, ...);
#define concat(...) concat_impl(__VA_ARGS__, make_string(NULL, 0))
*/
