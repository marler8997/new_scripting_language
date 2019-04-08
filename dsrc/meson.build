project('script', 'd')

deps = []

#if build_machine.system() == 'windows'
#  deps += meson.get_compiler('c').find_library('Shlwapi')
#endif

executable('script',
  'alloc.d',
  'lenstring.d',
  'vector.d',
  'path.d',
  'file.d',
  'which.d',
  'ast.d',
  'lex.d',
  'parse.d',
  'interpreter.d',
  'main.d',
  dependencies : deps,
)
