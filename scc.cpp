//
// Created by prwang on 6/30/2018.
//

/// \brief Algorithms dealing with strong connected components
#include <memory>
#include <utility>
#include <cstring>
#include <vector>

using namespace std;
#include "scc.h"
#include "interval.h"
#include "nodes.h"

struct edge {
  bool is_cut; int v;
  edge* next;
} *G[maxn], *G_topo[maxn],
    pool[maxn * 10], *pool_end = pool;


static int
    stack[maxn], sc, ts, //tarjan栈、时间戳
    dfn[maxn], low[maxn], //深度标记
    cor[maxn], n1, in_deg[maxn]; //对应等价类，等价类个数、等价类入度
vector<int> scc_cont[maxn];
static bool in_stack[maxn];


inline void add_edge3(int u, int v, edge** G1)
{
  G1[u] = new(pool_end++) edge{false, v, G1[u]};
}
void clear()
{
  pool_end = pool; ts = sc = n1 = n = 0;
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
    vector<int>& cur = scc_cont[++n1];
    cur.clear();
    do {
      in_stack[w = stack[--sc]] = false;
      cor[w] = n1;
      cur.push_back(w);
    } while (u != w);
  }
}

interval result[maxn];
// 全部公用一张图是没问题的，每一个函数的所有参数连的一个超级节点上。
// 在逻辑的图的表示上

/// \brief 求强连通分量并缩点建新图
static inline void do_tarjan(int start)
{

  tarjan(start, 0);

  for (int u = 1; u <= n; ++u) {
    //FIXME 1...n??? 应该只跟当前这个函数有关系。
    int u1 = cor[u];
    for (edge*p = G[u]; p; p = p->next) {

      if (p->is_cut) {
        int v1 = cor[p->v];
        add_edge3(cor[u], v1, G_topo);
        ++in_deg[v1];
      }
    }
  }

}

/// \brief 拓扑排序，输出到data中。
static inline void topo_sort(int* data)
{
  static int queue[maxn]; int qh(0), qt(0);
  for (int i = 1; i <= n1; ++i) {
    //FIXME 这个初始化不行！我每个函数要调用一次的
    if (in_deg[i] == 0) queue[qt++] = i;
  }
  while (qh < qt) {
    int o = queue[qh++];
    *data++ = o;
    for (edge* p = G_topo[o]; p; p = p->next) {
      if (--in_deg[p->v] == 0)
        queue[qt++] = p->v;
    }
  }
}


