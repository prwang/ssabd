//
// Created by prwang on 6/30/2018.
//

#ifndef PROJECT_COMMON_H
#define PROJECT_COMMON_H
#include <map>
#include <set>
#include <cassert>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <cstring>


extern bool verbose;
#define LOGM(...) if(verbose)fprintf(stderr, __VA_ARGS__)
#define CLR(x) memset(x, 0, sizeof(x));
constexpr int maxn = 500362;
using namespace std;

extern int n_op, n_scc;  /// \brief 最后一个节点、强连通分量编号
extern FILE* graph_file;

extern bool allow_same_scc;
extern unordered_map<string, struct func*> name2func;
extern struct OP *id2node[maxn];
extern vector<int> scc_cont[maxn]; /// \brief 每个强连通分量里面包含哪些点

#endif //PROJECT_COMMON_H
