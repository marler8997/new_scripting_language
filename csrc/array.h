#define DEFINE_ARRAY(name, element_type)                                \
  struct name                                                           \
  {                                                                     \
    element_type *ptr;                                                  \
    size_t length;                                                      \
  };                                                                    \
  static inline struct name make_##name(element_type *ptr, size_t length) \
  {                                                                     \
    struct name array;                                                  \
    array.ptr = ptr;                                                    \
    array.length = length;                                              \
    return array;                                                       \
  }
