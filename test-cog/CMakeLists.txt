cmake_minimum_required(VERSION 3.5)

#project(test_cog_read LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(test_cog_read main.cpp)

include(GdalStandardIncludes)

IF (EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")

  #set(EMS "SHELL:-sFETCH --proxy-to-worker")
  #set(EMS "SHELL:-sFETCH -s PROXY_TO_PTHREAD=1 -pthread")
  #set(EMS "SHELL:-sFETCH -pthread -s PTHREAD_POOL_SIZE=2 -s PROXY_TO_PTHREAD")

  # PROXY_TO_PTHREAD = main runs in a worker thread (so we can block without a problem)
  set(EMS "SHELL:-sFETCH -pthread -sPROXY_TO_PTHREAD -s ALLOW_MEMORY_GROWTH=1")
  target_link_options(test_cog_read PRIVATE ${EMS})

  gdal_standard_includes(test_cog_read)
  gdal_target_link_libraries(test_cog_read PRIVATE PROJ::proj)
  target_link_libraries(test_cog_read PRIVATE $<TARGET_NAME:${GDAL_LIB_TARGET_NAME}>)

ELSE()
  target_include_directories(test_cog_read PRIVATE /usr/include/gdal)
  #target_link_libraries(test_cog_read gdal)
  target_link_libraries(test_cog_read /home/martin/tmp/gdal-wasm/build-gdal-Desktop-Debug/libgdal.so)
ENDIF()


install(TARGETS test_cog_read
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
