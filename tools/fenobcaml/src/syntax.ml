type terminal = char

type nonterminal = string

type symbol =
  | Term of terminal
  | Nont of nonterminal

type expr =
  | Epsilon
  | Symbol of symbol
  | Concat of expr * expr
  | Union of expr * expr
  | Option of expr

let symbol_nont s = Symbol(Nont s)

let symbol_term c = Symbol(Term c)

let string_of_expr e =
  let buf = Buffer.create 100 in
  let emit = Buffer.add_string buf in
  let rec iter = function
    | Epsilon -> emit "~"
    | Symbol(Term(c)) -> emit (Printf.sprintf "'%c'" c)
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
