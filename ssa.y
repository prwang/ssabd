%{
#include "SDT.h"
extern "C" void yyerror(const char* s);
extern "C" int yylex(void);
%}

%union
{
  int ilv;
  float flv;
  string slv;
  ARITHOP aop;
  RELOP rop;
  struct {

  } rval;
  //FIXME 如果要为常数生成节点的话（为什么，为了不用写一元运算--其实已经写完了）
  // 那么比较里面的常数要不要生成节点？
  // 核心问题：essa的翻译方案
  // 
}

%token<ilv> iconst
%token<flv> fconst
%token<rop> relop
%token<aop> ariop
%token<slv> id
/*lexval是什么类型的？是yystype 类型的*/
/*没有递归那就内联！变量用函数名+变量名+版本号来表示 改名翻译完再说*/

%%
/*翻译方法：用$$代表左边的属性，$1 $2...代表右边的属性(实际上是栈)*/
program : func program
        | ;
/*return, 就是合并函数return的那个变量，和调用的那个变量*/ 
/*call, 就是生成复制参数的赋值语句*/
func    : id LP arglist RP LB Msetname deflist BBlist RB;
Msetname: { setname($-4.strval); }; 
arglist : arglist CMA id
        | id ;
        
BBlist  : BB BBlist
        | ;
BB      : label COL ari_list jump;
ari_list: ari_exp SCOL arilist
        | ;
rval    : id | fconst | iconst {};

/*TODO 这里要生成节点!*/
ari_exp : id E rval ariop rval {  }
        | id E LP INT RP rval
        | id E LP FLOAT RP rval
        | id E id LP arglist RP
        | id LP arglist RP; /*没有全局变量，没有指针类型，可以直接忽略*/

/*TODO 这里要对图进行操作*/
rel_exp : rval relop rval;
jump    : IF LP rel_exp RP GOTO label SCOL
        | GOTO label SCOL
        | ; /* 有可能fall到return里面*/
return : ;
%%
void yyerror(const char *s)
{
  printf("Parser ERR: %d: %s\n", yylineno, s);
  exit(2);
}

