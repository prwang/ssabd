%{
#include "SDT.h"
extern "C" void yyerror(const char* s);
extern "C" int yylex(void);
#define R(x, y, z) current_func->do_rval((x), (y), (z));
%}

%union
{
  double clv;
  string slv;
  RID rid;
  RVAL rval;
  VARDEF def;
  vector<VARDEF>* defL_C;
  vector<RVAL>* rval_list;
  var_type vartype;
}

%token ELSE
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
%token INT
%token FLOAT
%token RLABEL

%token PHILT
%token PHIGT
%token PHI

%token<slv> ID
%token<slv> PHIID
%token<slv> RELOP
%token<slv> ARIOP
%token<slv> IDEXT
%token<slv> LABEL

%token<clv> CONST

%type<defL_C> defL_C
%type<slv> jump_tar
%type<vartype> vartype
%type<rid> rid
%type<rval> rval
%type<def> def
%type<rval_list> rval_list
/*lexval是什么类型的？是yystype 类型的*/

%%
/*翻译方法：用$$代表左边的属性，$1 $2...代表右边的属性(实际上是栈)*/

program : func program
        | ;
/*return, 就是合并函数return的那个变量，和调用的那个变量*/ 
/*call, 就是生成复制参数的赋值语句*/
vartype : INT { $$ = INT; }
        | FLOAT {$$ = FLOAT; };
def     : vartype ID { $$ = VARDEF{$2, $1}; };
defL_C  : defL_C CMA def { ($$ = $1)->push_back($3); $1 = NULL; } 
        | def { $$ = new vector<VARDEF>; idlist->push_back($1); }; 
func    : ID LP defL_C RP LB MFinit defL_S BBlist RB
          { current_func->postfix(); current_func = nullptr; };

MFinit  : {
            // ID(-4) LP(-3) defL_C(-2) RP(-1) LB(0)
            vector<string> V;
            for (VARDEF & r : *$<defL_C>-2) {
              V.push_back(r.var);
            }
            current_func = new func($<ID>-4, V);
            for (VARDEF & r: *$<defL_C>-2) {
              current_func->add_var_def(r);
            }
            delete $<defL_C>-2;
          };        
defL_S  : defL_S def SCOL { current_func->add_var_def($2); } | ;
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
        | CONST  { $$.isvar = false; $$.C = $1; };

rval_list : rval_list CMA rval 
            { ($$ = $1)->push_back($3); $1= NULL; } 
          | rval 
            { $$ = new vector<RVAL>; idlist->push_back($1); }; 
BBlist  : BBlist BB  | ;
phi_list: phi_list phi_exp SCOL | ;
ari_list: ari_list ari_exp SCOL | ari_exp SCOL ;
jump_tar: LABEL { $$ = $1; }
        | LABEL LP RLABEL RP { $$ = DEFAULT_RET_BB_NAME; }
LblC    : LABEL COL { current_bb = $1; };

BB      : BBnormal | return | vreturn;
BBnormal: LblC phi_list ari_list jump; /*FIXME 不能label不能optional，否则和下一个bb连一起了*/

return  : phi_list RLABEL COL RETURN ID SCOL
          {
            auto it = nodes.find($5)
            assert(id != nodes.end());
            current_func->ret = &(*it)->output;
          }
vreturn : LblC phi_list RETURN SCOL { current_func->bad = true; };


phi_exp : PHIID ASN PHI PHILT rval_list  PHIGT
          { current_func->add_generic($1, new phi($5->size()), $5); $5 = NULL; }
ari_exp : ID ASN rval ARIOP rval { current_func->add_ariop($1, $4, $3, $5); }
        | ID ASN LP vartype RP rval
          { current_func->add_unary($1, $4 == INT ? new castint() : new cp(), $6); }
        | ID ASN rval { current_func->add_unary($1, new cp(), $3); }
        | ID ASN ID LP rval_list RP
          { current_func->add_generic($1, new func_call($5->size(), $3), $5); $5 = NULL; }
        | ID LP rval_list RP; /*忽略*/

/*        1  2  3    4     5    6  7    8        9    10   11   12       13 */
jump    : IF LP rval RELOP rval RP GOTO jump_tar SCOL ELSE GOTO jump_tar SCOL
         { current_func->add_rel($4, $8, $12, $3, $5); }
        | GOTO jump_tar SCOL | ; 
%%

void yyerror(const char *s)
{
  printf("Parser ERR: %d: %s\n", yylineno, s);
  exit(2);
}

