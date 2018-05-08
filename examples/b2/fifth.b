(def mm (macro (a b) (if (eval a) (if (eval b) 12 NIL) NIL)))
(def n 1)
(def change-n (func () (def n 2)))
(def nil-returning (func () NIL))

(mm (nil-returning) (change-n))
n
