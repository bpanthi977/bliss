(def n 1)
n
NIL
(if NIL
    2
    3)
(def and (func (a b) (if a
			 (if b
			     1
			     NIL)
			 NIL)))
(def change-n (func ()  (def n 2)))

(if (and NIL (change-n)) "good" "bad")
n

	      
