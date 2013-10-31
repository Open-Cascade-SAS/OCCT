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
#include <AIS_DimensionDisplayMode.hxx>
#include <StdSelect_Shape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>


//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_DimensionOwner::AIS_DimensionOwner (const Handle(SelectMgr_SelectableObject)& theSelObject,
                                        const AIS_DimensionDisplayMode theMode,
                                        const Standard_Integer thePriority)
: SelectMgr_EntityOwner(theSelObject, thePriority),
  myDisplayMode (theMode)
{
}

//=======================================================================
//function : SetDisplayMode
//purpose  : 
//=======================================================================

void AIS_DimensionOwner::SetDisplayMode (const AIS_DimensionDisplayMode theMode)
{
  myDisplayMode = theMode;
}

//=======================================================================
//function : DisplayMode
//purpose  : 
//=======================================================================

AIS_DimensionDisplayMode AIS_DimensionOwner::DisplayMode () const
{
  return myDisplayMode;
}

//=======================================================================
//function : IsHilighted
//purpose  : 
//=======================================================================

Standard_Boolean AIS_DimensionOwner::IsHilighted (const Handle(PrsMgr_PresentationManager)& thePM,
                                                  const Standard_Integer theMode) const
{
  if (HasSelectable())
  {
    Standard_Integer aMode = myDisplayMode != 0 ? myDisplayMode : theMode;
    return thePM->IsHighlighted(Selectable (), aMode);
  }
  return Standard_False;
}

//=======================================================================
//function : Hilight
//purpose  : 
//=======================================================================

void AIS_DimensionOwner::Hilight (const Handle(PrsMgr_PresentationManager)& thePM,
                                  const Standard_Integer theMode)
{
  if (HasSelectable())
  {
    Standard_Integer aMode = myDisplayMode != 0 ? myDisplayMode : theMode;
    thePM->Highlight (Selectable(),aMode);
  }
}

//=======================================================================
//function : Unhilight
//purpose  : 
//=======================================================================

void AIS_DimensionOwner::Unhilight (const Handle(PrsMgr_PresentationManager)& thePM,
                                    const Standard_Integer theMode)
{
  if (HasSelectable())
  {
    Standard_Integer aMode = myDisplayMode != 0 ? myDisplayMode : theMode;
    thePM->Unhighlight(Selectable(),aMode);
  }
}

//=======================================================================
//function : HilightWithColor
//purpose  : 
//=======================================================================

void AIS_DimensionOwner::HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                           const Quantity_NameOfColor theColor,
                                           const Standard_Integer theMode)
{
  // Highlight selectable part of dimension with color
  if (myDisplayMode != 0)
  {
    thePM->Color (Selectable(), theColor, myDisplayMode);
  }
  else
    thePM->Color (Selectable(), theColor, theMode);
}
