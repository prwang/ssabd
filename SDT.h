//
// Created by prwang on 6/30/2018.
//

#ifndef SSABD_SDT_H
#define SSABD_SDT_H
#include "nodes.h"
using namespace std;
struct func
{
  string func_name;
  unordered_map<pair<string, string>, string> redirection;
  // 我要知道每个节点，它属于哪个函数(已解决)的哪个基本块(FIXME ???)。
  // 一个函数，我之后要对他反复求值，那么我应该有它的节点列表，和求值顺序
  vector<int> order;
  //整数与实际的节点怎么映射？
  vector<interval> args;
  vector<binary_op> nodes;

};
extern string current_function;
extern func* current_env;
extern unordered_map<string, func*> env_table;

void function_postfix();



//一个函数一个词法环境
//没有递归
//这个env为什么有用？
//它实际上就是全局变量，在postfix的时候要使用。

/* 翻译的任务
 *  1. 生成所有的节点。节点包括，
 *  ari_exp中翻译：
 *    常数节点（存疑），二元运算节点，转型节点，
 *
 *
 *  3. postfix 的任务清单：
 *    ariexp中翻译：
 *      节点（已述，这个自然成为任务）
 *    函数调用中翻译：
 *      函数的列表
 *    条件跳转中翻译：
 *      禁止块外使用表
 * */
/// \brief arithmetic operators
enum ARITHOP { ADD, SUB, MUL, DIV, };
/// \brief relation operators
enum RELOP { LT, GT, LE, GE, EQ, NEQ, TRUE, FALSE };

//id + ver合适，
//环境里面，可以用id+ver查询变量的值
//类型怎么办，还有一种显示转型
//字面值：float一定是科学记数法
/*
 *
  版本的意义就是 所有版本的类型是一样的
  类型查询器 set(j, int)
  get(j)  = int
  get(j_3) = int 因为源程序的变量名没有下划线
  */
void add_arith();
void setname(const string&);
void settype(const string&, bool is_float);
void gettype(const string&);
extern void yyparse();
void parse_postfix();
#endif //SSABD_SDT_H

