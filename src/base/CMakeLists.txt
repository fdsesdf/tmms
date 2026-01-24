add_library(base
    Config.cpp
    FileLog.cpp
    FileMgr.cpp
    LogStream.cpp
    Logger.cpp
    StringUtils.cpp
    TTime.cpp
    Task.cpp
    TaskMgr.cpp
)

# 只在 base 里禁用 jsoncpp string_view（正确姿势）
target_compile_definitions(base PRIVATE JSONCPP_DISABLE_STRING_VIEW)

# jsoncpp 头（你现在用的是 third_party）
target_include_directories(base PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/jsoncpp/include
)

# 链接系统 jsoncpp
target_link_libraries(base PRIVATE jsoncpp)
