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

#include <Standard_NotImplemented.hxx>

#include <AIS_RadiusDimension.hxx>

#include <AIS.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_KindOfDimension.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <DsgPrs.hxx>
#include <ElCLib.hxx>
#include <GC_MakeCircle.hxx>
#include <gce_MakeCirc.hxx>
#include <gce_MakeDir.hxx>
#include <gce_MakeLin.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Root.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_HANDLE(AIS_RadiusDimension, AIS_Dimension)
IMPLEMENT_STANDARD_RTTIEXT(AIS_RadiusDimension, AIS_Dimension)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_RadiusDimension::AIS_RadiusDimension (const gp_Circ& theCircle)
: AIS_Dimension(),
  myFlyout (0.0),
  myCircle (theCircle)
{
  myFirstPoint = ElCLib::Value(0, myCircle);
  mySecondPoint = theCircle.Location();
  myIsInitialized = Standard_True;
  SetSpecialSymbol ('R');
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetKindOfDimension(AIS_KOD_RADIUS);
}

AIS_RadiusDimension::AIS_RadiusDimension (const gp_Circ& theCircle,
                                          const gp_Pnt& theAttachPoint)
: AIS_Dimension(),
  myFlyout (0.0),
  myCircle (theCircle)
{
  myFirstPoint = theAttachPoint;
  mySecondPoint = theCircle.Location();
  myIsInitialized = Standard_True;
  SetSpecialSymbol ('R');
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetKindOfDimension (AIS_KOD_RADIUS);
}

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================

AIS_RadiusDimension::AIS_RadiusDimension (const TopoDS_Shape& theShape)
: AIS_Dimension (),
  myFlyout (0.0)
{
  myFirstShape = theShape;
  myIsInitialized = Standard_False;
  SetSpecialSymbol ('R');
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetKindOfDimension (AIS_KOD_RADIUS);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_RadiusDimension::AIS_RadiusDimension (const gp_Circ& theCircle,
                                          const Handle(Prs3d_DimensionAspect)& theDimensionAspect,
                                          const Standard_Real theExtensionSize/* = 1.0*/)

: AIS_Dimension (theDimensionAspect,theExtensionSize),
  myFlyout (0.0),
  myCircle (theCircle)
{
  myFirstPoint = ElCLib::Value(0, myCircle);
  mySecondPoint = theCircle.Location();
  SetSpecialSymbol ('R');
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetKindOfDimension (AIS_KOD_RADIUS);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_RadiusDimension::AIS_RadiusDimension (const gp_Circ& theCircle,
                                          const gp_Pnt& theAttachPoint,
                                          const Handle(Prs3d_DimensionAspect)& theDimensionAspect,
                                          const Standard_Real theExtensionSize/* = 1.0*/)
: AIS_Dimension (theDimensionAspect,theExtensionSize),
  myFlyout (0.0),
  myCircle (theCircle)
{
  myFirstPoint = theAttachPoint;
  mySecondPoint = theCircle.Location();
  SetSpecialSymbol ('R');
  SetDisplaySpecialSymbol (AIS_DSS_Before);
  SetKindOfDimension (AIS_KOD_RADIUS);
}

//=======================================================================
//function : Compute
//purpose  : 
//=======================================================================

void AIS_RadiusDimension::Compute(const Handle(PrsMgr_PresentationManager3d)& /*thePM*/,
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

  drawLinearDimension (thePresentation, aFlyoutEnd1, aFlyoutEnd2, (AIS_DimensionDisplayMode)theMode, Standard_True);
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

//=======================================================================
//function : SetFlyout
//purpose  : 
//=======================================================================

void AIS_RadiusDimension::SetFlyout (const Standard_Real theFlyout)
{
  myFlyout = theFlyout;
}

//=======================================================================
//function : GetFlyout
//purpose  : 
//=======================================================================

Standard_Real AIS_RadiusDimension::GetFlyout () const
{
  return myFlyout;
}
