project(tmms LANGUAGES C CXX)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(base)
add_subdirectory(main)
add_subdirectory(network)
