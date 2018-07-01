%{
#include "parse.h"
#include "ssa.tab.h"
%}
%x comment
ndg [_A-Za-z]
dg [0-9]

LP \(
RP \)
LB \{
RB \}
CMA ,
COL :
SCOL ;
IF if
GOTO goto
RETURN return
E =

blank ([ \f\r\t\v]+)
label <({dg}|{ndg})+({blank}{dg}+)?>

float_const [-+]?[0-9]*\.?[0-9]+[eE][-+]?[0-9]+
int_const \d+
ari_op [+-*/]
rel_op [><]=?|[=!]=
id {ndg}*_dg\(\) 

%%

%%
int yywrap(void)
{
  return 1;
}
