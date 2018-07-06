//
// Created by prwang on 6/30/2018.
//
#include "main.h"

[[noreturn]] void print_usage(const char *prog)
{
  printf(R"(
Usage:
  %s  -f <function_name> -I <interval_1> ... -I <interval_n> [-v]
interval:
  NUMBER,NUMBER        No space is allowed around the comma ','
The program SSA file should be standard input, and
n is the number of the arguments of <function_mame>.
 Debug output is turned on if '-v' is specified. )", prog);
  exit(2);
}

bool verbose;
int main(int argc, char **argv)
{
  yyparse();
  vector<interval> arg;
  func *F = nullptr;
  for (int opt; (opt = getopt(argc, argv, "f:I:v")) != -1;) {
    switch (opt) {
    case 'v':
      verbose = true;
      break;
    case 'I': {
      interval I;
      sscanf(optarg, "%lf,%lf", &I.L, &I.R);
      arg.push_back(I);
      break;
    }
    case 'f':
      F = func_table[optarg];
      break;
    default:
      print_usage(argv[0]);
    }
  }
  if (!F) { print_usage(argv[0]); }
  F->eval(arg);
  printf("[%lf, %lf]\n", F->ret->L, F->ret->R);
}
