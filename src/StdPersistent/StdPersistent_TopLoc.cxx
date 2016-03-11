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

#include <StdPersistent_TopLoc.hxx>
#include <StdObjMgt_ReadData.hxx>
#include <StdObject_gp_Trsfs.hxx>


//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void StdPersistent_TopLoc::Datum3D::Read (StdObjMgt_ReadData& theReadData)
{
  gp_Trsf aTrsf;
  theReadData >> aTrsf;
  myTransient = new TopLoc_Datum3D (aTrsf);
}

//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void StdPersistent_TopLoc::ItemLocation::Read (StdObjMgt_ReadData& theReadData)
{
  theReadData >> myDatum >> myPower >> myNext;
}

//=======================================================================
//function : Import
//purpose  : Import transient object from the persistent data
//=======================================================================
TopLoc_Location StdPersistent_TopLoc::ItemLocation::Import() const
{
  TopLoc_Location aNext = myNext.Import();
  if (myDatum)
    return aNext * TopLoc_Location (myDatum->Import()).Powered (myPower);
  else
    return aNext;
}
