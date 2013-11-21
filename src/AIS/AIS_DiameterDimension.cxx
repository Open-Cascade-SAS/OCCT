// Created on: 1996-12-05
// Created by: Jacques MINOT/Odile Olivier/Sergey ZARITCHNY
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

#include <AIS_DiameterDimension.hxx>
#include <AIS.hxx>
#include <AIS_Drawer.hxx>
#include <ElCLib.hxx>
#include <gce_MakeDir.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Prs3d_Root.hxx>

IMPLEMENT_STANDARD_HANDLE(AIS_DiameterDimension, AIS_Dimension)
IMPLEMENT_STANDARD_RTTIEXT(AIS_DiameterDimension, AIS_Dimension)

namespace
{
  static const Standard_ExtCharacter THE_DIAMETER_SYMBOL (0x00D8);
};

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_DiameterDimension::AIS_DiameterDimension(const gp_Circ& theCircle)
: AIS_Dimension(),
  myCircle (theCircle)
{
  SetKindOfDimension(AIS_KOD_DIAMETER);
  myIsInitialized = Standard_True;
  SetSpecialSymbol (THE_DIAMETER_SYMBOL);
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetFlyout (0.0);
  // Count attach points
  myFirstPoint = ElCLib::Value (0, myCircle);
  mySecondPoint = myFirstPoint.Translated (gp_Vec(myFirstPoint, theCircle.Location())*2);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_DiameterDimension::AIS_DiameterDimension(const gp_Circ& theCircle, const gp_Pnt& theAttachPoint)
: AIS_Dimension(),
  myCircle (theCircle)
{
  SetKindOfDimension (AIS_KOD_DIAMETER);
  SetSpecialSymbol (THE_DIAMETER_SYMBOL);
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetFlyout (0.0);
  myFirstPoint = theAttachPoint;
  // Count the second point
  if (Abs(myFirstPoint.Distance (theCircle.Location()) - theCircle.Radius()) < Precision::Confusion())
  {
    mySecondPoint = myFirstPoint.Translated(gp_Vec(myFirstPoint, theCircle.Location())*2);
  }
  else
  {
    myFirstPoint = ElCLib::Value(0, myCircle);
    mySecondPoint = myFirstPoint.Translated(gp_Vec(myFirstPoint, theCircle.Location())*2);
  }
  myIsInitialized = Standard_True;
}

//=======================================================================
//function : Constructor
//purpose  : Universal constructor for diameter dimension of shape
//=======================================================================

AIS_DiameterDimension::AIS_DiameterDimension (const TopoDS_Shape& theShape)
: AIS_Dimension ()
{
  SetKindOfDimension (AIS_KOD_DIAMETER);
  SetSpecialSymbol (THE_DIAMETER_SYMBOL);
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetFlyout (0.0);
  myFirstShape = theShape;
  myIsInitialized = Standard_False;
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_DiameterDimension::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePM*/,
                                     const Handle(Prs3d_Presentation)& thePresentation, 
                                     const Standard_Integer theMode)
{
  thePresentation->Clear();

  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());

  if (!myIsInitialized)
  {
    if (!initCircularDimension (myFirstShape, myCircle,
                               myFirstPoint, mySecondPoint))
      return;
    else
      myIsInitialized = Standard_True;
  }

  if (!myIsWorkingPlaneCustom)
  {
   countDefaultPlane();
  }

  drawLinearDimension (thePresentation, (AIS_DimensionDisplayMode)theMode);
}

//=======================================================================
//function : computeValue
//purpose  : 
//=======================================================================

void AIS_DiameterDimension::computeValue ()
{
  myValue = myFirstPoint.Distance (mySecondPoint);
  AIS_Dimension::computeValue();
}

//=======================================================================
//function : countDefaultPlane
//purpose  : 
//=======================================================================

void AIS_DiameterDimension::countDefaultPlane ()
{
  // Compute normal of the default plane.
  //gp_Vec aVec1(mySecondPoint, myFirstPoint),
  //       aVec2(mySecondPoint, ElCLib::Value(M_PI_2, myCircle));
  myDefaultPlane = gp_Pln(gp_Ax3(myCircle.Position()));
  // Set computed value to <myWorkingPlane>
  ResetWorkingPlane ();
}
