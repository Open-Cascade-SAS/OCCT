// Created on: 2015-12-23
// Created by: Anastasia BORISOVA
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

#include <AIS_ManipulatorOwner.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_ManipulatorOwner,SelectMgr_EntityOwner)
//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_ManipulatorOwner::AIS_ManipulatorOwner (const Handle(SelectMgr_SelectableObject)& theSelObject,
                                            const Standard_Integer theIndex,
                                            const AIS_ManipulatorMode theMode,
                                            const Standard_Integer thePriority)
: SelectMgr_EntityOwner(theSelObject, thePriority),
  myIndex (theIndex),
  myMode (theMode)
{
  //
}

//=======================================================================
//function : HilightWithColor
//purpose  : 
//=======================================================================
void AIS_ManipulatorOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                             const Handle(Prs3d_Drawer)& theStyle,
                                             const Standard_Integer theMode)
{
  if (theMode == 0)
  {
    SelectMgr_EntityOwner::HilightWithColor (thePM, theStyle, theMode);
    return;
  }

  Selectable()->HilightOwnerWithColor (thePM, theStyle, this);
}

//=======================================================================
//function : IsHilighted
//purpose  : 
//=======================================================================
Standard_Boolean AIS_ManipulatorOwner::IsHilighted (const Handle(PrsMgr_PresentationManager)& thePM,
                                                    const Standard_Integer /*theMode*/) const
{
  if (!HasSelectable())
  {
    return Standard_False;
  }

  return thePM->IsHighlighted (Selectable(), myMode);
}

//=======================================================================
//function : Unhilight
//purpose  : 
//=======================================================================
void AIS_ManipulatorOwner::Unhilight (const Handle(PrsMgr_PresentationManager)& thePM,
                                      const Standard_Integer /*theMode*/)
{
  if (!HasSelectable())
  {
    return;
  }

  thePM->Unhighlight (Selectable());
}
