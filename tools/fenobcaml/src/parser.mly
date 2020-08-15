%{

%}
%token COLONEQ LPAREN RPAREN EPSILON QUESTION ASTERISK PLUS PIPE
%token <string> ID

%start toplevel
%type <string> toplevel
%%

toplevel :
| EPSILON { "" }
