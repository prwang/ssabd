%{
#include "SDT.h"
#include "ssa.tab.h"
#define STR yylval.slv = string(yytext)
%}
%x phi comment


ndg [_A-Za-z]
dg [0-9]
blank ([ \f\r\t\v]+)
label <({dg}|{ndg})+({blank}{dg}+)?>

const [-+]?({dg}+|{dg}*.{dg}+)([eE][-+]?[0-9]+)?
id {ndg}*_dg
id_ext \({dg}+|D\)
%%
";;"          { BEGIN comment; }
"#"           { BEGIN phi; }
<comment>"\n" { BEGIN INITIAL; }

<phi>"\n"     { BEGIN INITIAL; }
<phi>=        return ASN;
<phi>{id}     { STR; return ID; }
<phi>PHI      return PHI;
<phi>{blank}+ ;
<phi><        return PHILT;
<phi>>        return PHIGT;
<phi>","      return CMA;

{label}       { STR; return LABEL; }
const         { sscanf(yytext, "%lf", &yylval.clv); return CONST; }
"("           return LP;
")"           return RP;
"{"           return LB;
"}"           return RB;
","           return CMA;
":"           return COL;
";"           return SCOL;
if            return IF;
goto          return GOTO;
return        return RETURN;
=             return ASN;
{id}          { STR; return ID; }
{id_ext}      { STR; return IDEXT; }
[><]=?|[=!]=  { STR; return RELOP; }
[-+*/]        { STR;  return ARIOP; }

. { yyerror(((string)"unexpected token" + (string) yytext ).c_str()); }


%%
int yywrap(void)
{
  return 1;
}
