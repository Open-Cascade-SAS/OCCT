#!/bin/sh

FILES='
NCollection_Array1.hxx
NCollection_Array2.hxx
NCollection_List.hxx
NCollection_Map.hxx
NCollection_DataMap.hxx
NCollection_DoubleMap.hxx
NCollection_IndexedMap.hxx
NCollection_IndexedDataMap.hxx
NCollection_Queue.hxx
NCollection_SList.hxx
NCollection_Sequence.hxx
NCollection_Set.hxx
NCollection_Stack.hxx
NCollection_Vector.hxx
NCollection_BaseCollection.hxx
NCollection_TListIterator.hxx
NCollection_TListNode.hxx
NCollection_HArray1.hxx
NCollection_HArray2.hxx
NCollection_HSequence.hxx
NCollection_HSet.hxx'

#FILES=NCollection_Array2.hxx

for IN_FILE in $FILES; do
### for IN_FILE in $*; do
OUT_FILE=`echo $IN_FILE | sed 's/_/_Define/'` 
gawk '

# ------ Function: Print the backslash in the 79th position
    function PrintBSL(str) {
        str0 = "                                                                                "
        len = length (str)
        if (len > 78) $0 = str " \\"
        else $0 = str substr(str0, 1, 79-len) "\\"
    }
# -------------- Check and corect the file header -----------
/\/\/ *Copyright *:/ {
    print "//            Automatically created from " in_file " by GAWK"
    }
/\/\/ *File *:/ ||
/#(ifndef|define) *NCollection_/ {
    gsub ("NCollection_","NCollection_Define")
    }

# --------------- Modify the include statement for used templates ------
/^ *\#include <NCollection_BaseCollection.hxx>/ {
        print "\#include <NCollection_DefineBaseCollection.hxx>"
        next
    }
/^ *\#include <NCollection_T[A-Za-z]*\.hxx>/ {
    gsub ("NCollection_T","NCollection_DefineT")
    }
/^ *\#include <NCollection_Array[12]\.hxx>/ {
    gsub ("NCollection_Array","NCollection_DefineArray")
    }
/^ *\#include <NCollection_Sequence\.hxx>/ {
    gsub ("NCollection_Sequence","NCollection_DefineSequence")
    }
/^ *\#include <NCollection_Set\.hxx>/ {
    gsub ("NCollection_Set","NCollection_DefineSet")
    }

# -------------- Replace line #define NCOLLECTION_H... -----------------
/^ *#define NCOLLECTION_H.*\(.*\).*$/ {
      defh = $0
      do getline defcl; while (defcl ~ /^ *\\/)
      coll_type = gensub ("^.*public *NCollection_\([A-Za-z0-9_]*\) *<Type>.*$",
			  "\\1", 1, defcl)
      gsub (", *Type\\)",", _" coll_type "Type_\)", defh)
      gsub (" *\\\\$","", defh)
      PrintBSL(gensub ("NCOLLECTION_","DEFINE_", 1, defh))
      print
      PrintBSL("")
      print
      gsub ("NCollection_" coll_type " *<Type>", "_" coll_type "Type_", defcl)
      PrintBSL(gensub ("^\(.*\) *\\\\$","\\1", 1, defcl))
      print
      flag = 10
      next
    }

# -------------- Replace the line "\#define ...." for "template ...." --
/^ *template *< *class/,/^ *{ *$/ {
    if (flag == 0) {
        type = gensub ("^ *template.*NCollection_\([A-Za-z0-9]*\).*$","\\1","g")
        item_t = gensub("^ *template *< *class *\([A-Za-z0-9_]*\) *\(,|>\).*$",\
                         "\\1",1)
        item_tt= gensub("^ *template.*, *class *\([A-Za-z0-9_]*\) *\(,|>\).*$",\
                         "\\1",1)
        if (item_tt == $0) item_tt = ""
        if (type == "BaseCollection")
            PrintBSL("#define DEFINE_" toupper(type) "(_ClassName_, "item_t")")
        else if (item_tt == "")
            PrintBSL("#define DEFINE_" toupper(type) \
                     "(_ClassName_, _BaseCollection_, " item_t ")")
        else
            PrintBSL("#define DEFINE_" toupper(type) \
                     "(_ClassName_, _BaseCollection_, " item_t ", " item_tt ")")
        # Special addition for the Iterator
        if (type == "TListIterator")
        {
          print
          PrintBSL("DEFINE_TLISTNODE(TListNode, _BaseCollection_, " item_t ")")
        }
        flag = 1
    } else if (flag == 1) {
        gsub ("^ *", "class _ClassName_ ") 
        flag = 2
    }
    }

# --------------- Detect the final line of the template class definition ----
/^}; *$/{ flag = 0 }

# --------------- Replace C-style comments for C++-style ones ---------------
/\/\//  { if (flag == 2) $0=gensub ("// *\(.*\) *$","/\* \\1 \*/", "g") }

# --------------- Replace "Raise_if.." for "#ifdef DEB ..::Raise.. #endif" --
/^\# *ifdef *DEB/ {
    nline = 0
    if (flag == 0) {
	while (1) {
	    getline debl
	    if (debl ~ /^\# *endif/) break;
	    print debl
	}
    } else {
        getline debl;
        if (debl !~ /^ *if *\(/) {
	    print "ERROR: in " in_file " cannot find if after \#ifdef DEB" \
		> "/dev/stderr"
	    exit 1
	}
	if (debl ~ /^ *if *\(.*\) *$/)
	    condl = gensub ("^ *if *\\( *(.*) *\\) *$", "\\1", 1, debl)
	else
	    condl = gensub ("^ *if *\\( *(.*) *$", "\\1", 1, debl)
	while(1) {
	    getline debl;
	    if (debl ~ /^\# *endif/) break;
	    if (debl !~ /Standard.*::Raise/)
		condl = gensub("^ *(.*[^\\)]) *($|\\) *$)", condl "\\1", 1, debl)
	    else {
		PrintBSL(gensub ("^  \(.*\):: *Raise *\\(.*$",
			         "\\1_Raise_if", 1, debl))
		print
		PrintBSL(gensub ("^  ( *)Standard.*Raise *\\( *\"NCollection_" \
				    type "(.*)\" *\\) *\; *$",
			         "\\1  (" condl ", #_ClassName_ \"\\2\")\;",
				 1, debl))
		print
	    }
	}
    }
    next
}

# --------------- Remove #pragma warning (default:4291) from the output -----
/^\# *ifdef WNT/ {
    defh = $0
    has_printed = 0
    while (1) {
	getline defcl
	if (defcl ~ /^[ \t]*$/) continue;
	if (defcl ~ /^\# *pragma +warning *\(default *: *4291 *\)/) continue;
	if (defcl ~ /^\# *endif/) {
	    if (has_printed) print defcl
	    break
	}
	if (has_printed == 0) {
	    has_printed = 1
	    print defh
	}
	print defcl
    }
    next
}

# --------------- Replace "DEFINE_T.*..." for typedef ... -------------------
#                 in classes List, Queue, Set, Stack
/^ *typedef NCollection_T[A-Za-z]*<TheItemType>/ {
      if (flag > 1) {
        tsup = gensub("^ *typedef.*NCollection_\([A-Za-z0-9]*\).*$","\\1",1)
        tdef = gensub("^ *typedef.*<[A-Za-z0-9]*> *([A-Za-z0-9]*)\;.*$","\\1",1)
        gsub ("typedef.*$", "DEFINE_" toupper(tsup) "(" tdef \
              ",_BaseCollection_," item_t ")")
      }
    }

# --------------- Replace public T* with DEFINE... --------------------------
/^ *class.*: *public *NCollection_T[A-Za-z]*<.*>/ {
      if (flag > 1) {
        ttempl = gensub("^ *class.*: *public *(NCollection_T[A-Za-z0-9]*<.*>).*$","\\1",1)
        tshort = gensub("^ *class.*: *public *NCollection_(T[A-Za-z0-9]*)<.*$","\\1",1)
        spub = gensub("(^ *class.*: *public *)NCollection_(T[A-Za-z0-9]*)<.*($)",
                      "\\1\\2\\3",1)
        tpara = gensub("^.*<(.*)>","\\1",1)
        PrintBSL("  DEFINE_" toupper(tshort) "(" tshort ",_BaseCollection_," tpara ")")
        print
        PrintBSL(spub)
        print
        next
      }
    }

# ------- General case: replace the macro parameter for class name and print -
        { if (flag == 10) {
	    nsub = gsub ("NCollection_" coll_type " *<Type>",
		         "_" coll_type "Type_")
	    if (nsub) {
		gsub (" *\\\\$","")
		PrintBSL($0)
	    }
	  } else if (flag > 1) {
            gsub ("NCollection_BaseCollection *< *\("item_t"|"item_tt"\) *>", \
                  "_BaseCollection_")
            gsub ("NCollection_TListNode *< *\("item_t"|"item_tt"\) *>", \
                  "TListNode")
            gsub ("NCollection_TListIterator *< *\("item_t"|"item_tt"\) *>", \
                  "TListIterator")
            gsub (ttempl, tshort)
	    gsub (":: *Raise *\\( *\"NCollection_" type,
		  "::Raise (#_ClassName_ \"")
            PrintBSL(gensub ("NCollection_" type \
                             "\(<"item_t">|<"item_tt">|\)\(\[^A-Za-z\]|$\)", \
                             "_ClassName_\\2","g"))
        }
        print
        }

' in_file=$IN_FILE out_file=$OUT_FILE $IN_FILE > $OUT_FILE
echo "\t$IN_FILE converted to $OUT_FILE"
done
