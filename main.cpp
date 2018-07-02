//
// Created by prwang on 6/30/2018.
//
#include "main.h"
int timestamp;

int main()
{
  yyparse();
  parse_postfix();
  //TODO 读入区间和函数
  //TODO 调用它的eval进行求值。
}
