// Created on: 1996-12-05
// Created by: Jean-Pierre COMBE/Odile Olivier/Serguei Zaritchny
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

#include <PrsDim_RadiusDimension.hxx>

#include <PrsDim.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <ElCLib.hxx>
#include <gce_MakeDir.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsDim_RadiusDimension, PrsDim_Dimension)

namespace
{
static const Standard_ExtCharacter THE_RADIUS_SYMBOL('R');
}

//=================================================================================================

PrsDim_RadiusDimension::PrsDim_RadiusDimension(const gp_Circ& theCircle)
    : PrsDim_Dimension(PrsDim_KOD_RADIUS)
{
  SetMeasuredGeometry(theCircle);
  SetSpecialSymbol(THE_RADIUS_SYMBOL);
  SetDisplaySpecialSymbol(PrsDim_DisplaySpecialSymbol_Before);
  SetFlyout(0.0);
}

//=================================================================================================

PrsDim_RadiusDimension::PrsDim_RadiusDimension(const gp_Circ& theCircle,
                                               const gp_Pnt&  theAttachPoint)
    : PrsDim_Dimension(PrsDim_KOD_RADIUS)
{
  SetMeasuredGeometry(theCircle, theAttachPoint);
  SetSpecialSymbol(THE_RADIUS_SYMBOL);
  SetDisplaySpecialSymbol(PrsDim_DisplaySpecialSymbol_Before);
  SetFlyout(0.0);
}

//=================================================================================================

PrsDim_RadiusDimension::PrsDim_RadiusDimension(const TopoDS_Shape& theShape)
    : PrsDim_Dimension(PrsDim_KOD_RADIUS)
{
  SetMeasuredGeometry(theShape);
  SetSpecialSymbol(THE_RADIUS_SYMBOL);
  SetDisplaySpecialSymbol(PrsDim_DisplaySpecialSymbol_Before);
  SetFlyout(0.0);
}

//=================================================================================================

void PrsDim_RadiusDimension::SetMeasuredGeometry(const gp_Circ&         theCircle,
                                                 const gp_Pnt&          theAnchorPoint,
                                                 const Standard_Boolean theHasAnchor)
{
  myCircle          = theCircle;
  myGeometryType    = GeometryType_Edge;
  myShape           = BRepLib_MakeEdge(theCircle);
  myAnchorPoint     = theHasAnchor ? theAnchorPoint : ElCLib::Value(0, myCircle);
  myIsGeometryValid = IsValidCircle(myCircle) && IsValidAnchor(myCircle, myAnchorPoint);

  if (myIsGeometryValid)
  {
    ComputePlane();
  }

  SetToUpdate();
}

//=================================================================================================

void PrsDim_RadiusDimension::SetMeasuredGeometry(const TopoDS_Shape&    theShape,
                                                 const gp_Pnt&          theAnchorPoint,
                                                 const Standard_Boolean theHasAnchor)
{
  Standard_Boolean isClosed = Standard_False;
  myShape                   = theShape;
  myGeometryType            = GeometryType_UndefShapes;
  myIsGeometryValid =
    InitCircularDimension(theShape, myCircle, myAnchorPoint, isClosed) && IsValidCircle(myCircle);
  if (theHasAnchor)
  {
    myAnchorPoint     = theAnchorPoint;
    myIsGeometryValid = myIsGeometryValid && IsValidAnchor(myCircle, myAnchorPoint);
  }

  if (myIsGeometryValid)
  {
    ComputePlane();
  }

  SetToUpdate();
}

//=================================================================================================

Standard_Boolean PrsDim_RadiusDimension::CheckPlane(const gp_Pln& thePlane) const
{
  // Check if anchor point and circle center point belong to plane.
  if (!thePlane.Contains(myAnchorPoint, Precision::Confusion())
      && !thePlane.Contains(myCircle.Location(), Precision::Confusion()))
  {
    return Standard_False;
  }

  return Standard_True;
}

//=================================================================================================

void PrsDim_RadiusDimension::ComputePlane()
{
  if (!myIsGeometryValid)
  {
    return;
  }

  gp_Dir aDimensionX = gce_MakeDir(myAnchorPoint, myCircle.Location());

  myPlane = gp_Pln(gp_Ax3(myCircle.Location(), myCircle.Axis().Direction(), aDimensionX));
}

//=================================================================================================

const TCollection_AsciiString& PrsDim_RadiusDimension::GetModelUnits() const
{
  return myDrawer->DimLengthModelUnits();
}

//=================================================================================================

const TCollection_AsciiString& PrsDim_RadiusDimension::GetDisplayUnits() const
{
  return myDrawer->DimLengthDisplayUnits();
}

//=================================================================================================

void PrsDim_RadiusDimension::SetModelUnits(const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimLengthModelUnits(theUnits);
}

//=================================================================================================

void PrsDim_RadiusDimension::SetDisplayUnits(const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimLengthDisplayUnits(theUnits);
}

//=================================================================================================

Standard_Real PrsDim_RadiusDimension::ComputeValue() const
{
  if (!IsValid())
  {
    return 0.0;
  }

  return myCircle.Radius();
}

//=================================================================================================

void PrsDim_RadiusDimension::Compute(const Handle(PrsMgr_PresentationManager)&,
                                     const Handle(Prs3d_Presentation)& thePresentation,
                                     const Standard_Integer            theMode)
{
  mySelectionGeom.Clear(theMode);
  if (!IsValid())
  {
    return;
  }

  DrawLinearDimension(thePresentation, theMode, myAnchorPoint, myCircle.Location(), Standard_True);
}

//=================================================================================================

Standard_Boolean PrsDim_RadiusDimension::IsValidCircle(const gp_Circ& theCircle) const
{
  return theCircle.Radius() > Precision::Confusion();
}

//=================================================================================================

Standard_Boolean PrsDim_RadiusDimension::IsValidAnchor(const gp_Circ& theCircle,
                                                       const gp_Pnt&  theAnchor) const
{
  gp_Pln        aCirclePlane(theCircle.Location(), theCircle.Axis().Direction());
  Standard_Real anAnchorDist = theAnchor.Distance(theCircle.Location());

  return anAnchorDist > Precision::Confusion()
         && aCirclePlane.Contains(theAnchor, Precision::Confusion());
}

//=================================================================================================

gp_Pnt PrsDim_RadiusDimension::GetTextPosition() const
{
  if (IsTextPositionCustom())
  {
    return myFixedTextPosition;
  }

  // Counts text position according to the dimension parameters
  return GetTextPositionForLinear(myAnchorPoint, myCircle.Location(), Standard_True);
}

//=================================================================================================

void PrsDim_RadiusDimension::SetTextPosition(const gp_Pnt& theTextPos)
{
  if (!myIsGeometryValid)
  {
    return;
  }

  myIsTextPositionFixed = Standard_True;
  myFixedTextPosition   = theTextPos;

  SetToUpdate();
}
