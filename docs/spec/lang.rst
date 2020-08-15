#########################
プログラミング言語 Kceage
#########################

********
基本事項
********


****
型
****


基本型
======

* *fundamental-type*

  + :code:`Bool`
  + :code:`i32`
  + :code:`u8`
  + :code:`Unit`

関数ポインター型
================

* *function-type*

  + :code:`Fr`
    :code:`(`
    *type-list*\ :sub:`opt` \
    :code:`)`
    :code:`->`
    *type*


****
式
****


二項演算
========

構文
------

* *binary-operator-seq*

  + *primary-expression*

  + *primary-expression* :code:`+` *binary-operator-seq*

  + *primary-expression* :code:`-` *binary-operator-seq*

  + *primary-expression* :code:`*` *binary-operator-seq*

  + *primary-expression* :code:`/` *binary-operator-seq*

  + *primary-expression* :code:`=` *binary-operator-seq*

  + *primary-expression* :code:`<` *binary-operator-seq*

  + *primary-expression* :code:`>` *binary-operator-seq*

演算子の優先順位
----------------

if 式
========

構文
------

* *if-expression*

  + :code:`If` *expression*
    :code:`Then` *expression*
    :code:`Else` *expression*


let 式
========

構文
------

* *let-expression*

  + :code:`Let` *variable* :code:`=` *expression*


関数呼び出し
===========

構文
------

* *function-call*

  + *postfix-expression*
    :code:`(`
    *argument-expression-list*\ :sub:`opt` \
    :code:`)`


ブロック式
==========

* *block-expression*

  + :code:`{`
    *expression-seq*
    :code:`}`

* *expression-seq*

  + *expression*

  + *expression*
    :code:`;`
    *expression-seq*
