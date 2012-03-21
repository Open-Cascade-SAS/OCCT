# Copyright (c) 1999-2012 OPEN CASCADE SAS
#
# The content of this file is subject to the Open CASCADE Technology Public
# License Version 6.5 (the "License"). You may not use the content of this file
# except in compliance with the License. Please obtain a copy of the License
# at http://www.opencascade.org and read it completely before using this file.
#
# The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
# main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
#
# The Original Code and all software distributed under the License is
# distributed on an "AS IS" basis, without warranty of any kind, and the
# Initial Developer hereby disclaims all such warranties, including without
# limitation, any warranties of merchantability, fitness for a particular
# purpose or non-infringement. Please see the License for the specific terms
# and conditions governing the rights and limitations under the License.

#
# this prints the image of the BOOLEAN GRIDS
#
proc print_test { test_letter grille_number } {
    append test_file $test_letter 2 ;  
    append image_directory "/home/wb/mdl/gti/work/GRILLE/" $grille_number
    set grille_directory "/adv_20/BAG/test/GRILLES-BOOLEAN/"
    append grille_directory $grille_number
    append grille_name $grille_number
    cd $grille_directory
    uplevel #0 source $test_file  ;  
    clear ; 
    smallview AXON
    uplevel #0 display b1 b2
    uplevel #0 fit;
    uplevel #0 clear
    uplevel #0 vprops b1 x y z
    uplevel #0 vprops b2 u v w
    dtext -0.1 -0.1  -0.2 $test_letter ;
    dtext -0.1 -0.1  -0.5 $grille_name
    dtext u v w  solid2
    dtext x y z  solid1
    uplevel #0 compound b1 b2 c
    uplevel #0 display c
    uplevel #0 hlr rgn c
    uplevel #0 hlr hid c
    
    if { [ file isdirectory $image_directory ] == 0 } {
	uplevel #0 mkdir $image_directory 
    }
    cd $image_directory
    hcolor 5 16 0
    hcolor 12 11 0
    hardcopy $test_letter.ps 
}

proc print_grille  { number } {
    foreach letter { A B C D E F G H I J K L M N O P Q R S T U V W X } {
	puts " *** test : $letter *** "
	print_test $letter $number 
    }
}
