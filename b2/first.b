1
(def q (func (x) x))
(def ff (func (x) (func () 1)))
(q "se")
(def p (ff q))
((ff q))

