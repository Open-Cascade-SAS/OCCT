// Created by: Ilya SEVRIKOV
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <AIS_TrihedronOwner.hxx>

IMPLEMENT_STANDARD_RTTIEXT (AIS_TrihedronOwner, SelectMgr_EntityOwner)

// =======================================================================
// function : AIS_TrihedronOwner
// purpose  :
// =======================================================================
AIS_TrihedronOwner::AIS_TrihedronOwner (const Handle(SelectMgr_SelectableObject)& theSelObject,
                                        const Prs3d_DatumParts thePart,
                                        const Standard_Integer thePriority)
: SelectMgr_EntityOwner (theSelObject, thePriority),
  myDatumPart (thePart)
{
}

// =======================================================================
// function : HilightWithColor
// purpose  :
// =======================================================================
void AIS_TrihedronOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                           const Handle(Prs3d_Drawer)& theStyle,
                                           const Standard_Integer /*theMode*/)
{
  Selectable()->HilightOwnerWithColor (thePM, theStyle, this);
}

// =======================================================================
// function : IsHilighted
// purpose  :
// =======================================================================
Standard_Boolean AIS_TrihedronOwner::IsHilighted (const Handle(PrsMgr_PresentationManager)& thePM,
                                                  const Standard_Integer theMode) const
{
  if (!HasSelectable())
  {
    return Standard_False;
  }

  return thePM->IsHighlighted (Selectable(), theMode);
}

// =======================================================================
// function : Unhilight
// purpose  :
// =======================================================================
void AIS_TrihedronOwner::Unhilight (const Handle(PrsMgr_PresentationManager)& thePM,
                                    const Standard_Integer theMode)
{
  (void )theMode;
  if (!HasSelectable())
  {
    return;
  }

  thePM->Unhighlight (Selectable());
}
