// Created on: 1996-12-05
// Created by: Arnaud BOUZY/Odile Olivier
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

#include <AIS_AngleDimension.hxx>

#include <AIS.hxx>
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
#include <Standard_ProgramError.hxx>
#include <UnitsAPI.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>


IMPLEMENT_STANDARD_RTTIEXT(AIS_AngleDimension,AIS_Dimension)

namespace
{
  static const TCollection_ExtendedString THE_EMPTY_LABEL_STRING;
  static const Standard_Real              THE_EMPTY_LABEL_WIDTH = 0.0;
  static const Standard_ExtCharacter      THE_DEGREE_SYMBOL (0x00B0);
  static const Standard_Real              THE_3D_TEXT_MARGIN = 0.1;

  //! Returns true if the given points lie on a same line.
  static Standard_Boolean isSameLine (const gp_Pnt& theFirstPoint,
                                      const gp_Pnt& theCenterPoint,
                                      const gp_Pnt& theSecondPoint)
  {
    gp_Vec aVec1 (theFirstPoint, theCenterPoint);
    gp_Vec aVec2 (theCenterPoint, theSecondPoint);

    return aVec1.IsParallel (aVec2, Precision::Angular());
  }
}

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

  myFirstShape      = theFirstEdge;
  mySecondShape     = theSecondEdge;
  myThirdShape      = TopoDS_Shape();
  myGeometryType    = GeometryType_Edges;
  myIsGeometryValid = InitTwoEdgesAngle (aComputedPlane);

  if (myIsGeometryValid && !myIsPlaneCustom)
  {
    myPlane = aComputedPlane;
  }

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
  myIsGeometryValid       = IsValidPoints (myFirstPoint, myCenterPoint, mySecondPoint);

  Standard_Boolean anIsSameLine = isSameLine (myFirstPoint, myCenterPoint, mySecondPoint);
  if (myIsGeometryValid && !myIsPlaneCustom && !anIsSameLine)
  {
    ComputePlane();
  }

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
  myFirstShape      = theFirstVertex;
  mySecondShape     = theSecondVertex;
  myThirdShape      = theThirdVertex;
  myFirstPoint      = BRep_Tool::Pnt (theFirstVertex);
  myCenterPoint     = BRep_Tool::Pnt (theSecondVertex);
  mySecondPoint     = BRep_Tool::Pnt (theThirdVertex);
  myGeometryType    = GeometryType_Points;
  myIsGeometryValid = IsValidPoints (myFirstPoint, myCenterPoint, mySecondPoint);

  Standard_Boolean anIsSameLine = isSameLine (myFirstPoint, myCenterPoint, mySecondPoint);
  if (myIsGeometryValid && !myIsPlaneCustom && !anIsSameLine)
  {
    ComputePlane();
  }

  SetToUpdate();
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_AngleDimension::SetMeasuredGeometry (const TopoDS_Face& theCone)
{
  myFirstShape      = theCone;
  mySecondShape     = TopoDS_Shape();
  myThirdShape      = TopoDS_Shape();
  myGeometryType    = GeometryType_Face;
  myIsGeometryValid = InitConeAngle();

  if (myIsGeometryValid && !myIsPlaneCustom)
  {
    ComputePlane();
  }

  SetToUpdate();
}

//=======================================================================
//function : SetMeasuredGeometry
//purpose  : 
//=======================================================================
void AIS_AngleDimension::SetMeasuredGeometry (const TopoDS_Face& theFirstFace,
                                              const TopoDS_Face& theSecondFace)
{
  myFirstShape      = theFirstFace;
  mySecondShape     = theSecondFace;
  myThirdShape      = TopoDS_Shape();
  myGeometryType    = GeometryType_Faces;
  myIsGeometryValid = InitTwoFacesAngle();

  if (myIsGeometryValid && !myIsPlaneCustom)
  {
    ComputePlane();
  }

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
  myFirstShape      = theFirstFace;
  mySecondShape     = theSecondFace;
  myThirdShape      = TopoDS_Shape();
  myGeometryType    = GeometryType_Faces;
  myIsGeometryValid = InitTwoFacesAngle (thePoint);

  if (myIsGeometryValid && !myIsPlaneCustom)
  {
    ComputePlane();
  }

  SetToUpdate();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void AIS_AngleDimension::Init()
{
  SetType (AIS_TOA_Interior);
  SetArrowsVisibility (AIS_TOAV_Both);
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
                                           const gp_Pnt& theCenter) const
{
  // construct plane where the circle and the arc are located
  gce_MakePln aConstructPlane (theFirstAttach, theSecondAttach, theCenter);
  if (!aConstructPlane.IsDone())
  {
    return gp::Origin();
  }
  
  gp_Pln aPlane = aConstructPlane.Value();
  // to have an exterior angle presentation, a plane for further constructed circle should be reversed
  if (myType == AIS_TOA_Exterior)
  {
    gp_Ax1 anAxis = aPlane.Axis();
    gp_Dir aDir = anAxis.Direction();
    aDir.Reverse();
    aPlane.SetAxis(gp_Ax1(anAxis.Location(), aDir));
  }

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
//function : GetNormalForMinAngle
//purpose  :
//=======================================================================
gp_Dir AIS_AngleDimension::GetNormalForMinAngle() const
{
  const gp_Dir& aNormal = myPlane.Axis().Direction();
  gp_Dir aFirst (gp_Vec (myCenterPoint, myFirstPoint) );
  gp_Dir aSecond (gp_Vec (myCenterPoint, mySecondPoint) );

  return aFirst.AngleWithRef (aSecond, aNormal) < 0.0
    ? aNormal.Reversed()
    : aNormal;
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
  gp_Pln aPlane (myCenterPoint, GetNormalForMinAngle());

  // to have an exterior angle presentation, a plane for further constructed circle should be reversed
  if (myType == AIS_TOA_Exterior)
  {
    gp_Ax1 anAxis = aPlane.Axis();
    gp_Dir aDir = anAxis.Direction();
    aDir.Reverse();
    aPlane.SetAxis(gp_Ax1(anAxis.Location(), aDir));
  }

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
  Standard_Real anAngle = aCenterToFirstVec.Angle (aCenterToSecondVec);
  if (myType == AIS_TOA_Exterior)
    anAngle = 2.0 * M_PI - anAngle;
  // it sets 50 points on PI, and a part of points if angle is less
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
  gp_Pln aPlane (myCenterPoint, GetNormalForMinAngle());
  
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
    drawText (thePresentation,
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
    Standard_Real aSectorOfText = theTextWidth / aRadius;
    Standard_Real aTextBegin = aParamMid - aSectorOfText * 0.5;
    Standard_Real aTextEnd = aParamMid + aSectorOfText * 0.5;
    gp_Pnt aTextPntBeg = ElCLib::Value (aTextBegin, aCircle);
    gp_Pnt aTextPntEnd = ElCLib::Value (aTextEnd, aCircle);

    // Drawing arcs
    if (aTextBegin > aParamBeg)
    {
      DrawArc (thePresentation, theFirstAttach, aTextPntBeg, theCenter, aRadius, theMode);
    }
    if (aTextEnd < aParamEnd)
    {
      DrawArc (thePresentation, aTextPntEnd, theSecondAttach, theCenter, aRadius, theMode);
    }
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
  if (!myIsGeometryValid)
  {
    return;
  }

  // Compute working plane so that Y axis is codirectional
  // with Y axis of text coordinate system (necessary for text alignment)
  gp_Vec aFirstVec   = gp_Vec (myCenterPoint, myFirstPoint);
  gp_Vec aSecondVec  = gp_Vec (myCenterPoint, mySecondPoint);
  gp_Vec aDirectionN = aSecondVec ^ aFirstVec;
  gp_Vec aDirectionY = aFirstVec + aSecondVec;
  gp_Vec aDirectionX = aDirectionY ^ aDirectionN;

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

  Standard_Real anAngle = aVec1.AngleWithRef (aVec2, GetNormalForMinAngle());

  return anAngle > 0.0 ? anAngle : (2.0 * M_PI + anAngle);
}

//=======================================================================
//function : Compute
//purpose  : Having three gp_Pnt points compute presentation
//=======================================================================
void AIS_AngleDimension::Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePM*/,
                                  const Handle(Prs3d_Presentation)& thePresentation,
                                  const Standard_Integer theMode)
{
  mySelectionGeom.Clear (theMode);

  if (!IsValid())
  {
    return;
  }

  // Parameters for presentation
  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();

  Prs3d_Root::CurrentGroup(thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());

  Standard_Real anArrowLength = aDimensionAspect->ArrowAspect()->Length();

  // prepare label string and compute its geometrical width
  Standard_Real aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString (aLabelWidth);

  // add margins to label width
  if (aDimensionAspect->IsText3d())
  {
    aLabelWidth += aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN * 2.0;
  }

  // Get parameters from aspect or adjust it according with custom text position
  Standard_Real anExtensionSize = aDimensionAspect->ExtensionSize();
  Prs3d_DimensionTextHorizontalPosition aHorisontalTextPos = aDimensionAspect->TextHorizontalPosition();

  if (IsTextPositionCustom())
  {
    AdjustParameters (myFixedTextPosition,anExtensionSize, aHorisontalTextPos, myFlyout);
  }

  // Handle user-defined and automatic arrow placement
  Standard_Boolean isArrowsExternal = Standard_False;
  Standard_Integer aLabelPosition = LabelPosition_None;

  FitTextAlignment (aHorisontalTextPos, aLabelPosition, isArrowsExternal);

  gp_Pnt aFirstAttach = myCenterPoint.Translated (gp_Vec(myCenterPoint, myFirstPoint).Normalized() * GetFlyout());
  gp_Pnt aSecondAttach = myCenterPoint.Translated (gp_Vec(myCenterPoint, mySecondPoint).Normalized() * GetFlyout());

  //Arrows positions and directions
  gp_Vec aWorkingPlaneDir (GetNormalForMinAngle());

  gp_Dir aFirstExtensionDir  = aWorkingPlaneDir.Reversed() ^ gp_Vec (myCenterPoint, aFirstAttach);
  gp_Dir aSecondExtensionDir = aWorkingPlaneDir            ^ gp_Vec (myCenterPoint, aSecondAttach);

  gp_Vec aFirstArrowVec  = gp_Vec (aFirstExtensionDir)  * anArrowLength;
  gp_Vec aSecondArrowVec = gp_Vec (aSecondExtensionDir) * anArrowLength;

  if (isArrowsExternal)
  {
    aFirstArrowVec.Reverse();
    aSecondArrowVec.Reverse();
  }

  gp_Pnt aFirstArrowBegin  (0.0, 0.0, 0.0);
  gp_Pnt aFirstArrowEnd    (0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowBegin (0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowEnd   (0.0, 0.0, 0.0);

  aFirstArrowBegin  = aFirstAttach;
  aSecondArrowBegin = aSecondAttach;
  aFirstArrowEnd    = aFirstAttach.Translated (-aFirstArrowVec);
  aSecondArrowEnd   = aSecondAttach.Translated (-aSecondArrowVec);

  // Group1: stenciling text and the angle dimension arc
  Prs3d_Root::NewGroup (thePresentation);

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
        gp_Pnt aTextPos = IsTextPositionCustom() ? myFixedTextPosition
                                                : GetCenterOnArc (aFirstAttach, aSecondAttach, myCenterPoint);
        gp_Dir aTextDir = aDimensionDir;

        drawText (thePresentation,
                  aTextPos,
                  aTextDir,
                  aLabelString,
                  aLabelPosition);
      }

      if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
      {
        DrawArc (thePresentation,
                 (isArrowsExternal || !isArrowVisible(AIS_TOAV_First)) ? aFirstAttach : aFirstArrowEnd,
                 (isArrowsExternal || !isArrowVisible(AIS_TOAV_Second)) ? aSecondAttach : aSecondArrowEnd,
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
                     (isArrowsExternal && isArrowVisible(AIS_TOAV_First)) ? aFirstArrowEnd : aFirstAttach,
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
                     (isArrowsExternal && isArrowVisible(AIS_TOAV_Second)) ? aSecondArrowEnd : aSecondAttach,
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
             (isArrowsExternal || !isArrowVisible(AIS_TOAV_First)) ? aFirstAttach  : aFirstArrowEnd,
             (isArrowsExternal || !isArrowVisible(AIS_TOAV_Second)) ? aSecondAttach : aSecondArrowEnd,
             myCenterPoint,
             Abs(GetFlyout ()),
             theMode);
  }

  // arrows and arrow extensions
  if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
  {
    Prs3d_Root::NewGroup (thePresentation);

    if (isArrowVisible(AIS_TOAV_First))
      DrawArrow (thePresentation, aFirstArrowBegin,  gp_Dir (aFirstArrowVec));
    if (isArrowVisible(AIS_TOAV_Second))
      DrawArrow (thePresentation, aSecondArrowBegin, gp_Dir (aSecondArrowVec));
  }

  if ((theMode == ComputeMode_All || theMode == ComputeMode_Line) && isArrowsExternal)
  {
    Prs3d_Root::NewGroup (thePresentation);

    if (aHPosition != LabelPosition_Left && isArrowVisible(AIS_TOAV_First))
    {
      DrawExtension (thePresentation,
                     aDimensionAspect->ArrowTailSize(),
                     aFirstArrowEnd,
                     aFirstExtensionDir,
                     THE_EMPTY_LABEL_STRING,
                     THE_EMPTY_LABEL_WIDTH,
                     theMode,
                     LabelPosition_None);
    }

    if (aHPosition != LabelPosition_Right && isArrowVisible(AIS_TOAV_Second))
    {
      DrawExtension (thePresentation,
                     aDimensionAspect->ArrowTailSize(),
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

  mySelectionGeom.IsComputed = Standard_True;
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

  Standard_Boolean isParallelLines = aFirstLin.Direction().IsParallel (aSecondLin.Direction(), Precision::Angular());

  theComputedPlane = isParallelLines ? gp_Pln(gp::XOY())
                                     : gp_Pln (aSecondLin.Location(), gp_Vec (aFirstLin.Direction()) ^ gp_Vec (aSecondLin.Direction()));

  // Compute geometry for this plane and edges
  Standard_Boolean isInfinite1,isInfinite2;
  gp_Pnt aFirstPoint1, aLastPoint1, aFirstPoint2, aLastPoint2;
  Handle(Geom_Curve) aFirstCurve = aFirstLine, aSecondCurve = aSecondLine;
  if (!AIS::ComputeGeometry (aFirstEdge, aSecondEdge,
                             aFirstCurve, aSecondCurve,
                             aFirstPoint1, aLastPoint1,
                             aFirstPoint2, aLastPoint2,
                             isInfinite1, isInfinite2))
  {
    return Standard_False;
  }

  Standard_Boolean isSameLines = aFirstLin.Direction().IsEqual (aSecondLin.Direction(), Precision::Angular())
                              && aFirstLin.Location().IsEqual (aSecondLin.Location(),Precision::Confusion());

  // It can be the same gp_Lin geometry but the different begin and end parameters
  Standard_Boolean isSameEdges =
    (aFirstPoint1.IsEqual (aFirstPoint2, Precision::Confusion()) && aLastPoint1.IsEqual (aLastPoint2, Precision::Confusion()))
    || (aFirstPoint1.IsEqual (aLastPoint2, Precision::Confusion()) && aLastPoint1.IsEqual (aFirstPoint2, Precision::Confusion()));

  if (isParallelLines)
  {
    // Zero angle, it could not handle this geometry
    if (isSameLines && isSameEdges)
    {
      return Standard_False;
    }

    // Handle the case of Pi angle
    const Standard_Real aParam11 = ElCLib::Parameter (aFirstLin, aFirstPoint1);
    const Standard_Real aParam12 = ElCLib::Parameter (aFirstLin, aLastPoint1);
    const Standard_Real aParam21 = ElCLib::Parameter (aFirstLin, aFirstPoint2);
    const Standard_Real aParam22 = ElCLib::Parameter (aFirstLin, aLastPoint2);
    myCenterPoint = ElCLib::Value ( (Min (aParam11, aParam12) + Max (aParam21, aParam22)) * 0.5, aFirstLin);
    myFirstPoint = myCenterPoint.Translated (gp_Vec (aFirstLin.Direction()) * Abs (GetFlyout()));
    mySecondPoint = myCenterPoint.XYZ() + (aFirstLin.Direction().IsEqual (aSecondLin.Direction(), Precision::Angular())
      ? aFirstLin.Direction().Reversed().XYZ() * Abs (GetFlyout())
      : aSecondLin.Direction().XYZ() * Abs (GetFlyout()));
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
  gp_Pln aFirstPln, aSecondPln;
  Handle(Geom_Surface) aFirstBasisSurf, aSecondBasisSurf;
  AIS_KindOfSurface aFirstSurfType, aSecondSurfType;
  Standard_Real aFirstOffset, aSecondOffset;

  AIS::GetPlaneFromFace (aFirstFace, aFirstPln,
                         aFirstBasisSurf,aFirstSurfType,aFirstOffset);

  AIS::GetPlaneFromFace (aSecondFace, aSecondPln,
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
  gp_Pln aFirstPln, aSecondPln;
  Handle(Geom_Surface) aFirstBasisSurf, aSecondBasisSurf;
  AIS_KindOfSurface aFirstSurfType, aSecondSurfType;
  Standard_Real aFirstOffset, aSecondOffset;

  AIS::GetPlaneFromFace (aFirstFace, aFirstPln,
                         aFirstBasisSurf,aFirstSurfType,aFirstOffset);

  AIS::GetPlaneFromFace (aSecondFace, aSecondPln,
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

//=======================================================================
//function : isArrowVisible
//purpose  : compares given and internal arrows types, returns true if the the type should be shown
//=======================================================================
Standard_Boolean AIS_AngleDimension::isArrowVisible(const AIS_TypeOfAngleArrowVisibility& theArrowType) const
{
  switch (theArrowType)
  {
    case AIS_TOAV_Both:
      return myArrowsVisibility == AIS_TOAV_Both;
    case AIS_TOAV_First:
      return myArrowsVisibility == AIS_TOAV_Both || myArrowsVisibility == AIS_TOAV_First;
    case AIS_TOAV_Second:
      return myArrowsVisibility == AIS_TOAV_Both || myArrowsVisibility == AIS_TOAV_Second;
    case AIS_TOAV_None:
      return false;
  }
  return false;
}

//=======================================================================
//function : GetTextPosition
//purpose  : 
//=======================================================================
const gp_Pnt AIS_AngleDimension::GetTextPosition() const
{
  if (!IsValid())
  {
    return gp::Origin();
  }

  if (IsTextPositionCustom())
  {
    return myFixedTextPosition;
  }

  // Counts text position according to the dimension parameters
  gp_Pnt aTextPosition (gp::Origin());

  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();

  // Prepare label string and compute its geometrical width
  Standard_Real aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString (aLabelWidth);

  gp_Pnt aFirstAttach = myCenterPoint.Translated (gp_Vec(myCenterPoint, myFirstPoint).Normalized() * GetFlyout());
  gp_Pnt aSecondAttach = myCenterPoint.Translated (gp_Vec(myCenterPoint, mySecondPoint).Normalized() * GetFlyout());

  // Handle user-defined and automatic arrow placement
  Standard_Boolean isArrowsExternal = Standard_False;
  Standard_Integer aLabelPosition = LabelPosition_None;
  FitTextAlignment (aDimensionAspect->TextHorizontalPosition(),
                    aLabelPosition, isArrowsExternal);

  // Get text position
  switch (aLabelPosition & LabelPosition_HMask)
  {
  case LabelPosition_HCenter:
    {
      aTextPosition = GetCenterOnArc (aFirstAttach, aSecondAttach, myCenterPoint);
    }
    break;
  case LabelPosition_Left:
    {
      gp_Dir aPlaneNormal = gp_Vec (aFirstAttach, aSecondAttach) ^ gp_Vec (myCenterPoint, aFirstAttach);
      gp_Dir anExtensionDir = aPlaneNormal ^ gp_Vec (myCenterPoint, aFirstAttach);
      Standard_Real anExtensionSize = aDimensionAspect->ExtensionSize();
      Standard_Real anOffset = isArrowsExternal
          ? anExtensionSize + aDimensionAspect->ArrowAspect()->Length()
          : anExtensionSize;
      gp_Vec anExtensionVec  = gp_Vec (anExtensionDir) * -anOffset;
      aTextPosition = aFirstAttach.Translated (anExtensionVec);
    }
    break;
  case LabelPosition_Right:
    {
      gp_Dir aPlaneNormal = gp_Vec (aFirstAttach, aSecondAttach) ^ gp_Vec (myCenterPoint, aFirstAttach);
      gp_Dir anExtensionDir = aPlaneNormal ^ gp_Vec (myCenterPoint, aSecondAttach);
      Standard_Real anExtensionSize = aDimensionAspect->ExtensionSize();
      Standard_Real anOffset = isArrowsExternal
          ? anExtensionSize + aDimensionAspect->ArrowAspect()->Length()
          : anExtensionSize;
      gp_Vec anExtensionVec  = gp_Vec (anExtensionDir) * anOffset;
      aTextPosition = aSecondAttach.Translated (anExtensionVec);
    }
    break;
  }

  return aTextPosition;
}

//=======================================================================
//function : SetTextPosition
//purpose  : 
//=======================================================================
void AIS_AngleDimension::SetTextPosition (const gp_Pnt& theTextPos)
{
  if (!IsValid())
  {
    return;
  }

  // The text position point for angle dimension should belong to the working plane.
  if (!GetPlane().Contains (theTextPos, Precision::Confusion()))
  {
    throw Standard_ProgramError("The text position point for angle dimension doesn't belong to the working plane.");
  }

  myIsTextPositionFixed = Standard_True;
  myFixedTextPosition = theTextPos;
}

//=======================================================================
//function : AdjustParameters
//purpose  : 
//=======================================================================
void AIS_AngleDimension::AdjustParameters (const gp_Pnt& theTextPos,
                                           Standard_Real& theExtensionSize,
                                           Prs3d_DimensionTextHorizontalPosition& theAlignment,
                                           Standard_Real& theFlyout) const
{
  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();
  Standard_Real anArrowLength = aDimensionAspect->ArrowAspect()->Length();

  // Build circle with radius that is equal to distance from text position to the center point.
  Standard_Real aRadius = gp_Vec (myCenterPoint, theTextPos).Magnitude();

  // Set attach points in positive direction of the flyout.
  gp_Pnt aFirstAttach = myCenterPoint.Translated (gp_Vec (myCenterPoint, myFirstPoint).Normalized() * aRadius);
  gp_Pnt aSecondAttach = myCenterPoint.Translated (gp_Vec (myCenterPoint, mySecondPoint).Normalized() * aRadius);

  gce_MakeCirc aConstructCircle (myCenterPoint, GetPlane(), aRadius);
  if (!aConstructCircle.IsDone())
  {
    return;
  }
  gp_Circ aCircle = aConstructCircle.Value();

  // Default values
  theExtensionSize = aDimensionAspect->ArrowAspect()->Length();
  theAlignment = Prs3d_DTHP_Center;

  Standard_Real aParamBeg = ElCLib::Parameter (aCircle, aFirstAttach);
  Standard_Real aParamEnd = ElCLib::Parameter (aCircle, aSecondAttach);
  if (aParamEnd < aParamBeg)
  {
    Standard_Real aParam = aParamEnd;
    aParamEnd = aParamBeg;
    aParamBeg = aParam;
  }

  ElCLib::AdjustPeriodic (0.0, M_PI * 2, Precision::PConfusion(), aParamBeg, aParamEnd);
  Standard_Real aTextPar = ElCLib::Parameter (aCircle , theTextPos);

  // Horizontal center
  if (aTextPar > aParamBeg && aTextPar < aParamEnd)
  {
    theFlyout = aRadius;
    return;
  }

  aParamBeg += M_PI;
  aParamEnd += M_PI;
  ElCLib::AdjustPeriodic (0.0, M_PI * 2, Precision::PConfusion(), aParamBeg, aParamEnd);

  if (aTextPar > aParamBeg  && aTextPar < aParamEnd)
  {
    theFlyout = -aRadius;
    return;
  }

  // Text on the extensions
  gp_Lin aFirstLine = gce_MakeLin (myCenterPoint, myFirstPoint);
  gp_Lin aSecondLine = gce_MakeLin (myCenterPoint, mySecondPoint);
  gp_Pnt aFirstTextProj = AIS::Nearest (aFirstLine, theTextPos);
  gp_Pnt aSecondTextProj = AIS::Nearest (aSecondLine, theTextPos);
  Standard_Real aFirstDist = aFirstTextProj.Distance (theTextPos);
  Standard_Real aSecondDist = aSecondTextProj.Distance (theTextPos);

  if (aFirstDist <= aSecondDist)
  {
    aRadius = myCenterPoint.Distance (aFirstTextProj);
    Standard_Real aNewExtensionSize = aFirstDist - anArrowLength;
    theExtensionSize = aNewExtensionSize < 0.0 ? 0.0 : aNewExtensionSize;

    theAlignment = Prs3d_DTHP_Left;

    gp_Vec aPosFlyoutDir = gp_Vec (myCenterPoint, myFirstPoint).Normalized().Scaled (aRadius);

    theFlyout = aFirstTextProj.Distance (myCenterPoint.Translated (aPosFlyoutDir)) > Precision::Confusion()
                ? -aRadius : aRadius;
  }
  else
  {
    aRadius = myCenterPoint.Distance (aSecondTextProj);

    Standard_Real aNewExtensionSize = aSecondDist - anArrowLength;

    theExtensionSize = aNewExtensionSize < 0.0 ? 0.0 : aNewExtensionSize;

    theAlignment = Prs3d_DTHP_Right;

    gp_Vec aPosFlyoutDir = gp_Vec (myCenterPoint, mySecondPoint).Normalized().Scaled (aRadius);

    theFlyout = aSecondTextProj.Distance (myCenterPoint.Translated (aPosFlyoutDir)) > Precision::Confusion()
                ? -aRadius : aRadius;
  }
}

//=======================================================================
//function : FitTextAlignment
//purpose  : 
//=======================================================================
void AIS_AngleDimension::FitTextAlignment (const Prs3d_DimensionTextHorizontalPosition& theHorizontalTextPos,
                                           Standard_Integer& theLabelPosition,
                                           Standard_Boolean& theIsArrowsExternal) const
{
  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();

  Standard_Real anArrowLength = aDimensionAspect->ArrowAspect()->Length();

  // Prepare label string and compute its geometrical width
  Standard_Real aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString (aLabelWidth);

  // add margins to label width
  if (aDimensionAspect->IsText3d())
  {
    aLabelWidth += aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN * 2.0;
  }

  gp_Pnt aFirstAttach = myCenterPoint.Translated (gp_Vec (myCenterPoint, myFirstPoint).Normalized() * GetFlyout());
  gp_Pnt aSecondAttach = myCenterPoint.Translated (gp_Vec (myCenterPoint, mySecondPoint).Normalized() * GetFlyout());

  // Handle user-defined and automatic arrow placement
  switch (aDimensionAspect->ArrowOrientation())
  {
    case Prs3d_DAO_External: theIsArrowsExternal = true; break;
    case Prs3d_DAO_Internal: theIsArrowsExternal = false; break;
    case Prs3d_DAO_Fit:
    {
      gp_Vec anAttachVector (aFirstAttach, aSecondAttach);
      Standard_Real aDimensionWidth = anAttachVector.Magnitude();

      // Add margin to ensure a small tail between text and arrow
      Standard_Real anArrowMargin   = aDimensionAspect->IsText3d() 
                                    ? aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN
                                    : 0.0;

      Standard_Real anArrowsWidth   = (anArrowLength + anArrowMargin) * 2.0;

      theIsArrowsExternal = aDimensionWidth < aLabelWidth + anArrowsWidth;
      break;
    }
  }

  // Handle user-defined and automatic text placement
  switch (theHorizontalTextPos)
  {
    case Prs3d_DTHP_Left  : theLabelPosition |= LabelPosition_Left; break;
    case Prs3d_DTHP_Right : theLabelPosition |= LabelPosition_Right; break;
    case Prs3d_DTHP_Center: theLabelPosition |= LabelPosition_HCenter; break;
    case Prs3d_DTHP_Fit:
    {
      gp_Vec anAttachVector (aFirstAttach, aSecondAttach);
      Standard_Real aDimensionWidth = anAttachVector.Magnitude();
      Standard_Real anArrowsWidth   = anArrowLength * 2.0;
      Standard_Real aContentWidth   = theIsArrowsExternal ? aLabelWidth : aLabelWidth + anArrowsWidth;

      theLabelPosition |= aDimensionWidth < aContentWidth ? LabelPosition_Left : LabelPosition_HCenter;
      break;
    }
  }

  switch (aDimensionAspect->TextVerticalPosition())
  {
    case Prs3d_DTVP_Above  : theLabelPosition |= LabelPosition_Above; break;
    case Prs3d_DTVP_Below  : theLabelPosition |= LabelPosition_Below; break;
    case Prs3d_DTVP_Center : theLabelPosition |= LabelPosition_VCenter; break;
  }
}
