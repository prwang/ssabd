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
struct interval {
  double L, R;
  interval(double _L, double _R) : L(_L), R(_R) {
    if (L > R) {
      LOGM("IMPLICIT EMPTY INTERVAL");
      *this = interval();
    }
  };
  //空区间
  constexpr interval() : L(INFINITY), R(-INFINITY) { }
  bool is_empty() const { return L > R; }
private:
};
extern interval EMPTY[maxn], *const_pool; //所有常数放一起方便调试

/* 条件跳转的任务：
 * 1. 翻译阶段
 *   拦截原来的块间边，B1 ---var1--->B2或B3
 *   变成，B1 --var1-->区间交---var1t--->B2
 *   B1 --->var1--->区间交---var1f--->B3
 *   问题：ssa中已经给出的变量的块位置要不要解析？ 要的。
 *   对于所有来自外块的变量，都有可能被拦截，应该翻译为一个list，到时候统一patch
 *   常数的处理：只生成区间，不连边
 * 2. 如果包含future, 则在未解析的时候变成全通网络，future解析以后变成区间交，这需要特殊处理
 * */
//空区间会出现，强连通的时候一定有未初始化的变量，它在传播一次之后就会有值

inline interval Uni(interval u1, interval u2)
{
  return interval{min(u1.L, u2.L), max(u1.R, u2.R)};
}
#endif //SSABD_INTERVAL_H
