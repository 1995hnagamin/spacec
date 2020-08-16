open Printf

let () =
  let filename = Sys.argv.(1) in
  let ic = open_in filename in
  let rules = Parser.main Lexer.main (Lexing.from_channel ic) in
  let main = fst (List.hd rules) in
  let fset = Ling.first_set rules (Syntax.symbol_nont main) in
  printf "first set of %s:\n" main;
  Ling.FstSet.iter
    (fun el -> printf "- %s\n" (Ling.Fst.string_of el))
    fset
