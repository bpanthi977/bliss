;
; Basics of Bliss
;

; Comments start with ; character

; Every thing in Bliss is either an atom or a list
; Atom is indivisible. It may be:
; symbol, function, macro, integer, string, ...

(print (list 1 2 3))

; Above code is a list with first element a symbol and second element another
; list. The first element of the second list is yet another symbol and rest other
; are integers

; ("abcd" 12 efg)
; Above list contains a string, a integer and a symbol

; Symbols represent variables, macros, or functions
; e.g. print is a function. i.e. print symbol represents a function

; Lets print a string
(print "Hello World")
; You can pass second argument NIL to supress double quotes while printing strings
(print "

Quoteless string

" NIL)
; and you can pass T as second argument to append newline
(print 1 T)

; Lets create a variable and print it
(def var1 123)
(print var1) 
; def is used to create a global variable and assign values to it


; There are few predefined variables: NIL represents false
; T represents true
; NL represents newline
(print NL NIL)

;
; Arithmetics
;
; Only integers are supported with following operations:
; + - * / > = 
(print (+ 1 1) T)
(print (* 12 33) T)
(print (/ 5 (+ 2 3)) T)
(print (= 5 5) T)
(print (> 3 6))


;
; IF 
; (if condition expression-if-true expression-if-false)
(if (> 4 2)
    (print "true")
    (print "false"))


;
; Create functions
; (fun (function-argument-list) function-body-expressions)
(def square (fun (x)
     	    	 (* x x)))
		 
(print (square 12) T)

;
; Create macros 
; Macros are similar to functions. The difference is that they recieve their
; arguments unevaluated and they cannot form closures.
;

; Macros allow many interesting things.
; E.g. Bliss doesn't have a and operator (similar to C's &)
; but we can just create it using macro

(def and (macro (a b)
     	 	(if (eval a)
		    (if (eval b)
		    	T
			NIL)
		    NIL)))

(print (if (and (= 4 4) (> 4 7))
       	   "true"
	   "false")
	T)




;
; Function closure in action: 
(def x* (fun (x) (fun (y) (* x y))))
(def 2* (x* 2))
(print  (2* 3))
