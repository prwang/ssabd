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
   * 我要知道每个节点，它属于哪个函数(已解决)的哪个基本块
   * 为了在函数翻译结束之后，进行连边（要redirect）
   * 方案1：块内的在一块翻译完先连好边，把不能连的，块外的边搞一个list，存二元组
   * (string bb, node* n, bool which)，之后统一破解
   *
   *
   *
   * d=f(a, b, c)的翻译：
   * 调用函数之前当然要准备好所有的参数，因此要新建立一个新节点，代表函数调用。
   * 注意这与函数的超级节点是不同的。
   *
   * 它的id2node设为另一个无效的指针，当求值发现是这个玩意，就查另外一个表，
   * 给一个id，变成字符串。再用字符串找到函数，开始做新的函数
   *
   *
   */

#ifndef SSABD_SDT_H
#define SSABD_SDT_H

#include "nodes.h"

using namespace std;

enum rid_type { NORM, EXT, ARG };
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

/// \brief inter-block reference
struct IBR {
  string var;

  string bb;
  binary_op * node;
  bool which;
};

struct func {

/*
 * TODO
  类型查询器 set(j, int)
  get(j)  = int
  get(j_3) = int 因为源程序的变量名没有下划线

  */
  string func_name;
  using rdt_t =  unordered_map<pair<string, string>, string>;
  /// \brief 如果你是基本块string，要访问string这个变量，那么你实际应该访问string
  rdt_t redirection;
  /// \brief 用于tarjan缩点的开始节点
  int supernode_id;
  func(const string& name, const vector<string>& argname) : func_name(name)
  {
    id2node[supernode_id = ++n] = nullptr;
    ret = nullptr;
    args.resize(argname.size(), interval());
    int i = 0;
    for (const string& s : argname) {
      name2arg.insert(&args[i++]);
    }
  }
  vector<IBR> interblock_refs;
  unordered_map<string, interval*> name2arg;
  vector<interval> args;
  unordered_map<string, binary_op*> nodes;

  interval *ret;


  // 一个函数，我之后要对他反复求值，那么我应该有它的求值顺序
  //你给我一个order，我按顺序对里面的数字调用do_scc就可以了
  vector<int> order;
  void do_rval(RVAL rv, binary_op* op, bool which);
  void postfix();
  interval eval(const vector<interval>&);
};

extern func *current_func;
extern string current_bb;
extern unordered_map<string, func *> func_table;


/// \brief relation operators
enum RELOP {
  LT, GT, LE, GE, EQ, NEQ, TRUE, FALSE
};

inline string del_ver(string s)
{
  string::size_type k = s.find_last_of('_');
  if (k == string::npos || k == 0) return s;
  else return s.substr(0, k);

}
//同样的事情不要case两遍，让他把字符串直接传过来！
//直接返回一个node，让调用者去搞吧！
inline binary_op* make_ari_node(string op)
{
  const char* s=op.c_str();
  if (s[1] == '\0') {
    switch (s[0]) {
    case '+' :
      return new add();
    case '-' :
      return new sub();
    case '*' :
      return new mul();
    case '/' :
      return new div();
    }
  } else
    assert(0);
}




RELOP get_rel(const char *s)
{

  //FIXME 应该就地翻译
  if (s[2] == '\0' && s[1] == '=') {
    switch (s[0]) {
    case '<':
      //FIXME 应该就地翻译
    case '>':
      //FIXME 应该就地翻译
    case '=':
      //FIXME 应该就地翻译
    case '!':
      //FIXME 应该就地翻译
    default:
      assert(0);
    }
  } else if (s[1] == '\0') {
    switch (s[0]) {
    case '<':
      //FIXME 应该就地翻译
    case '>':
      //FIXME 应该就地翻译
    default:
      assert(0);
    }
  } else
    assert(0);

}

//id + ver合适，
//环境里面，可以用id+ver查询变量的值
//类型怎么办，还有一种显示转型
//字面值：float一定是科学记数法

void setname(const string &);

void settype(const string &, bool is_float);

void gettype(const string &);

extern void yyparse();

void parse_postfix();

#endif //SSABD_SDT_H

