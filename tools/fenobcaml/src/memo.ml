module type XType = sig
  type kt
  type vt
  val make : kt -> vt
end

module Make (X : XType) = struct
  type t = (X.kt, X.vt) Hashtbl.t

  let get tbl key =
    try Hashtbl.find tbl key with
    | Not_found ->
      let v = X.make key in
      Hashtbl.add tbl key v; v
end
