//
// Created by prwang on 7/2/2018.
//

#include "SDT.h"
#include "nodes.h"
#include <queue>

/*
/*
 * 我需要根据函数名字，查到它的哪些变量是使用了参数，然后去patch这些地方。
 * 一个函数被调用多次，我还是得复制它，调用了多少遍，就需要复制多少遍
 * 显然，我们应该以函数为单位进行解析，那么scc就不能用全局变量了，不一定
 * scc是静态的信息，当然可以逐个函数都做完
 * 缩点拓扑排序都可以静态做，最后输出一个求值顺序
 * 什么是动态信息？那就是每个节点的output.
 * 每次eval一个节点的时候需要读这个output.
 * output可以放在节点里面啊。没有递归那么
 * --
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
 *
 * 那么就要临时disable它们，直到，widen和future resolution之后，这些变量的值才有意义，
 * 才能用它们做区间交。
 * 那么phi应该提供查询是否是未定值的方法、区间交应该可以有状态设为是否disabled
 *
 * 一个比较节点(obj, ref) 如果obj不存在，结果为空；如果ref不存在，在第一步
 * 结果是obj！
 *
 * 读论文，结果是，这两种边叫做data dep和ctrl dep，在图中都要连出来
 *
 * 确定变量是未定值的，时机是什么？
 * 首先必须在do_scc里面，这样能准备好的都准备好了。
 * 必须分开做，因为目的是不同的，第0步把未定值进行传播，第1步把部分算出来的值进行传播。
 * 只有两种状态，可以从所有未定值的phi节点开始bfs，遇到
 *  运算节点和phi节点和区间交的obj，则运算结果是未定值的
 *  区间交的ref，则把这个区间交disable掉，并且对输出没有影响。
 *
 *
 */


/// \brief 三步法处理强连通分量
static inline void do_scc(int scc_id, int ts)
{
/// \brief 懒惰清空原来的输入
#define get_output(id) (time[id] == ts ? id2node[id]->output \
: (id2node[id]->output = interval()))

  //TODO step 0: 哪些是future？以未定值的phi为开始，染色所有节点
// * 那么phi应该提供查询是否是未定值的方法、区间交应该可以有状态设为是否disabled
  //不是所有的东西都是未定值的。如果ref是外围的，obj是内的，那直接可以定出来


  //FIXME 小心！spfa会爆队列！
  //你用标准的队列吧
  //小心！这个函数会重入
  queue<int> Q;
  bool in_queue[maxn];//FIXME 只用开scc那么大？!

  //TODO widening: spfa
  CLR(in_queue);
  /*比如一个节点的强连通分量中，显然可以是任何节点
   *你怎么建立抽象的图和具体的
   */
  for (int u : scc_cont[scc_id]) {
    OP* node = id2node[u];
    assert(node); node->eval(); //根据求值出来是否为空，判断是否所有操作数准备好
    if (!node->output.is_empty()) {
      Q.push(u);
    }
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

interval func::eval(const vector<interval> &real_args)
{

  assert(real_args.size() == args.size());
  copy(real_args.begin(), real_args.end(), args.begin());
  ++timestamp;
  for (int i : order) { do_scc(i, timestamp); }
  return *ret;
}
