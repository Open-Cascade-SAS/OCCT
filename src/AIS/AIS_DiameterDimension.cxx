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

// Modified     Mon 12-january-98
//              <ODL>, <SZY>


#define BUC60915        //GG 05/06/01 Enable to compute the requested arrow size
//                      if any in all dimensions.
#include <AIS_DiameterDimension.hxx>

#include <Adaptor3d_HCurve.hxx>
#include <AIS.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_DimensionOwner.hxx>
#include <DsgPrs_DiameterPresentation.hxx>
#include <DsgPrs_RadiusPresentation.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <GC_MakeCircle.hxx>
#include <gce_MakeDir.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Lin.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_Root.hxx>
#include <Precision.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveBox.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Macro.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_HANDLE(AIS_DiameterDimension, AIS_Dimension)
IMPLEMENT_STANDARD_RTTIEXT(AIS_DiameterDimension, AIS_Dimension)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_DiameterDimension::AIS_DiameterDimension(const gp_Circ& theCircle)
: AIS_Dimension(),
  myFlyout (0.0),
  myCircle (theCircle)
{
  SetKindOfDimension(AIS_KOD_DIAMETER);
  myIsInitialized = Standard_True;
  SetSpecialSymbol (0x00D8);
  SetDisplaySpecialSymbol (AIS_DSS_Before);
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
  myFlyout (0.0),
  myCircle (theCircle)
{
  SetKindOfDimension(AIS_KOD_DIAMETER);
  SetSpecialSymbol (0x00D8);
  SetDisplaySpecialSymbol (AIS_DSS_Before);
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
//purpose  : 
//=======================================================================

AIS_DiameterDimension::AIS_DiameterDimension (const gp_Circ& theCircle,
                                              const Handle(Prs3d_DimensionAspect)& theDimensionStyle,
                                              const Standard_Real theExtensionSize /*= 1.0*/)
: AIS_Dimension (theExtensionSize),
  myFlyout (0.0),
  myCircle (theCircle)
{
  SetKindOfDimension(AIS_KOD_DIAMETER);
  SetSpecialSymbol (0x00D8);
  SetDisplaySpecialSymbol(AIS_DSS_Before);
  myDrawer->SetDimensionAspect(theDimensionStyle);
  myIsInitialized = Standard_True;
}

//=======================================================================
//function : Constructor
//purpose  : Universal constructor for diameter dimension of shape
//=======================================================================

AIS_DiameterDimension::AIS_DiameterDimension (const TopoDS_Shape& theShape)
: AIS_Dimension (),
  myFlyout (0.)
{
  SetKindOfDimension(AIS_KOD_DIAMETER);
  SetSpecialSymbol (0x00D8);
  SetDisplaySpecialSymbol(AIS_DSS_Before);
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
   countDefaultPlane();

  //Count flyout direction
  gp_Ax1 aWorkingPlaneNormal = GetWorkingPlane().Axis();
  gp_Dir aTargetPointsVector = gce_MakeDir (myFirstPoint, mySecondPoint);
  // Count a flyout direction vector.
  gp_Dir aFlyoutVector = aWorkingPlaneNormal.Direction()^aTargetPointsVector;
  gp_Ax3 aLocalSystem (myFirstPoint, aTargetPointsVector, aFlyoutVector);

  // Create lines for layouts
  gp_Lin aLine1 (myFirstPoint, aFlyoutVector);
  gp_Lin aLine2 (mySecondPoint, aFlyoutVector);

  // Get flyout end points
  gp_Pnt aFlyoutEnd1 = ElCLib::Value (ElCLib::Parameter (aLine1, myFirstPoint) + GetFlyout(), aLine1);
  gp_Pnt aFlyoutEnd2 = ElCLib::Value (ElCLib::Parameter (aLine2, mySecondPoint) + GetFlyout(), aLine2);

    // Add layout lines to graphic group
  // Common to all type of dimension placement.
  if (theMode == 0)
  {
    Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments(4);
    aPrimSegments->AddVertex (myFirstPoint);
    aPrimSegments->AddVertex (aFlyoutEnd1);

    aPrimSegments->AddVertex (mySecondPoint);
    aPrimSegments->AddVertex (aFlyoutEnd2);

    Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
  }

  drawLinearDimension (thePresentation, aFlyoutEnd1, aFlyoutEnd2, (AIS_DimensionDisplayMode)theMode);
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

//=======================================================================
//function : SetFlyout
//purpose  : 
//=======================================================================

void AIS_DiameterDimension::SetFlyout (const Standard_Real theFlyout)
{
  myFlyout = theFlyout;
}

//=======================================================================
//function : GetFlyout
//purpose  : 
//=======================================================================

Standard_Real AIS_DiameterDimension::GetFlyout () const
{
  return myFlyout;
}
