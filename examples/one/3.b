;
; This file contains some frequently used
; functions and macros
;

;
; Now it feels interesting to see the language constructs/features 
; written in the language itself :)
;



;
; Boolean Operators
;
(def and (macro (a b) (if (eval a) (if (eval b) T))))
(def or (macro (a b) (if (eval a) T (if (eval b) T))))

;
; Looping Construct
;

; while loop
(def while (macro (cond :rest body)
		  (let ((res (fun ()
				(eval-each body)
				(if (eval cond)
				    (res)))))
		       (if (eval cond)
			   (res)))))


;
; List Manipulation
;

; nth element of a list
(def nth (fun (n list)
     	      (if (= n 0)
	      	  (first list)
		  (nth (- n 1) (rest list)))))

; reverses a list
(def reverse (fun (list)
     	     	  (let ((result ()))
		       (while list
		       	      (set result (push (first list) result))
			      (set list (rest list)))
		       result)))
