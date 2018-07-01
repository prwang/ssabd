//
// Created by prwang on 6/30/2018.
//

#ifndef SSABD_SDT_H
#define SSABD_SDT_H
#include <string>
using namespace std;

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
#endif //SSABD_SDT_H

