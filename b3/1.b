(def and (macro (a b) (if (eval a) (if (eval b) T))))

(def eval-each (fun (list)
		    (if list
			(eval (first list)))
		    (if list 
			(eval-each (rest list)))))

(def let (macro (var val :rest letbody)
		((fun ((eval (quote var)))
		      (eval-each letbody))
		 (eval val))))

(def while (macro (cond :rest body)
		  (let res (fun ()
				(eval-each body)
				(if (eval cond)
				    (res)))
		       (if (eval cond)
			   (res)))))


(def < (fun (a b) (if (> a b) NIL T)))

(def * (fun (a b)
	    (if (= b 1)
		a
		(+ a (* a (- b 1))))))
				
		       
