(def and (macro (a b) (if (eval a) (if (eval b) T))))

(def eval-each-return-value NIL)
(def eval-each (fun (list)
     		    (if list
		    	(set eval-each-return-value (eval (first list))))
		     (if (rest list)
		         (eval-each (rest list))
			 eval-each-return-value)))

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

(def reverse (fun (list)
     	     	  (let result ()
		       (while list
		       	      (set result (push (first list) result))
			      (set list (rest list)))
		       result)))

(def list (macro (:rest a)
     	  (let result ()
	          (while a
		  	 (set result (push (eval (first a)) result))
			 (set a (rest a)))
	          (set result (reverse result))
  		  (push (quote quote) (push (push (quote quote) (push result ())) () )))))

(print  (list 1 2  3))


(def < (fun (a b) (if (> a b) NIL T)))

(def * (fun (a b)
	    (if (= b 1)
		a
		(+ a (* a (- b 1))))))
				
		       
