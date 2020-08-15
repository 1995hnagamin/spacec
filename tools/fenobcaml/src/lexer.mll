{
open Parser
}
rule main = parse
| "::=" { COLONEQ }
| "("   { LPAREN }
| ")"   { RPAREN }
| "~"   { EPSILON }
| "?"   { QUESTION }
| "*"   { ASTERISK }
| "+"   { PLUS }
| "|"   { PIPE }
| eof { exit 0 }
