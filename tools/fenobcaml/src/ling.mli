type first_set_elem = Epsilon | Term of Syntax.terminal

module Fse :
sig
  type t = first_set_elem
  val compare : 'a -> 'a -> int
  val string_of : first_set_elem -> string
end

module FstSet :
sig
  type elt = Fse.t
  type t
  val mem : elt -> t -> bool
  val iter : (elt -> unit) -> t -> unit
end

val first_set : (Syntax.nonterminal * Syntax.expr) list -> Syntax.expr -> FstSet.t
