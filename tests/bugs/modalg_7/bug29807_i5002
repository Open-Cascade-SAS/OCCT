puts "========"
puts "0029807: Impossible to cut cone from prism"
puts "========"
puts ""

foreach a [directory res*] {unset $a}

cone s1 11.4307383137554 3.49999999999979 -89.7537975119388 0 0 1 1 0 0 80.000725670142287835190342147806 9.45659107381736
cone s2 -3.6479413426839 -11.578679656441 -89.9782110643133 0 0 1 0 1 0 5 0.250951325477062

intersect res s1 s2 1.0e-4

set che [whatis res]
set ind [string first "3d curve" $che]
if {${ind} >= 0} {
  #Only variable "res" exists
  renamevar res res_1
}

bclearobjects
bcleartools

set ic 1
set AllowRepeat 1
while { $AllowRepeat != 0 } {
  set che [whatis res_$ic]
  set ind [string first "3d curve" $che]
  if {${ind} < 0} {
    set AllowRepeat 0
  } else {
   
    bounds res_$ic U1 U2
    
    if {[dval U2-U1] < 1.0e-20} {
      puts "Error: Wrong curve's range!"
    }
    
    xdistcs res_$ic s1 U1 U2 100 2.0e-7
    xdistcs res_$ic s2 U1 U2 100 2.0e-7
    
    mkedge ee res_$ic
    baddobjects ee
    incr ic
  }
}

bfillds
bbuild result

smallview
don result*
fit

# Check gaps between edges in ce
checksection result -r 0
checkmaxtol result -min_tol 2.0e-7

checknbshapes result -edge 3 -vertex 3

checkview -screenshot -2d -path ${imagedir}/${test_image}.png
