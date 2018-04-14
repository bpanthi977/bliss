
(def eval-each (fun (list)
		    (if list
			(eval (first list)))
		    (if list 
			(eval-each (rest list)))))
(print NL)

(def x 5)

(def changex (fun () (def  x 6)))

(def and (fun (x y) (if x
			(if y
			    T
			    NIL)
			NIL)))

(def and (macro (a b) (if (eval a) (if (eval b) T))))


(def let (macro (var val :rest letbody)
		((fun ((eval (quote var)))
		      (eval-each letbody))
		 (eval val))))

(let a 5
     (print a))

(print a)

(def while (macro (cond :rest body)
		  (let res (fun ()
				(eval-each body)
				(if (eval cond)
				    (res)))
		       (if (eval cond)
			   (res)))))



(def * (fun (a b)
	    (if (= b 1)
		a
		(+ a (* a (- b 1))))))
(print NL)
(print (* 4 9999))
(print NL)

(while (> x 0)
       (print x)
       (set x (- x 1)))
(def testx (fun () (= x 1)))

(print  (if (and (testx) (changex)) "t" "f"))

(print x)
