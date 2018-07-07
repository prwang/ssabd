//
// Created by prwang on 6/30/2018.
//
#include "main.h"

[[noreturn]] void print_usage(const char *prog)
{
  printf(R"(
Usage:
  %s [-v] [-g <graph_file>] -f <function_name> -I <interval_1> ... -I <interval_n>  <file>
Options:
  -f    Mandatory. You have to specify the entry function name

  -I    Mandatory. You have to specify the input range(s) of the argument(s)
        of the entry function. <interval> should be
          NUMBER,NUMBER
        where NUMBER is a floating point literal including '+infinity'

  -v    Enable verbose mode

  -g    Dump the constraint graph to <graph_file> in DOT language,
        which may be later visualized with GraphViz.
 )", prog);
  exit(2);
}
extern FILE* yyin;
bool verbose;
FILE* graph_file = nullptr;
extern int yydebug;
int main(int argc, char **argv)
{
  vector<interval> arg;
  string func_name;
  for (int opt; (opt = getopt(argc, argv, "f:I:vg:")) != -1;) {
    switch (opt) {
    case 'v':
      yydebug = verbose = true;
      break;
    case 'I': {
      interval I;
      sscanf(optarg, "%lf,%lf", &I.L, &I.R);
      arg.push_back(I);
      break;
    }
    case 'f':
      func_name = optarg;
      break;
    case 'g':
      graph_file = fopen(optarg, "w");
      if (!graph_file) {
        fprintf(stderr, "Invalid graph file path\n");
        print_usage(argv[0]);
      }
      break;
    default:
      print_usage(argv[0]);
    }
  }
  if (optind >= argc) {
    print_usage(argv[0]);
  } else {
    yyin = fopen(argv[optind], "r");
  }
  yyparse();
  func* F = name2func[func_name];
  if (!F) {
    fprintf(stderr, "cannot find func");
    print_usage(argv[0]);
  }
  interval x = F->eval(arg);
  printf("[%lf, %lf]\n", x.L, x.R);
}
