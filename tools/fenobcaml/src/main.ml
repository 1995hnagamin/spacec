let () =
  let filename = Sys.argv.(1) in
  let ic = open_in filename in
  let program = Lexing.from_channel ic in
  Printf.printf "Hello world\n"
