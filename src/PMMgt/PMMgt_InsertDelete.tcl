# Copyright (c) 1999-2014 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

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
