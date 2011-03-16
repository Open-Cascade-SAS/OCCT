;#
;# Retourne la liste ordonnee des noms de Modules
;# 
proc OS:Modules { {plat ""} } {
    set ret [list FoundationClasses \
	   ModelingData \
	   ModelingAlgorithms \
	   Visualization \
	   ApplicationFramework \
	   DataExchange \
	   Draw \
	   ]
    return $ret
}
