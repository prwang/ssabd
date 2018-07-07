%{
#include "SDT.h"
#include "parser.hh"
void yyerror(const char *s);
#define STR do { if (!yytext) abort();  yylval.slv = string(yytext); } while (false)
%}
%x phi


ndg [_A-Za-z]
dg [0-9]
blank ([ \f\r\t\v]+)

const [-+]?({dg}+|{dg}*[.]{dg}+)([eE][-+]?[0-9]+)?
id_ign {ndg}+[.]{dg}+
id {ndg}({ndg}|{dg})*(_{dg}+)?
id_ext [(]({dg}+|D)[)]
%%
";;".*"\n"    yylineno++;
"#"           { BEGIN phi; }
<phi>"\n"     { yylineno++;  BEGIN INITIAL; }
<phi>=        return ASN;
<phi>PHI      return PHI;

<phi>{id}     { STR; return PHIID; }
<phi>{id_ext} { STR; return IDEXT; }
<phi>{blank}+ ;
<phi><        return PHILT;
<phi>>        return PHIGT;
<phi>","      return CMA;


"\n"          yylineno++;
{blank}+      ;
[><]=?|[=!]=  { STR; return RELOP; }
=             return ASN;
[-+*/]        { STR;  return ARIOP; }
"("           return LP;
")"           return RP;
"{"           return LB;
"}"           return RB;
","           return CMA;
":"           return COL;
";"           return SCOL;
if            return IF;
else          return ELSE;
goto          return GOTO;
return        return RETURN;
int           return INT;
float         return FLOAT;
{id}|{id_ign} { STR; return ID; }
{id_ext}      { STR; return IDEXT; }
{const}       { sscanf(yytext, "%lf", &yylval.clv); return CONST; }
"<bb "{dg}+">"  { STR; return LABEL; }
"<L"{dg}+">"      { return RLABEL; }

. {
  printf("%d: unexpected token %s\n", yylineno, yytext);
  exit(3);
 }


%%
int yywrap(void)
{
  return 1;
}
