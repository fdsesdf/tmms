aux_source_directory(. DIR_LIB_NET)
aux_source_directory(net DIR_LIB_NET)
aux_source_directory(base DIR_LIB_NET)
add_library(network ${DIR_LIB_NET})
add_subdirectory(net/tests)
target_link_libraries(network base)



