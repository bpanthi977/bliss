(def add (fun (a b)
	      (+ a b)))

(def sub (macro (a b)
		(- a b)))


(print (add 5 4))
(print (sub 5 4))

