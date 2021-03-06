#///////////////////////////////////////////////////////////////////////////////
#// chip8-emu
#//
#// Copyright (c) 2018 Thibault Schueller
#// This file is distributed under the MIT License
#///////////////////////////////////////////////////////////////////////////////

include(${CMAKE_SOURCE_DIR}/cmake/external.cmake)

set(CHIP8EMU_GCC_DEBUG_FLAGS
    "-Wall" "-Wextra" "-Wundef" "-Wshadow" "-funsigned-char" "-Wconversion"
    "-Wchar-subscripts" "-Wcast-align" "-Wwrite-strings" "-Wunused" "-Wuninitialized"
    "-Wpointer-arith" "-Wredundant-decls" "-Winline" "-Wformat"
    "-Wformat-security" "-Winit-self" "-Wdeprecated-declarations"
    "-Wmissing-include-dirs" "-Wmissing-declarations")
set(CHIP8EMU_GCC_RELEASE_FLAGS "")

# Use the same flags as GCC
set(CHIP8EMU_CLANG_DEBUG_FLAGS ${CHIP8EMU_GCC_DEBUG_FLAGS})
set(CHIP8EMU_CLANG_RELEASE_FLAGS ${CHIP8EMU_GCC_RELEASE_FLAGS})

# Useful for CMake-generated files
set(CHIP8EMU_GENERATED_BY_CMAKE_MSG "Generated by CMake")

# Automatically generate the header used for exporting/importing symbols
function(reaper_generate_export_header target project_label)
    # Construct export macro name
    string(TOUPPER ${target} TARGET_UPPERCASE)
    set(LIBRARY_API_MACRO ${TARGET_UPPERCASE}_API)

    # Construct the macro that is used in to tell if the library is being built
    # or just imported. The naming convention is CMake specific.
    set(LIBRARY_BUILDING_MACRO ${target}_EXPORTS)

    # Generate the file from the template.
    set(CHIP8EMU_EXPORT_TEMPLATE_PATH ${CMAKE_SOURCE_DIR}/src/LibraryExport.h.in)
    set(LIBRARY_GENERATED_EXPORT_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${project_label}Export.h)
    configure_file(${CHIP8EMU_EXPORT_TEMPLATE_PATH} ${LIBRARY_GENERATED_EXPORT_HEADER_PATH} NEWLINE_STYLE UNIX @ONLY)
endfunction()

# Helper function that add default compilation flags for reaper targets
function(reaper_configure_warnings target enabled)
    if(${enabled})
        if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE "$<$<CONFIG:DEBUG>:${CHIP8EMU_GCC_DEBUG_FLAGS}>")
            target_compile_options(${target} PRIVATE "$<$<CONFIG:RELEASE>:${CHIP8EMU_GCC_RELEASE_FLAGS}>")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${target} PRIVATE "$<$<CONFIG:DEBUG>:${CHIP8EMU_CLANG_DEBUG_FLAGS}>")
            target_compile_options(${target} PRIVATE "$<$<CONFIG:RELEASE>:${CHIP8EMU_CLANG_RELEASE_FLAGS}>")
        else()
            message(FATAL_ERROR "Could not detect compiler")
        endif()
    endif()
endfunction()

# Common helper that sets relevant C++ warnings and compilation flags
# see below for specific versions of the function.
# NOTE: call this function AFTER target_sources() calls
function(reaper_configure_target_common target project_label)
    target_include_directories(${target} PUBLIC ${CMAKE_SOURCE_DIR}/src)
    target_compile_definitions(${target} PRIVATE CHIP8EMU_BUILD_${CHIP8EMU_BUILD_TYPE})
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set_target_properties(${target} PROPERTIES CXX_STANDARD 14)
        target_compile_options(${target} PRIVATE "-fvisibility=hidden")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set_target_properties(${target} PROPERTIES CXX_STANDARD 14)
        target_compile_options(${target} PRIVATE "-fvisibility=hidden")
        add_clang_tidy_flags(${target})
    else()
        message(FATAL_ERROR "Could not detect compiler")
    endif()
endfunction()

# Use this function for engine libraries
function(reaper_configure_library target project_label)
    reaper_configure_target_common(${target} ${project_label})
    reaper_configure_warnings(${target} ON)
    reaper_generate_export_header(${target} ${project_label})
endfunction()

# Use this function for executables
function(reaper_configure_executable target project_label)
    reaper_configure_target_common(${target} ${project_label})
    reaper_configure_warnings(${target} ON)
endfunction()

# Use this function for external dependencies
function(reaper_configure_external_target target project_label)
    reaper_configure_target_common(${target} ${project_label})
    reaper_configure_warnings(${target} OFF)
endfunction()

# Standard test helper
function(reaper_add_tests library testfiles)
    if(CHIP8EMU_BUILD_TESTS)
        set(CHIP8EMU_TEST_FILES ${testfiles} ${ARGN}) # Expecting a list here
        set(CHIP8EMU_TEST_BIN ${library}_tests)
        set(CHIP8EMU_TEST_SRCS ${CHIP8EMU_TEST_FILES} ${CMAKE_SOURCE_DIR}/src/test/test_main.cpp)

        add_executable(${CHIP8EMU_TEST_BIN} ${CHIP8EMU_TEST_SRCS})

        reaper_configure_target_common(${CHIP8EMU_TEST_BIN} "${CHIP8EMU_TEST_BIN}")
        reaper_configure_warnings(${CHIP8EMU_TEST_BIN} ON)

        target_link_libraries(${CHIP8EMU_TEST_BIN}
            PUBLIC ${library}
            PRIVATE doctest
        )

        # Register wih ctest
        add_test(NAME ${CHIP8EMU_TEST_BIN} COMMAND $<TARGET_FILE:${CHIP8EMU_TEST_BIN}> WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

        set_target_properties(${CHIP8EMU_TEST_BIN} PROPERTIES FOLDER Test)
    endif()
endfunction()
