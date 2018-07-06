/// \brief 所有的区间运算
// Created by prwang on 6/30/2018.

#ifndef SSABD_NODES_H
#define SSABD_NODES_H

#include "interval.h"
#include <utility>
#include <cfloat>
#include <algorithm>
#include <cmath>

using namespace std;

struct OP {
  int id;
  interval output, old_output;
  OP() : id(++n), output() { id2node[id] = this; }
  ~OP() { id2node[id] = nullptr; }
  OP(const OP &) = delete;
  OP &operator=(const OP &) = delete;
  virtual bool eval() = 0; //返回求值后有没有改变
  virtual void set_input(const interval* val, int which) = 0;
};
struct binary_op:OP {
  const interval *input[2];
  void set_input(const interval* val, int which) override {
    assert(which == 0 || which == 1);
    input[which] = val;
  }
  binary_op() : input{nullptr, nullptr} {}

  bool eval() final override
  {
    old_output = output;
    output = (*this)(*input[0], *input[1]);
    return old_output == output;
  }
protected:
  virtual interval operator()(const interval &a, const interval &b) = 0;
};


struct add : binary_op {
  interval operator()(const interval &a, const interval &b) override
  {
    if (a.is_empty() || b.is_empty()) { return interval(); }
    return interval(a.L + b.L, a.R + b.R);
  }
};

struct sub : binary_op {
  interval operator()(const interval &a, const interval &b) override
  {
    if (a.is_empty() || b.is_empty()) { return interval(); }
    return interval(a.L - b.L, a.R - b.R);
  }
};

struct mul : binary_op {

  interval operator()(const interval &a, const interval &b) override
  {
    if (a.is_empty() || b.is_empty()) { return interval(); }
    double t[4] = {a.L * b.L, a.R * b.R, a.L * b.R, a.R * b.L};
    sort(t, t + 4);
    return interval(t[0], t[3]);
  }
};

struct _div : binary_op {

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


//就用double！
// int型的< >的问题在翻译的时候就地解决
//enabled是不是在求scc的时候就能求出来
//
struct compare : binary_op {
  bool ref_from_same_scc;
  compare() : ref_from_same_scc(false) {}
};
struct _less : compare {
  bool strict_int;
  _less(bool _strict_int)
      :  strict_int(_strict_int) { }
  interval operator()(const interval &obj, const interval &ref) override
  {
    if (ref_from_same_scc && !allow_same_scc) { return obj; }
    return interval(obj.L, min(obj.R, ref.R - (double) strict_int));
  }
};
struct _equal : compare {
  interval operator()(const interval &obj, const interval &ref) override
  {
    if (ref_from_same_scc && !allow_same_scc) { return obj; }
    return interval(max(obj.L, ref.L), min(obj.R, ref.R));
  }
};
struct _greater : compare {
  bool strict_int;
  _greater(bool _strict_int)
      : strict_int(_strict_int) {}
  interval operator()(const interval &obj, const interval &ref) override
  {
    if (ref_from_same_scc && !allow_same_scc) { return obj; }
    return interval(max(obj.L, ref.L + (double) strict_int), obj.R);
  }
};

struct unary_op : OP {
  const interval* input;
  void set_input(const interval* val, int which) override {
    assert(which == 0);
    input = val;
  }
  bool eval() final override
  {
    old_output = output;
    output = (*this)(*input);
    return output == old_output;
  }
protected:
  virtual interval operator()(const interval& x) = 0;
};
struct castint : unary_op {
  interval operator()(const interval& x) override
  {
#define CAST(x) ( isfinite(x) ? (double)(int)(x) : (x))
    return interval{CAST(input->L), CAST(input->R)};
  }
};
struct cp : unary_op {
  interval operator()(const interval& x) override { return *input; }
};
struct generic_func_call : OP {
  vector<const interval*> input;
  generic_func_call(int ary) : input(ary) {}
  void set_input(const interval* val, int which) override
  {
    assert((unsigned)which < input.size());
    input[which] = val;
  }
};

struct phi : generic_func_call {
  phi(int ary) : generic_func_call(ary) {}
  bool eval() override
  {
    old_output = output;
    int s = (int)input.size();
    assert(s >= 2);
    output = *input[0];
    for (int i = 1; i < s; ++i)
      output = Uni(output, *input[i]);
    return output == old_output;
  }
  void grow()
  {
    if (!old_output.is_empty()) {
      if (output.L < old_output.L) {
        output.L = -INFINITY;
      }
      if (output.R > old_output.R) {
        output.R = INFINITY;
      }
    }
  }
};

struct func_call : generic_func_call {
  string func_name;
  func_call(int ary, const string& _func_name)
      : generic_func_call(ary), func_name(_func_name) {}
  vector<const interval*> input;
  bool eval() override;
};
#endif
