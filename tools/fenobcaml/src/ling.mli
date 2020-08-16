module Fst :
sig
  type t = Epsilon | Term of Syntax.terminal
  val compare : 'a -> 'a -> int
  val string_of : t -> string
end

module FstSet :
sig
  type elt = Fst.t
  type t
  val mem : elt -> t -> bool
  val iter : (elt -> unit) -> t -> unit
end

type grammar = (Syntax.nonterminal * Syntax.expr) list

val first_set : grammar -> Syntax.expr -> FstSet.t

module Follow :
sig
  type t = Eos | Term of Syntax.terminal
  val string_of : t -> string
end

module FollowSet :
sig
  type elt = Follow.t
  type t
  val mem : elt -> t -> bool
  val iter : (elt -> unit) -> t -> unit
end

val follow_set_table : grammar -> string -> (string, FstSet.t) Hashtbl.t
