type terminal = string

type nonterminal = string

type symbol =
  | Term of terminal
  | Nont of nonterminal

type expr =
  | Symbol of symbol
  | Concat of expr * expr
  | Union of expr * expr
  | Option of expr

let string_of_expr e =
  let buf = Buffer.create 100 in
  let emit = Buffer.add_string buf in
  let rec iter = function
    | Symbol(Term(s)) -> List.iter emit ["\""; s; "\""]
    | Symbol(Nont(s)) -> emit s
    | Concat(a, b) ->
      emit "("; iter a; emit " "; iter b; emit ")"
    | Union(a, b) ->
      emit "("; iter a; emit "|"; iter b; emit ")"
    | Option(a) ->
      emit "("; iter a; emit ")?"
  in
  iter e;
  Buffer.contents buf
