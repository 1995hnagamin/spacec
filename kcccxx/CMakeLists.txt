cmake_minimum_required(VERSION 3.1)
project(kccc++ CXX)

find_package(Clang REQUIRED)

set(
    CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${LLVM_CMAKE_DIR}"
)

include(AddLLVM)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

include_directories(
    ${INC_DIR}
    ${LLVM_INCLUDE_DIRS}
)

link_directories(
    ${LLVM_LIBRARY_DIRS}
)

set(LLVM_LINK_COMPONENTS
    Core
    MC
    X86AsmParser
    X86CodeGen
    X86Desc
    X86Info
)

add_definitions(${LLVM_DEFINITIONS})

add_llvm_executable(kccc++
    ${SRC_DIR}/kccc++.cpp
    ${SRC_DIR}/ast.cpp
    ${SRC_DIR}/binop.cpp
    ${SRC_DIR}/codegen.cpp
    ${SRC_DIR}/lexer.cpp
    ${SRC_DIR}/parser.cpp
    ${SRC_DIR}/type.cpp
    ${SRC_DIR}/typechecker.cpp
)

set_property(TARGET kccc++ PROPERTY CXX_STANDARD 17)
