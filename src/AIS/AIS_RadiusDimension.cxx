// Created on: 1996-12-05
// Created by: Jean-Pierre COMBE/Odile Olivier/Serguei Zaritchny
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

#include <AIS_RadiusDimension.hxx>

#include <AIS.hxx>
#include <AIS_Drawer.hxx>
#include <ElCLib.hxx>
#include <gce_MakeDir.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Prs3d_Root.hxx>

IMPLEMENT_STANDARD_HANDLE(AIS_RadiusDimension, AIS_Dimension)
IMPLEMENT_STANDARD_RTTIEXT(AIS_RadiusDimension, AIS_Dimension)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_RadiusDimension::AIS_RadiusDimension (const gp_Circ& theCircle)
: AIS_Dimension(),
  myCircle (theCircle)
{
  myFirstPoint = ElCLib::Value(0, myCircle);
  mySecondPoint = theCircle.Location();
  myIsInitialized = Standard_True;
  SetSpecialSymbol ('R');
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetKindOfDimension (AIS_KOD_RADIUS);
  SetFlyout (0.0);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_RadiusDimension::AIS_RadiusDimension (const gp_Circ& theCircle,
                                          const gp_Pnt& theAttachPoint)
: AIS_Dimension(),
  myCircle (theCircle)
{
  myFirstPoint = theAttachPoint;
  mySecondPoint = theCircle.Location();
  myIsInitialized = Standard_True;
  SetSpecialSymbol ('R');
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetKindOfDimension (AIS_KOD_RADIUS);
  SetFlyout (0.0);
}

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================

AIS_RadiusDimension::AIS_RadiusDimension (const TopoDS_Shape& theShape)
: AIS_Dimension ()
{
  myFirstShape = theShape;
  myIsInitialized = Standard_False;
  SetSpecialSymbol ('R');
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetKindOfDimension (AIS_KOD_RADIUS);
  SetFlyout (0.0);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_RadiusDimension::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePM*/,
                                   const Handle(Prs3d_Presentation)& thePresentation,
                                   const Standard_Integer theMode)
{
  thePresentation->Clear();

  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());

  if (!myIsInitialized)
  {
    gp_Pnt aLastPoint;
    if (!initCircularDimension (myFirstShape, myCircle,
      myFirstPoint, aLastPoint))
      return;
    else
    {
      mySecondPoint = myCircle.Location();
      myIsInitialized = Standard_True;
    }
  }

  if (!myIsWorkingPlaneCustom)
  {
    countDefaultPlane();
  }

  drawLinearDimension (thePresentation, (AIS_DimensionDisplayMode)theMode, Standard_True);
}

//=======================================================================
//function : computeValue
//purpose  : 
//=======================================================================

void AIS_RadiusDimension::computeValue ()
{
  myValue = myFirstPoint.Distance (mySecondPoint);
  AIS_Dimension::computeValue ();
}

//=======================================================================
//function : countDefaultPlane
//purpose  : 
//=======================================================================

void AIS_RadiusDimension::countDefaultPlane ()
{
  // Compute normal of the default plane.
  gp_Vec aVec1(mySecondPoint, myFirstPoint),
         aVec2(mySecondPoint, ElCLib::Value(M_PI_2, myCircle));
  myDefaultPlane = gp_Pln(myCircle.Location(), aVec1^aVec2);
  // Set computed value to <myWorkingPlane>
  ResetWorkingPlane ();
}
