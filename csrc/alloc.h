#define malloc_or_exit(size) malloc_or_exit_func(size, __FILE__, __LINE__)
void *malloc_or_exit_func(size_t size, const char *sourcefile, unsigned line);
