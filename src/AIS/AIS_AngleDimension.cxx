// Created on: 1996-12-05
// Created by: Arnaud BOUZY/Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <AIS_AngleDimension.hxx>

#include <AIS.hxx>
#include <AIS_Drawer.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <GCPnts_UniformAbscissa.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <gce_MakeLin2d.hxx>
#include <gce_MakeLin.hxx>
#include <gce_MakeCirc.hxx>
#include <gce_MakeCone.hxx>
#include <gce_MakePln.hxx>
#include <gce_MakeDir.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <ProjLib.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_Selection.hxx>
#include <UnitsAPI.hxx>

IMPLEMENT_STANDARD_HANDLE (AIS_AngleDimension, AIS_Dimension)
IMPLEMENT_STANDARD_RTTIEXT (AIS_AngleDimension, AIS_Dimension)

namespace
{
  static const TCollection_ExtendedString THE_EMPTY_LABEL_STRING;
  static const Standard_Real              THE_EMPTY_LABEL_WIDTH = 0.0;
  static const Standard_ExtCharacter      THE_DEGREE_SYMBOL (0x00B0);
  static const Standard_Real              THE_3D_TEXT_MARGIN = 0.1;
};

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const TopoDS_Edge& theFirstEdge,
                                        const TopoDS_Edge& theSecondEdge)
: AIS_Dimension (AIS_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry (theFirstEdge, theSecondEdge);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const gp_Pnt& theFirstPoint,
                                        const gp_Pnt& theSecondPoint,
                                        const gp_Pnt& theThirdPoint)
: AIS_Dimension (AIS_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry (theFirstPoint, theSecondPoint, theThirdPoint);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const TopoDS_Vertex& theFirstVertex,
                                        const TopoDS_Vertex& theSecondVertex,
                                        const TopoDS_Vertex& theThirdVertex)
: AIS_Dimension (AIS_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry (theFirstVertex, theSecondVertex, theThirdVertex);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const TopoDS_Face& theCone)
: AIS_Dimension (AIS_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry (theCone);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const TopoDS_Face& theFirstFace,
                                        const TopoDS_Face& theSecondFace)
: AIS_Dimension (AIS_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry (theFirstFace, theSecondFace);
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_AngleDimension::AIS_AngleDimension (const TopoDS_Face& theFirstFace,
                                        const TopoDS_Face& theSecondFace,
                                        const gp_Pnt& thePoint)
: AIS_Dimension (AIS_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry (theFirstFace, theSecondFace, thePoint);
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_AngleDimension::SetMeasuredGeometry (const TopoDS_Edge& theFirstEdge,
                                              const TopoDS_Edge& theSecondEdge)
{
  gp_Pln aComputedPlane;

  myFirstShape   = theFirstEdge;
  mySecondShape  = theSecondEdge;
  myThirdShape   = TopoDS_Shape();
  myGeometryType = GeometryType_Edges;
  myIsValid      = InitTwoEdgesAngle (aComputedPlane);

  if (myIsValid && !myIsPlaneCustom)
  {
    myPlane = aComputedPlane;
  }

  myIsValid &= CheckPlane (myPlane);

  SetToUpdate();
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_AngleDimension::SetMeasuredGeometry (const gp_Pnt& theFirstPoint,
                                              const gp_Pnt& theSecondPoint,
                                              const gp_Pnt& theThirdPoint)
{
  myFirstPoint    = theFirstPoint;
  myCenterPoint   = theSecondPoint;
  mySecondPoint   = theThirdPoint;
  myFirstShape    = BRepLib_MakeVertex (myFirstPoint);
  mySecondShape   = BRepLib_MakeVertex (myCenterPoint);
  myThirdShape    = BRepLib_MakeVertex (mySecondPoint);
  myGeometryType  = GeometryType_Points;
  myIsValid       = IsValidPoints (myFirstPoint, myCenterPoint, mySecondPoint);

  if (myIsValid && !myIsPlaneCustom)
  {
    ComputePlane();
  }

  myIsValid &= CheckPlane (myPlane);

  SetToUpdate();
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_AngleDimension::SetMeasuredGeometry (const TopoDS_Vertex& theFirstVertex,
                                              const TopoDS_Vertex& theSecondVertex,
                                              const TopoDS_Vertex& theThirdVertex)
{
  myFirstShape   = theFirstVertex;
  mySecondShape  = theSecondVertex;
  myThirdShape   = theThirdVertex;
  myFirstPoint   = BRep_Tool::Pnt (theFirstVertex);
  myCenterPoint  = BRep_Tool::Pnt (theSecondVertex);
  mySecondPoint  = BRep_Tool::Pnt (theThirdVertex);
  myGeometryType = GeometryType_Points;
  myIsValid      = IsValidPoints (myFirstPoint, myCenterPoint, mySecondPoint);

  if (myIsValid && !myIsPlaneCustom)
  {
    ComputePlane();
  }

  myIsValid &= CheckPlane (myPlane);

  SetToUpdate();
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_AngleDimension::SetMeasuredGeometry (const TopoDS_Face& theCone)
{
  myFirstShape   = theCone;
  mySecondShape  = TopoDS_Shape();
  myThirdShape   = TopoDS_Shape();
  myGeometryType = GeometryType_Face;
  myIsValid      = InitConeAngle();

  if (myIsValid && !myIsPlaneCustom)
  {
    ComputePlane();
  }

  myIsValid &= CheckPlane (myPlane);

  SetToUpdate();
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_AngleDimension::SetMeasuredGeometry (const TopoDS_Face& theFirstFace,
                                              const TopoDS_Face& theSecondFace)
{
  myFirstShape   = theFirstFace;
  mySecondShape  = theSecondFace;
  myThirdShape   = TopoDS_Shape();
  myGeometryType = GeometryType_Faces;
  myIsValid      = InitTwoFacesAngle();

  if (myIsValid && !myIsPlaneCustom)
  {
    ComputePlane();
  }

  myIsValid &= CheckPlane (myPlane);

  SetToUpdate();
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_AngleDimension::SetMeasuredGeometry (const TopoDS_Face& theFirstFace,
                                              const TopoDS_Face& theSecondFace,
                                              const gp_Pnt& thePoint)
{
  myFirstShape   = theFirstFace;
  mySecondShape  = theSecondFace;
  myThirdShape   = TopoDS_Shape();
  myGeometryType = GeometryType_Faces;
  myIsValid      = InitTwoFacesAngle (thePoint);

  if (myIsValid && !myIsPlaneCustom)
  {
    ComputePlane();
  }

  myIsValid &= CheckPlane (myPlane);

  SetToUpdate();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void AIS_AngleDimension::Init()
{
  SetSpecialSymbol (THE_DEGREE_SYMBOL);
  SetDisplaySpecialSymbol (AIS_DSS_After);
  SetFlyout (15.0);
}

//=======================================================================
//function: GetCenterOnArc
//purpose :
//=======================================================================
gp_Pnt AIS_AngleDimension::GetCenterOnArc (const gp_Pnt& theFirstAttach,
                                           const gp_Pnt& theSecondAttach,
                                           const gp_Pnt& theCenter)
{
  // construct plane where the circle and the arc are located
  gce_MakePln aConstructPlane (theFirstAttach, theSecondAttach, theCenter);
  if (!aConstructPlane.IsDone())
  {
    return gp::Origin();
  }
  
  gp_Pln aPlane = aConstructPlane.Value();

  Standard_Real aRadius = theFirstAttach.Distance (theCenter);

  // construct circle forming the arc
  gce_MakeCirc aConstructCircle (theCenter, aPlane, aRadius);
  if (!aConstructCircle.IsDone())
  {
    return gp::Origin();
  }

  gp_Circ aCircle = aConstructCircle.Value();

  // compute angle parameters of arc end-points on circle
  Standard_Real aParamBeg = ElCLib::Parameter (aCircle, theFirstAttach);
  Standard_Real aParamEnd = ElCLib::Parameter (aCircle, theSecondAttach);
  ElCLib::AdjustPeriodic (0.0, M_PI * 2, Precision::PConfusion(), aParamBeg, aParamEnd);

  return ElCLib::Value ((aParamBeg + aParamEnd) * 0.5, aCircle);
}

//=======================================================================
//function : DrawArc
//purpose  : draws the arc between two attach points
//=======================================================================
void AIS_AngleDimension::DrawArc (const Handle(Prs3d_Presentation)& thePresentation,
                                  const gp_Pnt& theFirstAttach,
                                  const gp_Pnt& theSecondAttach,
                                  const gp_Pnt& theCenter,
                                  const Standard_Real theRadius,
                                  const Standard_Integer theMode)
{
  // construct plane where the circle and the arc are located
  gce_MakePln aConstructPlane (theFirstAttach, theSecondAttach, theCenter);
  if (!aConstructPlane.IsDone())
  {
    return;
  }

  gp_Pln aPlane = aConstructPlane.Value();

  // construct circle forming the arc
  gce_MakeCirc aConstructCircle (theCenter, aPlane, theRadius);
  if (!aConstructCircle.IsDone())
  {
    return;
  }

  gp_Circ aCircle = aConstructCircle.Value();

  // construct the arc
  GC_MakeArcOfCircle aConstructArc (aCircle, theFirstAttach, theSecondAttach, Standard_True);
  if (!aConstructArc.IsDone())
  {
    return;
  }

  // generate points with specified deflection
  const Handle(Geom_TrimmedCurve)& anArcCurve = aConstructArc.Value();
  
  GeomAdaptor_Curve anArcAdaptor (anArcCurve, anArcCurve->FirstParameter(), anArcCurve->LastParameter());

  // compute number of discretization elements in old-fanshioned way
  gp_Vec aCenterToFirstVec  (theCenter, theFirstAttach);
  gp_Vec aCenterToSecondVec (theCenter, theSecondAttach);
  const Standard_Real anAngle = aCenterToFirstVec.Angle (aCenterToSecondVec);
  const Standard_Integer aNbPoints = Max (4, Standard_Integer (50.0 * anAngle / M_PI));

  GCPnts_UniformAbscissa aMakePnts (anArcAdaptor, aNbPoints);
  if (!aMakePnts.IsDone())
  {
    return;
  }

  // init data arrays for graphical and selection primitives
  Handle(Graphic3d_ArrayOfPolylines) aPrimSegments = new Graphic3d_ArrayOfPolylines (aNbPoints);

  SelectionGeometry::Curve& aSensitiveCurve = mySelectionGeom.NewCurve();

  // load data into arrays
  for (Standard_Integer aPntIt = 1; aPntIt <= aMakePnts.NbPoints(); ++aPntIt)
  {
    gp_Pnt aPnt = anArcAdaptor.Value (aMakePnts.Parameter (aPntIt));

    aPrimSegments->AddVertex (aPnt);

    aSensitiveCurve.Append (aPnt);
  }

  // add display presentation
  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == ComputeMode_All)
  {
    Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_True);
  }
  Handle(Graphic3d_AspectLine3d) aDimensionLineStyle = myDrawer->DimensionAspect()->LineAspect()->Aspect();
  Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionLineStyle);
  Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == ComputeMode_All)
  {
    Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_False);
  }
}

//=======================================================================
//function: DrawArcWithText
//purpose :
//=======================================================================
void AIS_AngleDimension::DrawArcWithText (const Handle(Prs3d_Presentation)& thePresentation,
                                          const gp_Pnt& theFirstAttach,
                                          const gp_Pnt& theSecondAttach,
                                          const gp_Pnt& theCenter,
                                          const TCollection_ExtendedString& theText,
                                          const Standard_Real theTextWidth,
                                          const Standard_Integer theMode,
                                          const Standard_Integer theLabelPosition)
{
  // construct plane where the circle and the arc are located
  gce_MakePln aConstructPlane (theFirstAttach, theSecondAttach, theCenter);
  if (!aConstructPlane.IsDone())
  {
    return;
  }

  gp_Pln aPlane = aConstructPlane.Value();

  Standard_Real aRadius = theFirstAttach.Distance (myCenterPoint);

  // construct circle forming the arc
  gce_MakeCirc aConstructCircle (theCenter, aPlane, aRadius);
  if (!aConstructCircle.IsDone())
  {
    return;
  }

  gp_Circ aCircle = aConstructCircle.Value();

  // compute angle parameters of arc end-points on circle
  Standard_Real aParamBeg = ElCLib::Parameter (aCircle, theFirstAttach);
  Standard_Real aParamEnd = ElCLib::Parameter (aCircle, theSecondAttach);
  ElCLib::AdjustPeriodic (0.0, M_PI * 2, Precision::PConfusion(), aParamBeg, aParamEnd);

  // middle point of arc parameter on circle
  Standard_Real aParamMid = (aParamBeg + aParamEnd) * 0.5;

  // add text graphical primitives
  if (theMode == ComputeMode_All || theMode == ComputeMode_Text)
  {
    gp_Pnt aTextPos = ElCLib::Value (aParamMid, aCircle);
    gp_Dir aTextDir = gce_MakeDir (theFirstAttach, theSecondAttach);

    // Drawing text
    DrawText (thePresentation,
              aTextPos,
              aTextDir,
              theText,
              theLabelPosition);
  }

  if (theMode != ComputeMode_All && theMode != ComputeMode_Line)
  {
    return;
  }

  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();

  Standard_Boolean isLineBreak = aDimensionAspect->TextVerticalPosition() == Prs3d_DTVP_Center
                              && aDimensionAspect->IsText3d();

  if (isLineBreak)
  {
    // compute gap for label as parameteric size of sector on circle segment
    Standard_Real aSectorOnCircle = theTextWidth / aRadius;
  
    gp_Pnt aTextPntBeg = ElCLib::Value (aParamMid - aSectorOnCircle * 0.5, aCircle);
    gp_Pnt aTextPntEnd = ElCLib::Value (aParamMid + aSectorOnCircle * 0.5, aCircle);

    // Drawing arcs
    DrawArc (thePresentation, theFirstAttach, aTextPntBeg, theCenter, aRadius, theMode);
    DrawArc (thePresentation, theSecondAttach, aTextPntEnd, theCenter, aRadius, theMode);
  }
  else
  {
    DrawArc (thePresentation, theFirstAttach, theSecondAttach, theCenter, aRadius, theMode);
  }
}

//=======================================================================
//function : CheckPlane
//purpose  : 
//=======================================================================
Standard_Boolean AIS_AngleDimension::CheckPlane (const gp_Pln& thePlane)const
{
  if (!thePlane.Contains (myFirstPoint, Precision::Confusion()) &&
      !thePlane.Contains (mySecondPoint, Precision::Confusion()) &&
      !thePlane.Contains (myCenterPoint, Precision::Confusion()))
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : ComputePlane
//purpose  : 
//=======================================================================
void AIS_AngleDimension::ComputePlane()
{
  if (!IsValid())
  {
    return;
  }

  gp_Vec aFirstVec   = gp_Vec (myCenterPoint, myFirstPoint).Normalized();
  gp_Vec aSecondVec  = gp_Vec (myCenterPoint, mySecondPoint).Normalized();
  gp_Vec aDirectionN = aSecondVec.Crossed (aFirstVec).Normalized();
  gp_Vec aDirectionY = (aFirstVec + aSecondVec).Normalized();
  gp_Vec aDirectionX = aDirectionY.Crossed (aDirectionN).Normalized();

  myPlane = gp_Pln (gp_Ax3 (myCenterPoint, gp_Dir (aDirectionN), gp_Dir (aDirectionX)));
}

//=======================================================================
//function : GetModelUnits
//purpose  :
//=======================================================================
const TCollection_AsciiString& AIS_AngleDimension::GetModelUnits() const
{
  return myDrawer->DimAngleModelUnits();
}

//=======================================================================
//function : GetDisplayUnits
//purpose  :
//=======================================================================
const TCollection_AsciiString& AIS_AngleDimension::GetDisplayUnits() const
{
  return myDrawer->DimAngleDisplayUnits();
}

//=======================================================================
//function : SetModelUnits
//purpose  :
//=======================================================================
void AIS_AngleDimension::SetModelUnits (const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimAngleModelUnits (theUnits);
}

//=======================================================================
//function : SetDisplayUnits
//purpose  :
//=======================================================================
void AIS_AngleDimension::SetDisplayUnits (const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimAngleDisplayUnits (theUnits);
}

//=======================================================================
//function : ComputeValue
//purpose  : 
//=======================================================================
Standard_Real AIS_AngleDimension::ComputeValue() const
{
  if (!IsValid())
  {
    return 0.0;
  }

  gp_Vec aVec1 (myCenterPoint, myFirstPoint);
  gp_Vec aVec2 (myCenterPoint, mySecondPoint);

  Standard_Real anAngle = aVec2.AngleWithRef (aVec1, GetPlane().Axis().Direction());

  return anAngle > 0.0 ? anAngle : (2.0 * M_PI - anAngle);
}

//=======================================================================
//function : Compute
//purpose  : Having three gp_Pnt points compute presentation
//=======================================================================
void AIS_AngleDimension::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePM*/,
                                  const Handle(Prs3d_Presentation)& thePresentation,
                                  const Standard_Integer theMode)
{
  thePresentation->Clear();
  mySelectionGeom.Clear (theMode);

  if (!IsValid())
  {
    return;
  }

  // Parameters for presentation
  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();

  Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());

  Quantity_Length anArrowLength = aDimensionAspect->ArrowAspect()->Length();

  // prepare label string and compute its geometrical width
  Standard_Real aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString (aLabelWidth);

  // add margins to label width
  if (aDimensionAspect->IsText3d())
  {
    aLabelWidth += aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN * 2.0;
  }

  gp_Pnt aFirstAttach = myCenterPoint.Translated (gp_Vec(myCenterPoint, myFirstPoint).Normalized() * GetFlyout());
  gp_Pnt aSecondAttach = myCenterPoint.Translated (gp_Vec(myCenterPoint, mySecondPoint).Normalized() * GetFlyout());

  // Handle user-defined and automatic arrow placement
  bool isArrowsExternal = false;
  switch (aDimensionAspect->ArrowOrientation())
  {
    case Prs3d_DAO_External: isArrowsExternal = true; break;
    case Prs3d_DAO_Internal: isArrowsExternal = false; break;
    case Prs3d_DAO_Fit:
    {
      gp_Vec anAttachVector (aFirstAttach, aSecondAttach);
      Standard_Real aDimensionWidth = anAttachVector.Magnitude();

      // add margin to ensure a small tail between text and arrow
      Standard_Real anArrowMargin   = aDimensionAspect->IsText3d() 
                                    ? aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN
                                    : 0.0;

      Standard_Real anArrowsWidth   = (anArrowLength + anArrowMargin) * 2.0;

      isArrowsExternal = aDimensionWidth < aLabelWidth + anArrowsWidth;
      break;
    }
  }

  //Arrows positions and directions
  gp_Vec aWPDir = gp_Vec (GetPlane().Axis().Direction());

  gp_Dir aFirstExtensionDir  = aWPDir            ^ gp_Vec (myCenterPoint, aFirstAttach);
  gp_Dir aSecondExtensionDir = aWPDir.Reversed() ^ gp_Vec (myCenterPoint, aSecondAttach);

  gp_Vec aFirstArrowVec  = gp_Vec (aFirstExtensionDir)  * anArrowLength;
  gp_Vec aSecondArrowVec = gp_Vec (aSecondExtensionDir) * anArrowLength;

  gp_Pnt aFirstArrowBegin  (0.0, 0.0, 0.0);
  gp_Pnt aFirstArrowEnd    (0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowBegin (0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowEnd   (0.0, 0.0, 0.0);

  if (isArrowsExternal)
  {
    aFirstArrowVec.Reverse();
    aSecondArrowVec.Reverse();
  }

  aFirstArrowBegin  = aFirstAttach;
  aSecondArrowBegin = aSecondAttach;
  aFirstArrowEnd    = aFirstAttach.Translated (-aFirstArrowVec);
  aSecondArrowEnd   = aSecondAttach.Translated (-aSecondArrowVec);

  Standard_Integer aLabelPosition = LabelPosition_None;

  // Handle user-defined and automatic text placement
  switch (aDimensionAspect->TextHorizontalPosition())
  {
    case Prs3d_DTHP_Left  : aLabelPosition |= LabelPosition_Left; break;
    case Prs3d_DTHP_Right : aLabelPosition |= LabelPosition_Right; break;
    case Prs3d_DTHP_Center: aLabelPosition |= LabelPosition_HCenter; break;
    case Prs3d_DTHP_Fit:
    {
      gp_Vec anAttachVector (aFirstAttach, aSecondAttach);
      Standard_Real aDimensionWidth = anAttachVector.Magnitude();
      Standard_Real anArrowsWidth   = anArrowLength * 2.0;
      Standard_Real aContentWidth   = isArrowsExternal ? aLabelWidth : aLabelWidth + anArrowsWidth;

      aLabelPosition |= aDimensionWidth < aContentWidth ? LabelPosition_Left : LabelPosition_HCenter;
      break;
    }
  }

  switch (aDimensionAspect->TextVerticalPosition())
  {
    case Prs3d_DTVP_Above  : aLabelPosition |= LabelPosition_Above; break;
    case Prs3d_DTVP_Below  : aLabelPosition |= LabelPosition_Below; break;
    case Prs3d_DTVP_Center : aLabelPosition |= LabelPosition_VCenter; break;
  }

  // Group1: stenciling text and the angle dimension arc
  Prs3d_Root::NewGroup (thePresentation);

  Standard_Real anExtensionSize = aDimensionAspect->ExtensionSize();

  Standard_Integer aHPosition = aLabelPosition & LabelPosition_HMask;

  // draw text label
  switch (aHPosition)
  {
    case LabelPosition_HCenter :
    {
      Standard_Boolean isLineBreak = aDimensionAspect->TextVerticalPosition() == Prs3d_DTVP_Center
                                  && aDimensionAspect->IsText3d();

      if (isLineBreak)
      {
        DrawArcWithText (thePresentation,
                         aFirstAttach,
                         aSecondAttach,
                         myCenterPoint,
                         aLabelString,
                         aLabelWidth,
                         theMode,
                         aLabelPosition);
        break;
      }

      // compute text primitives
      if (theMode == ComputeMode_All || theMode == ComputeMode_Text)
      {
        gp_Vec aDimensionDir (aFirstAttach, aSecondAttach);
        gp_Pnt aTextPos = GetCenterOnArc (aFirstAttach, aSecondAttach, myCenterPoint);
        gp_Dir aTextDir = aDimensionDir;

        DrawText (thePresentation,
                  aTextPos,
                  aTextDir,
                  aLabelString,
                  aLabelPosition);
      }

      if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
      {
        DrawArc (thePresentation,
                 isArrowsExternal ? aFirstAttach : aFirstArrowEnd,
                 isArrowsExternal ? aSecondAttach : aSecondArrowEnd,
                 myCenterPoint,
                 Abs (GetFlyout()),
                 theMode);
      }
    }
    break;

    case LabelPosition_Left :
    {
      DrawExtension (thePresentation,
                     anExtensionSize,
                     isArrowsExternal ? aFirstArrowEnd : aFirstAttach,
                     aFirstExtensionDir,
                     aLabelString,
                     aLabelWidth,
                     theMode,
                     aLabelPosition);
    }
    break;

    case LabelPosition_Right :
    {
      DrawExtension (thePresentation,
                     anExtensionSize,
                     isArrowsExternal ? aSecondArrowEnd : aSecondAttach,
                     aSecondExtensionDir,
                     aLabelString,
                     aLabelWidth,
                     theMode,
                     aLabelPosition);
    }
    break;
  }

  // dimension arc without text
  if ((theMode == ComputeMode_All || theMode == ComputeMode_Line) && aHPosition != LabelPosition_HCenter)
  {
    Prs3d_Root::NewGroup (thePresentation);

    DrawArc (thePresentation,
             isArrowsExternal ? aFirstAttach  : aFirstArrowEnd,
             isArrowsExternal ? aSecondAttach : aSecondArrowEnd,
             myCenterPoint,
             Abs(GetFlyout ()),
             theMode);
  }

  // arrows and arrow extensions
  if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
  {
    Prs3d_Root::NewGroup (thePresentation);

    DrawArrow (thePresentation, aFirstArrowBegin,  gp_Dir (aFirstArrowVec));
    DrawArrow (thePresentation, aSecondArrowBegin, gp_Dir (aSecondArrowVec));
  }

  if ((theMode == ComputeMode_All || theMode == ComputeMode_Line) && isArrowsExternal)
  {
    Prs3d_Root::NewGroup (thePresentation);

    if (aHPosition != LabelPosition_Left)
    {
      DrawExtension (thePresentation,
                     anExtensionSize,
                     aFirstArrowEnd,
                     aFirstExtensionDir,
                     THE_EMPTY_LABEL_STRING,
                     THE_EMPTY_LABEL_WIDTH,
                     theMode,
                     LabelPosition_None);
    }

    if (aHPosition != LabelPosition_Right)
    {
      DrawExtension (thePresentation,
                     anExtensionSize,
                     aSecondArrowEnd,
                     aSecondExtensionDir,
                     THE_EMPTY_LABEL_STRING,
                     THE_EMPTY_LABEL_WIDTH,
                     theMode,
                     LabelPosition_None);
    }
  }

  // flyouts
  if (theMode == ComputeMode_All)
  {
    Prs3d_Root::NewGroup (thePresentation);

    Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments (4);
    aPrimSegments->AddVertex (myCenterPoint);
    aPrimSegments->AddVertex (aFirstAttach);
    aPrimSegments->AddVertex (myCenterPoint);
    aPrimSegments->AddVertex (aSecondAttach);

    Handle(Graphic3d_AspectLine3d) aFlyoutStyle = myDrawer->DimensionAspect()->LineAspect()->Aspect();
    Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aFlyoutStyle);
    Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
  }

  myIsComputed = Standard_True;
}

//=======================================================================
//function : ComputeFlyoutSelection
//purpose  : computes selection for flyouts
//=======================================================================
void AIS_AngleDimension::ComputeFlyoutSelection (const Handle(SelectMgr_Selection)& theSelection,
                                                 const Handle(SelectMgr_EntityOwner)& theOwner)
{
  gp_Pnt aFirstAttach  = myCenterPoint.Translated (gp_Vec (myCenterPoint, myFirstPoint).Normalized()  * GetFlyout());
  gp_Pnt aSecondAttach = myCenterPoint.Translated (gp_Vec (myCenterPoint, mySecondPoint).Normalized() * GetFlyout());

  Handle(Select3D_SensitiveGroup) aSensitiveEntity = new Select3D_SensitiveGroup (theOwner);
  aSensitiveEntity->Add (new Select3D_SensitiveSegment (theOwner, myCenterPoint, aFirstAttach));
  aSensitiveEntity->Add (new Select3D_SensitiveSegment (theOwner, myCenterPoint, aSecondAttach));

  theSelection->Add (aSensitiveEntity);
}

//=======================================================================
//function : InitTwoEdgesAngle
//purpose  : 
//=======================================================================
Standard_Boolean AIS_AngleDimension::InitTwoEdgesAngle (gp_Pln& theComputedPlane)
{
  TopoDS_Edge aFirstEdge  = TopoDS::Edge (myFirstShape);
  TopoDS_Edge aSecondEdge = TopoDS::Edge (mySecondShape);

  BRepAdaptor_Curve aMakeFirstLine  (aFirstEdge);
  BRepAdaptor_Curve aMakeSecondLine (aSecondEdge);

  if (aMakeFirstLine.GetType() != GeomAbs_Line || aMakeSecondLine.GetType() != GeomAbs_Line)
  {
    return  Standard_False;
  }

  Handle(Geom_Line) aFirstLine  = new Geom_Line (aMakeFirstLine.Line());
  Handle(Geom_Line) aSecondLine = new Geom_Line (aMakeSecondLine.Line());

  gp_Lin aFirstLin  = aFirstLine->Lin();
  gp_Lin aSecondLin = aSecondLine->Lin();

  Standard_Boolean isParallelLines = Abs (aFirstLin.Angle (aSecondLin) - M_PI) <= Precision::Angular();

  gp_Pnt aPoint  = aFirstLine->Value (0.0);
  gp_Dir aNormal = isParallelLines
                     ? gp_Vec (aSecondLin.Normal (aPoint).Direction()) ^ gp_Vec (aSecondLin.Direction())
                     : gp_Vec (aFirstLin.Direction()) ^ gp_Vec (aSecondLin.Direction());

  theComputedPlane = gp_Pln (aPoint, aNormal);

    // Compute geometry for this plane and edges
  Standard_Boolean isInfinite1,isInfinite2;
  gp_Pnt aFirstPoint1, aLastPoint1, aFirstPoint2, aLastPoint2;
  gp_Lin2d aFirstLin2d, aSecondLin2d;

  if (!AIS::ComputeGeometry (aFirstEdge, aSecondEdge,
                             aFirstLine, aSecondLine,
                             aFirstPoint1, aLastPoint1,
                             aFirstPoint2, aLastPoint2,
                             isInfinite1, isInfinite2))
  {
    return Standard_False;
  }

  if (aFirstLin.Direction().IsParallel (aSecondLin.Direction(), Precision::Angular()))
  {
    myFirstPoint  = aFirstLin.Location();
    mySecondPoint = ElCLib::Value (ElCLib::Parameter (aFirstLin, myFirstPoint), aSecondLin);

    if (mySecondPoint.Distance (myFirstPoint) <= Precision::Confusion())
    {
      mySecondPoint.Translate (gp_Vec (aSecondLin.Direction()) * Abs (GetFlyout()));
    }

    myCenterPoint.SetXYZ ((myFirstPoint.XYZ() + mySecondPoint.XYZ()) / 2.0);
  }
  else
  {
    // Find intersection
    gp_Lin2d aFirstLin2d  = ProjLib::Project (theComputedPlane, aFirstLin);
    gp_Lin2d aSecondLin2d = ProjLib::Project (theComputedPlane, aSecondLin);

    IntAna2d_AnaIntersection anInt2d (aFirstLin2d, aSecondLin2d);
    gp_Pnt2d anIntersectPoint;
    if (!anInt2d.IsDone() || anInt2d.IsEmpty())
    {
      return Standard_False;
    }

    anIntersectPoint = gp_Pnt2d (anInt2d.Point(1).Value());
    myCenterPoint = ElCLib::To3d (theComputedPlane.Position().Ax2(), anIntersectPoint);

    if (isInfinite1 || isInfinite2)
    {
      myFirstPoint  = myCenterPoint.Translated (gp_Vec (aFirstLin.Direction()) * Abs (GetFlyout()));
      mySecondPoint = myCenterPoint.Translated (gp_Vec (aSecondLin.Direction()) * Abs (GetFlyout()));

      return IsValidPoints (myFirstPoint, myCenterPoint, mySecondPoint);
    }

    // |
    // | <- dimension should be here
    // *----
    myFirstPoint  = myCenterPoint.Distance (aFirstPoint1) > myCenterPoint.Distance (aLastPoint1)
                  ? aFirstPoint1
                  : aLastPoint1;

    mySecondPoint = myCenterPoint.Distance (aFirstPoint2) > myCenterPoint.Distance (aLastPoint2)
                  ? aFirstPoint2
                  : aLastPoint2;
  }

  return IsValidPoints (myFirstPoint, myCenterPoint, mySecondPoint);
}

//=======================================================================
//function : InitTwoFacesAngle
//purpose  : initialization of angle dimension between two faces
//=======================================================================
Standard_Boolean AIS_AngleDimension::InitTwoFacesAngle()
{
  TopoDS_Face aFirstFace = TopoDS::Face (myFirstShape);
  TopoDS_Face aSecondFace = TopoDS::Face (mySecondShape);

  gp_Dir aFirstDir, aSecondDir;
  gp_Pln aFirstPlane, aSecondPlane;
  Handle(Geom_Surface) aFirstBasisSurf, aSecondBasisSurf;
  AIS_KindOfSurface aFirstSurfType, aSecondSurfType;
  Standard_Real aFirstOffset, aSecondOffset;

  AIS::GetPlaneFromFace (aFirstFace, aFirstPlane,
                         aFirstBasisSurf,aFirstSurfType,aFirstOffset);

  AIS::GetPlaneFromFace (aSecondFace, aSecondPlane,
                         aSecondBasisSurf, aSecondSurfType, aSecondOffset);

  if (aFirstSurfType == AIS_KOS_Plane && aSecondSurfType == AIS_KOS_Plane)
  {
    //Planar faces angle
    Handle(Geom_Plane) aFirstPlane = Handle(Geom_Plane)::DownCast (aFirstBasisSurf);
    Handle(Geom_Plane) aSecondPlane = Handle(Geom_Plane)::DownCast (aSecondBasisSurf);
    return AIS::InitAngleBetweenPlanarFaces (aFirstFace,
                                             aSecondFace,
                                             myCenterPoint,
                                             myFirstPoint,
                                             mySecondPoint)
           && IsValidPoints (myFirstPoint,
                             myCenterPoint,
                             mySecondPoint);
  }
  else
  {
    // Curvilinear faces angle
    return AIS::InitAngleBetweenCurvilinearFaces (aFirstFace,
                                                  aSecondFace,
                                                  aFirstSurfType,
                                                  aSecondSurfType,
                                                  myCenterPoint,
                                                  myFirstPoint,
                                                  mySecondPoint)
           && IsValidPoints (myFirstPoint,
                             myCenterPoint,
                             mySecondPoint);
  }
}

//=======================================================================
//function : InitTwoFacesAngle
//purpose  : initialization of angle dimension between two faces
//=======================================================================
Standard_Boolean AIS_AngleDimension::InitTwoFacesAngle (const gp_Pnt thePointOnFirstFace)
{
  TopoDS_Face aFirstFace = TopoDS::Face (myFirstShape);
  TopoDS_Face aSecondFace = TopoDS::Face (mySecondShape);

  gp_Dir aFirstDir, aSecondDir;
  gp_Pln aFirstPlane, aSecondPlane;
  Handle(Geom_Surface) aFirstBasisSurf, aSecondBasisSurf;
  AIS_KindOfSurface aFirstSurfType, aSecondSurfType;
  Standard_Real aFirstOffset, aSecondOffset;

  AIS::GetPlaneFromFace (aFirstFace, aFirstPlane,
                         aFirstBasisSurf,aFirstSurfType,aFirstOffset);

  AIS::GetPlaneFromFace (aSecondFace, aSecondPlane,
                         aSecondBasisSurf, aSecondSurfType, aSecondOffset);

  myFirstPoint = thePointOnFirstFace;
  if (aFirstSurfType == AIS_KOS_Plane && aSecondSurfType == AIS_KOS_Plane)
  {
    //Planar faces angle
    Handle(Geom_Plane) aFirstPlane = Handle(Geom_Plane)::DownCast (aFirstBasisSurf);
    Handle(Geom_Plane) aSecondPlane = Handle(Geom_Plane)::DownCast (aSecondBasisSurf);
    return AIS::InitAngleBetweenPlanarFaces (aFirstFace,
                                             aSecondFace,
                                             myCenterPoint,
                                             myFirstPoint,
                                             mySecondPoint,
                                             Standard_True)
           && IsValidPoints (myFirstPoint,
                             myCenterPoint,
                             mySecondPoint);
  }
  else
  {
    // Curvilinear faces angle
    return AIS::InitAngleBetweenCurvilinearFaces (aFirstFace,
                                                  aSecondFace,
                                                  aFirstSurfType,
                                                  aSecondSurfType,
                                                  myCenterPoint,
                                                  myFirstPoint,
                                                  mySecondPoint,
                                                  Standard_True)
           && IsValidPoints (myFirstPoint,
                             myCenterPoint,
                             mySecondPoint);
  }
}

//=======================================================================
//function : InitConeAngle
//purpose  : initialization of the cone angle
//=======================================================================
Standard_Boolean AIS_AngleDimension::InitConeAngle()
{
  if (myFirstShape.IsNull())
  {
    return Standard_False;
  }

  TopoDS_Face aConeShape = TopoDS::Face (myFirstShape);
  gp_Pln aPln;
  gp_Cone aCone;
  gp_Circ aCircle;
  // A surface from the Face
  Handle(Geom_Surface) aSurf;
  Handle(Geom_OffsetSurface) aOffsetSurf; 
  Handle(Geom_ConicalSurface) aConicalSurf;
  Handle(Geom_SurfaceOfRevolution) aRevSurf;
  Handle(Geom_Line) aLine;
  BRepAdaptor_Surface aConeAdaptor (aConeShape);
  TopoDS_Face aFace;
  AIS_KindOfSurface aSurfType;
  Standard_Real anOffset = 0.;
  Handle(Standard_Type) aType;

  Standard_Real aMaxV = aConeAdaptor.FirstVParameter();
  Standard_Real aMinV = aConeAdaptor.LastVParameter();

  AIS::GetPlaneFromFace (aConeShape, aPln, aSurf, aSurfType, anOffset);

  if (aSurfType == AIS_KOS_Revolution)
  {
    // Surface of revolution
    aRevSurf = Handle(Geom_SurfaceOfRevolution)::DownCast(aSurf);
    gp_Lin aLin (aRevSurf->Axis());
    Handle(Geom_Curve) aBasisCurve = aRevSurf->BasisCurve();
    //Must be a part of line (basis curve should be linear)
    if (aBasisCurve ->DynamicType() != STANDARD_TYPE(Geom_Line))
      return Standard_False;

    gp_Pnt aFirst1 = aConeAdaptor.Value (0., aMinV);
    gp_Pnt aLast1 = aConeAdaptor.Value (0., aMaxV);
    gp_Vec aVec1 (aFirst1, aLast1);

    //Projection <aFirst> on <aLin>
    gp_Pnt aFirst2 = ElCLib::Value (ElCLib::Parameter (aLin, aFirst1), aLin);
    // Projection <aLast> on <aLin>
    gp_Pnt aLast2 = ElCLib::Value (ElCLib::Parameter (aLin, aLast1), aLin);

    gp_Vec aVec2 (aFirst2, aLast2);

    // Check if two parts of revolution are parallel (it's a cylinder) or normal (it's a circle).
    if (aVec1.IsParallel (aVec2, Precision::Angular())
        || aVec1.IsNormal (aVec2,Precision::Angular()))
      return Standard_False;

    gce_MakeCone aMkCone (aRevSurf->Axis(), aFirst1, aLast1);
    aCone =  aMkCone.Value();
    myCenterPoint = aCone.Apex();
  }
  else
  {
    aType = aSurf->DynamicType();
    if (aType == STANDARD_TYPE(Geom_OffsetSurface) || anOffset > 0.01)
    {
      // Offset surface
      aOffsetSurf = new Geom_OffsetSurface (aSurf, anOffset);
      aSurf = aOffsetSurf->Surface();
      BRepBuilderAPI_MakeFace aMkFace(aSurf, Precision::Confusion());
      aMkFace.Build();
      if (!aMkFace.IsDone())
        return Standard_False;
      aConeAdaptor.Initialize (aMkFace.Face());
    }
    aCone = aConeAdaptor.Cone();
    aConicalSurf = Handle(Geom_ConicalSurface)::DownCast (aSurf);
    myCenterPoint =  aConicalSurf->Apex();
  }

  // A circle where the angle is drawn
  Handle(Geom_Curve) aCurve;
  Standard_Real aMidV = ( aMinV + aMaxV ) / 2.5;
  aCurve = aSurf->VIso (aMidV);
  aCircle = Handle(Geom_Circle)::DownCast (aCurve)->Circ();

  aCurve = aSurf->VIso(aMaxV);
  gp_Circ aCircVmax = Handle(Geom_Circle)::DownCast(aCurve)->Circ();
  aCurve = aSurf->VIso(aMinV);
  gp_Circ aCircVmin = Handle(Geom_Circle)::DownCast(aCurve)->Circ();

  if (aCircVmax.Radius() < aCircVmin.Radius())
  {
   gp_Circ aTmpCirc = aCircVmax;
   aCircVmax = aCircVmin;
   aCircVmin = aTmpCirc;
  }

  myFirstPoint  = ElCLib::Value (0, aCircle);
  mySecondPoint = ElCLib::Value (M_PI, aCircle);
  return Standard_True;
}

//=======================================================================
//function : IsValidPoints
//purpose  : 
//=======================================================================
Standard_Boolean AIS_AngleDimension::IsValidPoints (const gp_Pnt& theFirstPoint,
                                                    const gp_Pnt& theCenterPoint,
                                                    const gp_Pnt& theSecondPoint) const
{
  return theFirstPoint.Distance (theCenterPoint) > Precision::Confusion()
      && theSecondPoint.Distance (theCenterPoint) > Precision::Confusion()
      && gp_Vec (theCenterPoint, theFirstPoint).Angle (
           gp_Vec (theCenterPoint, theSecondPoint)) > Precision::Angular();
}
