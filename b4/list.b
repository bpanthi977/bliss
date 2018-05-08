(def list (macro (:rest a)
     	  (push (quote quote) (push a ()))))

(print (list 1 2 3 4 5))
