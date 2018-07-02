project(ssabd)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
set(CMAKE_CXX_STANDARD 14)
flex_target(ssaScan ssa.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc)
bison_target(ssaParse ssa.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cc)
add_executable(main main.cpp scc.cpp
        ${FLEX_ssaScan_OUTPUTS}
        ${BISON_ssaParse_OUTPUTS}
        common.h SDT.h SDT.cpp nodes.h interval.h scc.h main.h eval.cpp eval.h)