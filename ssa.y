%{
#include "SDT.h"
extern "C" void yyerror(const char* s);
extern "C" int yylex(void);

%}

%union
{
  double clv;
  string slv;
  RID rid;
  RVAL rval;
  vector<RVAL>* rval_list;
}

%token LP
%token RP
%token LB
%token RB
%token CMA
%token SCOL
%token COL
%token IF
%token GOTO
%token RETURN
%token ASN

%token PHILT
%token PHIGT
%token PHI

%token<slv> ID
%token<slv> RELOP
%token<slv> ARIOP
%token<slv> IDEXT
%token<slv> LABEL

%token<clv> CONST


%type<rid> rid
%type<rval> rval
%type<rval_list> rval_list
/*lexval是什么类型的？是yystype 类型的*/

%%
/*翻译方法：用$$代表左边的属性，$1 $2...代表右边的属性(实际上是栈)*/

program : func program
        | ;
/*return, 就是合并函数return的那个变量，和调用的那个变量*/ 
/*call, 就是生成复制参数的赋值语句*/
func    : ID LP rval_list RP LB MFinit deflist BBlist RB;
MFinit  : {
            // ID(-4) LP(-3) rval_list(-2) RP(-1) LB(0)
            vector<string> V;
            for (RVAL & r : *$-2) {
              assert(r.isvar && r.r.type == NORM);
              V.push_back(r.r.var);
            }
            delete $-2;
            current_func = new func($-4, V);
          };        
rid     : rid IDEXT { 
            $$ = $1;
            if ($2[1] == 'D') {
              $$.type = ARG;
            } else if ($$.type == NORM) {
              $$.type = EXT;
            }
          }
        | ID { $$.var = $1; $$.type = NORM; };

rval    : rid { $$.isvar = true; $$.rid = $1; }
        | const  { $$.isvar = false; $$.C = $1; };

rval_list : rval_list CMA rval 
            { ($$ = $1)->push_back($3); $1= NULL; } 
          | rval 
            { $$= new vector<string>; idlist->push_back($1); }; 
        
BBlist  : BB BBlist
        | ;
BB      : LABEL COL BBname phi_list ari_list jump;
BBname  : { current_bb = $-1; };
phi_exp : ID E PHI PHILT rval_list  PHIGT {
            // ID(1) E(2) PHI(3) PHILT(4) rval_list(5)  PHIGT(6)
            const vector<RVAL>& V = *$5;
            int x = (int)v.size(); 
            assert(x >= 2);
            phi *result = new phi();
            current_func->do_rval(V[0], result, 0);
            current_func->do_rval(V[1], result, 1);
            for (int i = 2; i < x; ++i) {
              phi *p1 = new phi(); 
              p1->input[0] = &result->output;
              add_edge(result->id, p1->id);
              current_func->do_rval(V[i], p1, 1);
              result = p1;
            }
            delete $5; $5 = NULL;
          }
         
phi_list: phi_list phi_exp SCOL 
        | ;
ari_list: phi_list ari_exp SCOL 
        | ;

/*TODO 这里要生成节点!*/
ari_exp : ID E rval ariop rval {  }
        | ID E LP INT RP rval
        | ID E LP FLOAT RP rval
        | ID E ID LP rval_list RP
        | ID LP rval_list RP; /*没有全局变量，没有指针类型，可以直接忽略*/
rel_exp : rval relop rval;

/*
 * 生成两个限制节点，并且禁止连边

 * int:
 * a<=b  true:
 * less_equal(obj:a, ref:b)
 * greater_equal(obj:b, obj:a)
 *
 * false: equivalent to b <= a-1
 * less_equal(obj:b, ref:a - 1)
 * greater_equal(obj:a, ref:b + 1)
 *
 * */
jump    : IF LP rel_exp RP GOTO LABEL SCOL
        | GOTO LABEL SCOL
        | ; /* 有可能fall到return里面*/
return : ;
%%

void yyerror(const char *s)
{
  printf("Parser ERR: %d: %s\n", yylineno, s);
  exit(2);
}

