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

val first_set : (Syntax.nonterminal * Syntax.expr) list -> Syntax.expr -> FstSet.t
