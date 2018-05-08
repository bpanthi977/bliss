(def qq (macro (a rest) (print (eval a)) (print rest)))

(qq 1 (1 2 3))

()
