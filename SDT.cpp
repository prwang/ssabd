//
// Created by prwang on 6/30/2018.
#include <unordered_map>
#include "SDT.h"
#include "scc.h"

func *current_func;
unordered_map<string, func *> func_table;
string current_bb;

//(bb1, var) -> var_t  (bb2, var) -> var_f
/*
 读入控制流图，最终play with数据流图*
 哪些节点是翻译生成的，哪些节点是postfix生成的？
 最开始预想的不变式是所有的节点都翻译生成。
 第一个例外是块外引用条件的变量，要加上区间的限制
 第二个例外是整数类型的小于语义，实际上是a <= b - 1


 */
void func::postfix()
{
  /* 每解析一个函数，就调用一次。那么就不需要搞很多env了
   * 并且这里给parse_postfix 留出余地。
   */
  //连边
  for (const IBR &i : interblock_refs) {
    auto t = redirection.find(make_pair(i.bb, i.var));
    auto N = nodes.find(t == redirection.end() ? i.var : t->second);
    assert(N != nodes.end());
    i.node->input[i.which] = &N->second->output;
    add_edge(N->second->id, i.node->id);
  }

  //TODO 想出怎么对着一个函数求强连通分量和拓扑排序
  //你要从supernode这里开始拓扑排序，则断言，只有这个点的起始入度是0
  //没有问题
}

/*
 * 我需要根据函数名字，查到它的哪些变量是使用了参数，然后去patch这些地方。
 * 一个函数被调用多次，我还是得复制它，调用了多少遍，就需要复制多少遍
 * 显然，我们应该以函数为单位进行解析，那么scc就不能用全局变量了，不一定
 * scc是静态的信息，当然可以逐个函数都做完
 * 缩点拓扑排序都可以静态做，最后输出一个求值顺序
 * 什么是动态信息？那就是每个节点的output.
 * 每次eval一个节点的时候需要读这个output.
 * output可以放在节点里面啊。没有递归那么
 *
 */
void func::do_rval(RVAL rv, binary_op *op, bool which)
{
  if (rv.isvar) {
    switch (rv.r.type) {
    case NORM: {
      auto it = nodes.find(rv.r.var);
      assert(it != nodes.end());
      op->input[which] = &it->second->output;
      add_edge(it->second->id, op->id);
      break;
    }
    case EXT:
      op->input[which] = nullptr;
      interblock_refs.push_back(IBR{rv.r.var, current_bb, op, which});
      break;
    case ARG: {
      auto it = name2arg.find(del_ver(rv.r.var));
      assert(it != name2arg.end());
      op->input[which] = it->second;
      add_edge(supernode_id, op->id);
      break;
    }
    default:
      assert(false);
    }
  } else {
    op->input[which] = new interval(rv.C, rv.C);
  }
}

void parse_postfix()
{

}

//

