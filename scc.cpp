//
// Created by prwang on 6/30/2018.
//

/// \brief Algorithms dealing with strong connected components
#include <memory>
#include <utility>
#include <cstring>
#include <vector>
#include <queue>

using namespace std;
#include "scc.h"
#include "SDT.h"
#include "interval.h"
#include "nodes.h"

struct edge {
  bool is_cut; int v;
  edge* next;
} *G[maxn], *G_topo[maxn],
    pool[maxn * 10], *pool_end = pool;
//变量：正向边在这里，反向边是input[]
//常量：只有input，没有边

int n_scc;
static int
    stack[maxn], sc, ts, //tarjan栈、时间戳
    dfn[maxn], low[maxn], //深度标记
    cor[maxn], in_deg[maxn]; //对应等价类，等价类个数、等价类入度
vector<int> scc_cont[maxn];
static bool in_stack[maxn];


inline void add_edge3(int u, int v, edge** G1)
{
  G1[u] = new(pool_end++) edge{false, v, G1[u]};
}
void clear()
{
  pool_end = pool; ts = sc = n_scc = n = 0;
  CLR(G); CLR(G_topo); CLR(in_deg);
  CLR(stack); CLR(dfn); CLR(low); CLR(cor); CLR(in_stack);
}
void add_edge(int u, int v) { add_edge3(u, v, G); }

/// \brief 求强连通分量
static void tarjan(int u, int fr)
{
  dfn[u] = low[u] = ++ts;
  in_stack[stack[sc++] = u] = true;
  for (edge* p = G[u]; p; p = p->next) {
    int v = p->v;
    if (dfn[v] == -1) {
      tarjan(v, u);
      low[u] = min(low[u], low[v]);
      if (dfn[u] < low[v]) {
        p->is_cut = true;
      }
      else if (in_stack[v]) {
        low[u] = min(low[u], dfn[v]);
      }
    }
  }

  if (low[u] == dfn[u]) {
    int w;
    vector<int>& cur = scc_cont[++n_scc];
    cur.clear();
    do {
      in_stack[w = stack[--sc]] = false;
      cor[w] = n_scc;
      cur.push_back(w);
    } while (u != w);
  }
}


/// \brief 强连通分量缩点并拓扑排序
void func::do_topo()
{
  //求强连通分量
  tarjan(first_node, 0);
  //缩点建新图
  last_node = n;
  for (int u = first_node; u <= last_node; ++u) {
    int u1 = cor[u];
    for (edge*p = G[u]; p; p = p->next) {

      if (p->is_cut) {
        int v1 = cor[p->v];
        add_edge3(cor[u], v1, G_topo);
        ++in_deg[v1];
      }
    }
  }
  last_scc = n_scc;
  //拓扑排序
  queue<int> Q;
  for (int i = first_scc; i <= last_scc; ++i) {
    if (in_deg[i] == 0) Q.push(i);
  }
  while (!Q.empty()) {
    int o = Q.front(); Q.pop();
    order.push_back(o);
    for (edge* p = G_topo[o]; p; p = p->next) {
      if (--in_deg[p->v] == 0) { Q.push(p->v); }
    }
  }
}


