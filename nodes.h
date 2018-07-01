//
// Created by prwang on 6/30/2018.
//

#ifndef SSABD_NODES_H
#define SSABD_NODES_H

#include "interval.h"
#include <utility>

using namespace std;

//二元运算 ：加减乘除
struct binary_op {
  virtual interval operator()(interval a, interval b) = 0;
};
//一元运算：线性、分母、转型、
struct unary_op {
  virtual interval operator()(interval a) = 0;
};
//一元运算只有转型，其他的用加常数节点来实现。

template<typename _T>
struct N { // container only, T = int or float
  using T = typename _T;

  static inline pair<T, T> Cast(interval i);
  static bool Zero(interval i);

  struct linear_op : unary_op {
    T bias, scale;

    interval operator()(interval a) override
    {
      switch (a.flags) {
      case I_NORM: {
        auto b = N<T>::Cast(a);
        T x = b.first * scale + bias, y = b.second * scale + bias;
        return scale > 0 ? interval(x, y) : interval(y, x);
      }
      case I_NINFT: {
        auto b = N<T>::Cast(a);
        T x = b.second * scale * bias;
        if (scale == 0) { return interval((T) 0, (T) 0); }
        return scale > 0 ? interval(true, x) : interval(x, true);
      }
      case I_PINFT: {
        auto b = N<T>::Cast(a);
        T x = b.first * scale * bias;
        if (scale == 0) { return interval((T) 0, (T) 0); }
        return scale < 0 ? interval(true, x) : interval(x, true);
      }
      case I_NPINFT: {
        if (scale == 0) { return interval((T) 0, (T) 0); }
        return interval(a.is_float);
      }
      default:
        assert(false);
      }
    }
  };


  struct divided : unary_op {
    T numerator;

    interval operator()(interval a) override
    {
      if (Zero(a)) {
        return interval(a.is_float);
      }
      auto b = N<T>::Cast(a);
      switch (a.flags) {
      case I_NORM: {
        T x = numerator / b.first, y = numerator / b.second;
        if (x > y) swap(x, y);
        return interval(x, y);
      }
      case I_PINFT: {
        assert(b.first > (T)0);
        T x = numerator / b.first;
        return numerator > 0 ? interval((T)0, x) : interval(x, (T)0);
      }
      case I_NINFT: {
        assert(b.second < (T)0);
        T x = numerator / b.second;
        return numerator < 0 ? interval((T)0, x) : interval(x, (T)0);
      }
      default: assert(false);
      }
    }
  };

};
template<> bool N<int>::Zero(interval i)
{
  assert(!i.is_float);
  return (i.flags & I_NINFT || i.iL <= 0)
         && (i.flags & I_PINFT ||  i.iR >= 0);
}
template<> bool N<float>::Zero(interval i)
{
  assert(i.is_float);
  return (i.flags & I_NINFT || i.fL <= 0)
         && (i.flags & I_PINFT ||  i.fR >= 0);
}
template<>
pair<int, int> N<int>::Cast(interval i)
{
  assert(!i.is_float);
  return make_pair(i.iL, i.iR);
}

template<>
pair<float, float> N<float>::Cast(interval i)
{
  assert(i.is_float);
  return make_pair(i.fL, i.fR);
}

#endif //SSABD_NODES_H
