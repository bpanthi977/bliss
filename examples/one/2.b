(def n 1)
(print  n T)

; and is defined as a function
(def and (fun (a b) (if a
			 (if b
			     1
			     NIL)
			 NIL)))
(def change-n (fun ()  (def n 2)))

(if (and NIL (change-n)) "good" "bad")
(print n)
; but clearly this is not a good definition of and
; because the second parameter is unnecessarily evaluated
; and the value of n has changed
; See last section of one/1.b for proper definition of and as macro


;
; Function closure in action: 
(def x* (fun (x) (fun (y) (* x y))))
(def 2* (x* 2))
(print  (2* 3))


