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

constexpr int maxn = 500362;
struct edge {
  bool is_cut; int v;
  edge* next;
} *G[maxn], *G_topo[maxn],
    pool[maxn * 10], *pool_end = pool;


static int  n, //点数
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
#define CLR(x) memset(x, 0, sizeof(x));
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

/// \brief 求强连通分量并缩点建新图
static inline void do_tarjan(int _n)
{
  n = _n;
  n1 = 0; pool_end = pool;
  tarjan(1, 0);
  for (int u = 1; u <= n; ++u) {
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

/// \brief 三步法处理强连通分量
static inline void do_scc(int scc_id)
{
  //widening: spfa
  static int queue[maxn]; int qh(0), qt(0);
  for (int u : scc_cont[scc_id]) {
    queue[qt++] = u;
  }
  while (qh < qt) {
    //TODO case type of
    // operation: 所有参数齐备，调用operator()进行求值

    // PHI: evaluate_partial 先算出一个东西再说
    // 特别地这里要和上一次的结果去比较，如果变大了要产生无穷

  }
//TODO
  //note 这里不能走is_cut == true 的边

  //future resolution
  //narrowing
}

/// \brief 拓扑排序并调用强连通分量的处理函数
static inline void topo_sort()
{
  static int queue[maxn]; int qh(0), qt(0);
  for (int i = 1; i <= n1; ++i) {
    if (in_deg[i] == 0) queue[qt++] = i;
  }
  while (qh < qt) {
    int o = queue[qh++];
    do_scc(o); //对变量的求值都放在do_scc里面了，不用再求值了
    for (edge* p = G_topo[o]; p; p = p->next) {
      if (--in_deg[p->v] == 0)
        queue[qt++] = p->v;
    }
  }
}


