//
// Created by prwang on 6/30/2018.
//

/* 翻译的任务
 *  1. 生成所有的节点。节点包括，
 *  ari_exp中翻译：
 *    常数节点，二元运算节点，转型节点，
 *
 *
 *  3. postfix 的任务清单：
 *    ariexp中翻译：
 *      节点（已述，这个自然成为任务）
 *    函数调用中翻译：
 *      函数的列表
 *    条件跳转中翻译：
 *    redirection
 *
   *
   *
   *
   *
   */

#ifndef SSABD_SDT_H
#define SSABD_SDT_H

#include "nodes.h"
#include <algorithm>

using namespace std;

enum rid_type {
  NORM, EXT, ARG
};
enum var_type {
  INT, FLOAT,
};
struct VARDEF {
  string var;
  var_type type;
};
struct RID {
  string var;
  rid_type type;
};
struct RVAL {
  bool isvar;
  union {
    double C;
    RID r;
  };
};

inline string del_version(string s)
{
  string::size_type k = s.find_last_of('_');
  if (k == string::npos || k == 0/*temporary*/) { return s; }
  else { return s.substr(0, k); }
}

struct DEF {

};
/// \brief inter-block reference
struct IBR {
  string var;
  string bb;
  OP *node;
  int which;
};
#define DEFAULT_RET_BB_NAME ";DEFAULT_RET_BB_NAME"

struct func {
  string func_name;
  string ret_bb_name;
  int first_node; /// \brief 连接所有参数的虚拟节点，求值的开始
  int last_node, first_scc, last_scc;
  bool bad; ///\brief 函数是否是void 类型的，如果是则不需要这个函数
  struct pairhash {
    template<typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const
    {
      return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
    }
  };
  /// \brief 如果你是基本块string，要访问string这个变量，那么你实际应该访问string
  using rdt_t =  unordered_map<pair<string, string>, string, pairhash>;
  rdt_t redirection;

  /// \brief 用于tarjan缩点的开始节点
  func(const string &name, const vector<string> &argname) : func_name(name)
  {
    ret_bb_name = DEFAULT_RET_BB_NAME;
    map_func_name[func_name] = this;
    id2node[first_node = ++n] = nullptr;
    first_scc = n_scc + 1;
    ret = nullptr;
    args.resize(argname.size(), interval());
    for (int i = 0; i < (int) argname.size(); ++i) {
      name2arg.insert(make_pair(argname[i], &args[i++]));
    }
  }

  ~func()
  {
    map_func_name[func_name] = nullptr;
    for (auto &x : nodes) {
      delete x.second;
    }
  }

  vector<IBR> interblock_refs; /// \brief 跨块引用，最后解析
  unordered_map<string, interval *> name2arg; ///\brief 参数名到参数值
  vector<interval> args; ///\brief 参数值存储位置
  unordered_map<string, OP *> nodes; /// \brief 变量定义表
  unordered_map<string, var_type> types; ///\brief 变量类型表
  bool is_var_int(const string &varname)
  {
    return types[del_version(varname)] == INT;
  }

  interval *ret; /// \brief 返回值变量位置

  // 一个函数，我之后要对他反复求值，那么我应该有它的求值顺序
  //你给我一个order，我按顺序对里面的数字调用do_scc就可以了
  vector<int> order;

  void do_rval(const RVAL &rv, OP *op, int which);

  void postfix();

  void add_var_def(const VARDEF &x) { types[x.var] = x.type; }

  interval eval(const vector<interval> &);

  void do_topo();

  void add_generic(const string &name, generic_func_call *op, vector<RVAL> *r)
  {
    int size = (int) r->size();
    for (int i = 0; i < size; ++i) {
      do_rval((*r)[i], op, i);
    }
    nodes[name] = op;
    delete r;
  }

  void add_ariop(const string &name,
                 const string &op_name, const RVAL &l, const RVAL &r)
  {
    binary_op *op = make_ari_node(op_name);
    do_rval(l, op, 0);
    do_rval(r, op, 1);
    nodes[name] = op;
  }

  void add_rel(const string &op,
                const string &bb_t, const string &bb_f,
                const RVAL &l, const RVAL &r)
  { _add_rel(op, &bb_t, &bb_f, &l, &r);}
  void _add_rel(const string &op,
               const string *bb_t, const string *bb_f,
               const RVAL *l, const RVAL *r)
  {
    //基本情况 a<b a==b
    //两种交换：交换lr，交换tf
    const char *s = op.c_str();
    bool swlr(false), swtf(false);
    //(bb1, var) -> var_t  (bb2, var) -> var_f
    if (s[2] == '\0' && s[1] == '=') {
      switch (s[0]) {
      case '<':
        swlr = swtf = true;
        goto LESS;
      case '>':
        swlr = true;
        goto LESS;
      case '!':
        swtf = true; //fall through
      case '=':
        if (swtf) { swap(bb_t, bb_f); }
        if (!l->isvar) { swap(l, r); }
        assert(l->isvar);
        if (r->isvar) {
          //TODO v == v 4个
        } else {
          // TODO c == v 两个
        }
        return;

      default:
        assert(false);
      }
    } else if (s[1] == '\0') {
      switch (s[0]) {
      case '>':
        swtf = true; //fall through
      case '<':
      LESS:
        if (swtf) { swap(bb_t, bb_f); }
        if (swlr) { swap(l, r); }
#define GETTYPE

//注意要查两遍???，因为gcc的temporary没有名字带下划线
        //一个玩意是temporary当且仅当下划线加数字的形式
        //所以直接del就可以了
#define is_const_int(C) (int(C) == (C))
        if (l->isvar && r->isvar) {
          //TODO  v < v 两个
        } else if (l->isvar) {
          bool I = is_var_int(l->r.var);
          if (I) { assert(is_const_int(r->C)); }
          //TODO  v < c 一个
          //用字符串就得不重名。搞一个特殊字符
          OP *lt = new _less(I), *ge = new _greater(false);
          nodes[l->r.var + ";true"] = lt;
          string tr = l->r.var + ";true";
          lt->set_input(new interval(r->C, r->C), 1);

        } else if (r->isvar) {
          bool I = is_var_int(r->r.var);
          if (I) { assert(is_const_int(l->C)); }

          //TODO c < v 一个
        } else { assert(false); }

      default:
        assert(false);
      }
    } else
      assert(false);

  }

  void add_unary(const string &name, unary_op *op, const RVAL &rv)
  {
    do_rval(rv, op, 0);
    nodes[name] = op;
  }

private:
  static binary_op *make_ari_node(const string &op)
  {
    const char *s = op.c_str();
    if (s[1] == '\0') {
      switch (s[0]) {
      case '+' :
        return new add();
      case '-' :
        return new sub();
      case '*' :
        return new mul();
      case '/' :
        return new _div();
      }
    } else
      assert(false);
  }
};

extern func *current_func;
extern string current_bb;
extern unordered_map<string, func *> func_table;


/// \brief relation operators
enum RELOP {
  LT, GT, LE, GE, EQ, NEQ, TRUE, FALSE
};


extern void yyparse();

void parse_postfix();

#endif //SSABD_SDT_H

