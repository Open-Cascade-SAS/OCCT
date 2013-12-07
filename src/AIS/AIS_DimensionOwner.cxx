// Created on: 1996-12-05
// Created by: Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <AIS_DimensionOwner.ixx>
#include <AIS_Dimension.hxx>

#include <StdSelect_Shape.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

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
};

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
//function : Hilight
//purpose  : 
//=======================================================================
void AIS_DimensionOwner::Hilight (const Handle(PrsMgr_PresentationManager)& thePM,
                                  const Standard_Integer /*theMode*/)
{
  if (!HasSelectable())
  {
    return;
  }

  thePM->Highlight (Selectable(), HighlightMode (mySelectionMode));
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

  thePM->Unhighlight (Selectable(), HighlightMode (mySelectionMode));
}

//=======================================================================
//function : HilightWithColor
//purpose  : 
//=======================================================================
void AIS_DimensionOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                           const Quantity_NameOfColor theColor,
                                           const Standard_Integer /*theMode*/)
{
  thePM->Color (Selectable(), theColor, HighlightMode (mySelectionMode));
}
