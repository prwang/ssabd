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


#define LOGM(...) fprintf(stderr, __VA_ARGS__)
#define CLR(x) memset(x, 0, sizeof(x));
constexpr int maxn = 500362; // TODO 全部改对之后改成动态分配的
using namespace std;

extern int timestamp,  /// \brief 求值时间戳（用于重置)
    n, n_scc;  /// \brief 最后一个节点、强连通分量编号

extern unordered_map<string, struct func*> map_func_name;
extern struct OP *id2node[maxn];
extern vector<int> scc_cont[maxn]; /// \brief 每个强连通分量里面包含哪些点

#endif //PROJECT_COMMON_H
