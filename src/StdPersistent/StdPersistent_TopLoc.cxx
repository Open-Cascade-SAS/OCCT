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
#include <StdObjMgt_WriteData.hxx>
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

void StdPersistent_TopLoc::Datum3D::Write(StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myTransient->Transformation();
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
//function : Write
//purpose  : Write persistent data to a file
//=======================================================================
void StdPersistent_TopLoc::ItemLocation::Write (StdObjMgt_WriteData& theWriteData) const
{
  theWriteData << myDatum << myPower << myNext;
}

//=======================================================================
//function : PChildren
//purpose  : Gets persistent child objects
//=======================================================================
void StdPersistent_TopLoc::ItemLocation::PChildren
  (StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
{
  theChildren.Append(myDatum);
  myNext.PChildren(theChildren);
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

//=======================================================================
//function : Translate
//purpose  : Create a persistent object from a location
//=======================================================================
Handle(StdPersistent_TopLoc::ItemLocation) 
StdPersistent_TopLoc::Translate (const TopLoc_Location& theLoc,
                                 StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(ItemLocation) aPLoc = new ItemLocation;
  aPLoc->myDatum = Translate(theLoc.FirstDatum(), theMap);
  aPLoc->myPower = theLoc.FirstPower();
  aPLoc->myNext = StdObject_Location::Translate(theLoc.NextLocation(), theMap);
  return aPLoc;
}

//=======================================================================
//function : Translate
//purpose  : Create a persistent object from a location datum
//=======================================================================
Handle(StdPersistent_TopLoc::Datum3D) 
StdPersistent_TopLoc::Translate (const Handle(TopLoc_Datum3D)& theDatum,
                                 StdObjMgt_TransientPersistentMap& theMap)
{
  Handle(Datum3D) aPDatum;
  if (theMap.IsBound(theDatum))
  {
    aPDatum = Handle(Datum3D)::DownCast(theMap.Find(theDatum));
  }
  else
  {
    aPDatum = new Datum3D;
    aPDatum->Transient(theDatum);
    theMap.Bind(theDatum, aPDatum);
  }
  return aPDatum;
}
