%{
#define YYDEBUG 1
#include "SDT.h"
void yyerror(const char* s);
int yylex(void);
#define R(x, y, z) current_func->do_rval((x), (y), (z));
%}


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
%type<defL_C> defL_CO
%type<slv> jump_tar
%type<is_def_int> is_def_int
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
is_def_int : INT { $$ = true; }
        | FLOAT {$$ = false; };
def     : is_def_int ID { $$ = VARDEF{$2, $1}; };

defL_C  : defL_C CMA def { ($$ = $1)->push_back($3); $1 = NULL; }
        | def { $$ = new vector<VARDEF>; $$->push_back($1); };
defL_CO : defL_C { $$ = $1; $1 = NULL; } | { $$ = new vector<VARDEF>; } ;
func    : ID LP defL_CO RP LB MFinit defL_S BBlist RB
          { current_func->postfix(); current_func = nullptr; };

MFinit  : {
            // ID(-4) LP(-3) defL_C(-2) RP(-1) LB(0)
            vector<string> V;
            for (VARDEF & r : *$<defL_C>-2) {
              V.push_back(r.var);
            }
            current_func = new func($<slv>-4, V);
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
        | PHIID { $$.var = $1; $$.type = NORM; };

rval    : rid { $$.isvar = true; $$.r = $1; }
        | CONST  { $$.isvar = false; $$.C = $1; };

rval_list : rval_list CMA rval 
            { ($$ = $1)->push_back($3); $1= NULL; } 
          | rval 
            { $$ = new vector<RVAL>; $$->push_back($1); };
BBlist  : BBlist BB  | ;
phi_list: phi_list phi_exp | ;
ari_list: ari_list ari_exp SCOL | ;
jump_tar: LABEL { $$ = $1; }
        | LABEL LP RLABEL RP { $$ = DEFAULT_RET_BB_NAME; }

LblC    : LABEL COL { current_func->setbb($1); };

BB      : BBjump | BBfall | return | vreturn;
BBjump  : LblC phi_list ari_list jump { current_func->fall = false; } ;
BBfall  : LblC phi_list ari_list ari_exp SCOL { current_func->fall = true; };

return  : M1 phi_list RLABEL COL RETURN ID SCOL { current_func->add_return($6); }
vreturn : LblC phi_list RETURN SCOL { current_func->bad = true; };
M1      : { current_func->setbb(); };


phi_exp : PHIID ASN PHI PHILT rval_list  PHIGT
          { current_func->add_generic($1, new phi($5->size()), $5); $5 = NULL; }
ari_exp : ID ASN rval ARIOP rval { current_func->add_ariop($1, $4, $3, $5); }
        | ID ASN LP is_def_int RP rval
          { current_func->add_unary($1, $4 ? (unary_op*)(new castint())
           : (unary_op*)(new cp()), $6); }
        | ID ASN rval { current_func->add_unary($1, new cp(), $3); }
        | ID ASN ID LP rval_list RP
          { current_func->add_generic($1, new func_call($5->size(), $3), $5); $5 = NULL; }
        | ID LP rval_list RP; /*忽略*/

/*        1  2  3    4     5    6  7    8        9    10   11   12       13 */
jump    : IF LP rval RELOP rval RP GOTO jump_tar SCOL ELSE GOTO jump_tar SCOL
         { current_func->add_rel($4, $8, $12, $3, $5); }
        | GOTO jump_tar SCOL{current_func->set_cfg($2, ""); };
%%

void yyerror(const char *s)
{
  printf("Parser ERR: %s\n", s);
  exit(2);
}

