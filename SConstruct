import os

# NOTE: uncomment -save-temps to examine ASM code...
# swithced to c++0x because of libcds
BASE_FLAGS = '-Wall -Wextra' + ' -std=c++0x' #+ ' -save-temps' # -ansi -pedantic
DEBUG_FLAGS = BASE_FLAGS + ' -g'
RELEASE_FLAGS = BASE_FLAGS + ' -DNDEBUG -O3' #-pg

env = Environment(ENV=os.environ)
Export('env')

# Getting OS env variables for use with Clang static analyser scan-build
env["CC"] = os.getenv("CC") or env["CC"]
env["CXX"] = os.getenv("CXX") or env["CXX"]
env["ENV"].update(x for x in os.environ.items() if x[0].startswith("CCC_"))

# Debug flags: > scons debug=0 to turn debugging off
# Support for multiple debug levels if required
flags = RELEASE_FLAGS
debug_level = ARGUMENTS.get('debug', 1)
if int(debug_level) > 0:
    flags = DEBUG_FLAGS + ' -DDEBUG=' + str(debug_level)
env.Append(CCFLAGS = flags.split())

# Other flags
env.Append(LIBPATH=[ "./", "deps/libcds-v1.0.2/lib" ])
env.Append(CPPPATH=[ "./", "deps",
                     "deps/boost_1_43_0",
                     "deps/tclap-1.2.0/include",
                     "deps/libcds-v1.0.2/includes"])

# Libs
env.StaticLibrary(target = "Indexes", source = env.Glob("indexes/*.cc"),
    LIBS = [ "libcds" ])
env.StaticLibrary(target = "Nanotime", source = env.Glob("deps/nanotime_wrapper/*.c"))
env.StaticLibrary(target = "CppUnitLite",
    source = env.Glob("deps/CppUnitLite/*.cpp"))

# Unit Tests
tests = env.Program("runtests", LIBS = ["CppUnitLite", "Indexes", "libcds" ],
    source=[ "tests/TestRunner.cc", env.Glob("tests/*Test.cc")])
# Run the unit tests as part of the build process
env.AddPostAction(tests, "./runtests")
env.AlwaysBuild(tests)
env.Alias('test', tests)

# Program
wt_test = env.Program(target = "wt_test", 
    LIBS = ["Indexes", "Nanotime", "libcds"], source = ["main.cc"])
