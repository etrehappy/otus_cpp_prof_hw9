cmake_minimum_required(VERSION 3.12)

project(async_prj)

set(Parser    
    parser.h parser.cpp
)
set(CommandReader
    cmd_reader.h cmd_reader.cpp    
)

set(AsyncFiles
    async.h async.cpp    
    m_exceptions.h
    safe_queue.h safe_queue.cpp
    controller.h controller.cpp
)

if(WIN32)
    set (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()

add_library(async SHARED ${AsyncFiles} ${Parser} ${CommandReader})

if(MSVC)
        source_group("Async" FILES ${AsyncFiles})
        source_group("Parser" FILES ${Parser})
        source_group("CommandReader" FILES ${CommandReader})
endif ()

install(TARGETS async
    RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/async/bin           # .dll
    LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/async/lib           # .so Unix
    ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/async/lib           # .lib, .a 
)

install(FILES async.h parser.h cmd_reader.h m_exceptions.h
    DESTINATION ${CMAKE_BINARY_DIR}/async/include
)

target_compile_options(async PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX >
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
        )

set_target_properties(async PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
)

