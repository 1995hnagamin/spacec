module S = Syntax

type grammar = (Syntax.nonterminal * Syntax.expr) list

module Fst = struct
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

module FstSet = Set.Make(Fst)

let first_set grammar exp =
  let rec iter = function
    | S.Epsilon -> FstSet.singleton Epsilon
    | S.Symbol(S.Term c) -> FstSet.singleton (Term c)
    | S.Symbol(S.Nont s) ->
      let rhss =
        rev_filter_map
          (fun (n, rhs) -> if n = s then Some(rhs) else None)
          grammar
      in
      List.fold_left
        (fun f r -> FstSet.union f (iter r))
        FstSet.empty rhss
    | S.Concat(a, b) ->
      let aset = iter a in
      if FstSet.mem Epsilon aset
      then FstSet.union (FstSet.remove Epsilon aset) (iter b)
      else aset
    | S.Union(a, b) -> FstSet.union (iter a) (iter b)
    | S.Option(a) -> FstSet.add Epsilon (iter a)
  in
  iter exp

let first_set_table tbl grammar exp =
  let rec iter = function
    | S.Epsilon -> FstSet.singleton Epsilon
    | S.Symbol(S.Term c) -> FstSet.singleton (Term c)
    | S.Symbol(S.Nont s) -> begin
        match Hashtbl.find_opt tbl s with
        | Some(set) -> set
        | None ->
          let rhss =
            rev_filter_map
              (fun (n, rhs) -> if n = s then Some(rhs) else None)
              grammar
          in
          let set =
            List.fold_left
              (fun f r -> FstSet.union f (iter r))
              FstSet.empty rhss
          in
          Hashtbl.add tbl s set;
          set
      end
    | S.Concat(a, b) ->
      let aset = iter a in
      if FstSet.mem Epsilon aset
      then FstSet.union (FstSet.remove Epsilon aset) (iter b)
      else aset
    | S.Union(a, b) -> FstSet.union (iter a) (iter b)
    | S.Option(a) -> FstSet.add Epsilon (iter a)
  in
  iter exp

module Follow = struct
  type t = Eos | Term of Syntax.terminal
  let compare = compare
  let string_of = function
    | Eos -> "$"
    | Term(c) -> Printf.sprintf "'%c'" c
end

module FollowSet = Set.Make(Follow)

module StrSet = Set.Make(String)

let rec add_nonterminal exp set =
  match exp with
  | S.Epsilon -> set
  | S.Symbol(S.Term c) -> set
  | S.Symbol(S.Nont s) -> StrSet.add s set
  | S.Concat(a, b) -> add_nonterminal a (add_nonterminal b set)
  | S.Union(a, b) -> add_nonterminal a (add_nonterminal b set)
  | S.Option(a) -> add_nonterminal a set

let nont_set grammar =
  List.fold_left
    (fun set (nA, rhs) -> add_nonterminal rhs (StrSet.add nA set))
    StrSet.empty grammar

let create_dep_graph grammar start =
  let graph = Hashtbl.create (List.length grammar) in
  let fstab = Hashtbl.create (List.length grammar) in
  let read_rule nA rhs =
    let rec iter = function
      | S.Concat(S.Symbol(S.Nont nB), beta) ->
        let b1st = first_set_table fstab grammar beta in
        (if FstSet.mem Fst.Epsilon b1st then Hashtbl.add graph nA nB);
        iter beta
      | S.Concat(_, beta) -> iter beta
      | S.Union(a, b) -> iter a; iter b
      | _ -> ()
    in
    iter rhs
  in
  List.iter (fun (nA, rhs) -> read_rule nA rhs) grammar;
  (fstab, graph)

let follow_set_table grammar start =
  let (fstab, graph) = create_dep_graph grammar start in
  StrSet.iter
    (fun nA ->
       let set = first_set_table fstab grammar (S.symbol_nont nA) in
       Hashtbl.add fstab nA set)
    (nont_set grammar);
  fstab
