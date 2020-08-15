let () =
  let filename = Sys.argv.(1) in
  let ic = open_in filename in
  let rules = Parser.main Lexer.main (Lexing.from_channel ic) in
  List.iter
    (fun (_, e) -> Printf.printf "%s\n" (Syntax.string_of_expr e))
    rules
