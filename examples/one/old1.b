(def and (macro (a b) (if (eval a) (if (eval b) T))))

(def while (macro (cond :rest body)
		  (let ((res (fun ()
				(eval-each body)
				(if (eval cond)
				    (res)))))
		       (if (eval cond)
			   (res)))))

(def reverse (fun (list)
     	     	  (let ((result ()))
		       (while list
		       	      (set result (push (first list) result))
			      (set list (rest list)))
		       result)))

; Definition of list as a macro
; NOTE: list function is already provided by bliss but
; this implemention shows how powerful macros can be
; this worked in previous version where macro's return
; value were evaluated
; (def list (macro (:rest a)
;       (let ((result ()))
; 	          (while a
; 		  	 (set result (push (eval (first a)) result))
; 			 (set a (rest a)))
; 	          (set result (reverse result))
;   		  (push (quote quote) (push (push (quote quote) (push result ())) () )))))



; This implemention of let is also worth examining
; although variable argument name for functions is not
; provided in this version of Bliss, it can also be implemented
; in this version of Bliss (though it will be abit longer)
;(def let (macro (var val :rest letbody)
;               ((fun ((eval (quote var)))      
;                     (eval-each letbody))
;                (eval val))))


(print  (list 1 2  3))


(def < (fun (a b) (if (> a b) NIL T)))

				
		       
