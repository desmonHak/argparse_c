cmake_minimum_required(VERSION 3.31)
project(argparse_c C)

set(CMAKE_C_STANDARD 11)

include_directories(include)
include_directories(lib/lexpar_c/include)
include_directories(lib/lexpar_c/lib/structs_data_C)
include_directories(lib/lexpar_c/lib/structs_data_C/CIterators/include)
include_directories(lib/lexpar_c/lib/structs_data_C/DebugLibC)
include_directories(lib/lexpar_c/lib/structs_data_C/DebugLibC/colors-C-C-plus-plus)
include_directories(lib/lexpar_c/lib/structs_data_C/DebugLibC/colors-C-C-plus-plus/old-code)

add_executable(argparse_c
        examples/code.c
        include/argparse_c.h
        lib/lexpar_c/include/ast_c.h
        lib/lexpar_c/include/global.h
        lib/lexpar_c/include/lexer.h
        lib/lexpar_c/include/token.h
        lib/lexpar_c/lib/structs_data_C/CIterators/include/CIterators.h
        lib/lexpar_c/lib/structs_data_C/CIterators/include/CSortting.h
        lib/lexpar_c/lib/structs_data_C/CIterators/src/CIterators.c
        lib/lexpar_c/lib/structs_data_C/CIterators/src/CSortting.c
        lib/lexpar_c/lib/structs_data_C/DebugLibC/colors-C-C-plus-plus/src/colors.c
        lib/lexpar_c/lib/structs_data_C/DebugLibC/colors-C-C-plus-plus/colors.h
        lib/lexpar_c/lib/structs_data_C/DebugLibC/src/debug_c.c
        lib/lexpar_c/lib/structs_data_C/DebugLibC/src/error_c.c
        lib/lexpar_c/lib/structs_data_C/DebugLibC/src/time_code.c
        lib/lexpar_c/lib/structs_data_C/DebugLibC/debug_c.h
        lib/lexpar_c/lib/structs_data_C/DebugLibC/error_c.h
        lib/lexpar_c/lib/structs_data_C/DebugLibC/time_code.h
        lib/lexpar_c/lib/structs_data_C/src/array-list.c
        lib/lexpar_c/lib/structs_data_C/src/ast.c
        lib/lexpar_c/lib/structs_data_C/src/hash-table.c
        lib/lexpar_c/lib/structs_data_C/src/matriz-list.c
        lib/lexpar_c/lib/structs_data_C/src/vector-list.c
        lib/lexpar_c/lib/structs_data_C/array-list.h
        lib/lexpar_c/lib/structs_data_C/ast.h
        lib/lexpar_c/lib/structs_data_C/hash-table.h
        lib/lexpar_c/lib/structs_data_C/matriz-list.h
        lib/lexpar_c/lib/structs_data_C/vector-list.h
        lib/lexpar_c/src/ast_c.c
        lib/lexpar_c/src/lexer.c
        lib/lexpar_c/src/token.c
        src/argparse_c.c)
