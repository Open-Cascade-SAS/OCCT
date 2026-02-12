// Created on: 2013-11-11
// Created by: Anastasia BORISOVA
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <PrsDim_Dimension.hxx>

#include <PrsDim.hxx>
#include <PrsDim_DimensionOwner.hxx>
#include <AIS_InteractiveContext.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <ElCLib.hxx>
#include <Font_BRepTextBuilder.hxx>
#include <GC_MakeCircle.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gce_MakeDir.hxx>
#include <gce_MakeLin.hxx>
#include <gce_MakePln.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <Poly_Triangle.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_ProgramError.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <V3d_Viewer.hxx>
#include <Units_UnitsDictionary.hxx>
#include <UnitsAPI.hxx>

IMPLEMENT_STANDARD_RTTIEXT(PrsDim_Dimension, AIS_InteractiveObject)

namespace
{
// default text margin and resolution
constexpr double THE_3D_TEXT_MARGIN = 0.1;

// default selection priorities
constexpr int THE_NEUTRAL_SEL_PRIORITY = 5;
constexpr int THE_LOCAL_SEL_PRIORITY   = 6;
} // namespace

//=================================================================================================

PrsDim_Dimension::PrsDim_Dimension(const PrsDim_KindOfDimension theType)
    : mySelToleranceForText2d(0.0),
      myValueType(ValueType_Computed),
      myCustomValue(0.0),

      myIsTextPositionFixed(false),
      mySpecialSymbol(' '),
      myDisplaySpecialSymbol(PrsDim_DisplaySpecialSymbol_No),
      myGeometryType(GeometryType_UndefShapes),
      myIsPlaneCustom(false),
      myFlyout(0.0),
      myIsGeometryValid(false),
      myKindOfDimension(theType)
{
}

//=================================================================================================

void PrsDim_Dimension::SetCustomValue(const double theValue)
{
  if (myValueType == ValueType_CustomReal && myCustomValue == theValue)
  {
    return;
  }

  myValueType   = ValueType_CustomReal;
  myCustomValue = theValue;

  SetToUpdate();
}

//=================================================================================================

void PrsDim_Dimension::SetCustomValue(const TCollection_ExtendedString& theValue)
{
  if (myValueType == ValueType_CustomText && myCustomStringValue == theValue)
  {
    return;
  }

  myValueType         = ValueType_CustomText;
  myCustomStringValue = theValue;

  SetToUpdate();
}

//=================================================================================================

void PrsDim_Dimension::SetCustomPlane(const gp_Pln& thePlane)
{
  myPlane         = thePlane;
  myIsPlaneCustom = true;

  // Disable fixed (custom) text position
  UnsetFixedTextPosition();

  // Check validity if geometry has been set already.
  if (IsValid())
  {
    SetToUpdate();
  }
}

//=================================================================================================

void PrsDim_Dimension::SetDimensionAspect(
  const occ::handle<Prs3d_DimensionAspect>& theDimensionAspect)
{
  myDrawer->SetDimensionAspect(theDimensionAspect);

  SetToUpdate();
}

//=================================================================================================

void PrsDim_Dimension::SetDisplaySpecialSymbol(
  const PrsDim_DisplaySpecialSymbol theDisplaySpecSymbol)
{
  if (myDisplaySpecialSymbol == theDisplaySpecSymbol)
  {
    return;
  }

  myDisplaySpecialSymbol = theDisplaySpecSymbol;

  SetToUpdate();
}

//=================================================================================================

void PrsDim_Dimension::SetSpecialSymbol(const char16_t theSpecialSymbol)
{
  if (mySpecialSymbol == theSpecialSymbol)
  {
    return;
  }

  mySpecialSymbol = theSpecialSymbol;

  SetToUpdate();
}

//=================================================================================================

void PrsDim_Dimension::SetSelToleranceForText2d(const double theTol)
{
  if (mySelToleranceForText2d == theTol)
  {
    return;
  }

  mySelToleranceForText2d = theTol;

  SetToUpdate();
}

//=================================================================================================

void PrsDim_Dimension::SetFlyout(const double theFlyout)
{
  if (myFlyout == theFlyout)
  {
    return;
  }

  myFlyout = theFlyout;

  // Disable fixed text position
  UnsetFixedTextPosition();

  SetToUpdate();
}

//=================================================================================================

const TCollection_AsciiString& PrsDim_Dimension::GetDisplayUnits() const
{
  return TCollection_AsciiString::EmptyString();
}

//=================================================================================================

const TCollection_AsciiString& PrsDim_Dimension::GetModelUnits() const
{
  return TCollection_AsciiString::EmptyString();
}

//=================================================================================================

double PrsDim_Dimension::ValueToDisplayUnits() const
{
  return UnitsAPI::AnyToAny(GetValue(), GetModelUnits().ToCString(), GetDisplayUnits().ToCString());
}

//=================================================================================================

TCollection_ExtendedString PrsDim_Dimension::GetValueString(double& theWidth) const
{
  TCollection_ExtendedString aValueStr;
  if (myValueType == ValueType_CustomText)
  {
    aValueStr = myCustomStringValue;
  }
  else
  {
    // format value string using "Sprintf"
    TCollection_AsciiString aFormatStr = myDrawer->DimensionAspect()->ValueStringFormat();

    char aFmtBuffer[256];
    Sprintf(aFmtBuffer, aFormatStr.ToCString(), ValueToDisplayUnits());
    aValueStr = TCollection_ExtendedString(aFmtBuffer);
  }

  // add units to values string
  if (myDrawer->DimensionAspect()->IsUnitsDisplayed())
  {
    aValueStr += " ";
    aValueStr += TCollection_ExtendedString(GetDisplayUnits());
  }

  switch (myDisplaySpecialSymbol)
  {
    case PrsDim_DisplaySpecialSymbol_Before:
      aValueStr.Insert(1, mySpecialSymbol);
      break;
    case PrsDim_DisplaySpecialSymbol_After:
      aValueStr.Insert(aValueStr.Length() + 1, mySpecialSymbol);
      break;
    case PrsDim_DisplaySpecialSymbol_No:
      break;
  }

  // Get text style parameters
  occ::handle<Prs3d_TextAspect> aTextAspect = myDrawer->DimensionAspect()->TextAspect();
  NCollection_UtfString<char>   anUTFString(aValueStr.ToExtString());

  theWidth = 0.0;

  if (myDrawer->DimensionAspect()->IsText3d())
  {
    // text width produced by BRepFont
    Font_BRepFont aFont;
    if (aFont.FindAndInit(aTextAspect->Aspect()->Font(),
                          aTextAspect->Aspect()->GetTextFontAspect(),
                          aTextAspect->Height(),
                          Font_StrictLevel_Any))
    {
      for (NCollection_UtfIterator<char> anIter = anUTFString.Iterator(); *anIter != 0;)
      {
        char32_t aCurrChar = *anIter;
        char32_t aNextChar = *(++anIter);
        theWidth += aFont.AdvanceX(aCurrChar, aNextChar);
      }
    }
  }
  else
  {
    // Text width for 1:1 scale 2D case
    Font_FTFontParams                aFontParams;
    const Graphic3d_RenderingParams& aRendParams =
      GetContext()->CurrentViewer()->DefaultRenderingParams();
    aFontParams.PointSize   = (unsigned int)aTextAspect->Height();
    aFontParams.Resolution  = aRendParams.Resolution;
    aFontParams.FontHinting = aRendParams.FontHinting;
    if (occ::handle<Font_FTFont> aFont =
          Font_FTFont::FindAndCreate(aTextAspect->Aspect()->Font(),
                                     aTextAspect->Aspect()->GetTextFontAspect(),
                                     aFontParams,
                                     Font_StrictLevel_Any))
    {
      for (NCollection_UtfIterator<char> anIter = anUTFString.Iterator(); *anIter != 0;)
      {
        char32_t aCurrChar = *anIter;
        char32_t aNextChar = *(++anIter);
        theWidth += (double)aFont->AdvanceX(aCurrChar, aNextChar);
      }
    }
  }

  return aValueStr;
}

//=================================================================================================

void PrsDim_Dimension::DrawArrow(const occ::handle<Prs3d_Presentation>& thePresentation,
                                 const gp_Pnt&                          theLocation,
                                 const gp_Dir&                          theDirection)
{
  occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();

  double aLength    = myDrawer->DimensionAspect()->ArrowAspect()->Length();
  double anAngle    = myDrawer->DimensionAspect()->ArrowAspect()->Angle();
  bool   isZoomable = myDrawer->DimensionAspect()->ArrowAspect()->IsZoomable();

  if (myDrawer->DimensionAspect()->IsArrows3d())
  {
    Prs3d_Arrow::Draw(aGroup, theLocation, theDirection, anAngle, aLength);
    aGroup->SetGroupPrimitivesAspect(myDrawer->DimensionAspect()->ArrowAspect()->Aspect());
  }
  else
  {
    gp_Pnt        aLocation = isZoomable ? theLocation : gp::Origin();
    gp_Pnt        aLeftPoint(gp::Origin());
    gp_Pnt        aRightPoint(gp::Origin());
    const gp_Dir& aPlane = GetPlane().Axis().Direction();

    PointsForArrow(aLocation, theDirection, aPlane, aLength, anAngle, aLeftPoint, aRightPoint);

    occ::handle<Graphic3d_ArrayOfTriangles> anArrow = new Graphic3d_ArrayOfTriangles(3);

    anArrow->AddVertex(aLeftPoint);
    anArrow->AddVertex(aLocation);
    anArrow->AddVertex(aRightPoint);

    // Set aspect for arrow triangles
    Graphic3d_PolygonOffset aPolOffset;
    aPolOffset.Mode                                       = Aspect_POM_Off;
    aPolOffset.Factor                                     = 0.0f;
    aPolOffset.Units                                      = 0.0f;
    occ::handle<Graphic3d_AspectFillArea3d> aShadingStyle = new Graphic3d_AspectFillArea3d();
    aShadingStyle->SetInteriorStyle(Aspect_IS_SOLID);
    aShadingStyle->SetColor(myDrawer->DimensionAspect()->ArrowAspect()->Aspect()->Color());
    aShadingStyle->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
    aShadingStyle->SetPolygonOffset(aPolOffset);

    aGroup->SetPrimitivesAspect(aShadingStyle);
    aGroup->AddPrimitiveArray(anArrow);
    if (!isZoomable)
    {
      aGroup->SetTransformPersistence(
        new Graphic3d_TransformPers(Graphic3d_TMF_ZoomPers, theLocation));
    }
  }

  SelectionGeometry::Arrow& aSensitiveArrow = mySelectionGeom.NewArrow();
  aSensitiveArrow.Position                  = theLocation;
  aSensitiveArrow.Direction                 = theDirection;
}

//=================================================================================================

void PrsDim_Dimension::drawText(const occ::handle<Prs3d_Presentation>& thePresentation,
                                const gp_Pnt&                          theTextPos,
                                const gp_Dir&                          theTextDir,
                                const TCollection_ExtendedString&      theText,
                                const int                              theLabelPosition)
{
  occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();
  if (myDrawer->DimensionAspect()->IsText3d())
  {
    // getting font parameters
    occ::handle<Prs3d_TextAspect> aTextAspect = myDrawer->DimensionAspect()->TextAspect();
    Quantity_Color                aColor      = aTextAspect->Aspect()->Color();
    Font_FontAspect               aFontAspect = aTextAspect->Aspect()->GetTextFontAspect();
    double                        aFontHeight = aTextAspect->Height();

    // creating TopoDS_Shape for text
    Font_BRepFont aFont(aTextAspect->Aspect()->Font().ToCString(), aFontAspect, aFontHeight);
    NCollection_UtfString<char> anUTFString(theText.ToExtString());

    Font_BRepTextBuilder aBuilder;
    TopoDS_Shape         aTextShape = aBuilder.Perform(aFont, anUTFString);

    // compute text width with kerning
    double aTextWidth  = 0.0;
    double aTextHeight = aFont.Ascender() + aFont.Descender();

    for (NCollection_UtfIterator<char> anIter = anUTFString.Iterator(); *anIter != 0;)
    {
      char32_t aCurrChar = *anIter;
      char32_t aNextChar = *(++anIter);
      aTextWidth += aFont.AdvanceX(aCurrChar, aNextChar);
    }

    // formatting text position in XOY plane
    int aHLabelPos = theLabelPosition & LabelPosition_HMask;
    int aVLabelPos = theLabelPosition & LabelPosition_VMask;

    gp_Dir aTextDir(aHLabelPos == LabelPosition_Left ? -theTextDir : theTextDir);

    // compute label offsets
    double aMarginSize    = aFontHeight * THE_3D_TEXT_MARGIN;
    double aCenterHOffset = 0.0;
    double aCenterVOffset = 0.0;
    switch (aHLabelPos)
    {
      case LabelPosition_HCenter:
        aCenterHOffset = 0.0;
        break;
      case LabelPosition_Right:
        aCenterHOffset = aTextWidth / 2.0 + aMarginSize;
        break;
      case LabelPosition_Left:
        aCenterHOffset = -aTextWidth / 2.0 - aMarginSize;
        break;
    }
    switch (aVLabelPos)
    {
      case LabelPosition_VCenter:
        aCenterVOffset = 0.0;
        break;
      case LabelPosition_Above:
        aCenterVOffset = aTextHeight / 2.0 + aMarginSize;
        break;
      case LabelPosition_Below:
        aCenterVOffset = -aTextHeight / 2.0 - aMarginSize;
        break;
    }

    // compute shape offset transformation
    double aShapeHOffset = aCenterHOffset - aTextWidth / 2.0;
    double aShapeVOffset = aCenterVOffset - aTextHeight / 2.0;

    // center shape in its bounding box (suppress border spacing added by FT_Font)
    Bnd_Box aShapeBnd;
    BRepBndLib::AddClose(aTextShape, aShapeBnd);

    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aShapeBnd.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    double aXalign = aTextWidth * 0.5 - (aXmax + aXmin) * 0.5;
    double aYalign = aTextHeight * 0.5 - (aYmax + aYmin) * 0.5;
    aShapeHOffset += aXalign;
    aShapeVOffset += aYalign;

    gp_Trsf anOffsetTrsf;
    anOffsetTrsf.SetTranslation(gp::Origin(), gp_Pnt(aShapeHOffset, aShapeVOffset, 0.0));
    aTextShape.Move(anOffsetTrsf);

    // transform text to myWorkingPlane coordinate system
    gp_Ax3  aTextCoordSystem(theTextPos, GetPlane().Axis().Direction(), aTextDir);
    gp_Trsf aTextPlaneTrsf;
    aTextPlaneTrsf.SetTransformation(aTextCoordSystem, gp_Ax3(gp::XOY()));
    aTextShape.Move(aTextPlaneTrsf);

    // set text flipping anchors
    gp_Trsf aCenterOffsetTrsf;
    gp_Pnt  aCenterOffset(aCenterHOffset, aCenterVOffset, 0.0);
    aCenterOffsetTrsf.SetTranslation(gp::Origin(), aCenterOffset);

    gp_Pnt aCenterOfLabel(gp::Origin());
    aCenterOfLabel.Transform(aCenterOffsetTrsf);
    aCenterOfLabel.Transform(aTextPlaneTrsf);

    gp_Ax2 aFlippingAxes(aCenterOfLabel, GetPlane().Axis().Direction(), aTextDir);
    aGroup->SetFlippingOptions(true, aFlippingAxes);

    // draw text
    if (myDrawer->DimensionAspect()->IsTextShaded())
    {
      // Setting text shading and color parameters
      if (!myDrawer->HasOwnShadingAspect())
        myDrawer->SetShadingAspect(new Prs3d_ShadingAspect());

      myDrawer->ShadingAspect()->Aspect()->SetShadingModel(Graphic3d_TypeOfShadingModel_Unlit);
      myDrawer->ShadingAspect()->Aspect()->SetInteriorColor(aColor);
      myDrawer->ShadingAspect()->Aspect()->SetBackInteriorColor(aColor);

      // drawing text
      StdPrs_ShadedShape::Add(thePresentation, aTextShape, myDrawer);
    }
    else
    {
      // Setting color for text
      if (!myDrawer->HasOwnFreeBoundaryAspect())
      {
        myDrawer->SetFreeBoundaryAspect(new Prs3d_LineAspect(aColor, Aspect_TOL_SOLID, 1.0));
      }
      myDrawer->FreeBoundaryAspect()->Aspect()->SetColor(aColor);

      // drawing text
      if (occ::handle<Graphic3d_ArrayOfPrimitives> anEdges =
            StdPrs_WFShape::AddAllEdges(aTextShape, myDrawer))
      {
        aGroup->SetGroupPrimitivesAspect(myDrawer->FreeBoundaryAspect()->Aspect());
        aGroup->AddPrimitiveArray(anEdges);
      }
    }
    thePresentation->CurrentGroup()->SetFlippingOptions(false, gp_Ax2());

    mySelectionGeom.TextPos    = aCenterOfLabel;
    mySelectionGeom.TextDir    = aTextDir;
    mySelectionGeom.TextWidth  = aTextWidth + aMarginSize * 2.0;
    mySelectionGeom.TextHeight = aTextHeight;

    return;
  }

  // generate primitives for 2D text
  myDrawer->DimensionAspect()->TextAspect()->Aspect()->SetDisplayType(Aspect_TODT_DIMENSION);

  Prs3d_Text::Draw(aGroup, myDrawer->DimensionAspect()->TextAspect(), theText, theTextPos);

  mySelectionGeom.TextPos    = theTextPos;
  mySelectionGeom.TextDir    = theTextDir;
  mySelectionGeom.TextWidth  = 0.0;
  mySelectionGeom.TextHeight = 0.0;
}

//=================================================================================================

void PrsDim_Dimension::DrawExtension(const occ::handle<Prs3d_Presentation>& thePresentation,
                                     const double                           theExtensionSize,
                                     const gp_Pnt&                          theExtensionStart,
                                     const gp_Dir&                          theExtensionDir,
                                     const TCollection_ExtendedString&      theLabelString,
                                     const double                           theLabelWidth,
                                     const int                              theMode,
                                     const int                              theLabelPosition)
{
  // reference line for extension starting at its connection point
  gp_Lin anExtensionLine(theExtensionStart, theExtensionDir);

  bool hasLabel = theLabelString.Length() > 0;
  if (hasLabel && (theMode == ComputeMode_All || theMode == ComputeMode_Text))
  {
    // compute text primitives; get its model width
    gp_Pnt aTextPos = ElCLib::Value(theExtensionSize, anExtensionLine);
    gp_Dir aTextDir = theExtensionDir;

    occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();
    drawText(thePresentation, aTextPos, aTextDir, theLabelString, theLabelPosition);
  }

  if (theMode != ComputeMode_All && theMode != ComputeMode_Line)
  {
    return;
  }

  bool isShortLine =
    !myDrawer->DimensionAspect()->IsText3d() || theLabelPosition & LabelPosition_VCenter;

  // compute graphical primitives and sensitives for extension line
  gp_Pnt anExtStart = theExtensionStart;
  gp_Pnt anExtEnd   = !hasLabel || isShortLine
                        ? ElCLib::Value(theExtensionSize, anExtensionLine)
                        : ElCLib::Value(theExtensionSize + theLabelWidth, anExtensionLine);

  // add graphical primitives
  occ::handle<Graphic3d_ArrayOfSegments> anExtPrimitive = new Graphic3d_ArrayOfSegments(2);
  anExtPrimitive->AddVertex(anExtStart);
  anExtPrimitive->AddVertex(anExtEnd);

  // add selection primitives
  SelectionGeometry::Curve& aSensitiveCurve = mySelectionGeom.NewCurve();
  aSensitiveCurve.Append(anExtStart);
  aSensitiveCurve.Append(anExtEnd);

  occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();
  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == ComputeMode_All)
  {
    aGroup->SetStencilTestOptions(true);
  }
  occ::handle<Graphic3d_AspectLine3d> aDimensionLineStyle =
    myDrawer->DimensionAspect()->LineAspect()->Aspect();
  aGroup->SetPrimitivesAspect(aDimensionLineStyle);
  aGroup->AddPrimitiveArray(anExtPrimitive);
  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == ComputeMode_All)
  {
    aGroup->SetStencilTestOptions(false);
  }
}

//=================================================================================================

void PrsDim_Dimension::DrawLinearDimension(const occ::handle<Prs3d_Presentation>& thePresentation,
                                           const int                              theMode,
                                           const gp_Pnt&                          theFirstPoint,
                                           const gp_Pnt&                          theSecondPoint,
                                           const bool                             theIsOneSide)
{
  // do not build any dimension for equal points
  if (theFirstPoint.IsEqual(theSecondPoint, Precision::Confusion()))
  {
    throw Standard_ProgramError("Can not build presentation for equal points.");
  }

  occ::handle<Prs3d_DimensionAspect> aDimensionAspect = myDrawer->DimensionAspect();

  // For extensions we need to know arrow size, text size and extension size: get it from aspect
  double anArrowLength   = aDimensionAspect->ArrowAspect()->Length();
  double anExtensionSize = aDimensionAspect->ExtensionSize();
  // prepare label string and compute its geometrical width
  double                     aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString(aLabelWidth);

  // add margins to cut dimension lines for 3d text
  if (aDimensionAspect->IsText3d())
  {
    aLabelWidth += aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN * 2.0;
  }

  // handle user-defined and automatic arrow placement
  bool isArrowsExternal = false;
  int  aLabelPosition   = LabelPosition_None;

  Prs3d_DimensionTextHorizontalPosition aHorisontalTextPos =
    aDimensionAspect->TextHorizontalPosition();
  if (IsTextPositionCustom())
  {
    if (!AdjustParametersForLinear(myFixedTextPosition,
                                   theFirstPoint,
                                   theSecondPoint,
                                   anExtensionSize,
                                   aHorisontalTextPos,
                                   myFlyout,
                                   myPlane,
                                   myIsPlaneCustom))
    {
      throw Standard_ProgramError("Can not adjust plane to the custom label position.");
    }
  }

  FitTextAlignmentForLinear(theFirstPoint,
                            theSecondPoint,
                            theIsOneSide,
                            aHorisontalTextPos,
                            aLabelPosition,
                            isArrowsExternal);

  // compute dimension line points
  gp_Pnt aLineBegPoint, aLineEndPoint;
  ComputeFlyoutLinePoints(theFirstPoint, theSecondPoint, aLineBegPoint, aLineEndPoint);
  gp_Lin aDimensionLine = gce_MakeLin(aLineBegPoint, aLineEndPoint);

  // compute arrows positions and directions
  gp_Dir aFirstArrowDir      = aDimensionLine.Direction().Reversed();
  gp_Dir aSecondArrowDir     = aDimensionLine.Direction();
  gp_Dir aFirstExtensionDir  = aDimensionLine.Direction().Reversed();
  gp_Dir aSecondExtensionDir = aDimensionLine.Direction();

  gp_Pnt aFirstArrowBegin(0.0, 0.0, 0.0);
  gp_Pnt aFirstArrowEnd(0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowBegin(0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowEnd(0.0, 0.0, 0.0);

  if (isArrowsExternal)
  {
    aFirstArrowDir.Reverse();
    aSecondArrowDir.Reverse();
  }

  aFirstArrowBegin  = aLineBegPoint;
  aSecondArrowBegin = aLineEndPoint;
  aFirstArrowEnd    = aLineBegPoint;
  aSecondArrowEnd   = aLineEndPoint;

  if (aDimensionAspect->ArrowAspect()->IsZoomable())
  {
    aFirstArrowEnd.Translate(-gp_Vec(aFirstArrowDir).Scaled(anArrowLength));
    aSecondArrowEnd.Translate(-gp_Vec(aSecondArrowDir).Scaled(anArrowLength));
  }

  gp_Pnt aCenterLineBegin = isArrowsExternal ? aLineBegPoint : aFirstArrowEnd;

  gp_Pnt aCenterLineEnd = isArrowsExternal || theIsOneSide ? aLineEndPoint : aSecondArrowEnd;

  switch (aLabelPosition & LabelPosition_HMask)
  {
    // ------------------------------------------------------------------------ //
    //                                CENTER                                    //
    // -------------------------------------------------------------------------//
    case LabelPosition_HCenter: {
      // add label on dimension or extension line to presentation
      gp_Pnt aTextPos = IsTextPositionCustom()
                          ? myFixedTextPosition
                          : (aCenterLineBegin.XYZ() + aCenterLineEnd.XYZ()) * 0.5;
      gp_Dir aTextDir = aDimensionLine.Direction();

      // add text primitives
      if (theMode == ComputeMode_All || theMode == ComputeMode_Text)
      {
        thePresentation->NewGroup();
        drawText(thePresentation, aTextPos, aTextDir, aLabelString, aLabelPosition);
      }

      // add dimension line primitives
      if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
      {
        bool isLineBreak = aDimensionAspect->TextVerticalPosition() == Prs3d_DTVP_Center
                           && aDimensionAspect->IsText3d();

        occ::handle<Graphic3d_ArrayOfSegments> aPrimSegments =
          new Graphic3d_ArrayOfSegments(isLineBreak ? 4 : 2);

        // compute continuous or sectioned main line segments
        if (isLineBreak)
        {
          double aPTextPosition = ElCLib::Parameter(aDimensionLine, aTextPos);
          gp_Pnt aSection1Beg   = aCenterLineBegin;
          gp_Pnt aSection1End = ElCLib::Value(aPTextPosition - (aLabelWidth * 0.5), aDimensionLine);
          gp_Pnt aSection2Beg = ElCLib::Value(aPTextPosition + (aLabelWidth * 0.5), aDimensionLine);
          gp_Pnt aSection2End = aCenterLineEnd;

          aPrimSegments->AddVertex(aSection1Beg);
          aPrimSegments->AddVertex(aSection1End);
          aPrimSegments->AddVertex(aSection2Beg);
          aPrimSegments->AddVertex(aSection2End);

          SelectionGeometry::Curve& aLeftSensitiveCurve  = mySelectionGeom.NewCurve();
          SelectionGeometry::Curve& aRightSensitiveCurve = mySelectionGeom.NewCurve();
          aLeftSensitiveCurve.Append(aSection1Beg);
          aLeftSensitiveCurve.Append(aSection1End);
          aRightSensitiveCurve.Append(aSection2Beg);
          aRightSensitiveCurve.Append(aSection2End);
        }
        else
        {
          aPrimSegments->AddVertex(aCenterLineBegin);
          aPrimSegments->AddVertex(aCenterLineEnd);

          SelectionGeometry::Curve& aSensitiveCurve = mySelectionGeom.NewCurve();
          aSensitiveCurve.Append(aCenterLineBegin);
          aSensitiveCurve.Append(aCenterLineEnd);
        }

        // set text label justification
        Graphic3d_VerticalTextAlignment aTextJustificaton = Graphic3d_VTA_BOTTOM;
        switch (aLabelPosition & LabelPosition_VMask)
        {
          case LabelPosition_Above:
          case LabelPosition_VCenter:
            aTextJustificaton = Graphic3d_VTA_BOTTOM;
            break;
          case LabelPosition_Below:
            aTextJustificaton = Graphic3d_VTA_TOP;
            break;
        }
        aDimensionAspect->TextAspect()->SetVerticalJustification(aTextJustificaton);

        // main dimension line, short extension
        {
          occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();
          if (!aDimensionAspect->IsText3d() && theMode == ComputeMode_All)
          {
            aGroup->SetStencilTestOptions(true);
          }
          aGroup->SetPrimitivesAspect(aDimensionAspect->LineAspect()->Aspect());
          aGroup->AddPrimitiveArray(aPrimSegments);
          if (!aDimensionAspect->IsText3d() && theMode == ComputeMode_All)
          {
            aGroup->SetStencilTestOptions(false);
          }
        }

        // add arrows to presentation
        {
          occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();
          DrawArrow(thePresentation, aFirstArrowBegin, aFirstArrowDir);
          if (!theIsOneSide)
          {
            DrawArrow(thePresentation, aSecondArrowBegin, aSecondArrowDir);
          }
        }

        if (!isArrowsExternal)
        {
          break;
        }

        // add arrow extension lines to presentation
        {
          DrawExtension(thePresentation,
                        aDimensionAspect->ArrowTailSize(),
                        aFirstArrowEnd,
                        aFirstExtensionDir,
                        TCollection_ExtendedString::EmptyString(),
                        0.0,
                        theMode,
                        LabelPosition_None);
          if (!theIsOneSide)
          {
            DrawExtension(thePresentation,
                          aDimensionAspect->ArrowTailSize(),
                          aSecondArrowEnd,
                          aSecondExtensionDir,
                          TCollection_ExtendedString::EmptyString(),
                          0.0,
                          theMode,
                          LabelPosition_None);
          }
        }
      }
      break;
    }
      // ------------------------------------------------------------------------ //
      //                                LEFT                                      //
      // -------------------------------------------------------------------------//

    case LabelPosition_Left: {
      // add label on dimension or extension line to presentation
      {
        // Left extension with the text
        DrawExtension(thePresentation,
                      anExtensionSize,
                      isArrowsExternal ? aFirstArrowEnd : aFirstArrowBegin,
                      aFirstExtensionDir,
                      aLabelString,
                      aLabelWidth,
                      theMode,
                      aLabelPosition);
      }

      // add dimension line primitives
      if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
      {
        // add central dimension line
        {
          occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();

          // add graphical primitives
          occ::handle<Graphic3d_ArrayOfSegments> aPrimSegments = new Graphic3d_ArrayOfSegments(2);
          aPrimSegments->AddVertex(aCenterLineBegin);
          aPrimSegments->AddVertex(aCenterLineEnd);

          aGroup->SetPrimitivesAspect(aDimensionAspect->LineAspect()->Aspect());
          aGroup->AddPrimitiveArray(aPrimSegments);

          // add selection primitives
          SelectionGeometry::Curve& aSensitiveCurve = mySelectionGeom.NewCurve();
          aSensitiveCurve.Append(aCenterLineBegin);
          aSensitiveCurve.Append(aCenterLineEnd);
        }

        // add arrows to presentation
        {
          occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();
          DrawArrow(thePresentation, aFirstArrowBegin, aFirstArrowDir);
          if (!theIsOneSide)
          {
            DrawArrow(thePresentation, aSecondArrowBegin, aSecondArrowDir);
          }
        }

        if (!isArrowsExternal || theIsOneSide)
        {
          break;
        }

        // add extension lines for external arrows
        {
          DrawExtension(thePresentation,
                        aDimensionAspect->ArrowTailSize(),
                        aSecondArrowEnd,
                        aSecondExtensionDir,
                        TCollection_ExtendedString::EmptyString(),
                        0.0,
                        theMode,
                        LabelPosition_None);
        }
      }

      break;
    }
      // ------------------------------------------------------------------------ //
      //                                RIGHT                                     //
      // -------------------------------------------------------------------------//

    case LabelPosition_Right: {
      // add label on dimension or extension line to presentation

      // Right extension with text
      DrawExtension(thePresentation,
                    anExtensionSize,
                    isArrowsExternal ? aSecondArrowEnd : aSecondArrowBegin,
                    aSecondExtensionDir,
                    aLabelString,
                    aLabelWidth,
                    theMode,
                    aLabelPosition);

      if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
      {
        // add central dimension line
        {
          occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();

          // add graphical primitives
          occ::handle<Graphic3d_ArrayOfSegments> aPrimSegments = new Graphic3d_ArrayOfSegments(2);
          aPrimSegments->AddVertex(aCenterLineBegin);
          aPrimSegments->AddVertex(aCenterLineEnd);
          aGroup->SetGroupPrimitivesAspect(aDimensionAspect->LineAspect()->Aspect());
          aGroup->AddPrimitiveArray(aPrimSegments);

          // add selection primitives
          SelectionGeometry::Curve& aSensitiveCurve = mySelectionGeom.NewCurve();
          aSensitiveCurve.Append(aCenterLineBegin);
          aSensitiveCurve.Append(aCenterLineEnd);
        }

        // add arrows to presentation
        {
          thePresentation->NewGroup();
          DrawArrow(thePresentation, aSecondArrowBegin, aSecondArrowDir);
          if (!theIsOneSide)
          {
            DrawArrow(thePresentation, aFirstArrowBegin, aFirstArrowDir);
          }
        }

        if (!isArrowsExternal || theIsOneSide)
        {
          break;
        }

        // add extension lines for external arrows
        {
          DrawExtension(thePresentation,
                        aDimensionAspect->ArrowTailSize(),
                        aFirstArrowEnd,
                        aFirstExtensionDir,
                        TCollection_ExtendedString::EmptyString(),
                        0.0,
                        theMode,
                        LabelPosition_None);
        }
      }

      break;
    }
  }

  // add flyout lines to presentation
  if (theMode == ComputeMode_All)
  {
    occ::handle<Graphic3d_Group> aGroup = thePresentation->NewGroup();

    occ::handle<Graphic3d_ArrayOfSegments> aPrimSegments = new Graphic3d_ArrayOfSegments(4);
    aPrimSegments->AddVertex(theFirstPoint);
    aPrimSegments->AddVertex(aLineBegPoint);

    aPrimSegments->AddVertex(theSecondPoint);
    aPrimSegments->AddVertex(aLineEndPoint);

    aGroup->SetGroupPrimitivesAspect(aDimensionAspect->LineAspect()->Aspect());
    aGroup->AddPrimitiveArray(aPrimSegments);
  }

  mySelectionGeom.IsComputed = true;
}

//=================================================================================================

void PrsDim_Dimension::ComputeFlyoutLinePoints(const gp_Pnt& theFirstPoint,
                                               const gp_Pnt& theSecondPoint,
                                               gp_Pnt&       theLineBegPoint,
                                               gp_Pnt&       theLineEndPoint)
{
  // compute dimension line points
  gp_Ax1 aPlaneNormal = GetPlane().Axis();
  // compute flyout direction vector
  gp_Dir aTargetPointsVector = gce_MakeDir(theFirstPoint, theSecondPoint);
  gp_Dir aFlyoutVector       = aPlaneNormal.Direction() ^ aTargetPointsVector;
  // create lines for layouts
  gp_Lin aLine1(theFirstPoint, aFlyoutVector);
  gp_Lin aLine2(theSecondPoint, aFlyoutVector);

  // Get flyout end points
  theLineBegPoint = ElCLib::Value(ElCLib::Parameter(aLine1, theFirstPoint) + GetFlyout(), aLine1);
  theLineEndPoint = ElCLib::Value(ElCLib::Parameter(aLine2, theSecondPoint) + GetFlyout(), aLine2);
}

//=================================================================================================

void PrsDim_Dimension::ComputeLinearFlyouts(const occ::handle<SelectMgr_Selection>&   theSelection,
                                            const occ::handle<SelectMgr_EntityOwner>& theOwner,
                                            const gp_Pnt&                             theFirstPoint,
                                            const gp_Pnt& theSecondPoint)
{
  // count flyout direction
  gp_Ax1 aPlaneNormal        = GetPlane().Axis();
  gp_Dir aTargetPointsVector = gce_MakeDir(theFirstPoint, theSecondPoint);

  // count a flyout direction vector.
  gp_Dir aFlyoutVector = aPlaneNormal.Direction() ^ aTargetPointsVector;

  // create lines for layouts
  gp_Lin aLine1(theFirstPoint, aFlyoutVector);
  gp_Lin aLine2(theSecondPoint, aFlyoutVector);

  // get flyout end points
  gp_Pnt aFlyoutEnd1 =
    ElCLib::Value(ElCLib::Parameter(aLine1, theFirstPoint) + GetFlyout(), aLine1);
  gp_Pnt aFlyoutEnd2 =
    ElCLib::Value(ElCLib::Parameter(aLine2, theSecondPoint) + GetFlyout(), aLine2);

  // fill sensitive entity for flyouts
  occ::handle<Select3D_SensitiveGroup> aSensitiveEntity = new Select3D_SensitiveGroup(theOwner);
  aSensitiveEntity->Add(new Select3D_SensitiveSegment(theOwner, theFirstPoint, aFlyoutEnd1));
  aSensitiveEntity->Add(new Select3D_SensitiveSegment(theOwner, theSecondPoint, aFlyoutEnd2));
  theSelection->Add(aSensitiveEntity);
}

//=======================================================================
// function : CircleFromPlanarFace
// purpose  : if possible computes circle from planar face
//=======================================================================
bool PrsDim_Dimension::CircleFromPlanarFace(const TopoDS_Face&       theFace,
                                            occ::handle<Geom_Curve>& theCurve,
                                            gp_Pnt&                  theFirstPoint,
                                            gp_Pnt&                  theLastPoint)
{
  TopExp_Explorer anIt(theFace, TopAbs_EDGE);
  for (; anIt.More(); anIt.Next())
  {
    TopoDS_Edge aCurEdge = TopoDS::Edge(anIt.Current());
    if (PrsDim::ComputeGeometry(aCurEdge, theCurve, theFirstPoint, theLastPoint))
    {
      if (theCurve->IsInstance(STANDARD_TYPE(Geom_Circle)))
      {
        return true;
      }
    }
  }
  return false;
}

//=======================================================================
// function : CircleFromEdge
// purpose  : if possible computes circle from edge
//=======================================================================
bool PrsDim_Dimension::CircleFromEdge(const TopoDS_Edge& theEdge,
                                      gp_Circ&           theCircle,
                                      gp_Pnt&            theFirstPoint,
                                      gp_Pnt&            theLastPoint)
{
  BRepAdaptor_Curve anAdaptedCurve(theEdge);
  switch (anAdaptedCurve.GetType())
  {
    case GeomAbs_Circle: {
      theCircle = anAdaptedCurve.Circle();
      break;
    }
    case GeomAbs_Ellipse: {
      gp_Elips anEll = anAdaptedCurve.Ellipse();
      if ((anEll.MinorRadius() - anEll.MajorRadius()) >= Precision::Confusion())
      {
        return false;
      }
      theCircle = gp_Circ(anEll.Position(), anEll.MinorRadius());
      break;
    }
    case GeomAbs_Line:
    case GeomAbs_Hyperbola:
    case GeomAbs_Parabola:
    case GeomAbs_BezierCurve:
    case GeomAbs_BSplineCurve:
    case GeomAbs_OtherCurve:
    default:
      return false;
  }

  theFirstPoint = anAdaptedCurve.Value(anAdaptedCurve.FirstParameter());
  theLastPoint  = anAdaptedCurve.Value(anAdaptedCurve.LastParameter());
  return true;
}

//=================================================================================================

bool PrsDim_Dimension::InitCircularDimension(const TopoDS_Shape& theShape,
                                             gp_Circ&            theCircle,
                                             gp_Pnt&             theMiddleArcPoint,
                                             bool&               theIsClosed)
{
  gp_Pln                    aPln;
  occ::handle<Geom_Surface> aBasisSurf;
  PrsDim_KindOfSurface      aSurfType = PrsDim_KOS_OtherSurface;
  gp_Pnt                    aFirstPoint, aLastPoint;
  double                    anOffset    = 0.0;
  double                    aFirstParam = 0.0;
  double                    aLastParam  = 0.0;

  // Discover circular geometry
  switch (theShape.ShapeType())
  {
    case TopAbs_FACE: {
      PrsDim::GetPlaneFromFace(TopoDS::Face(theShape), aPln, aBasisSurf, aSurfType, anOffset);

      if (aSurfType == PrsDim_KOS_Plane)
      {
        occ::handle<Geom_Curve> aCurve;
        if (!CircleFromPlanarFace(TopoDS::Face(theShape), aCurve, aFirstPoint, aLastPoint))
        {
          return false;
        }

        theCircle = occ::down_cast<Geom_Circle>(aCurve)->Circ();
      }
      else
      {
        gp_Pnt              aCurPos;
        BRepAdaptor_Surface aSurf1(TopoDS::Face(theShape));
        double              aFirstU = aSurf1.FirstUParameter();
        double              aLastU  = aSurf1.LastUParameter();
        double              aFirstV = aSurf1.FirstVParameter();
        double              aLastV  = aSurf1.LastVParameter();
        double              aMidU   = (aFirstU + aLastU) * 0.5;
        double              aMidV   = (aFirstV + aLastV) * 0.5;
        aSurf1.D0(aMidU, aMidV, aCurPos);
        occ::handle<Adaptor3d_Curve> aBasisCurve;
        bool                         isExpectedType = false;
        if (aSurfType == PrsDim_KOS_Cylinder)
        {
          isExpectedType = true;
        }
        else
        {
          if (aSurfType == PrsDim_KOS_Revolution)
          {
            aBasisCurve = aSurf1.BasisCurve();
            if (aBasisCurve->GetType() == GeomAbs_Line)
            {
              isExpectedType = true;
            }
          }
          else if (aSurfType == PrsDim_KOS_Extrusion)
          {
            aBasisCurve = aSurf1.BasisCurve();
            if (aBasisCurve->GetType() == GeomAbs_Circle)
            {
              isExpectedType = true;
            }
          }
        }

        if (!isExpectedType)
        {
          return false;
        }

        occ::handle<Geom_Curve> aCurve = aBasisSurf->VIso(aMidV);
        if (aCurve->DynamicType() == STANDARD_TYPE(Geom_Circle))
        {
          theCircle = occ::down_cast<Geom_Circle>(aCurve)->Circ();
        }
        else if (aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
        {
          occ::handle<Geom_TrimmedCurve> aTrimmedCurve = occ::down_cast<Geom_TrimmedCurve>(aCurve);
          aFirstU                                      = aTrimmedCurve->FirstParameter();
          aLastU                                       = aTrimmedCurve->LastParameter();
          if (aTrimmedCurve->BasisCurve()->DynamicType() == STANDARD_TYPE(Geom_Circle))
          {
            theCircle = occ::down_cast<Geom_Circle>(aTrimmedCurve->BasisCurve())->Circ();
          }
        }
        else
        {
          // Compute a circle from 3 points on "aCurve"
          gp_Pnt aP1, aP2;
          aSurf1.D0(aFirstU, aMidV, aP1);
          aSurf1.D0(aLastU, aMidV, aP2);
          GC_MakeCircle aMkCirc(aP1, aCurPos, aP2);
          theCircle = aMkCirc.Value()->Circ();
        }

        aFirstPoint = ElCLib::Value(aFirstU, theCircle);
        aLastPoint  = ElCLib::Value(aLastU, theCircle);
      }
      break;
    }
    case TopAbs_WIRE: {
      TopoDS_Edge     anEdge;
      TopExp_Explorer anIt(theShape, TopAbs_EDGE);
      if (anIt.More())
      {
        anEdge = TopoDS::Edge(anIt.Current());
      }
      if (!PrsDim_Dimension::CircleFromEdge(anEdge, theCircle, aFirstPoint, aLastPoint))
      {
        return false;
      }
      break;
    }
    case TopAbs_EDGE: {
      TopoDS_Edge anEdge = TopoDS::Edge(theShape);
      if (!PrsDim_Dimension::CircleFromEdge(anEdge, theCircle, aFirstPoint, aLastPoint))
      {
        return false;
      }
      break;
    }
    case TopAbs_COMPOUND:
    case TopAbs_COMPSOLID:
    case TopAbs_SOLID:
    case TopAbs_SHELL:
    case TopAbs_VERTEX:
    case TopAbs_SHAPE:
    default:
      return false;
  }

  theIsClosed = aFirstPoint.IsEqual(aLastPoint, Precision::Confusion());

  gp_Pnt aCenter = theCircle.Location();

  if (theIsClosed) // Circle
  {
    gp_Dir anXDir     = theCircle.XAxis().Direction();
    theMiddleArcPoint = aCenter.Translated(gp_Vec(anXDir) * theCircle.Radius());
  }
  else // Arc
  {
    aFirstParam = ElCLib::Parameter(theCircle, aFirstPoint);
    aLastParam  = ElCLib::Parameter(theCircle, aLastPoint);
    if (aFirstParam > aLastParam)
    {
      aFirstParam -= 2.0 * M_PI;
    }

    double aParCurPos = (aFirstParam + aLastParam) * 0.5;
    gp_Vec aVec =
      gp_Vec(aCenter, ElCLib::Value(aParCurPos, theCircle)).Normalized() * theCircle.Radius();
    theMiddleArcPoint = aCenter.Translated(aVec);
  }

  return true;
}

//=================================================================================================

void PrsDim_Dimension::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                        const int                               theMode)
{
  if (!mySelectionGeom.IsComputed)
  {
    return;
  }

  PrsDim_DimensionSelectionMode aSelectionMode = (PrsDim_DimensionSelectionMode)theMode;

  // init appropriate entity owner
  occ::handle<SelectMgr_EntityOwner> aSensitiveOwner;

  switch (aSelectionMode)
  {
    // neutral selection owner
    case PrsDim_DimensionSelectionMode_All:
      aSensitiveOwner = new SelectMgr_EntityOwner(this, THE_NEUTRAL_SEL_PRIORITY);
      break;

    // local selection owners
    case PrsDim_DimensionSelectionMode_Line:
    case PrsDim_DimensionSelectionMode_Text:
      aSensitiveOwner = new PrsDim_DimensionOwner(this, aSelectionMode, THE_LOCAL_SEL_PRIORITY);
      break;
  }

  if (aSelectionMode == PrsDim_DimensionSelectionMode_All
      || aSelectionMode == PrsDim_DimensionSelectionMode_Line)
  {
    // sensitives for dimension line segments
    occ::handle<Select3D_SensitiveGroup> aGroupOfSensitives =
      new Select3D_SensitiveGroup(aSensitiveOwner);

    SelectionGeometry::SeqOfCurves::Iterator aCurveIt(mySelectionGeom.DimensionLine);
    for (; aCurveIt.More(); aCurveIt.Next())
    {
      const SelectionGeometry::HCurve& aCurveData = aCurveIt.Value();

      NCollection_Array1<gp_Pnt> aSensitivePnts(1, aCurveData->Length());
      for (int aPntIt = 1; aPntIt <= aCurveData->Length(); ++aPntIt)
      {
        aSensitivePnts.ChangeValue(aPntIt) = aCurveData->Value(aPntIt);
      }

      aGroupOfSensitives->Add(new Select3D_SensitiveCurve(aSensitiveOwner, aSensitivePnts));
    }

    double anArrowLength = myDrawer->DimensionAspect()->ArrowAspect()->Length();
    double anArrowAngle  = myDrawer->DimensionAspect()->ArrowAspect()->Angle();

    // sensitives for arrows
    SelectionGeometry::SeqOfArrows::Iterator anArrowIt(mySelectionGeom.Arrows);
    for (; anArrowIt.More(); anArrowIt.Next())
    {
      const SelectionGeometry::HArrow& anArrow = anArrowIt.Value();

      gp_Pnt        aSidePnt1(gp::Origin());
      gp_Pnt        aSidePnt2(gp::Origin());
      const gp_Dir& aPlane = GetPlane().Axis().Direction();
      const gp_Pnt& aPeak  = anArrow->Position;
      const gp_Dir& aDir   = anArrow->Direction;

      // compute points for arrow in plane
      PointsForArrow(aPeak, aDir, aPlane, anArrowLength, anArrowAngle, aSidePnt1, aSidePnt2);

      aGroupOfSensitives->Add(
        new Select3D_SensitiveTriangle(aSensitiveOwner, aPeak, aSidePnt1, aSidePnt2));

      if (!myDrawer->DimensionAspect()->IsArrows3d())
      {
        continue;
      }

      // compute points for orthogonal sensitive plane
      gp_Dir anOrthoPlane = anArrow->Direction.Crossed(aPlane);

      PointsForArrow(aPeak, aDir, anOrthoPlane, anArrowLength, anArrowAngle, aSidePnt1, aSidePnt2);

      aGroupOfSensitives->Add(
        new Select3D_SensitiveTriangle(aSensitiveOwner, aPeak, aSidePnt1, aSidePnt2));
    }

    theSelection->Add(aGroupOfSensitives);
  }

  // sensitives for text element
  if (aSelectionMode == PrsDim_DimensionSelectionMode_All
      || aSelectionMode == PrsDim_DimensionSelectionMode_Text)
  {
    occ::handle<Select3D_SensitiveEntity> aTextSensitive;

    gp_Ax2 aTextAxes(mySelectionGeom.TextPos,
                     GetPlane().Axis().Direction(),
                     mySelectionGeom.TextDir);

    if (myDrawer->DimensionAspect()->IsText3d())
    {
      // sensitive planar rectangle for text
      double aDx = mySelectionGeom.TextWidth * 0.5;
      double aDy = mySelectionGeom.TextHeight * 0.5;

      gp_Trsf aLabelPlane;
      aLabelPlane.SetTransformation(aTextAxes, gp::XOY());

      NCollection_Array1<gp_Pnt> aRectanglePoints(1, 4);
      aRectanglePoints.ChangeValue(1) = gp_Pnt(-aDx, -aDy, 0.0).Transformed(aLabelPlane);
      aRectanglePoints.ChangeValue(2) = gp_Pnt(-aDx, aDy, 0.0).Transformed(aLabelPlane);
      aRectanglePoints.ChangeValue(3) = gp_Pnt(aDx, aDy, 0.0).Transformed(aLabelPlane);
      aRectanglePoints.ChangeValue(4) = gp_Pnt(aDx, -aDy, 0.0).Transformed(aLabelPlane);

      NCollection_Array1<Poly_Triangle> aTriangles(1, 2);
      aTriangles.ChangeValue(1) = Poly_Triangle(1, 2, 3);
      aTriangles.ChangeValue(2) = Poly_Triangle(1, 3, 4);

      occ::handle<Poly_Triangulation> aRectanglePoly =
        new Poly_Triangulation(aRectanglePoints, aTriangles);

      aTextSensitive = new Select3D_SensitiveTriangulation(aSensitiveOwner,
                                                           aRectanglePoly,
                                                           TopLoc_Location(),
                                                           true);
    }
    else
    {
      gp_Circ aTextGeom(aTextAxes, mySelToleranceForText2d != 0.0 ? mySelToleranceForText2d : 1.0);
      aTextSensitive = new Select3D_SensitiveCircle(aSensitiveOwner, aTextGeom, true);
    }

    theSelection->Add(aTextSensitive);
  }

  // callback for flyout sensitive calculation
  if (aSelectionMode == PrsDim_DimensionSelectionMode_All)
  {
    ComputeFlyoutSelection(theSelection, aSensitiveOwner);
  }
}

//=================================================================================================

void PrsDim_Dimension::PointsForArrow(const gp_Pnt& thePeakPnt,
                                      const gp_Dir& theDirection,
                                      const gp_Dir& thePlane,
                                      const double  theArrowLength,
                                      const double  theArrowAngle,
                                      gp_Pnt&       theSidePnt1,
                                      gp_Pnt&       theSidePnt2)
{
  gp_Lin anArrowLin(thePeakPnt, theDirection.Reversed());
  gp_Pnt anArrowEnd = ElCLib::Value(theArrowLength, anArrowLin);
  gp_Lin anEdgeLin(anArrowEnd, theDirection.Crossed(thePlane));

  double anEdgeLength = std::tan(theArrowAngle) * theArrowLength;

  theSidePnt1 = ElCLib::Value(anEdgeLength, anEdgeLin);
  theSidePnt2 = ElCLib::Value(-anEdgeLength, anEdgeLin);
}

//=================================================================================================

gp_Pnt PrsDim_Dimension::GetTextPositionForLinear(const gp_Pnt& theFirstPoint,
                                                  const gp_Pnt& theSecondPoint,
                                                  const bool    theIsOneSide) const
{
  if (!IsValid())
  {
    return gp::Origin();
  }

  gp_Pnt aTextPosition(gp::Origin());

  occ::handle<Prs3d_DimensionAspect> aDimensionAspect = myDrawer->DimensionAspect();

  // Get label alignment and arrow orientation.
  int  aLabelPosition   = 0;
  bool isArrowsExternal = false;
  FitTextAlignmentForLinear(theFirstPoint,
                            theSecondPoint,
                            theIsOneSide,
                            aDimensionAspect->TextHorizontalPosition(),
                            aLabelPosition,
                            isArrowsExternal);

  // Compute dimension line points.
  gp_Dir aPlaneNormal = GetPlane().Axis().Direction();
  gp_Vec aTargetPointsVec(theFirstPoint, theSecondPoint);

  // Compute flyout direction vector
  gp_Dir aFlyoutVector = aPlaneNormal ^ gp_Dir(aTargetPointsVec);

  // create lines for layouts
  gp_Lin aLine1(theFirstPoint, aFlyoutVector);
  gp_Lin aLine2(theSecondPoint, aFlyoutVector);
  // Get flyout end points
  gp_Pnt aLineBegPoint =
    ElCLib::Value(ElCLib::Parameter(aLine1, theFirstPoint) + GetFlyout(), aLine1);
  gp_Pnt aLineEndPoint =
    ElCLib::Value(ElCLib::Parameter(aLine2, theSecondPoint) + GetFlyout(), aLine2);

  // Get text position.
  switch (aLabelPosition & LabelPosition_HMask)
  {
    case LabelPosition_Left: {
      gp_Dir aTargetPointsDir = gce_MakeDir(theFirstPoint, theSecondPoint);
      double anExtensionSize  = aDimensionAspect->ExtensionSize();

      double anOffset       = isArrowsExternal
                                ? anExtensionSize + aDimensionAspect->ArrowAspect()->Length()
                                : anExtensionSize;
      gp_Vec anExtensionVec = gp_Vec(aTargetPointsDir) * -anOffset;
      aTextPosition         = aLineEndPoint.Translated(anExtensionVec);
    }
    break;
    case LabelPosition_Right: {
      gp_Dir aTargetPointsDir = gce_MakeDir(theFirstPoint, theSecondPoint);
      double anExtensionSize  = aDimensionAspect->ExtensionSize();

      double anOffset       = isArrowsExternal
                                ? anExtensionSize + aDimensionAspect->ArrowAspect()->Length()
                                : anExtensionSize;
      gp_Vec anExtensionVec = gp_Vec(aTargetPointsDir) * anOffset;
      aTextPosition         = aLineBegPoint.Translated(anExtensionVec);
    }
    break;
    case LabelPosition_HCenter: {
      aTextPosition = (aLineBegPoint.XYZ() + aLineEndPoint.XYZ()) * 0.5;
    }
    break;
  }

  return aTextPosition;
}

//=================================================================================================

bool PrsDim_Dimension::AdjustParametersForLinear(
  const gp_Pnt&                          theTextPos,
  const gp_Pnt&                          theFirstPoint,
  const gp_Pnt&                          theSecondPoint,
  double&                                theExtensionSize,
  Prs3d_DimensionTextHorizontalPosition& theAlignment,
  double&                                theFlyout,
  gp_Pln&                                thePlane,
  bool&                                  theIsPlaneOld) const
{
  occ::handle<Prs3d_DimensionAspect> aDimensionAspect = myDrawer->DimensionAspect();
  double                             anArrowLength    = aDimensionAspect->ArrowAspect()->Length();

  gp_Dir aTargetPointsDir = gce_MakeDir(theFirstPoint, theSecondPoint);
  gp_Vec aTargetPointsVec(theFirstPoint, theSecondPoint);

  // Don't set new plane if the text position lies on the attachment points line.
  gp_Lin aTargetPointsLin(theFirstPoint, aTargetPointsDir);
  if (!aTargetPointsLin.Contains(theTextPos, Precision::Confusion()))
  {
    // Set new automatic plane.
    thePlane      = gce_MakePln(theTextPos, theFirstPoint, theSecondPoint);
    theIsPlaneOld = false;
  }

  // Compute flyout direction vector.
  gp_Dir aPlaneNormal    = GetPlane().Axis().Direction();
  gp_Dir aPositiveFlyout = aPlaneNormal ^ aTargetPointsDir;

  // Additional check of collinearity of the plane normal and attachment points vector.
  if (aPlaneNormal.IsParallel(aTargetPointsDir, Precision::Angular()))
  {
    return false;
  }

  // Set flyout.
  gp_Vec aFirstToTextVec(theFirstPoint, theTextPos);

  double aCos = aFirstToTextVec.Normalized() * gp_Vec(aTargetPointsDir);

  gp_Pnt aTextPosProj =
    theFirstPoint.Translated(gp_Vec(aTargetPointsDir) * aFirstToTextVec.Magnitude() * aCos);

  // Compute flyout value and direction.
  gp_Vec aFlyoutVector = gp_Vec(aTextPosProj, theTextPos);

  theFlyout = 0.0;
  if (aFlyoutVector.Magnitude() > Precision::Confusion())
  {
    theFlyout = gp_Dir(aFlyoutVector).IsOpposite(aPositiveFlyout, Precision::Angular())
                  ? -aFlyoutVector.Magnitude()
                  : aFlyoutVector.Magnitude();
  }

  // Compute attach points (through which main dimension line passes).
  gp_Pnt aFirstAttach  = theFirstPoint.Translated(aFlyoutVector);
  gp_Pnt aSecondAttach = theSecondPoint.Translated(aFlyoutVector);

  // Set horizontal text alignment.
  if (aCos < 0.0)
  {
    theAlignment = Prs3d_DTHP_Left;

    double aNewExtSize = theTextPos.Distance(aFirstAttach) - anArrowLength;
    theExtensionSize   = aNewExtSize < 0.0 ? 0.0 : aNewExtSize;
  }
  else if (aTextPosProj.Distance(theFirstPoint) > theFirstPoint.Distance(theSecondPoint))
  {
    theAlignment = Prs3d_DTHP_Right;

    double aNewExtSize = theTextPos.Distance(aSecondAttach) - anArrowLength;
    theExtensionSize   = aNewExtSize < 0.0 ? 0.0 : aNewExtSize;
  }
  else
  {
    theAlignment = Prs3d_DTHP_Center;
  }
  return true;
}

//=================================================================================================

void PrsDim_Dimension::FitTextAlignmentForLinear(
  const gp_Pnt&                                theFirstPoint,
  const gp_Pnt&                                theSecondPoint,
  const bool                                   theIsOneSide,
  const Prs3d_DimensionTextHorizontalPosition& theHorizontalTextPos,
  int&                                         theLabelPosition,
  bool&                                        theIsArrowsExternal) const
{
  theLabelPosition    = LabelPosition_None;
  theIsArrowsExternal = false;

  // Compute dimension line points
  gp_Ax1 aPlaneNormal        = GetPlane().Axis();
  gp_Dir aTargetPointsVector = gce_MakeDir(theFirstPoint, theSecondPoint);

  // compute flyout direction vector
  gp_Dir aFlyoutVector = aPlaneNormal.Direction() ^ aTargetPointsVector;

  // create lines for layouts
  gp_Lin aLine1(theFirstPoint, aFlyoutVector);
  gp_Lin aLine2(theSecondPoint, aFlyoutVector);

  // Get flyout end points
  gp_Pnt aLineBegPoint =
    ElCLib::Value(ElCLib::Parameter(aLine1, theFirstPoint) + GetFlyout(), aLine1);
  gp_Pnt aLineEndPoint =
    ElCLib::Value(ElCLib::Parameter(aLine2, theSecondPoint) + GetFlyout(), aLine2);

  occ::handle<Prs3d_DimensionAspect> aDimensionAspect = myDrawer->DimensionAspect();

  // For extensions we need to know arrow size, text size and extension size: get it from aspect
  double anArrowLength = aDimensionAspect->ArrowAspect()->Length();

  // prepare label string and compute its geometrical width
  double                     aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString(aLabelWidth);

  // Add margins to cut dimension lines for 3d text
  if (aDimensionAspect->IsText3d())
  {
    aLabelWidth += aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN * 2.0;
  }

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
      // Add margin to ensure a small tail between text and arrow
      double anArrowMargin = aDimensionAspect->IsText3d()
                               ? aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN
                               : 0.0;

      double aDimensionWidth = aLineBegPoint.Distance(aLineEndPoint);
      double anArrowsWidth =
        theIsOneSide ? anArrowLength + anArrowMargin : (anArrowLength + anArrowMargin) * 2.0;

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
      double aDimensionWidth = aLineBegPoint.Distance(aLineEndPoint);
      double anArrowsWidth   = theIsOneSide ? anArrowLength : 2.0 * anArrowLength;
      double aContentWidth   = theIsArrowsExternal ? aLabelWidth : aLabelWidth + anArrowsWidth;

      theLabelPosition |=
        aDimensionWidth < aContentWidth ? LabelPosition_Left : LabelPosition_HCenter;
      break;
    }
  }

  // Handle vertical text placement options
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

//=================================================================================================

void PrsDim_Dimension::UnsetFixedTextPosition()
{
  myIsTextPositionFixed = false;
  myFixedTextPosition   = gp::Origin();
}
