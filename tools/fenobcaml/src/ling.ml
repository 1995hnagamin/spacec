module S = Syntax

module Fse = struct
  type t = Epsilon | Term of Syntax.terminal
  let compare = compare
  let string_of = function
    | Epsilon -> "~"
    | Term(c) -> Printf.sprintf "'%c'" c
end

let rev_filter_map f xs =
  List.fold_left
    (fun ys x ->
       match f x with
       | Some(y) -> y ::ys
       | None -> ys)
    [] xs

module FstSet = Set.Make(Fse)

let first_set grammar exp =
  let rec iter = function
    | S.Epsilon -> FstSet.singleton Epsilon
    | S.Symbol(S.Term c) -> FstSet.singleton (Term c)
    | S.Symbol(S.Nont s) ->
      let rhss = rev_filter_map
                   (fun (n, rhs) -> if n = s then Some(rhs) else None)
                   grammar
      in
      List.fold_left (fun f r -> FstSet.union f (iter r)) FstSet.empty rhss
    | S.Concat(a, b) ->
      let aset = iter a in
      if FstSet.mem Epsilon aset
      then FstSet.union (FstSet.remove Epsilon aset) (iter b)
      else aset
    | S.Union(a, b) -> FstSet.union (iter a) (iter b)
    | S.Option(a) -> FstSet.add Epsilon (iter a)
  in
  iter exp
