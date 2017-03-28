// Copyright (c) 2015 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <StdObject_Location.hxx>
#include <StdPersistent_TopLoc.hxx>

//=======================================================================
//function : Translate
//purpose  : Creates a persistent wrapper object for a location
//=======================================================================
StdObject_Location 
StdObject_Location::Translate (const TopLoc_Location& theLoc,
                               StdObjMgt_TransientPersistentMap& theMap)
{
  StdObject_Location aLoc;
  if (!theLoc.IsIdentity())
    aLoc.myData = StdPersistent_TopLoc::Translate(theLoc, theMap);
  return aLoc;
}

//=======================================================================
//function : Location
//purpose  : Changes current location
//=======================================================================
void StdObject_Location::PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  theChildren.Append(myData);
}

//=======================================================================
//function : Import
//purpose  : Import transient object from the persistent data
//=======================================================================
TopLoc_Location StdObject_Location::Import() const
{
  Handle(StdPersistent_TopLoc::ItemLocation) anItemLocation =
    Handle(StdPersistent_TopLoc::ItemLocation)::DownCast (myData);
  return anItemLocation ? anItemLocation->Import() : TopLoc_Location();
}
