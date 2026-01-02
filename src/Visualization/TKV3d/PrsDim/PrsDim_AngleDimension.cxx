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

#include <PrsDim_AngleDimension.hxx>

#include <PrsDim.hxx>
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
#include <Prs3d_ShadingAspect.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <SelectMgr_Selection.hxx>
#include <Standard_ProgramError.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsDim_AngleDimension, PrsDim_Dimension)

namespace
{
constexpr double   THE_EMPTY_LABEL_WIDTH = 0.0;
constexpr char16_t THE_DEGREE_SYMBOL     = 0x00B0;
constexpr double   THE_3D_TEXT_MARGIN    = 0.1;

//! Returns true if the given points lie on a same line.
static bool isSameLine(const gp_Pnt& theFirstPoint,
                       const gp_Pnt& theCenterPoint,
                       const gp_Pnt& theSecondPoint)
{
  gp_Vec aVec1(theFirstPoint, theCenterPoint);
  gp_Vec aVec2(theCenterPoint, theSecondPoint);

  return aVec1.IsParallel(aVec2, Precision::Angular());
}
} // namespace

//=================================================================================================

PrsDim_AngleDimension::PrsDim_AngleDimension(const TopoDS_Edge& theFirstEdge,
                                             const TopoDS_Edge& theSecondEdge)
    : PrsDim_Dimension(PrsDim_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry(theFirstEdge, theSecondEdge);
}

//=================================================================================================

PrsDim_AngleDimension::PrsDim_AngleDimension(const gp_Pnt& theFirstPoint,
                                             const gp_Pnt& theSecondPoint,
                                             const gp_Pnt& theThirdPoint)
    : PrsDim_Dimension(PrsDim_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry(theFirstPoint, theSecondPoint, theThirdPoint);
}

//=================================================================================================

PrsDim_AngleDimension::PrsDim_AngleDimension(const TopoDS_Vertex& theFirstVertex,
                                             const TopoDS_Vertex& theSecondVertex,
                                             const TopoDS_Vertex& theThirdVertex)
    : PrsDim_Dimension(PrsDim_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry(theFirstVertex, theSecondVertex, theThirdVertex);
}

//=================================================================================================

PrsDim_AngleDimension::PrsDim_AngleDimension(const TopoDS_Face& theCone)
    : PrsDim_Dimension(PrsDim_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry(theCone);
}

//=================================================================================================

PrsDim_AngleDimension::PrsDim_AngleDimension(const TopoDS_Face& theFirstFace,
                                             const TopoDS_Face& theSecondFace)
    : PrsDim_Dimension(PrsDim_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry(theFirstFace, theSecondFace);
}

//=================================================================================================

PrsDim_AngleDimension::PrsDim_AngleDimension(const TopoDS_Face& theFirstFace,
                                             const TopoDS_Face& theSecondFace,
                                             const gp_Pnt&      thePoint)
    : PrsDim_Dimension(PrsDim_KOD_PLANEANGLE)
{
  Init();
  SetMeasuredGeometry(theFirstFace, theSecondFace, thePoint);
}

//=================================================================================================

void PrsDim_AngleDimension::SetMeasuredGeometry(const TopoDS_Edge& theFirstEdge,
                                                const TopoDS_Edge& theSecondEdge)
{
  gp_Pln aComputedPlane;

  myFirstShape      = theFirstEdge;
  mySecondShape     = theSecondEdge;
  myThirdShape      = TopoDS_Shape();
  myGeometryType    = GeometryType_Edges;
  myIsGeometryValid = InitTwoEdgesAngle(aComputedPlane);

  if (myIsGeometryValid && !myIsPlaneCustom)
  {
    myPlane = aComputedPlane;
  }

  SetToUpdate();
}

//=================================================================================================

void PrsDim_AngleDimension::SetMeasuredGeometry(const gp_Pnt& theFirstPoint,
                                                const gp_Pnt& theSecondPoint,
                                                const gp_Pnt& theThirdPoint)
{
  myFirstPoint      = theFirstPoint;
  myCenterPoint     = theSecondPoint;
  mySecondPoint     = theThirdPoint;
  myFirstShape      = BRepLib_MakeVertex(myFirstPoint);
  mySecondShape     = BRepLib_MakeVertex(myCenterPoint);
  myThirdShape      = BRepLib_MakeVertex(mySecondPoint);
  myGeometryType    = GeometryType_Points;
  myIsGeometryValid = IsValidPoints(myFirstPoint, myCenterPoint, mySecondPoint);

  bool anIsSameLine = isSameLine(myFirstPoint, myCenterPoint, mySecondPoint);
  if (myIsGeometryValid && !myIsPlaneCustom && !anIsSameLine)
  {
    ComputePlane();
  }

  SetToUpdate();
}

//=================================================================================================

void PrsDim_AngleDimension::SetMeasuredGeometry(const TopoDS_Vertex& theFirstVertex,
                                                const TopoDS_Vertex& theSecondVertex,
                                                const TopoDS_Vertex& theThirdVertex)
{
  myFirstShape      = theFirstVertex;
  mySecondShape     = theSecondVertex;
  myThirdShape      = theThirdVertex;
  myFirstPoint      = BRep_Tool::Pnt(theFirstVertex);
  myCenterPoint     = BRep_Tool::Pnt(theSecondVertex);
  mySecondPoint     = BRep_Tool::Pnt(theThirdVertex);
  myGeometryType    = GeometryType_Points;
  myIsGeometryValid = IsValidPoints(myFirstPoint, myCenterPoint, mySecondPoint);

  bool anIsSameLine = isSameLine(myFirstPoint, myCenterPoint, mySecondPoint);
  if (myIsGeometryValid && !myIsPlaneCustom && !anIsSameLine)
  {
    ComputePlane();
  }

  SetToUpdate();
}

//=================================================================================================

void PrsDim_AngleDimension::SetMeasuredGeometry(const TopoDS_Face& theCone)
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

//=================================================================================================

void PrsDim_AngleDimension::SetMeasuredGeometry(const TopoDS_Face& theFirstFace,
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

//=================================================================================================

void PrsDim_AngleDimension::SetMeasuredGeometry(const TopoDS_Face& theFirstFace,
                                                const TopoDS_Face& theSecondFace,
                                                const gp_Pnt&      thePoint)
{
  myFirstShape      = theFirstFace;
  mySecondShape     = theSecondFace;
  myThirdShape      = TopoDS_Shape();
  myGeometryType    = GeometryType_Faces;
  myIsGeometryValid = InitTwoFacesAngle(thePoint);

  if (myIsGeometryValid && !myIsPlaneCustom)
  {
    ComputePlane();
  }

  SetToUpdate();
}

//=================================================================================================

void PrsDim_AngleDimension::Init()
{
  SetType(PrsDim_TypeOfAngle_Interior);
  SetArrowsVisibility(PrsDim_TypeOfAngleArrowVisibility_Both);
  SetSpecialSymbol(THE_DEGREE_SYMBOL);
  SetDisplaySpecialSymbol(PrsDim_DisplaySpecialSymbol_After);
  SetFlyout(15.0);
}

//=================================================================================================

gp_Pnt PrsDim_AngleDimension::GetCenterOnArc(const gp_Pnt& theFirstAttach,
                                             const gp_Pnt& theSecondAttach,
                                             const gp_Pnt& theCenter) const
{
  // construct plane where the circle and the arc are located
  gce_MakePln aConstructPlane(theFirstAttach, theSecondAttach, theCenter);
  if (!aConstructPlane.IsDone())
  {
    return gp::Origin();
  }

  gp_Pln aPlane = aConstructPlane.Value();
  // to have an exterior angle presentation, a plane for further constructed circle should be
  // reversed
  if (myType == PrsDim_TypeOfAngle_Exterior)
  {
    gp_Ax1 anAxis = aPlane.Axis();
    gp_Dir aDir   = anAxis.Direction();
    aDir.Reverse();
    aPlane.SetAxis(gp_Ax1(anAxis.Location(), aDir));
  }

  double aRadius = theFirstAttach.Distance(theCenter);

  // construct circle forming the arc
  gce_MakeCirc aConstructCircle(theCenter, aPlane, aRadius);
  if (!aConstructCircle.IsDone())
  {
    return gp::Origin();
  }

  gp_Circ aCircle = aConstructCircle.Value();

  // compute angle parameters of arc end-points on circle
  double aParamBeg = ElCLib::Parameter(aCircle, theFirstAttach);
  double aParamEnd = ElCLib::Parameter(aCircle, theSecondAttach);
  ElCLib::AdjustPeriodic(0.0, M_PI * 2, Precision::PConfusion(), aParamBeg, aParamEnd);

  return ElCLib::Value((aParamBeg + aParamEnd) * 0.5, aCircle);
}

//=================================================================================================

gp_Dir PrsDim_AngleDimension::GetNormalForMinAngle() const
{
  const gp_Dir& aNormal = myPlane.Axis().Direction();
  gp_Dir        aFirst(gp_Vec(myCenterPoint, myFirstPoint));
  gp_Dir        aSecond(gp_Vec(myCenterPoint, mySecondPoint));

  return aFirst.AngleWithRef(aSecond, aNormal) < 0.0 ? aNormal.Reversed() : aNormal;
}

//=======================================================================
// function : DrawArc
// purpose  : draws the arc between two attach points
//=======================================================================
void PrsDim_AngleDimension::DrawArc(const occ::handle<Prs3d_Presentation>& thePresentation,
                                    const gp_Pnt&                          theFirstAttach,
                                    const gp_Pnt&                          theSecondAttach,
                                    const gp_Pnt&                          theCenter,
                                    const double                           theRadius,
                                    const int                              theMode)
{
  gp_Pln aPlane(myCenterPoint, GetNormalForMinAngle());

  // to have an exterior angle presentation, a plane for further constructed circle should be
  // reversed
  if (myType == PrsDim_TypeOfAngle_Exterior)
  {
    gp_Ax1 anAxis = aPlane.Axis();
    gp_Dir aDir   = anAxis.Direction();
    aDir.Reverse();
    aPlane.SetAxis(gp_Ax1(anAxis.Location(), aDir));
  }

  // construct circle forming the arc
  gce_MakeCirc aConstructCircle(theCenter, aPlane, theRadius);
  if (!aConstructCircle.IsDone())
  {
    return;
  }

  gp_Circ aCircle = aConstructCircle.Value();

  // construct the arc
  GC_MakeArcOfCircle aConstructArc(aCircle, theFirstAttach, theSecondAttach, true);
  if (!aConstructArc.IsDone())
  {
    return;
  }

  // generate points with specified deflection
  const occ::handle<Geom_TrimmedCurve>& anArcCurve = aConstructArc.Value();

  GeomAdaptor_Curve anArcAdaptor(anArcCurve,
                                 anArcCurve->FirstParameter(),
                                 anArcCurve->LastParameter());

  // compute number of discretization elements in old-fanshioned way
  gp_Vec aCenterToFirstVec(theCenter, theFirstAttach);
  gp_Vec aCenterToSecondVec(theCenter, theSecondAttach);
  double anAngle = aCenterToFirstVec.Angle(aCenterToSecondVec);
  if (myType == PrsDim_TypeOfAngle_Exterior)
    anAngle = 2.0 * M_PI - anAngle;
  // it sets 50 points on PI, and a part of points if angle is less
  const int aNbPoints = std::max(4, int(50.0 * anAngle / M_PI));

  GCPnts_UniformAbscissa aMakePnts(anArcAdaptor, aNbPoints);
  if (!aMakePnts.IsDone())
  {
    return;
  }

  // init data arrays for graphical and selection primitives
  occ::handle<Graphic3d_ArrayOfPolylines> aPrimSegments = new Graphic3d_ArrayOfPolylines(aNbPoints);

  SelectionGeometry::Curve& aSensitiveCurve = mySelectionGeom.NewCurve();

  // load data into arrays
  for (int aPntIt = 1; aPntIt <= aMakePnts.NbPoints(); ++aPntIt)
  {
    gp_Pnt aPnt = anArcAdaptor.Value(aMakePnts.Parameter(aPntIt));

    aPrimSegments->AddVertex(aPnt);

    aSensitiveCurve.Append(aPnt);
  }

  // add display presentation
  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == ComputeMode_All)
  {
    thePresentation->CurrentGroup()->SetStencilTestOptions(true);
  }
  occ::handle<Graphic3d_AspectLine3d> aDimensionLineStyle =
    myDrawer->DimensionAspect()->LineAspect()->Aspect();
  thePresentation->CurrentGroup()->SetPrimitivesAspect(aDimensionLineStyle);
  thePresentation->CurrentGroup()->AddPrimitiveArray(aPrimSegments);
  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == ComputeMode_All)
  {
    thePresentation->CurrentGroup()->SetStencilTestOptions(false);
  }
}

//=================================================================================================

void PrsDim_AngleDimension::DrawArcWithText(const occ::handle<Prs3d_Presentation>& thePresentation,
                                            const gp_Pnt&                          theFirstAttach,
                                            const gp_Pnt&                          theSecondAttach,
                                            const gp_Pnt&                          theCenter,
                                            const TCollection_ExtendedString&      theText,
                                            const double                           theTextWidth,
                                            const int                              theMode,
                                            const int                              theLabelPosition)
{
  gp_Pln aPlane(myCenterPoint, GetNormalForMinAngle());

  double aRadius = theFirstAttach.Distance(myCenterPoint);

  // construct circle forming the arc
  gce_MakeCirc aConstructCircle(theCenter, aPlane, aRadius);
  if (!aConstructCircle.IsDone())
  {
    return;
  }

  gp_Circ aCircle = aConstructCircle.Value();

  // compute angle parameters of arc end-points on circle
  double aParamBeg = ElCLib::Parameter(aCircle, theFirstAttach);
  double aParamEnd = ElCLib::Parameter(aCircle, theSecondAttach);
  ElCLib::AdjustPeriodic(0.0, M_PI * 2, Precision::PConfusion(), aParamBeg, aParamEnd);

  // middle point of arc parameter on circle
  double aParamMid = (aParamBeg + aParamEnd) * 0.5;

  // add text graphical primitives
  if (theMode == ComputeMode_All || theMode == ComputeMode_Text)
  {
    gp_Pnt aTextPos = ElCLib::Value(aParamMid, aCircle);
    gp_Dir aTextDir = gce_MakeDir(theFirstAttach, theSecondAttach);

    // Drawing text
    drawText(thePresentation, aTextPos, aTextDir, theText, theLabelPosition);
  }

  if (theMode != ComputeMode_All && theMode != ComputeMode_Line)
  {
    return;
  }

  occ::handle<Prs3d_DimensionAspect> aDimensionAspect = myDrawer->DimensionAspect();

  bool isLineBreak =
    aDimensionAspect->TextVerticalPosition() == Prs3d_DTVP_Center && aDimensionAspect->IsText3d();

  if (isLineBreak)
  {
    // compute gap for label as parametric size of sector on circle segment
    double aSectorOfText = theTextWidth / aRadius;
    double aTextBegin    = aParamMid - aSectorOfText * 0.5;
    double aTextEnd      = aParamMid + aSectorOfText * 0.5;
    gp_Pnt aTextPntBeg   = ElCLib::Value(aTextBegin, aCircle);
    gp_Pnt aTextPntEnd   = ElCLib::Value(aTextEnd, aCircle);

    // Drawing arcs
    if (aTextBegin > aParamBeg)
    {
      DrawArc(thePresentation, theFirstAttach, aTextPntBeg, theCenter, aRadius, theMode);
    }
    if (aTextEnd < aParamEnd)
    {
      DrawArc(thePresentation, aTextPntEnd, theSecondAttach, theCenter, aRadius, theMode);
    }
  }
  else
  {
    DrawArc(thePresentation, theFirstAttach, theSecondAttach, theCenter, aRadius, theMode);
  }
}

//=================================================================================================

bool PrsDim_AngleDimension::CheckPlane(const gp_Pln& thePlane) const
{
  return !(!thePlane.Contains(myFirstPoint, Precision::Confusion())
           && !thePlane.Contains(mySecondPoint, Precision::Confusion())
           && !thePlane.Contains(myCenterPoint, Precision::Confusion()));
}

//=================================================================================================

void PrsDim_AngleDimension::ComputePlane()
{
  if (!myIsGeometryValid)
  {
    return;
  }

  // Compute working plane so that Y axis is codirectional
  // with Y axis of text coordinate system (necessary for text alignment)
  gp_Vec aFirstVec   = gp_Vec(myCenterPoint, myFirstPoint);
  gp_Vec aSecondVec  = gp_Vec(myCenterPoint, mySecondPoint);
  gp_Vec aDirectionN = aSecondVec ^ aFirstVec;
  gp_Vec aDirectionY = aFirstVec + aSecondVec;
  gp_Vec aDirectionX = aDirectionY ^ aDirectionN;

  myPlane = gp_Pln(gp_Ax3(myCenterPoint, gp_Dir(aDirectionN), gp_Dir(aDirectionX)));
}

//=================================================================================================

const TCollection_AsciiString& PrsDim_AngleDimension::GetModelUnits() const
{
  return myDrawer->DimAngleModelUnits();
}

//=================================================================================================

const TCollection_AsciiString& PrsDim_AngleDimension::GetDisplayUnits() const
{
  return myDrawer->DimAngleDisplayUnits();
}

//=================================================================================================

void PrsDim_AngleDimension::SetModelUnits(const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimAngleModelUnits(theUnits);
}

//=================================================================================================

void PrsDim_AngleDimension::SetDisplayUnits(const TCollection_AsciiString& theUnits)
{
  myDrawer->SetDimAngleDisplayUnits(theUnits);
}

//=================================================================================================

double PrsDim_AngleDimension::ComputeValue() const
{
  if (!IsValid())
  {
    return 0.0;
  }

  gp_Vec aVec1(myCenterPoint, myFirstPoint);
  gp_Vec aVec2(myCenterPoint, mySecondPoint);

  double anAngle = aVec1.AngleWithRef(aVec2, GetNormalForMinAngle());

  return anAngle > 0.0 ? anAngle : (2.0 * M_PI + anAngle);
}

//=======================================================================
// function : Compute
// purpose  : Having three gp_Pnt points compute presentation
//=======================================================================
void PrsDim_AngleDimension::Compute(const occ::handle<PrsMgr_PresentationManager>&,
                                    const occ::handle<Prs3d_Presentation>& thePresentation,
                                    const int                              theMode)
{
  mySelectionGeom.Clear(theMode);

  if (!IsValid())
  {
    return;
  }

  // Parameters for presentation
  occ::handle<Prs3d_DimensionAspect> aDimensionAspect = myDrawer->DimensionAspect();

  thePresentation->CurrentGroup()->SetPrimitivesAspect(aDimensionAspect->LineAspect()->Aspect());

  double anArrowLength = aDimensionAspect->ArrowAspect()->Length();

  // prepare label string and compute its geometrical width
  double                     aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString(aLabelWidth);

  // add margins to label width
  if (aDimensionAspect->IsText3d())
  {
    aLabelWidth += aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN * 2.0;
  }

  // Get parameters from aspect or adjust it according with custom text position
  double                                anExtensionSize = aDimensionAspect->ExtensionSize();
  Prs3d_DimensionTextHorizontalPosition aHorisontalTextPos =
    aDimensionAspect->TextHorizontalPosition();

  if (IsTextPositionCustom())
  {
    AdjustParameters(myFixedTextPosition, anExtensionSize, aHorisontalTextPos, myFlyout);
  }

  // Handle user-defined and automatic arrow placement
  bool isArrowsExternal = false;
  int  aLabelPosition   = LabelPosition_None;

  FitTextAlignment(aHorisontalTextPos, aLabelPosition, isArrowsExternal);

  gp_Pnt aFirstAttach =
    myCenterPoint.Translated(gp_Vec(myCenterPoint, myFirstPoint).Normalized() * GetFlyout());
  gp_Pnt aSecondAttach =
    myCenterPoint.Translated(gp_Vec(myCenterPoint, mySecondPoint).Normalized() * GetFlyout());

  // Arrows positions and directions
  gp_Vec aWorkingPlaneDir(GetNormalForMinAngle());

  gp_Dir aFirstExtensionDir  = aWorkingPlaneDir.Reversed() ^ gp_Vec(myCenterPoint, aFirstAttach);
  gp_Dir aSecondExtensionDir = aWorkingPlaneDir ^ gp_Vec(myCenterPoint, aSecondAttach);

  gp_Vec aFirstArrowVec  = gp_Vec(aFirstExtensionDir) * anArrowLength;
  gp_Vec aSecondArrowVec = gp_Vec(aSecondExtensionDir) * anArrowLength;

  if (isArrowsExternal)
  {
    aFirstArrowVec.Reverse();
    aSecondArrowVec.Reverse();
  }

  gp_Pnt aFirstArrowBegin(0.0, 0.0, 0.0);
  gp_Pnt aFirstArrowEnd(0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowBegin(0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowEnd(0.0, 0.0, 0.0);

  aFirstArrowBegin  = aFirstAttach;
  aSecondArrowBegin = aSecondAttach;
  aFirstArrowEnd    = aFirstAttach;
  aSecondArrowEnd   = aSecondAttach;

  if (aDimensionAspect->ArrowAspect()->IsZoomable())
  {
    aFirstArrowEnd.Translate(-aFirstArrowVec);
    aSecondArrowEnd.Translate(-aSecondArrowVec);
  }

  // Group1: stenciling text and the angle dimension arc
  thePresentation->NewGroup();

  int aHPosition = aLabelPosition & LabelPosition_HMask;

  // draw text label
  switch (aHPosition)
  {
    case LabelPosition_HCenter: {
      bool isLineBreak = aDimensionAspect->TextVerticalPosition() == Prs3d_DTVP_Center
                         && aDimensionAspect->IsText3d();

      if (isLineBreak)
      {
        DrawArcWithText(thePresentation,
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
        gp_Vec aDimensionDir(aFirstAttach, aSecondAttach);
        gp_Pnt aTextPos = IsTextPositionCustom()
                            ? myFixedTextPosition
                            : GetCenterOnArc(aFirstAttach, aSecondAttach, myCenterPoint);
        gp_Dir aTextDir = aDimensionDir;

        drawText(thePresentation, aTextPos, aTextDir, aLabelString, aLabelPosition);
      }

      if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
      {
        DrawArc(thePresentation,
                (isArrowsExternal || !isArrowVisible(PrsDim_TypeOfAngleArrowVisibility_First))
                  ? aFirstAttach
                  : aFirstArrowEnd,
                (isArrowsExternal || !isArrowVisible(PrsDim_TypeOfAngleArrowVisibility_Second))
                  ? aSecondAttach
                  : aSecondArrowEnd,
                myCenterPoint,
                std::abs(GetFlyout()),
                theMode);
      }
    }
    break;

    case LabelPosition_Left: {
      DrawExtension(thePresentation,
                    anExtensionSize,
                    (isArrowsExternal && isArrowVisible(PrsDim_TypeOfAngleArrowVisibility_First))
                      ? aFirstArrowEnd
                      : aFirstAttach,
                    aFirstExtensionDir,
                    aLabelString,
                    aLabelWidth,
                    theMode,
                    aLabelPosition);
    }
    break;

    case LabelPosition_Right: {
      DrawExtension(thePresentation,
                    anExtensionSize,
                    (isArrowsExternal && isArrowVisible(PrsDim_TypeOfAngleArrowVisibility_Second))
                      ? aSecondArrowEnd
                      : aSecondAttach,
                    aSecondExtensionDir,
                    aLabelString,
                    aLabelWidth,
                    theMode,
                    aLabelPosition);
    }
    break;
  }

  // dimension arc without text
  if ((theMode == ComputeMode_All || theMode == ComputeMode_Line)
      && aHPosition != LabelPosition_HCenter)
  {
    thePresentation->NewGroup();

    DrawArc(thePresentation,
            (isArrowsExternal || !isArrowVisible(PrsDim_TypeOfAngleArrowVisibility_First))
              ? aFirstAttach
              : aFirstArrowEnd,
            (isArrowsExternal || !isArrowVisible(PrsDim_TypeOfAngleArrowVisibility_Second))
              ? aSecondAttach
              : aSecondArrowEnd,
            myCenterPoint,
            std::abs(GetFlyout()),
            theMode);
  }

  // arrows and arrow extensions
  if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
  {
    thePresentation->NewGroup();

    if (isArrowVisible(PrsDim_TypeOfAngleArrowVisibility_First))
      DrawArrow(thePresentation, aFirstArrowBegin, gp_Dir(aFirstArrowVec));
    if (isArrowVisible(PrsDim_TypeOfAngleArrowVisibility_Second))
      DrawArrow(thePresentation, aSecondArrowBegin, gp_Dir(aSecondArrowVec));
  }

  if ((theMode == ComputeMode_All || theMode == ComputeMode_Line) && isArrowsExternal)
  {
    thePresentation->NewGroup();

    if (aHPosition != LabelPosition_Left && isArrowVisible(PrsDim_TypeOfAngleArrowVisibility_First))
    {
      DrawExtension(thePresentation,
                    aDimensionAspect->ArrowTailSize(),
                    aFirstArrowEnd,
                    aFirstExtensionDir,
                    TCollection_ExtendedString::EmptyString(),
                    THE_EMPTY_LABEL_WIDTH,
                    theMode,
                    LabelPosition_None);
    }

    if (aHPosition != LabelPosition_Right
        && isArrowVisible(PrsDim_TypeOfAngleArrowVisibility_Second))
    {
      DrawExtension(thePresentation,
                    aDimensionAspect->ArrowTailSize(),
                    aSecondArrowEnd,
                    aSecondExtensionDir,
                    TCollection_ExtendedString::EmptyString(),
                    THE_EMPTY_LABEL_WIDTH,
                    theMode,
                    LabelPosition_None);
    }
  }

  // flyouts
  if (theMode == ComputeMode_All)
  {
    thePresentation->NewGroup();

    occ::handle<Graphic3d_ArrayOfSegments> aPrimSegments = new Graphic3d_ArrayOfSegments(4);
    aPrimSegments->AddVertex(myCenterPoint);
    aPrimSegments->AddVertex(aFirstAttach);
    aPrimSegments->AddVertex(myCenterPoint);
    aPrimSegments->AddVertex(aSecondAttach);

    occ::handle<Graphic3d_AspectLine3d> aFlyoutStyle =
      myDrawer->DimensionAspect()->LineAspect()->Aspect();
    thePresentation->CurrentGroup()->SetPrimitivesAspect(aFlyoutStyle);
    thePresentation->CurrentGroup()->AddPrimitiveArray(aPrimSegments);
  }

  mySelectionGeom.IsComputed = true;
}

//=======================================================================
// function : ComputeFlyoutSelection
// purpose  : computes selection for flyouts
//=======================================================================
void PrsDim_AngleDimension::ComputeFlyoutSelection(
  const occ::handle<SelectMgr_Selection>&   theSelection,
  const occ::handle<SelectMgr_EntityOwner>& theOwner)
{
  gp_Pnt aFirstAttach =
    myCenterPoint.Translated(gp_Vec(myCenterPoint, myFirstPoint).Normalized() * GetFlyout());
  gp_Pnt aSecondAttach =
    myCenterPoint.Translated(gp_Vec(myCenterPoint, mySecondPoint).Normalized() * GetFlyout());

  occ::handle<Select3D_SensitiveGroup> aSensitiveEntity = new Select3D_SensitiveGroup(theOwner);
  aSensitiveEntity->Add(new Select3D_SensitiveSegment(theOwner, myCenterPoint, aFirstAttach));
  aSensitiveEntity->Add(new Select3D_SensitiveSegment(theOwner, myCenterPoint, aSecondAttach));

  theSelection->Add(aSensitiveEntity);
}

//=================================================================================================

bool PrsDim_AngleDimension::InitTwoEdgesAngle(gp_Pln& theComputedPlane)
{
  TopoDS_Edge aFirstEdge  = TopoDS::Edge(myFirstShape);
  TopoDS_Edge aSecondEdge = TopoDS::Edge(mySecondShape);

  BRepAdaptor_Curve aMakeFirstLine(aFirstEdge);
  BRepAdaptor_Curve aMakeSecondLine(aSecondEdge);

  if (aMakeFirstLine.GetType() != GeomAbs_Line || aMakeSecondLine.GetType() != GeomAbs_Line)
  {
    return false;
  }

  occ::handle<Geom_Line> aFirstLine  = new Geom_Line(aMakeFirstLine.Line());
  occ::handle<Geom_Line> aSecondLine = new Geom_Line(aMakeSecondLine.Line());

  gp_Lin aFirstLin  = aFirstLine->Lin();
  gp_Lin aSecondLin = aSecondLine->Lin();

  bool isParallelLines =
    aFirstLin.Direction().IsParallel(aSecondLin.Direction(), Precision::Angular());

  theComputedPlane = isParallelLines
                       ? gp_Pln(gp::XOY())
                       : gp_Pln(aSecondLin.Location(),
                                gp_Vec(aFirstLin.Direction()) ^ gp_Vec(aSecondLin.Direction()));

  // Compute geometry for this plane and edges
  bool                    isInfinite1, isInfinite2;
  gp_Pnt                  aFirstPoint1, aLastPoint1, aFirstPoint2, aLastPoint2;
  occ::handle<Geom_Curve> aFirstCurve = aFirstLine, aSecondCurve = aSecondLine;
  if (!PrsDim::ComputeGeometry(aFirstEdge,
                               aSecondEdge,
                               aFirstCurve,
                               aSecondCurve,
                               aFirstPoint1,
                               aLastPoint1,
                               aFirstPoint2,
                               aLastPoint2,
                               isInfinite1,
                               isInfinite2))
  {
    return false;
  }

  bool isSameLines = aFirstLin.Direction().IsEqual(aSecondLin.Direction(), Precision::Angular())
                     && aFirstLin.Location().IsEqual(aSecondLin.Location(), Precision::Confusion());

  // It can be the same gp_Lin geometry but the different begin and end parameters
  bool isSameEdges = (aFirstPoint1.IsEqual(aFirstPoint2, Precision::Confusion())
                      && aLastPoint1.IsEqual(aLastPoint2, Precision::Confusion()))
                     || (aFirstPoint1.IsEqual(aLastPoint2, Precision::Confusion())
                         && aLastPoint1.IsEqual(aFirstPoint2, Precision::Confusion()));

  if (isParallelLines)
  {
    // Zero angle, it could not handle this geometry
    if (isSameLines && isSameEdges)
    {
      return false;
    }

    // Handle the case of Pi angle
    const double aParam11 = ElCLib::Parameter(aFirstLin, aFirstPoint1);
    const double aParam12 = ElCLib::Parameter(aFirstLin, aLastPoint1);
    const double aParam21 = ElCLib::Parameter(aFirstLin, aFirstPoint2);
    const double aParam22 = ElCLib::Parameter(aFirstLin, aLastPoint2);
    myCenterPoint =
      ElCLib::Value((std::min(aParam11, aParam12) + std::max(aParam21, aParam22)) * 0.5, aFirstLin);
    myFirstPoint  = myCenterPoint.Translated(gp_Vec(aFirstLin.Direction()) * std::abs(GetFlyout()));
    mySecondPoint = myCenterPoint.XYZ()
                    + (aFirstLin.Direction().IsEqual(aSecondLin.Direction(), Precision::Angular())
                         ? aFirstLin.Direction().Reversed().XYZ() * std::abs(GetFlyout())
                         : aSecondLin.Direction().XYZ() * std::abs(GetFlyout()));
  }
  else
  {
    // Find intersection
    gp_Lin2d aFirstLin2d  = ProjLib::Project(theComputedPlane, aFirstLin);
    gp_Lin2d aSecondLin2d = ProjLib::Project(theComputedPlane, aSecondLin);

    IntAna2d_AnaIntersection anInt2d(aFirstLin2d, aSecondLin2d);
    gp_Pnt2d                 anIntersectPoint;
    if (!anInt2d.IsDone() || anInt2d.IsEmpty())
    {
      return false;
    }

    anIntersectPoint = gp_Pnt2d(anInt2d.Point(1).Value());
    myCenterPoint    = ElCLib::To3d(theComputedPlane.Position().Ax2(), anIntersectPoint);

    if (isInfinite1 || isInfinite2)
    {
      myFirstPoint =
        myCenterPoint.Translated(gp_Vec(aFirstLin.Direction()) * std::abs(GetFlyout()));
      mySecondPoint =
        myCenterPoint.Translated(gp_Vec(aSecondLin.Direction()) * std::abs(GetFlyout()));

      return IsValidPoints(myFirstPoint, myCenterPoint, mySecondPoint);
    }

    // |
    // | <- dimension should be here
    // *----
    myFirstPoint = myCenterPoint.Distance(aFirstPoint1) > myCenterPoint.Distance(aLastPoint1)
                     ? aFirstPoint1
                     : aLastPoint1;

    mySecondPoint = myCenterPoint.Distance(aFirstPoint2) > myCenterPoint.Distance(aLastPoint2)
                      ? aFirstPoint2
                      : aLastPoint2;
  }

  return IsValidPoints(myFirstPoint, myCenterPoint, mySecondPoint);
}

//=======================================================================
// function : InitTwoFacesAngle
// purpose  : initialization of angle dimension between two faces
//=======================================================================
bool PrsDim_AngleDimension::InitTwoFacesAngle()
{
  TopoDS_Face aFirstFace  = TopoDS::Face(myFirstShape);
  TopoDS_Face aSecondFace = TopoDS::Face(mySecondShape);

  gp_Pln                    aFirstPln, aSecondPln;
  occ::handle<Geom_Surface> aFirstBasisSurf, aSecondBasisSurf;
  PrsDim_KindOfSurface      aFirstSurfType, aSecondSurfType;
  double                    aFirstOffset, aSecondOffset;

  PrsDim::GetPlaneFromFace(aFirstFace, aFirstPln, aFirstBasisSurf, aFirstSurfType, aFirstOffset);

  PrsDim::GetPlaneFromFace(aSecondFace,
                           aSecondPln,
                           aSecondBasisSurf,
                           aSecondSurfType,
                           aSecondOffset);

  if (aFirstSurfType == PrsDim_KOS_Plane && aSecondSurfType == PrsDim_KOS_Plane)
  {
    // Planar faces angle
    occ::handle<Geom_Plane> aFirstPlane  = occ::down_cast<Geom_Plane>(aFirstBasisSurf);
    occ::handle<Geom_Plane> aSecondPlane = occ::down_cast<Geom_Plane>(aSecondBasisSurf);
    return PrsDim::InitAngleBetweenPlanarFaces(aFirstFace,
                                               aSecondFace,
                                               myCenterPoint,
                                               myFirstPoint,
                                               mySecondPoint)
           && IsValidPoints(myFirstPoint, myCenterPoint, mySecondPoint);
  }
  else
  {
    // Curvilinear faces angle
    return PrsDim::InitAngleBetweenCurvilinearFaces(aFirstFace,
                                                    aSecondFace,
                                                    aFirstSurfType,
                                                    aSecondSurfType,
                                                    myCenterPoint,
                                                    myFirstPoint,
                                                    mySecondPoint)
           && IsValidPoints(myFirstPoint, myCenterPoint, mySecondPoint);
  }
}

//=======================================================================
// function : InitTwoFacesAngle
// purpose  : initialization of angle dimension between two faces
//=======================================================================
bool PrsDim_AngleDimension::InitTwoFacesAngle(const gp_Pnt& thePointOnFirstFace)
{
  TopoDS_Face aFirstFace  = TopoDS::Face(myFirstShape);
  TopoDS_Face aSecondFace = TopoDS::Face(mySecondShape);

  gp_Pln                    aFirstPln, aSecondPln;
  occ::handle<Geom_Surface> aFirstBasisSurf, aSecondBasisSurf;
  PrsDim_KindOfSurface      aFirstSurfType, aSecondSurfType;
  double                    aFirstOffset, aSecondOffset;

  PrsDim::GetPlaneFromFace(aFirstFace, aFirstPln, aFirstBasisSurf, aFirstSurfType, aFirstOffset);

  PrsDim::GetPlaneFromFace(aSecondFace,
                           aSecondPln,
                           aSecondBasisSurf,
                           aSecondSurfType,
                           aSecondOffset);

  myFirstPoint = thePointOnFirstFace;
  if (aFirstSurfType == PrsDim_KOS_Plane && aSecondSurfType == PrsDim_KOS_Plane)
  {
    // Planar faces angle
    occ::handle<Geom_Plane> aFirstPlane  = occ::down_cast<Geom_Plane>(aFirstBasisSurf);
    occ::handle<Geom_Plane> aSecondPlane = occ::down_cast<Geom_Plane>(aSecondBasisSurf);
    return PrsDim::InitAngleBetweenPlanarFaces(aFirstFace,
                                               aSecondFace,
                                               myCenterPoint,
                                               myFirstPoint,
                                               mySecondPoint,
                                               true)
           && IsValidPoints(myFirstPoint, myCenterPoint, mySecondPoint);
  }
  else
  {
    // Curvilinear faces angle
    return PrsDim::InitAngleBetweenCurvilinearFaces(aFirstFace,
                                                    aSecondFace,
                                                    aFirstSurfType,
                                                    aSecondSurfType,
                                                    myCenterPoint,
                                                    myFirstPoint,
                                                    mySecondPoint,
                                                    true)
           && IsValidPoints(myFirstPoint, myCenterPoint, mySecondPoint);
  }
}

//=======================================================================
// function : InitConeAngle
// purpose  : initialization of the cone angle
//=======================================================================
bool PrsDim_AngleDimension::InitConeAngle()
{
  if (myFirstShape.IsNull())
  {
    return false;
  }

  TopoDS_Face aConeShape = TopoDS::Face(myFirstShape);
  gp_Pln      aPln;
  gp_Cone     aCone;
  gp_Circ     aCircle;
  // A surface from the Face
  occ::handle<Geom_Surface>             aSurf;
  occ::handle<Geom_OffsetSurface>       aOffsetSurf;
  occ::handle<Geom_ConicalSurface>      aConicalSurf;
  occ::handle<Geom_SurfaceOfRevolution> aRevSurf;
  occ::handle<Geom_Line>                aLine;
  BRepAdaptor_Surface                   aConeAdaptor(aConeShape);
  TopoDS_Face                           aFace;
  PrsDim_KindOfSurface                  aSurfType;
  double                                anOffset = 0.;
  occ::handle<Standard_Type>            aType;

  const double aMaxV = aConeAdaptor.FirstVParameter();
  const double aMinV = aConeAdaptor.LastVParameter();
  PrsDim::GetPlaneFromFace(aConeShape, aPln, aSurf, aSurfType, anOffset);
  if (aSurfType == PrsDim_KOS_Revolution)
  {
    // Surface of revolution
    aRevSurf = occ::down_cast<Geom_SurfaceOfRevolution>(aSurf);
    gp_Lin                  aLin(aRevSurf->Axis());
    occ::handle<Geom_Curve> aBasisCurve = aRevSurf->BasisCurve();
    // Must be a part of line (basis curve should be linear)
    if (aBasisCurve->DynamicType() != STANDARD_TYPE(Geom_Line))
      return false;

    gp_Pnt aFirst1 = aConeAdaptor.Value(0., aMinV);
    gp_Pnt aLast1  = aConeAdaptor.Value(0., aMaxV);
    gp_Vec aVec1(aFirst1, aLast1);

    // Projection <aFirst> on <aLin>
    gp_Pnt aFirst2 = ElCLib::Value(ElCLib::Parameter(aLin, aFirst1), aLin);
    // Projection <aLast> on <aLin>
    gp_Pnt aLast2 = ElCLib::Value(ElCLib::Parameter(aLin, aLast1), aLin);

    gp_Vec aVec2(aFirst2, aLast2);

    // Check if two parts of revolution are parallel (it's a cylinder) or normal (it's a circle).
    if (aVec1.IsParallel(aVec2, Precision::Angular())
        || aVec1.IsNormal(aVec2, Precision::Angular()))
      return false;

    gce_MakeCone aMkCone(aRevSurf->Axis(), aFirst1, aLast1);
    aCone         = aMkCone.Value();
    myCenterPoint = aCone.Apex();
  }
  else
  {
    aType = aSurf->DynamicType();
    if (aType == STANDARD_TYPE(Geom_OffsetSurface) || anOffset > 0.01)
    {
      // Offset surface
      aOffsetSurf = new Geom_OffsetSurface(aSurf, anOffset);
      aSurf       = aOffsetSurf->Surface();
      BRepBuilderAPI_MakeFace aMkFace(aSurf, Precision::Confusion());
      aMkFace.Build();
      if (!aMkFace.IsDone())
        return false;
      aConeAdaptor.Initialize(aMkFace.Face());
    }
    aCone         = aConeAdaptor.Cone();
    aConicalSurf  = occ::down_cast<Geom_ConicalSurface>(aSurf);
    myCenterPoint = aConicalSurf->Apex();
  }

  // A circle where the angle is drawn
  occ::handle<Geom_Curve> aCurve;
  double                  aMidV = (aMinV + aMaxV) / 2.5;
  aCurve                        = aSurf->VIso(aMidV);
  aCircle                       = occ::down_cast<Geom_Circle>(aCurve)->Circ();

  aCurve            = aSurf->VIso(aMaxV);
  gp_Circ aCircVmax = occ::down_cast<Geom_Circle>(aCurve)->Circ();
  aCurve            = aSurf->VIso(aMinV);
  gp_Circ aCircVmin = occ::down_cast<Geom_Circle>(aCurve)->Circ();

  if (aCircVmax.Radius() < aCircVmin.Radius())
  {
    gp_Circ aTmpCirc = aCircVmax;
    aCircVmax        = aCircVmin;
    aCircVmin        = aTmpCirc;
  }

  myFirstPoint  = ElCLib::Value(0, aCircle);
  mySecondPoint = ElCLib::Value(M_PI, aCircle);
  return true;
}

//=================================================================================================

bool PrsDim_AngleDimension::IsValidPoints(const gp_Pnt& theFirstPoint,
                                          const gp_Pnt& theCenterPoint,
                                          const gp_Pnt& theSecondPoint) const
{
  return theFirstPoint.Distance(theCenterPoint) > Precision::Confusion()
         && theSecondPoint.Distance(theCenterPoint) > Precision::Confusion()
         && gp_Vec(theCenterPoint, theFirstPoint).Angle(gp_Vec(theCenterPoint, theSecondPoint))
              > Precision::Angular();
}

//=======================================================================
// function : isArrowVisible
// purpose  : compares given and internal arrows types, returns true if the type should be shown
//=======================================================================
bool PrsDim_AngleDimension::isArrowVisible(
  const PrsDim_TypeOfAngleArrowVisibility theArrowType) const
{
  switch (theArrowType)
  {
    case PrsDim_TypeOfAngleArrowVisibility_Both:
      return myArrowsVisibility == PrsDim_TypeOfAngleArrowVisibility_Both;
    case PrsDim_TypeOfAngleArrowVisibility_First:
      return myArrowsVisibility == PrsDim_TypeOfAngleArrowVisibility_Both
             || myArrowsVisibility == PrsDim_TypeOfAngleArrowVisibility_First;
    case PrsDim_TypeOfAngleArrowVisibility_Second:
      return myArrowsVisibility == PrsDim_TypeOfAngleArrowVisibility_Both
             || myArrowsVisibility == PrsDim_TypeOfAngleArrowVisibility_Second;
    case PrsDim_TypeOfAngleArrowVisibility_None:
      return false;
  }
  return false;
}

//=================================================================================================

gp_Pnt PrsDim_AngleDimension::GetTextPosition() const
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
  gp_Pnt aTextPosition(gp::Origin());

  occ::handle<Prs3d_DimensionAspect> aDimensionAspect = myDrawer->DimensionAspect();

  // Prepare label string and compute its geometrical width
  double                     aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString(aLabelWidth);

  gp_Pnt aFirstAttach =
    myCenterPoint.Translated(gp_Vec(myCenterPoint, myFirstPoint).Normalized() * GetFlyout());
  gp_Pnt aSecondAttach =
    myCenterPoint.Translated(gp_Vec(myCenterPoint, mySecondPoint).Normalized() * GetFlyout());

  // Handle user-defined and automatic arrow placement
  bool isArrowsExternal = false;
  int  aLabelPosition   = LabelPosition_None;
  FitTextAlignment(aDimensionAspect->TextHorizontalPosition(), aLabelPosition, isArrowsExternal);

  // Get text position
  switch (aLabelPosition & LabelPosition_HMask)
  {
    case LabelPosition_HCenter: {
      aTextPosition = GetCenterOnArc(aFirstAttach, aSecondAttach, myCenterPoint);
    }
    break;
    case LabelPosition_Left: {
      gp_Dir aPlaneNormal =
        gp_Vec(aFirstAttach, aSecondAttach) ^ gp_Vec(myCenterPoint, aFirstAttach);
      gp_Dir anExtensionDir  = aPlaneNormal ^ gp_Vec(myCenterPoint, aFirstAttach);
      double anExtensionSize = aDimensionAspect->ExtensionSize();
      double anOffset        = isArrowsExternal
                                 ? anExtensionSize + aDimensionAspect->ArrowAspect()->Length()
                                 : anExtensionSize;
      gp_Vec anExtensionVec  = gp_Vec(anExtensionDir) * -anOffset;
      aTextPosition          = aFirstAttach.Translated(anExtensionVec);
    }
    break;
    case LabelPosition_Right: {
      gp_Dir aPlaneNormal =
        gp_Vec(aFirstAttach, aSecondAttach) ^ gp_Vec(myCenterPoint, aFirstAttach);
      gp_Dir anExtensionDir  = aPlaneNormal ^ gp_Vec(myCenterPoint, aSecondAttach);
      double anExtensionSize = aDimensionAspect->ExtensionSize();
      double anOffset        = isArrowsExternal
                                 ? anExtensionSize + aDimensionAspect->ArrowAspect()->Length()
                                 : anExtensionSize;
      gp_Vec anExtensionVec  = gp_Vec(anExtensionDir) * anOffset;
      aTextPosition          = aSecondAttach.Translated(anExtensionVec);
    }
    break;
  }

  return aTextPosition;
}

//=================================================================================================

void PrsDim_AngleDimension::SetTextPosition(const gp_Pnt& theTextPos)
{
  if (!IsValid())
  {
    return;
  }

  // The text position point for angle dimension should belong to the working plane.
  if (!GetPlane().Contains(theTextPos, Precision::Confusion()))
  {
    throw Standard_ProgramError(
      "The text position point for angle dimension doesn't belong to the working plane.");
  }

  myIsTextPositionFixed = true;
  myFixedTextPosition   = theTextPos;
}

//=================================================================================================

void PrsDim_AngleDimension::AdjustParameters(const gp_Pnt& theTextPos,
                                             double&       theExtensionSize,
                                             Prs3d_DimensionTextHorizontalPosition& theAlignment,
                                             double&                                theFlyout) const
{
  occ::handle<Prs3d_DimensionAspect> aDimensionAspect = myDrawer->DimensionAspect();
  double                             anArrowLength    = aDimensionAspect->ArrowAspect()->Length();

  // Build circle with radius that is equal to distance from text position to the center point.
  double aRadius = gp_Vec(myCenterPoint, theTextPos).Magnitude();

  // Set attach points in positive direction of the flyout.
  gp_Pnt aFirstAttach =
    myCenterPoint.Translated(gp_Vec(myCenterPoint, myFirstPoint).Normalized() * aRadius);
  gp_Pnt aSecondAttach =
    myCenterPoint.Translated(gp_Vec(myCenterPoint, mySecondPoint).Normalized() * aRadius);

  gce_MakeCirc aConstructCircle(myCenterPoint, GetPlane(), aRadius);
  if (!aConstructCircle.IsDone())
  {
    return;
  }
  gp_Circ aCircle = aConstructCircle.Value();

  // Default values
  theExtensionSize = aDimensionAspect->ArrowAspect()->Length();
  theAlignment     = Prs3d_DTHP_Center;

  double aParamBeg = ElCLib::Parameter(aCircle, aFirstAttach);
  double aParamEnd = ElCLib::Parameter(aCircle, aSecondAttach);
  if (aParamEnd < aParamBeg)
  {
    double aParam = aParamEnd;
    aParamEnd     = aParamBeg;
    aParamBeg     = aParam;
  }

  ElCLib::AdjustPeriodic(0.0, M_PI * 2, Precision::PConfusion(), aParamBeg, aParamEnd);
  double aTextPar = ElCLib::Parameter(aCircle, theTextPos);

  // Horizontal center
  if (aTextPar > aParamBeg && aTextPar < aParamEnd)
  {
    theFlyout = aRadius;
    return;
  }

  aParamBeg += M_PI;
  aParamEnd += M_PI;
  ElCLib::AdjustPeriodic(0.0, M_PI * 2, Precision::PConfusion(), aParamBeg, aParamEnd);

  if (aTextPar > aParamBeg && aTextPar < aParamEnd)
  {
    theFlyout = -aRadius;
    return;
  }

  // Text on the extensions
  gp_Lin aFirstLine      = gce_MakeLin(myCenterPoint, myFirstPoint);
  gp_Lin aSecondLine     = gce_MakeLin(myCenterPoint, mySecondPoint);
  gp_Pnt aFirstTextProj  = PrsDim::Nearest(aFirstLine, theTextPos);
  gp_Pnt aSecondTextProj = PrsDim::Nearest(aSecondLine, theTextPos);
  double aFirstDist      = aFirstTextProj.Distance(theTextPos);
  double aSecondDist     = aSecondTextProj.Distance(theTextPos);

  if (aFirstDist <= aSecondDist)
  {
    aRadius                  = myCenterPoint.Distance(aFirstTextProj);
    double aNewExtensionSize = aFirstDist - anArrowLength;
    theExtensionSize         = aNewExtensionSize < 0.0 ? 0.0 : aNewExtensionSize;

    theAlignment = Prs3d_DTHP_Left;

    gp_Vec aPosFlyoutDir = gp_Vec(myCenterPoint, myFirstPoint).Normalized().Scaled(aRadius);

    theFlyout =
      aFirstTextProj.Distance(myCenterPoint.Translated(aPosFlyoutDir)) > Precision::Confusion()
        ? -aRadius
        : aRadius;
  }
  else
  {
    aRadius = myCenterPoint.Distance(aSecondTextProj);

    double aNewExtensionSize = aSecondDist - anArrowLength;

    theExtensionSize = aNewExtensionSize < 0.0 ? 0.0 : aNewExtensionSize;

    theAlignment = Prs3d_DTHP_Right;

    gp_Vec aPosFlyoutDir = gp_Vec(myCenterPoint, mySecondPoint).Normalized().Scaled(aRadius);

    theFlyout =
      aSecondTextProj.Distance(myCenterPoint.Translated(aPosFlyoutDir)) > Precision::Confusion()
        ? -aRadius
        : aRadius;
  }
}

//=================================================================================================

void PrsDim_AngleDimension::FitTextAlignment(
  const Prs3d_DimensionTextHorizontalPosition& theHorizontalTextPos,
  int&                                         theLabelPosition,
  bool&                                        theIsArrowsExternal) const
{
  occ::handle<Prs3d_DimensionAspect> aDimensionAspect = myDrawer->DimensionAspect();

  double anArrowLength = aDimensionAspect->ArrowAspect()->Length();

  // Prepare label string and compute its geometrical width
  double                     aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString(aLabelWidth);

  // add margins to label width
  if (aDimensionAspect->IsText3d())
  {
    aLabelWidth += aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN * 2.0;
  }

  gp_Pnt aFirstAttach =
    myCenterPoint.Translated(gp_Vec(myCenterPoint, myFirstPoint).Normalized() * GetFlyout());
  gp_Pnt aSecondAttach =
    myCenterPoint.Translated(gp_Vec(myCenterPoint, mySecondPoint).Normalized() * GetFlyout());

  // Handle user-defined and automatic arrow placement
  switch (aDimensionAspect->ArrowOrientation())
  {
    case Prs3d_DAO_External:
      theIsArrowsExternal = true;
      break;
    case Prs3d_DAO_Internal:
      theIsArrowsExternal = false;
      break;
    case Prs3d_DAO_Fit: {
      gp_Vec anAttachVector(aFirstAttach, aSecondAttach);
      double aDimensionWidth = anAttachVector.Magnitude();

      // Add margin to ensure a small tail between text and arrow
      double anArrowMargin = aDimensionAspect->IsText3d()
                               ? aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN
                               : 0.0;

      double anArrowsWidth = (anArrowLength + anArrowMargin) * 2.0;

      theIsArrowsExternal = aDimensionWidth < aLabelWidth + anArrowsWidth;
      break;
    }
  }

  // Handle user-defined and automatic text placement
  switch (theHorizontalTextPos)
  {
    case Prs3d_DTHP_Left:
      theLabelPosition |= LabelPosition_Left;
      break;
    case Prs3d_DTHP_Right:
      theLabelPosition |= LabelPosition_Right;
      break;
    case Prs3d_DTHP_Center:
      theLabelPosition |= LabelPosition_HCenter;
      break;
    case Prs3d_DTHP_Fit: {
      gp_Vec anAttachVector(aFirstAttach, aSecondAttach);
      double aDimensionWidth = anAttachVector.Magnitude();
      double anArrowsWidth   = anArrowLength * 2.0;
      double aContentWidth   = theIsArrowsExternal ? aLabelWidth : aLabelWidth + anArrowsWidth;

      theLabelPosition |=
        aDimensionWidth < aContentWidth ? LabelPosition_Left : LabelPosition_HCenter;
      break;
    }
  }

  switch (aDimensionAspect->TextVerticalPosition())
  {
    case Prs3d_DTVP_Above:
      theLabelPosition |= LabelPosition_Above;
      break;
    case Prs3d_DTVP_Below:
      theLabelPosition |= LabelPosition_Below;
      break;
    case Prs3d_DTVP_Center:
      theLabelPosition |= LabelPosition_VCenter;
      break;
  }
}
