//
// Created by prwang on 7/2/2018.
//

#include "eval.h"

/*
 * future是什么东西？
 * 是未解析的条件跳转。
 * 你phi节点的规则， 没有齐备则
 *  1* 输出正负无穷。那你完了，就不可能用上输入的初值的信息
 *  2* 把已有的区间并起来，
 *  那么你后面要知道这个区间有可能太小了，所有用到这个区间的条件跳转应该都是无效的。
 *
 *
 * 第一步，含future的区间应该是大区间，
 * 所以应该有单独的标记来确定
 * 首先，判定强连通分量的边含不含虚线？含。
 * 那么区间交就是二元运算。intersect(obj, ref)
 *
 * 因为是按拓扑序做的，phi不齐备当且仅当位于同一个强连通分量中
 *
 * 一个变量是未定值的，要么就是一个不完整的phi，要么依赖一个未定值的变量。
 * 所有未定值变量的特点是，它们的实际范围可能更大，所以用它做条件变量的区间交结果会错
 * 那么就要临时disable它们，直到，widen和future resolution之后，这些变量的值才有意义，
 * 才能用它们做区间交。
 * 那么phi应该提供查询是否是未定值的方法、区间交应该可以有状态设为是否disabled
 *
 * 确定变量是未定值的，时机是什么？
 * 首先必须在do_scc里面，这样能准备好的都准备好了。
 * 必须分开做，因为目的是不同的，第0步把未定值进行传播，第1步把部分算出来的值进行传播。
 * 只有两种状态，可以从所有未定值的phi节点开始bfs，遇到
 *  运算节点和phi节点和区间交的obj，则运算结果是未定值的
 *  区间交的ref，则把这个区间交disable掉，并且对输出没有影响。
 *
 */


/// \brief 三步法处理强连通分量
static inline void do_scc(int scc_id)
{
  //FIXME 小心！spfa会爆队列！
  //你用标准的队列吧
  static int queue[maxn]; int qh(0), qt(0);
  bool in_queue[maxn];
  //TODO step 0: 哪些是future？以未定值的phi为开始，染色所有节点

  //TODO widening: spfa
  CLR(in_queue); qh = qt = 0;
  /*比如一个节点的强连通分量中，显然可以是任何节点
   *你怎么建立抽象的图和具体的
   */
  for (int u : scc_cont[scc_id]) {
    //FIXME ??? 要入队所有有值的变量！ if(...)
    if (false)
      in_queue[queue[qt++] = u] = true;
  }
  while (qh < qt) {
    //TODO case type of
    // operation: 所有参数齐备，调用operator()进行求值

    // PHI: evaluate_partial 先算出一个东西再说，
    // 特别地这里要和上一次的结果去比较，如果变大了要产生无穷

  }

  //note 这里不能走is_cut == true 的边

  //TODO step 2 future resolution
  //TODO step 3 narrowing
}

