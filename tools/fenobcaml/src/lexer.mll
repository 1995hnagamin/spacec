{
open Parser
}
rule main = parse
| [' ' '\n']+ { main lexbuf }
| "::=" { COLONEQ }
| "("   { LPAREN }
| ")"   { RPAREN }
| "~"   { EPSILON }
| "?"   { QUESTION }
| "*"   { ASTERISK }
| "+"   { PLUS }
| "|"   { PIPE }
| ";"   { SEMICOLON }
| ","   { COMMA }
| ['a'-'z' 'A'-'Z'] ['a'-'z' 'A'-'Z' '0'-'9' '_' '\'']* {
    ID(Lexing.lexeme lexbuf)
  }
| '"' ['a'-'z' '0'-'9' '+' '*' '(' ')']* '"' {
    let str = Lexing.lexeme lexbuf in
    let len = String.length str in
    StrLit(String.sub str 1 (len-2))
  }
| eof { EOF }
