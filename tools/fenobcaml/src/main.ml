open Printf

let () =
  let filename = Sys.argv.(1) in
  let ic = open_in filename in
  let grammar = Parser.main Lexer.main (Lexing.from_channel ic) in
  let start = fst (List.hd grammar) in
  let fstab = Ling.follow_set_table grammar start in
  Hashtbl.iter
    (fun nA set ->
       printf "First Set of %s:\n" nA;
       Ling.FstSet.iter (fun e -> printf "- %s\n" (Ling.Fst.string_of e))
         set)
    fstab
