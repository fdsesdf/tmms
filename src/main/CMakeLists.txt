add_executable(tmms main.cpp)

target_link_libraries(tmms
    base
    jsoncpp
)

# 同样禁用 string_view
target_compile_definitions(tmms PRIVATE JSONCPP_DISABLE_STRING_VIEW)
