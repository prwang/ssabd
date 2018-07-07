/// \brief 求强连通分量并缩点
#include <memory>
#include <vector>
#include <queue>
#include <stack>
#include "scc.h"
#include "SDT.h"

using namespace std;

edge *G[maxn], *G_topo[maxn],
    pool[maxn * 10], *pool_end = pool;

int n_op, n_scc; //节点、scc个数
int id2scc[maxn], in_deg[maxn]; //对应等价类，scc入度
vector<int> scc_cont[maxn];

inline void add_edge3(int u, int v, edge **G1)
{
  G1[u] = new(pool_end++) edge{v, G1[u]};
}

void add_edge(int u, int v) { add_edge3(u, v, G); }

/// \brief 求强连通分量
static void tarjan(int u)
{
  static int ts, //dfn编号
      dfn[maxn], low[maxn]; //深度标记
  static stack<int> S; //tarjan栈
  dfn[u] = low[u] = ++ts;
  S.push(u);
  for (edge *p = G[u]; p; p = p->next) {
    int v = p->v;
    if (!dfn[v]) {
      tarjan(v);
      low[u] = min(low[u], low[v]);
    } else if (!id2scc[v]) {
      low[u] = min(low[u], dfn[v]);
    }
  }
  if (low[u] == dfn[u]) {
    int w;
    vector<int> &cur = scc_cont[++n_scc];
    cur.clear();
    do {
      w = S.top();
      S.pop();
      id2scc[w] = n_scc;
      cur.push_back(w);
    } while (u != w);
  }
}

void func::dump_graph()
{
  for (int u = first_node; u <= last_node; ++u) {
    for (edge *p = G[u]; p; p = p->next) {
      fprintf(graph_file, "\"%s:%d(%d)\"->\"%s:%d(%d)\";\n",
              u == first_node ? "first_node" : typeid(*id2node[u]).name(), u,
              id2scc[u],
              typeid(*id2node[p->v]).name(), p->v, id2scc[p->v] );
    }
  }
  fprintf(graph_file,
          "\"%s:%d(%d)\"->\"RETURN!\";\n",
          typeid(*id2node[ret_id]).name(), ret_id, id2scc[ret_id]);
  fflush(graph_file);
}
/// \brief 强连通分量缩点并拓扑排序
void func::do_topo()
{
  //求强连通分量
  tarjan(first_node);
  //缩点建新图
  for (int u = first_node; u <= last_node; ++u) {
    int u1 = id2scc[u];
    for (edge *p = G[u]; p; p = p->next) {
      int v1 = id2scc[p->v];
      if (v1 != u1) {
        add_edge3(u1, v1, G_topo);
        ++in_deg[v1];
      } else {
        //位于同一强连通分量的边，如果是连到了一个比较算符的ref，
        // 则在widen阶段不能进行比较操作（认为利用了future）
        compare *n2 = dynamic_cast<compare *>(id2node[p->v]);
        if (n2 && n2->input[1] == &id2node[u]->output) {
          n2->ref_from_same_scc = true;
        }
      }
    }
  }
  last_scc = n_scc;
  //拓扑排序
  queue<int> Q;
  int scc0 = id2scc[first_node];
  assert(scc_cont[scc0].size() == 1);
  assert(in_deg[scc0] == 0);
  for (int i = first_scc; i <= last_scc; ++i) {
    assert(i == scc0 || in_deg[i] != 0);
  }
  Q.push(scc0);
  LOGM("order is --");
  while (!Q.empty()) {
    int o = Q.front();
    Q.pop();
    LOGM("%d ", o);
    if (o != scc0) { order.push_back(o); }
    for (edge *p = G_topo[o]; p; p = p->next) {
      if (--in_deg[p->v] == 0) { Q.push(p->v); }
    }
  }
  LOGM("\n");
}


