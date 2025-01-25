// Created on: 1995-05-23
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
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

#include <SelectMgr_EntityOwner.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_EntityOwner, Standard_Transient)

//=================================================================================================

SelectMgr_EntityOwner::SelectMgr_EntityOwner(const Standard_Integer thePriority)
    : mySelectable(NULL),
      mypriority(thePriority),
      myIsSelected(Standard_False),
      myFromDecomposition(Standard_False)
{
  //
}

//=================================================================================================

SelectMgr_EntityOwner::SelectMgr_EntityOwner(const Handle(SelectMgr_SelectableObject)& theSelObj,
                                             const Standard_Integer                    thePriority)
    : mySelectable(theSelObj.get()),
      mypriority(thePriority),
      myIsSelected(Standard_False),
      myFromDecomposition(Standard_False)
{
  //
}

//=================================================================================================

SelectMgr_EntityOwner::SelectMgr_EntityOwner(const Handle(SelectMgr_EntityOwner)& theOwner,
                                             const Standard_Integer               thePriority)
    : mySelectable(theOwner->mySelectable),
      mypriority(thePriority),
      myIsSelected(Standard_False),
      myFromDecomposition(Standard_False)
{
  //
}

//=================================================================================================

void SelectMgr_EntityOwner::HilightWithColor(const Handle(PrsMgr_PresentationManager)& thePM,
                                             const Handle(Prs3d_Drawer)&               theStyle,
                                             const Standard_Integer                    theMode)
{
  if (mySelectable == NULL)
  {
    return;
  }

  if (IsAutoHilight())
  {
    const Graphic3d_ZLayerId aHiLayer = theStyle->ZLayer() != Graphic3d_ZLayerId_UNKNOWN
                                          ? theStyle->ZLayer()
                                          : mySelectable->ZLayer();
    thePM->Color(mySelectable, theStyle, theMode, NULL, aHiLayer);
  }
  else
  {
    mySelectable->HilightOwnerWithColor(thePM, theStyle, this);
  }
}

// =======================================================================
// function : Select
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_EntityOwner::Select(const AIS_SelectionScheme theSelScheme,
                                               const Standard_Boolean    theIsDetected) const
{
  switch (theSelScheme)
  {
    case AIS_SelectionScheme_UNKNOWN: {
      return myIsSelected;
    }
    case AIS_SelectionScheme_Replace: {
      return theIsDetected;
    }
    case AIS_SelectionScheme_Add: {
      return !myIsSelected || theIsDetected || IsForcedHilight();
    }
    case AIS_SelectionScheme_Remove: {
      return myIsSelected && !theIsDetected;
    }
    case AIS_SelectionScheme_XOR: {
      if (theIsDetected)
      {
        return !myIsSelected && !IsForcedHilight();
      }
      return myIsSelected;
    }
    case AIS_SelectionScheme_Clear: {
      return Standard_False;
    }
    case AIS_SelectionScheme_ReplaceExtra: {
      return theIsDetected;
    }
  }
  return Standard_False;
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void SelectMgr_EntityOwner::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, this)

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, mySelectable)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mypriority)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsSelected)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFromDecomposition)
}
