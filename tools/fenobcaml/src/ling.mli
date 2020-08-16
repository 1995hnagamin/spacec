module Fse :
sig
  type t = Epsilon | Term of Syntax.terminal
  val compare : 'a -> 'a -> int
  val string_of : t -> string
end

module FstSet :
sig
  type elt = Fse.t
  type t
  val mem : elt -> t -> bool
  val iter : (elt -> unit) -> t -> unit
end

type grammar = (Syntax.nonterminal * Syntax.expr) list

val first_set : grammar -> Syntax.expr -> FstSet.t
