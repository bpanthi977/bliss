(def f (fun (a)
       	    (fun (b)
	    	 (+ a b))))

(print (f 3))
(print ((f 3) 2))


