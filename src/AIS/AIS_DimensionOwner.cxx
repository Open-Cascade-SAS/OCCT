// Created on: 1996-12-05
// Created by: Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
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

#include <AIS_DimensionOwner.hxx>

#include <AIS_Dimension.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Standard_Type.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_DimensionOwner,SelectMgr_EntityOwner)

namespace
{
  //=======================================================================
  //function : HighlightMode
  //purpose  : Return corresponding compute mode for selection type.
  //=======================================================================
  static AIS_Dimension::ComputeMode HighlightMode (const Standard_Integer theSelMode)
  {
    switch (theSelMode)
    {
      case AIS_DSM_Line : return AIS_Dimension::ComputeMode_Line;
      case AIS_DSM_Text : return AIS_Dimension::ComputeMode_Text;
      default:
        return AIS_Dimension::ComputeMode_All;
    }
  }
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_DimensionOwner::AIS_DimensionOwner (const Handle(SelectMgr_SelectableObject)& theSelObject,
                                        const AIS_DimensionSelectionMode theMode,
                                        const Standard_Integer thePriority)
: SelectMgr_EntityOwner(theSelObject, thePriority),
  mySelectionMode (theMode)
{
}

//=======================================================================
//function : AIS_DimensionSelectionMode
//purpose  : 
//=======================================================================
AIS_DimensionSelectionMode AIS_DimensionOwner::SelectionMode () const
{
  return mySelectionMode;
}

//=======================================================================
//function : IsHilighted
//purpose  : 
//=======================================================================
Standard_Boolean AIS_DimensionOwner::IsHilighted (const Handle(PrsMgr_PresentationManager)& thePM,
                                                  const Standard_Integer /*theMode*/) const
{
  if (!HasSelectable())
  {
    return Standard_False;
  }

  return thePM->IsHighlighted (Selectable(), HighlightMode (mySelectionMode));
}

//=======================================================================
//function : Unhilight
//purpose  : 
//=======================================================================
void AIS_DimensionOwner::Unhilight (const Handle(PrsMgr_PresentationManager)& thePM,
                                    const Standard_Integer /*theMode*/)
{
  if (!HasSelectable())
  {
    return;
  }

  thePM->Unhighlight (Selectable());
}

//=======================================================================
//function : HilightWithColor
//purpose  : 
//=======================================================================
void AIS_DimensionOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                           const Handle(Prs3d_Drawer)& theStyle,
                                           const Standard_Integer /*theMode*/)
{
  thePM->Color (Selectable(), theStyle, HighlightMode (mySelectionMode));
}
