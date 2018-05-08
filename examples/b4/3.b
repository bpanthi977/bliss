(def m1 (macro (n2 nn)
       	      (print n2)
	      (print nn)))

(def m2 (macro (n2 nn)
     	       (m1 nn n2 )))

(m1 n 2)
(print NL)
(m2 n 2)
