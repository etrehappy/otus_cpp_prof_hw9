cmake_minimum_required(VERSION 3.12)

project(async_cli_prj)

set(main_files 
    main.cpp    
)

add_subdirectory(async)

add_executable(async_cli ${main_files})


target_include_directories(async_cli PRIVATE ${CMAKE_BINARY_DIR}/async/include)
target_link_directories(async_cli PRIVATE ${CMAKE_BINARY_DIR}/async/lib) 
target_link_libraries(async_cli async)

add_custom_command(TARGET async_cli POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:async>
        $<TARGET_FILE_DIR:async_cli>
)

if(MSVC)
	    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT async_cli)
        source_group("Main" FILES ${main_files})
endif ()

target_compile_options(async_cli PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX >
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
        )

set_target_properties(async_cli PROPERTIES
	CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
)

