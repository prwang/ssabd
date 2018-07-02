//
// Created by prwang on 7/1/2018.
//

#ifndef SSABD_INTERVAL_H
#define SSABD_INTERVAL_H
#include <cassert>
#include <algorithm>
#include <cmath>
#include "common.h"
using namespace std;
//就用double！
// int型的< >的问题在翻译的时候就地解决！（生成一个减一操作）
struct interval {
  double L, R;
  interval(double _L, double _R) : L(_L), R(_R) {
    if (L > R) {
      LOGM("IMPLICIT EMPTY INTERVAL");
      *this = interval();
    }
  };
  //空区间
  interval() { L = INFINITY; R = -INFINITY; }
  bool is_empty() const { return L > R; }
private:
};
/* 条件跳转的任务：
 * 1. 翻译阶段
 *   拦截原来的块间边，B1 ---var1--->B2或B3
 *   变成，B1 --var1-->区间交---var1t--->B2
 *   B1 --->var1--->区间交---var1f--->B3
 *
 *   问题：ssa中已经给出的变量的块位置要不要解析？ 要的。
 *   对于所有来自外块的变量，都有可能被拦截，应该翻译为一个list，到时候统一patch
 * 2. 执行阶段： 变成区间交操作节点，有两种，跟常数比、和变量比。
 *    现在的设计，是二元运算已经没有常数了，所有的常数都翻译成0元运算。
 *    条件跳转能否纳入这个框架呢？
 *    简单回答：可以
 *    问题实际上是如果条件比较有一个变量是已知的，那么在widen阶段会不会被忽略。
 *    现在widen是一个spfa，他会调用每个节点的functor
 *    那么区间交的functor是怎么写的呢？
 *    例如 if(a<b)的true分支
 *    得出a < sup b，  b > inf a
 *    如果a已经解析，也就是不在这个强连通分量里面被更新，那么得出的b的范围就是精确的。
 *    在第三步会不会反过来影响a的范围呢，会的，这是标准操作，影响本分支内已知变量的范围。
 *
 *    如果包含future, 则在未解析的时候变成全通网络，future解析以后变成区间交，这需要特殊处理
 * */
//空区间会出现，强连通的时候可能有未初始化的变量，它在传播一次之后就会有值

//区间交会产生空区间，算术运算应该对所有空区间返回空区间，以便不可能分支的东西不会干扰到可能分支的东西
interval Ins(interval u1, interval u2)
{
  if (u1.L > u2.L) swap(u1, u2);
  if (u1.R <= u2.L) return interval();
  else return interval(u2.L, min(u1.R, u2.R));
}
#endif //SSABD_INTERVAL_H
