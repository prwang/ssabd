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
constexpr int maxn = 500362;
extern int timestamp;
using namespace std;



extern int n; /// \brief 节点个数。有多少个语句就有多少个节点，翻译时创建
extern struct binary_op *id2node[maxn]; /// \brief 提供节点号到实际node的转换
constexpr struct binary_op* func_null = &((struct binary_op* )nullptr)[-1];

#endif //PROJECT_COMMON_H
