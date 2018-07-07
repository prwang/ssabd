//
// Created by prwang on 6/30/2018.
#include <unordered_map>
#include "SDT.h"
#include "scc.h"

func *current_func;

struct OP *id2node[maxn];
unordered_map<string, struct func *> name2func;

interval EMPTY[maxn],
    *const_pool = EMPTY + 1; //顾名思义，数组首地址为空区间指针


void func::postfix()
{
  if (this->bad) {
    return delete this;
  }
  assert(current_bb == DEFAULT_RET_BB_NAME);
  for (auto x : redirection0) x();

  for (const IBR &i : interblock_refs) {
    auto t = redirection.find(make_pair(i.bb, i.var));
    const string &key = (t == redirection.end() ? i.var : t->second);
    auto N = nodes.find(key);
    assert(N != nodes.end());
    OP *node = N->second;
    i.node->set_input(&node->output, i.which);
    add_edge(node->id, i.node->id);
  }
  last_node = n_op;
  for (int i = first_node + 1; i <= last_node; ++i) {
    OP* nd = id2node[i];
    if (nd->is_orphan) {
      add_edge(first_node, nd->id);
      nd->is_orphan = false;
    }
  }
  //缩点拓扑排序，并设置比较的from_same_scc
  do_topo();

  if (graph_file) {
    dump_graph();
  }
}

void func::do_rval(const RVAL &rv, OP *op, int which)
{
  if (rv.isvar) {
    op->is_orphan = false;
    switch (rv.r.type) {
    case NORM: case EXT: {
      if (current_bb_defs.count(rv.r.var)) {
        auto it = nodes.find(rv.r.var);
        assert(it != nodes.end());
        op->set_input(&it->second->output, which);
        add_edge(it->second->id, op->id);
      } else {
      //case EXT:
        op->set_input(nullptr, which);
        LOGM("creating IBR: %s %s\n", rv.r.var.c_str(), current_bb.c_str());
        interblock_refs.push_back(IBR{rv.r.var, current_bb, op, which});
      }
      break;
    }
    case ARG: {
      auto it = name2arg.find(del_version(rv.r.var));
      assert(it != name2arg.end());
      op->set_input(it->second, which);
      add_edge(first_node, op->id);
      break;
    }
    default:
      assert(false);
    }
  } else {
    op->set_input(new(const_pool++) interval(rv.C, rv.C), which);
  }
}

bool func_call::same_after_eval()
{
  old_output = output;
  func *f = name2func[func_name];
  vector<interval> value;
  for (auto i : input) { value.push_back(*i); }
  output = f->eval(value);
  return old_output == output;
}


//

