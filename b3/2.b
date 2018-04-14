(include "b3/1.b")

(def i 10)
(def j 10)
(while (> i 0)
       (set j i)
       (while (> j 0)
	      (print j)
      	      (print " ")	
	      (set j (- j 2)))
      (print NL)
      (set i (- i 1)))

(print NL)

(print (* 2 231))
(def x* (fun (x) (fun (y) (* x y))))
(def 2* (x* 2))
(print  (2* 3))





	      
	      


