%{
#include "SDT.h"
#include "parser.hh"
void yyerror(const char *s);
#define STR yylval.slv = string(yytext)
%}
%x phi comment


ndg [_A-Za-z]
dg [0-9]
blank ([ \f\r\t\v]+)

const [-+]?({dg}+|{dg}*.{dg}+)([eE][-+]?[0-9]+)?
id_ign {ndg}+[.]{dg}+
id ({ndg}|{dg})*_({dg}+)?
id_ext \({dg}+|D\)
%%
";;"          { BEGIN comment; }
"#"           { BEGIN phi; }
<comment>"\n" { BEGIN INITIAL; }

<phi>"\n"     { BEGIN INITIAL; }
<phi>=        return ASN;
<phi>{id}     { STR; return PHIID; }
<phi>PHI      return PHI;
<phi>{blank}+ ;
<phi><        return PHILT;
<phi>>        return PHIGT;
<phi>","      return CMA;

{id}          { STR; return ID; }
{id_ext}      { STR; return IDEXT; }
[><]=?|[=!]=  { STR; return RELOP; }
[-+*/]        { STR;  return ARIOP; }
"<bb "{dg}+">"  { STR; return LABEL; }
"<L"{dg}+">"      { return RLABEL; }
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
int           return INT;
float         return FLOAT;
=             return ASN;

. {
  printf("%d: unexpected token %s\n", yylineno, yytext);
  exit(3);
 }


%%
int yywrap(void)
{
  return 1;
}
