%{
open Syntax

let split_str_literal str =
  let len = String.length str in
  let tail = Symbol(Term(str.[len-1])) in
  let chars = List.init (len-1) (String.get str) in
  List.fold_right (fun c e -> Concat(Symbol(Term c), e)) chars tail

%}
%token COLONEQ LPAREN RPAREN EPSILON QUESTION ASTERISK PLUS PIPE
%token SEMICOLON EOF COMMA
%token <string> ID
%token <string> StrLit

%start main
%type <(Syntax.nonterminal * Syntax.expr) list> main
%%

main :
| rule_list EOF { $1 }

rule_list :
| rule SEMICOLON { [$1] }
| rule SEMICOLON rule_list { $1 :: $3 }

rule :
| ID COLONEQ exp { ($1, $3) }

exp :
| factor { $1 }
| factor PIPE exp { Union($1, $3 ) }

factor :
| token { $1 }
| token COMMA factor { Concat($1, $3) }

token :
| StrLit { split_str_literal $1 }
| ID { Symbol(Nont $1) }
| LPAREN exp RPAREN { $2 }
