project('script', 'c')

deps = []

#if build_machine.system() == 'windows'
#  deps += meson.get_compiler('c').find_library('Shlwapi')
#endif

executable('script',
  'alloc.c',
  'lenstring.c',
  'vector.c',
  'path.c',
  'file.c',
  'which.c',
  'ast.c',
  'lex.c',
  'parse.c',
  'interpreter.c',
  'main.c',
  dependencies : deps,
)
