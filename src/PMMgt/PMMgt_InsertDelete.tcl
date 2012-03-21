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

proc PMMgt_InsertDelete:AdmFileType {} {
    return "dbadmfile";
}

proc PMMgt_InsertDelete:OutputDirTypeName {} {
    return "dbtmpfile";
}


proc PMMgt_InsertDelete:HandleInputFile { ID } { 

    scan $ID "%\[^:\]:%\[^:\]:%\[^:\]"  unit type name

    switch $name {
	Handle_PMMgt_PManaged.hxx  {return 1;} 
	default {
	    return 0;
	}
    }
}
proc PMMgt_InsertDelete:AddDelete {name func} {
    set status 0
    set dbmsprof [wokprofile -b]
    if {$dbmsprof != "DFLT"} {
	set status [catch { exec grep  -s "^$func" $name >& /dev/null}]
	if {$status != 0} { 
	    set status [catch {exec sed -e "s?public:?$func?" <$name >${name}.new}]
	    msgprint -i -c "PMMgt_InsertDelete:Execute" "mv ${name}.new  $name"
	    set status [catch {exec mv ${name}.new  $name}]
	    if {$status == 0} {
		set status [catch {exec chmod 664 $name}]
	    }
	}
    }

    return $status
}

proc PMMgt_InsertDelete:Execute { unit args } {
    
    msgprint -i -c "PMMgt_InsertDelete:Execute" "Insert virtual void delete() in Handle"
    set dbmsprof [wokprofile -b]

    foreach file  $args {
	scan $file "%\[^:\]:%\[^:\]:%\[^:\]"  Unit type name

	if {$name == "Handle_PMMgt_PManaged.hxx"} {
	    set vistarget [woklocate -p PMMgt:pubinclude:$name [wokinfo -N $unit]]
	    set target    [wokinfo -p pubinclude:$name $unit]
	    set status [PMMgt_InsertDelete:AddDelete $vistarget "public: Standard_EXPORT void Delete();"]
	}
    }

    return 0;
}
