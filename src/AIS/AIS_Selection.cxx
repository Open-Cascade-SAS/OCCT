// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <AIS_Selection.hxx>

#include <AIS_InteractiveObject.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Selection, Standard_Transient)

namespace
{
  static const Standard_Integer THE_MaxSizeOfResult = 100000;
}

//=======================================================================
//function : AIS_Selection
//purpose  :
//=======================================================================
AIS_Selection::AIS_Selection()
{
  // for maximum performance on medium selections (< 100000 objects)
  myResultMap.ReSize (THE_MaxSizeOfResult);
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void AIS_Selection::Clear()
{
  myresult.Clear();
  myResultMap.Clear();
}

//=======================================================================
//function : Select
//purpose  :
//=======================================================================
AIS_SelectStatus AIS_Selection::Select (const Handle(SelectMgr_EntityOwner)& theObject)
{
  if (theObject.IsNull()
  || !theObject->HasSelectable())
  {
    return AIS_SS_NotDone;
  }

  if (!myResultMap.IsBound (theObject))
  {
    AIS_NListOfEntityOwner::Iterator aListIter;
    myresult.Append  (theObject, aListIter);
    myResultMap.Bind (theObject, aListIter);
    return AIS_SS_Added;
  }

  AIS_NListOfEntityOwner::Iterator aListIter = myResultMap.Find (theObject);
  if (myIterator == aListIter)
  {
    if (myIterator.More())
    {
      myIterator.Next();
    }
    else
    {
      myIterator = AIS_NListOfEntityOwner::Iterator();
    }
  }

  // In the mode of advanced mesh selection only one owner is created for all selection modes.
  // It is necessary to check the current detected entity
  // and remove the owner from map only if the detected entity is the same as previous selected (IsForcedHilight call)
  if (theObject->IsForcedHilight())
  {
    return AIS_SS_Added;
  }

  myresult.Remove (aListIter);
  myResultMap.UnBind (theObject);

  // update list iterator for next object in <myresult> list if any
  if (aListIter.More())
  {
    const Handle(SelectMgr_EntityOwner)& aNextObject = aListIter.Value();
    if (myResultMap.IsBound (aNextObject))
    {
      myResultMap (aNextObject) = aListIter;
    }
    else
    {
      myResultMap.Bind (aNextObject, aListIter);
    }
  }
  return AIS_SS_Removed;
}

//=======================================================================
//function : AddSelect
//purpose  :
//=======================================================================
AIS_SelectStatus AIS_Selection::AddSelect (const Handle(SelectMgr_EntityOwner)& theObject)
{
  if (theObject.IsNull()
  || !theObject->HasSelectable()
  ||  myResultMap.IsBound (theObject))
  {
    return AIS_SS_NotDone;
  }

  AIS_NListOfEntityOwner::Iterator aListIter;
  myresult.Append  (theObject, aListIter);
  myResultMap.Bind (theObject, aListIter);
  return AIS_SS_Added;
}
