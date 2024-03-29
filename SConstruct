#!/usr/bin/env python3

import sys
import os
import os.path
import glob
import re

if sys.version_info < (3,):
    def isbasestring(s):
        return isinstance(s,basestring)
else:
    def isbasestring(s):
        return isinstance(s, (str,bytes))

def add_kel_source_files(self, sources, filetype, lib_env=None, shared=False, target_post=""):

    if isbasestring(filetype):
        dir_path = self.Dir('.').abspath
        filetype = sorted(glob.glob(dir_path+"/"+filetype))

    for path in filetype:
        target_name = re.sub( r'(.*?)(\.cpp|\.c\+\+)', r'\1' + target_post, path )
        if shared:
            target_name+='.os'
            sources.append( self.SharedObject( target=target_name, source=path ) )
        else:
            target_name+='.o'
            sources.append( self.StaticObject( target=target_name, source=path ) )
    pass

def isAbsolutePath(key, dirname, env):
	assert os.path.isabs(dirname), "%r must have absolute path syntax" % (key,)

env_vars = Variables(
	args = ARGUMENTS
)

env_vars.Add('prefix',
	help='Installation target location of build results and headers',
	default='/usr/local',
	validator=isAbsolutePath
)

env=Environment(ENV=os.environ, variables=env_vars, CPPPATH=['#source','#','#driver'],
    CXX='clang++',
    CPPDEFINES=['SAW_UNIX_XCB'],
    CXXFLAGS=['-std=c++20','-g','-Wall','-Wextra'],
    LIBS=['forstio','xcb','X11','X11-xcb'])
env.__class__.add_source_files = add_kel_source_files

env.sources = []
env.headers = []

env.gl_sources = []
env.gl_headers = []

Export('env')
SConscript('source/forstio/window/SConscript')
SConscript('driver/SConscript')

# Library build

env_library = env.Clone()

env.objects_shared = []
env_library.add_source_files(env.objects_shared, env.sources, shared=True)
env_library.add_source_files(env.objects_shared, env.gl_sources, shared=True)
env.library_shared = env_library.SharedLibrary('#bin/forstio-window', [env.objects_shared])

env.objects_static = []
env_library.add_source_files(env.objects_static, env.sources)
env_library.add_source_files(env.objects_static, env.gl_sources)
env.library_static = env_library.StaticLibrary('#bin/forstio-window', [env.objects_static])

env.Alias('library', [env.library_shared, env.library_static])
env.Alias('library_shared', env.library_shared)
env.Alias('library_static', env.library_static)

env.Default('library')

# Tests
SConscript('test/SConscript')

env.Alias('test', env.test_program)

# Clang format part
env.Append(BUILDERS={'ClangFormat' : Builder(action = 'clang-format --style=file -i $SOURCE')})
env.format_actions = []
def format_iter(env,files):
    for f in files:
        env.format_actions.append(env.AlwaysBuild(env.ClangFormat(target=f+"-clang-format",source=f)))
    pass

format_iter(env,env.sources + env.headers + env.gl_sources + env.gl_headers)

env.Alias('format', env.format_actions)

env.Alias('all', ['format', 'library_shared', 'library_static'])

env.Install('$prefix/lib/', [env.library_shared, env.library_static])
env.Install('$prefix/include/forstio/window/', [env.headers])
env.Install('$prefix/include/forstio/window/gl/', [env.gl_headers])
env.Alias('install', '$prefix')
