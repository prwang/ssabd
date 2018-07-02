//
// Created by prwang on 6/30/2018.
//

#ifndef SSABD_NODES_H
#define SSABD_NODES_H

#include "interval.h"
#include <utility>
#include <cfloat>
#include <algorithm>
#include <cmath>

using namespace std;

//所有的运算都是二元运算！
//op节点内部是不是有状态的？
//你怎么设计？是运算和状态分开？还是合到一起？
//现在弄一大堆functor，那么就是要分开了，实际上可以合到一起
//所谓状态，就是有一个输出的interval，并且有两个输入的指针，代表我从哪里取结果。
// 所有的输出值被初始化为空？必须有输出值，作为记忆化，否则你想递归？
// dummy用NULL行不行 是可以的。
struct binary_op {
  int id; //在图中有一个节点号
  interval output;
  const interval *input[2];

  binary_op() : id(++n), output(), input{nullptr, nullptr} {}

  binary_op(const binary_op &) = delete;

  binary_op &operator=(const binary_op &) = delete;

  void eval()
  {
    output = (*this)(*input[0], *input[1]);
  }

protected:
  virtual interval operator()(const interval &a, const interval &b) = 0;
};



struct add : binary_op {
  add() : binary_op() {}

  interval operator()(const interval &a, const interval &b) override
  {
    if (a.is_empty() || b.is_empty()) { return interval(); }
    return interval(a.L + b.L, a.R + b.R);
  }
};

struct sub : binary_op {
  sub() : binary_op() {}

  interval operator()(const interval &a, const interval &b) override
  {
    if (a.is_empty() || b.is_empty()) { return interval(); }
    return interval(a.L - b.L, a.R - b.R);
  }
};

struct mul : binary_op {
  mul() : binary_op() {}

  interval operator()(const interval &a, const interval &b) override
  {
    if (a.is_empty() || b.is_empty()) { return interval(); }
    double t[4] = {a.L * b.L, a.R * b.R, a.L * b.R, a.R * b.L};
    sort(t, t + 4);
    return interval(t[0], t[3]);
  }
};

struct div : binary_op {
  div() : binary_op() {}

  interval operator()(const interval &a, const interval &b) override
  {
    if (a.is_empty() || b.is_empty()) { return interval(); }
    if (b.L <= 0 && b.R >= 0) {
      return interval(-INFINITY, INFINITY);
    }
    double t[4] = {a.L / b.L, a.R / b.R, a.L / b.R, a.R / b.L};
    sort(t, t + 4);
    return interval(t[0], t[3]);
  }
};

struct phi : binary_op {
  phi() : binary_op() {}

  interval operator()(const interval &a, const interval &b)
  {
    return interval{min(a.L, b.L), max(a.R, b.R)};
  }
};

struct _less : binary_op {
  bool enabled, strict_int;

  _less(bool _strict_int)
      : binary_op(), enabled(true), strict_int(_strict_int) {}

  void enable(bool enable) { enabled = enable; }

  interval operator()(const interval &obj, const interval &ref) override
  {
    if (!enabled) { return obj; }
    return interval(obj.L, min(obj.R, ref.R - (double) strict_int));
  }
};

struct _greater : binary_op {
  bool enabled, strict_int;

  _greater(bool _strict_int)
      : binary_op(), enabled(true), strict_int(_strict_int) {}

  void enable(bool enable) { enabled = enable; }

  interval operator()(const interval &obj, const interval &ref) override
  {
    if (!enabled) { return obj; }
    return interval(max(obj.L, ref.L + (double) strict_int), obj.R);
  }
};
//一元运算只有转型，其他的用加常数节点来实现。

struct castint : binary_op {
  castint() : binary_op() {}
  interval operator()(const interval &x, const interval &) override
  {
#define CAST(x) ( isfinite(x) ? (double)(int)(x) : (x))
    return interval{ CAST(x.L), CAST(x.R) };
  }
};
#if 0
//FIXME 你不能直接new一个interval吗? 可以的
struct initial : binary_op {
  interval constant;

  initial(interval c) : binary_op(), constant(c) {}

  interval
  operator()(const interval &, const interval &) override { return constant; }
};
#endif


#endif
