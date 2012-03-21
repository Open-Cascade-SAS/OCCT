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

# first , Opening Document write on Unix 
puts "First , Opening Document write on Unix "
set Data [file join $env(CASROOT) src TCAF]

set DocName [file join ${Data} FileUnix.std]
if [ file exists ${DocName}  ] {
    
    Open ${DocName} Unix
    DumpDocument Unix
} else {
    puts "  ${DocName} does not exist "
}

# first , Opening Document write on WindowsNT
puts ""
puts "now , Opening Document write on WindowsNT "
puts ""

set DocName [file join ${Data} FileWNT.std]
puts "trying to Open : ${DocName}  "
if  [ file exists  ${DocName} ] {
    catch {  Open ${DocName} WNT } filesta
    if { ${filesta} == "" } { 
	DumpDocument WNT
    } else {
	puts " Problem when trying to read ${DocName}  ==> $filesta "
    }
} else {
    puts "  ${DocName} does not exist "
}
puts "List the Document in the Session :"
ListDocuments

puts ""
puts "now , We create  new Document "
puts ""
NewDocument New MDTV-Standard
UndoLimit New 5
NewCommand New

Label   New  0:20
SetReal New  0:20 .12345


set newName /tmp/New.std
if { [info exists env(TEMP)] } {
    set newName $env(TEMP)/New.std
}
SaveAs New  ${newName}
puts " "
puts "List the Document in the Session :"
ListDocuments
puts " "
puts "We close it"
Close  New
puts "List the Document in the Session :"
ListDocuments

puts "Verify if the new Document is saved : ${newName} "
if   [file exists ${newName}] { 
   puts " ... exists : Yes "
   puts " ... size   : [file size ${newName}]"

   puts " "
   puts " We try to read it :"
   Open ${newName} ReadDoc
   DumpDocument ReadDoc
    puts "List the Document in the Session :"
    ListDocuments
 
}  else {
   puts " ... exists : No "
}
puts " "
puts " "
puts "end "
