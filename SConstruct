
source  = ['Source/sternh.cpp']
binary  = 'sternh'
app     = 'Sternhalma'
version = '0.0.2'

# Get a build environment
env,lib = SConscript('Platform/build.scons', ['app', 'version'])

# Project specific build config
env.Append(CCFLAGS = ['-O3', '-DTERMINAL_EMULATOR'])

# Builders
exe = env.Program(binary, source)
Depends(exe, lib)

env.Tar(app+'_'+env['target']+'_'+env['machine']+'_'+version+'.tgz',
        [exe, env['platform_files'], 'LICENSE'])

