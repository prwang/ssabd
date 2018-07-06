//
// Created by prwang on 6/30/2018.
#include <unordered_map>
#include "SDT.h"
#include "scc.h"

func *current_func;
unordered_map<string, func *> func_table;
string current_bb;
struct OP *id2node[maxn];
unordered_map<string, struct func *> map_func_name;

interval EMPTY[maxn],
    *const_pool = EMPTY + 1; //顾名思义，数组首地址为空区间指针


void func::postfix()
{
  if (this->bad) { return delete this; }

  //连边
  for (const IBR &i : interblock_refs) {
    auto t = redirection.find(make_pair(i.bb, i.var));
    const string &key = (t == redirection.end() ? i.var : t->second);
    auto N = nodes.find(key);
    assert(N != nodes.end());
    OP *node = N->second;
    i.node->set_input(&node->output, i.which);
    add_edge(node->id, i.node->id);
  }
  //缩点拓扑排序
  do_topo();
}

void func::do_rval(const RVAL &rv, OP *op, int which)
{
  if (rv.isvar) {
    switch (rv.r.type) {
    case NORM: {
      auto it = nodes.find(rv.r.var);
      assert(it != nodes.end());
      op->set_input(&it->second->output, which);
      add_edge(it->second->id, op->id);
      break;
    }
    case EXT:
      op->set_input(nullptr, which);
      interblock_refs.push_back(IBR{rv.r.var, current_bb, op, which});
      break;
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

void func_call::eval()
{
  func *f = map_func_name[func_name];
  vector<interval> value;
  for (auto i : input) { value.push_back(*i); }
  f->eval(value);
  output = *f->ret;
}


void parse_postfix()
{

}

//

