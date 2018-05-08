(def n 1)
(print  n)
(print NL)
(def and (fun (a b) (if a
			 (if b
			     1
			     NIL)
			 NIL)))
(def change-n (fun ()  (def n 2)))
(print  (change-n))
(print (if 2
	   (if 2
	       1
	       NIL)
	   NIL))
(print NIL)
(if (and NIL (change-n)) "good" "bad")	      
