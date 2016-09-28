// Created on: 1996-12-05
// Created by: Jacques MINOT/Odile Olivier/Sergey ZARITCHNY
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

#include <AIS_DiameterDimension.hxx>

#include <AIS.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <ElCLib.hxx>
#include <GeomAPI_IntCS.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Plane.hxx>
#include <gce_MakeDir.hxx>
#include <Standard_ProgramError.hxx>


IMPLEMENT_STANDARD_RTTIEXT(AIS_DiameterDimension,AIS_Dimension)

namespace
{
  static const Standard_ExtCharacter THE_DIAMETER_SYMBOL (0x00D8);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_DiameterDimension::AIS_DiameterDimension (const gp_Circ& theCircle)
: AIS_Dimension (AIS_KOD_DIAMETER)
{
  SetMeasuredGeometry (theCircle);
  SetSpecialSymbol (THE_DIAMETER_SYMBOL);
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetFlyout (0.0);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_DiameterDimension::AIS_DiameterDimension (const gp_Circ& theCircle,
                                              const gp_Pln& thePlane)
: AIS_Dimension (AIS_KOD_DIAMETER)
{
  SetCustomPlane (thePlane);
  SetMeasuredGeometry (theCircle);
  SetSpecialSymbol (THE_DIAMETER_SYMBOL);
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetFlyout (0.0);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_DiameterDimension::AIS_DiameterDimension (const TopoDS_Shape& theShape)
: AIS_Dimension (AIS_KOD_DIAMETER)
{
  SetMeasuredGeometry (theShape);
  SetSpecialSymbol (THE_DIAMETER_SYMBOL);
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetFlyout (0.0);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_DiameterDimension::AIS_DiameterDimension (const TopoDS_Shape& theShape,
                                              const gp_Pln& thePlane)
: AIS_Dimension (AIS_KOD_DIAMETER)
{
  SetCustomPlane (thePlane);
  SetMeasuredGeometry (theShape);
  SetSpecialSymbol (THE_DIAMETER_SYMBOL);
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetFlyout (0.0);
}

//=======================================================================
//function : AnchorPoint
//purpose  : 
//=======================================================================
gp_Pnt AIS_DiameterDimension::AnchorPoint()
{
  if (!IsValid())
  {
    return gp::Origin();
  }

  return myAnchorPoint;
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_DiameterDimension::SetMeasuredGeometry (const gp_Circ& theCircle)
{
  myCircle          = theCircle;
  myGeometryType    = GeometryType_Edge;
  myShape           = BRepLib_MakeEdge (theCircle);
  myAnchorPoint     = gp::Origin();
  myIsGeometryValid = IsValidCircle (myCircle);

  if (myIsGeometryValid && myIsPlaneCustom)
  {
    ComputeAnchorPoint();
  }
  else if (!myIsPlaneCustom)
  {
    ComputePlane();
    myAnchorPoint = ElCLib::Value (0.0, myCircle);
  }

  SetToUpdate();
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_DiameterDimension::SetMeasuredGeometry (const TopoDS_Shape& theShape)
{
  gp_Pnt aDummyPnt (gp::Origin());
  Standard_Boolean isClosed = Standard_False;

  myGeometryType    = GeometryType_UndefShapes;
  myShape           = theShape;
  myAnchorPoint     = gp::Origin();
  myIsGeometryValid = InitCircularDimension (theShape, myCircle, aDummyPnt, isClosed)
                      && IsValidCircle (myCircle)
                      && isClosed;

  if (myIsGeometryValid && myIsPlaneCustom)
  {
    ComputeAnchorPoint();
  }
  else if (!myIsPlaneCustom)
  {
    ComputePlane();
    myAnchorPoint = ElCLib::Value (0.0, myCircle);
  }

  SetToUpdate();
}

//=======================================================================
//function : CheckPlane
//purpose  : 
//=======================================================================
Standard_Boolean AIS_DiameterDimension::CheckPlane (const gp_Pln& thePlane) const
{
  // Check if the circle center point belongs to plane.
  if (!thePlane.Contains (myCircle.Location(), Precision::Confusion()))
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : ComputePlane
//purpose  : 
//=======================================================================
void AIS_DiameterDimension::ComputePlane()
{
  if (!myIsGeometryValid)
  {
    return;
  }

  myPlane = gp_Pln (gp_Ax3 (myCircle.Position()));
}

//=======================================================================
//function : ComputeAnchorPoint
//purpose  : 
//=======================================================================
void AIS_DiameterDimension::ComputeAnchorPoint()
{
  // Anchor point is an intersection of dimension plane and circle.
  Handle(Geom_Circle) aCircle = new Geom_Circle (myCircle);
  Handle(Geom_Plane) aPlane = new Geom_Plane (myPlane);
  GeomAPI_IntCS anIntersector (aCircle, aPlane);
  if (!anIntersector.IsDone())
  {
    myIsGeometryValid = Standard_False;
    return;
  }

  // The circle lays on the plane.
  if (anIntersector.NbPoints() != 2)
  {
    myAnchorPoint = ElCLib::Value (0.0, myCircle);
    myIsGeometryValid = Standard_True;
    return;
  }

  gp_Pnt aFirstPoint = anIntersector.Point (1);
  gp_Pnt aSecondPoint = anIntersector.Point (2);

  // Choose one of two intersection points that stands with
  // positive direction of flyout.
  // An anchor point is supposed to be the left attachment point.
  gp_Dir aFirstDir = gce_MakeDir (aFirstPoint, myCircle.Location());
  gp_Dir aDir = myPlane.Axis().Direction() ^ aFirstDir;
  myAnchorPoint = (gp_Vec (aDir) * gp_Vec(myCircle.Position().Direction()) > 0.0)
                  ? aFirstPoint
                  : aSecondPoint;

}

//=======================================================================
//function : GetModelUnits
//purpose  :
//=======================================================================
const TCollection_AsciiString& AIS_DiameterDimension::GetModelUnits() const
{
  return myDrawer->DimLengthModelUnits();
}

//=======================================================================
//function : GetDisplayUnits
//purpose  :
//=======================================================================
const TCollection_AsciiString& AIS_DiameterDimension::GetDisplayUnits() const
{
  return myDrawer->DimLengthDisplayUnits();
}

//=======================================================================
//function : SetModelUnits
//purpose  :
//=======================================================================
void AIS_DiameterDimension::SetModelUnits (const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimLengthModelUnits (theUnits);
}

//=======================================================================
//function : SetDisplayUnits
//purpose  :
//=======================================================================
void AIS_DiameterDimension::SetDisplayUnits (const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimLengthDisplayUnits (theUnits);
}

//=======================================================================
//function : ComputeValue
//purpose  : 
//=======================================================================
Standard_Real AIS_DiameterDimension::ComputeValue() const
{
  if (!IsValid())
  {
    return 0.0;
  }

  return myCircle.Radius() * 2.0;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================
void AIS_DiameterDimension::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePM*/,
                                     const Handle(Prs3d_Presentation)& thePresentation, 
                                     const Standard_Integer theMode)
{
  mySelectionGeom.Clear (theMode);

  if (!IsValid())
  {
    return;
  }

  gp_Pnt aFirstPnt (gp::Origin());
  gp_Pnt aSecondPnt (gp::Origin());
  ComputeSidePoints (myCircle, aFirstPnt, aSecondPnt);

  DrawLinearDimension (thePresentation, theMode, aFirstPnt, aSecondPnt);
}

//=======================================================================
//function : ComputeFlyoutSelection
//purpose  : 
//=======================================================================
void AIS_DiameterDimension::ComputeFlyoutSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                    const Handle(SelectMgr_EntityOwner)& theEntityOwner)
{
  if (!IsValid())
  {
    return;
  }

  gp_Pnt aFirstPnt (gp::Origin());
  gp_Pnt aSecondPnt (gp::Origin());
  ComputeSidePoints (myCircle, aFirstPnt, aSecondPnt);

  ComputeLinearFlyouts (theSelection, theEntityOwner, aFirstPnt, aSecondPnt);
}

//=======================================================================
//function : ComputeSidePoints
//purpose  : 
//=======================================================================
void AIS_DiameterDimension::ComputeSidePoints (const gp_Circ& theCircle,
                                               gp_Pnt& theFirstPnt,
                                               gp_Pnt& theSecondPnt)
{
  theFirstPnt = AnchorPoint();

  gp_Vec aRadiusVector (theCircle.Location(), theFirstPnt);
  theSecondPnt = theCircle.Location().Translated (-aRadiusVector);
}

//=======================================================================
//function : IsValidCircle
//purpose  : 
//=======================================================================
Standard_Boolean AIS_DiameterDimension::IsValidCircle (const gp_Circ& theCircle) const
{
  return (theCircle.Radius() * 2.0) > Precision::Confusion();
}

//=======================================================================
//function : IsValidAnchor
//purpose  : 
//=======================================================================
Standard_Boolean AIS_DiameterDimension::IsValidAnchor (const gp_Circ& theCircle,
                                                       const gp_Pnt& theAnchor) const
{
  gp_Pln aCirclePlane (theCircle.Location(), theCircle.Axis().Direction());
  Standard_Real anAnchorDist = theAnchor.Distance (theCircle.Location());
  Standard_Real aRadius      = myCircle.Radius();

  return Abs (anAnchorDist - aRadius) > Precision::Confusion()
      && aCirclePlane.Contains (theAnchor, Precision::Confusion());
}

//=======================================================================
//function : GetTextPosition
//purpose  : 
//=======================================================================
const gp_Pnt AIS_DiameterDimension::GetTextPosition() const
{
  if (IsTextPositionCustom())
  {
    return myFixedTextPosition;
  }
  
  // Counts text position according to the dimension parameters
  return GetTextPositionForLinear (myAnchorPoint, myCircle.Location());
}

//=======================================================================
//function : GetTextPosition
//purpose  : 
//=======================================================================
void AIS_DiameterDimension::SetTextPosition (const gp_Pnt& theTextPos)
{
  if (!IsValid())
  {
    return;
  }

  myIsTextPositionFixed = Standard_True;
  myFixedTextPosition = theTextPos;

  SetToUpdate();
}
