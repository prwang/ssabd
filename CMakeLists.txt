project(ssabd)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
set(CMAKE_CXX_STANDARD 14)
flex_target(ssaScan ssa.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc)
bison_target(ssaParse ssa.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
        VERBOSE ${CMAKE_CURRENT_BINARY_DIR}/parser.log)

add_executable(main main.cpp scc.cpp
        common.h SDT.h SDT.cpp nodes.h interval.h scc.h main.h eval.cpp eval.h
        ${BISON_ssaParse_OUTPUTS}
        ${FLEX_ssaScan_OUTPUTS} )

target_include_directories(main PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})