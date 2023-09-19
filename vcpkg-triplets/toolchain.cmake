
message(STATUS "Using qgis-js toolchain")

include("$ENV{EMSCRIPTEN_DIR}/cmake/Modules/Platform/Emscripten.cmake")

# https://github.com/retifrav/vcpkg-registry/blob/master/triplets/decovar-wasm32-emscripten-pthreads.cmake

set(CMAKE_C_FLAGS_INIT   "${CMAKE_C_FLAGS_INIT}   -pthread")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -pthread")

# to figure out exceptions thrown
set(CMAKE_C_FLAGS_INIT   "${CMAKE_C_FLAGS_INIT}   -fwasm-exceptions")
set(CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT} -fwasm-exceptions")
