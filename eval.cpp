//
// Created by prwang on 7/2/2018.
//

#include "SDT.h"
#include "scc.h"
#include <functional>

#include <queue>


bool allow_same_scc;

/// \brief 强连通分量内部迭代求值
template<bool clear/** 是否清空*/>
static inline void propagate(int scc_id, function<void(OP *)> cb)
{
  static bool in_queue[maxn]; //没有递归，不会发生冲突
  queue<int> Q;
  for (int u : scc_cont[scc_id]) {
    assert(!in_queue[u]);
    OP *nd = id2node[u];
    assert(nd);
    if (clear) { nd->output = interval(); }
    if (!nd->same_after_eval()) {
      in_queue[u] = true;
      Q.push(u);
    }
  }
  while (Q.size()) {
    int u = Q.front();
    Q.pop();
    in_queue[u] = false;
    for (edge *p = G[u]; p; p = p->next) {
      if (id2scc[p->v] == id2scc[u]) {
        OP *nd = id2node[p->v];
        assert(nd);
        if (!nd->same_after_eval()) {
          in_queue[p->v] = true;
          Q.push(p->v);
        }
        cb(nd);
      }
    }
  }
}

static inline void do_scc(int scc_id)
{
  //step 1: widening
  allow_same_scc = false;
  propagate<true>(scc_id, [](OP *nd) -> void {
    phi *other_phi = dynamic_cast<phi *>(nd);
    if (other_phi) {
      other_phi->grow();
    }
  });
  //step 2 future resolution
  allow_same_scc = true;
  //step 3 narrowing
  propagate<false>(scc_id, [](OP *) -> void {});
}

interval func::eval(const vector<interval> &real_args)
{
  assert(real_args.size() == args.size());
  copy(real_args.begin(), real_args.end(), args.begin());
  for (int i : order) { do_scc(i); }
  return *ret;
}
