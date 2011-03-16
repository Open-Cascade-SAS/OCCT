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
