//
// Created by prwang on 7/1/2018.
//

#ifndef SSABD_SCC_H
#define SSABD_SCC_H
#include "common.h"
void add_edge(int u, int v);
extern struct edge {
  bool is_cut; int v;
  edge* next;
} *G[maxn], *G_topo[maxn],
    pool[maxn * 10], *pool_end;
#endif //SSABD_SCC_H
