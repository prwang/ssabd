//
// Created by prwang on 6/30/2018.
//

#ifndef PROJECT_COMMON_H
#define PROJECT_COMMON_H
#include <map>
#include <set>
#include <cassert>
#include <string>
#include <cmath>

using namespace std;
struct env_t_factory
{
  struct env_t
  {
  private:
    env_t *previous;
    map<int, string> nametb;
    map<string, int> stringtb;
  };
  env_t * current;
  void push();
  void pop();
} ENV; //似乎只有一层？词法环境的必要性存疑


#endif //PROJECT_COMMON_H
