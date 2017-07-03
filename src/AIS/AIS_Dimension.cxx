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

#include <AIS_Dimension.hxx>

#include <AIS.hxx>
#include <AIS_DimensionOwner.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <ElCLib.hxx>
#include <Font_BRepFont.hxx>
#include <Font_BRepTextBuilder.hxx>
#include <GC_MakeCircle.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gce_MakeDir.hxx>
#include <gce_MakeLin.hxx>
#include <gce_MakePln.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_Group.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Select3D_SensitiveTriangulation.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_CString.hxx>
#include <Standard_ProgramError.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <Units.hxx>
#include <Units_UnitsDictionary.hxx>
#include <UnitsAPI.hxx>
#include <UnitsAPI_SystemUnits.hxx>


IMPLEMENT_STANDARD_RTTIEXT(AIS_Dimension,AIS_InteractiveObject)

namespace
{
  // default text strings
  static const TCollection_ExtendedString THE_EMPTY_LABEL;
  static const TCollection_AsciiString    THE_UNDEFINED_UNITS;

  // default text margin and resolution
  static const Standard_Real THE_3D_TEXT_MARGIN    = 0.1;
  static const unsigned int  THE_2D_TEXT_RESOLUTION = 72;

  // default selection priorities
  static const Standard_Integer THE_NEUTRAL_SEL_PRIORITY = 5;
  static const Standard_Integer THE_LOCAL_SEL_PRIORITY   = 6;
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_Dimension::AIS_Dimension (const AIS_KindOfDimension theType)
: AIS_InteractiveObject  (),
  mySelToleranceForText2d(0.0),
  myValueType            (ValueType_Computed),
  myCustomValue          (0.0),
  myCustomStringValue    (),
  myIsTextPositionFixed  (Standard_False), 
  mySpecialSymbol        (' '),
  myDisplaySpecialSymbol (AIS_DSS_No),
  myGeometryType         (GeometryType_UndefShapes),
  myIsPlaneCustom        (Standard_False),
  myFlyout               (0.0),
  myIsGeometryValid      (Standard_False),
  myKindOfDimension      (theType)
{
}

//=======================================================================
//function : SetCustomValue
//purpose  : 
//=======================================================================
void AIS_Dimension::SetCustomValue (const Standard_Real theValue)
{
  if (myValueType == ValueType_CustomReal && myCustomValue == theValue)
  {
    return;
  }

  myValueType = ValueType_CustomReal;
  myCustomValue = theValue;

  SetToUpdate();
}

//=======================================================================
//function : SetCustomValue
//purpose  : 
//=======================================================================
void AIS_Dimension::SetCustomValue (const TCollection_ExtendedString& theValue)
{
  if (myValueType == ValueType_CustomText && myCustomStringValue == theValue)
  {
    return;
  }

  myValueType = ValueType_CustomText;
  myCustomStringValue = theValue;

  SetToUpdate();
}

//=======================================================================
//function : GetCustomValue
//purpose  : 
//=======================================================================
const TCollection_ExtendedString& AIS_Dimension::GetCustomValue () const
{
  return myCustomStringValue;
}

//=======================================================================
//function : GetPlane
//purpose  : 
//=======================================================================
const gp_Pln& AIS_Dimension::GetPlane() const
{
  return myPlane;
}

//=======================================================================
//function : SetUserPlane
//purpose  : 
//=======================================================================
void AIS_Dimension::SetCustomPlane (const gp_Pln& thePlane)
{
  myPlane = thePlane;
  myIsPlaneCustom = Standard_True;

  // Disable fixed (custom) text position
  UnsetFixedTextPosition();

  // Check validity if geometry has been set already.
  if (IsValid())
  {
    SetToUpdate();
  }
}

//=======================================================================
//function : SetDimensionAspect
//purpose  :
//=======================================================================
void AIS_Dimension::SetDimensionAspect (const Handle(Prs3d_DimensionAspect)& theDimensionAspect)
{
  myDrawer->SetDimensionAspect (theDimensionAspect);

  SetToUpdate();
}

//=======================================================================
//function : SetDisplaySpecialSymbol
//purpose  :
//=======================================================================
void AIS_Dimension::SetDisplaySpecialSymbol (const AIS_DisplaySpecialSymbol theDisplaySpecSymbol)
{
  if (myDisplaySpecialSymbol == theDisplaySpecSymbol)
  {
    return;
  }

  myDisplaySpecialSymbol = theDisplaySpecSymbol;

  SetToUpdate();
}

//=======================================================================
//function : SetSpecialSymbol
//purpose  :
//=======================================================================
void AIS_Dimension::SetSpecialSymbol (const Standard_ExtCharacter theSpecialSymbol)
{
  if (mySpecialSymbol == theSpecialSymbol)
  {
    return;
  }

  mySpecialSymbol = theSpecialSymbol;

  SetToUpdate();
}

//=======================================================================
//function : SetSelToleranceForText2d
//purpose  :
//=======================================================================
void AIS_Dimension::SetSelToleranceForText2d (const Standard_Real theTol)
{
  if (mySelToleranceForText2d == theTol)
  {
    return;
  }

  mySelToleranceForText2d = theTol;

  SetToUpdate();
}

//=======================================================================
//function : SetFlyout
//purpose  :
//=======================================================================
void AIS_Dimension::SetFlyout (const Standard_Real theFlyout)
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

//=======================================================================
//function : GetDisplayUnits
//purpose  :
//=======================================================================
const TCollection_AsciiString& AIS_Dimension::GetDisplayUnits() const
{
  return THE_UNDEFINED_UNITS;
}

//=======================================================================
//function : GetModelUnits
//purpose  :
//=======================================================================
const TCollection_AsciiString& AIS_Dimension::GetModelUnits() const
{
  return THE_UNDEFINED_UNITS;
}

//=======================================================================
//function : ValueToDisplayUnits
//purpose  :
//=======================================================================
Standard_Real AIS_Dimension::ValueToDisplayUnits() const
{
  return UnitsAPI::AnyToAny (GetValue(),
                             GetModelUnits().ToCString(),
                             GetDisplayUnits().ToCString());
}

//=======================================================================
//function : GetValueString
//purpose  : 
//=======================================================================
TCollection_ExtendedString AIS_Dimension::GetValueString (Standard_Real& theWidth) const
{
  TCollection_ExtendedString aValueStr;
  if (myValueType == ValueType_CustomText)
  {
    aValueStr = myCustomStringValue;
  }
  else
  {
    // format value string using "sprintf"
    TCollection_AsciiString aFormatStr = myDrawer->DimensionAspect()->ValueStringFormat();

    char aFmtBuffer[256];
    sprintf (aFmtBuffer, aFormatStr.ToCString(), ValueToDisplayUnits());
    aValueStr = TCollection_ExtendedString (aFmtBuffer);
  }

  // add units to values string
  if (myDrawer->DimensionAspect()->IsUnitsDisplayed())
  {
    aValueStr += " ";
    aValueStr += TCollection_ExtendedString (GetDisplayUnits());
  }

  switch (myDisplaySpecialSymbol)
  {
    case AIS_DSS_Before : aValueStr.Insert (1, mySpecialSymbol); break;
    case AIS_DSS_After  : aValueStr.Insert (aValueStr.Length() + 1, mySpecialSymbol); break;
    case AIS_DSS_No     : break;
  }

  // Get text style parameters
  Handle(Prs3d_TextAspect) aTextAspect = myDrawer->DimensionAspect()->TextAspect();
  NCollection_Utf8String anUTFString (aValueStr.ToExtString());

  theWidth = 0.0;

  if (myDrawer->DimensionAspect()->IsText3d())
  {
    // text width produced by BRepFont
    Font_BRepFont aFont (aTextAspect->Aspect()->Font().ToCString(),
                         aTextAspect->Aspect()->GetTextFontAspect(),
                         aTextAspect->Height());

    for (NCollection_Utf8Iter anIter = anUTFString.Iterator(); *anIter != 0; )
    {
      Standard_Utf32Char aCurrChar = *anIter;
      Standard_Utf32Char aNextChar = *(++anIter);
      theWidth += aFont.AdvanceX (aCurrChar, aNextChar);
    }
  }
  else
  {
    // Text width for 1:1 scale 2D case
    Handle(Font_FTFont) aFont = new Font_FTFont();
    aFont->Init (aTextAspect->Aspect()->Font().ToCString(),
                 aTextAspect->Aspect()->GetTextFontAspect(),
                 (const unsigned int)aTextAspect->Height(),
                 THE_2D_TEXT_RESOLUTION);

    for (NCollection_Utf8Iter anIter = anUTFString.Iterator(); *anIter != 0; )
    {
      Standard_Utf32Char aCurrChar = *anIter;
      Standard_Utf32Char aNextChar = *(++anIter);
      theWidth += (Standard_Real) aFont->AdvanceX (aCurrChar, aNextChar);
    }
  }

  return aValueStr;
}

//=======================================================================
//function : DrawArrow
//purpose  : 
//=======================================================================
void AIS_Dimension::DrawArrow (const Handle(Prs3d_Presentation)& thePresentation,
                               const gp_Pnt& theLocation,
                               const gp_Dir& theDirection)
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::NewGroup (thePresentation);

  Standard_Real aLength = myDrawer->DimensionAspect()->ArrowAspect()->Length();
  Standard_Real anAngle = myDrawer->DimensionAspect()->ArrowAspect()->Angle();

  if (myDrawer->DimensionAspect()->IsArrows3d())
  {
    Prs3d_Arrow::Draw (aGroup,
                       theLocation,
                       theDirection,
                       anAngle,
                       aLength);
    aGroup->SetGroupPrimitivesAspect (myDrawer->DimensionAspect()->ArrowAspect()->Aspect());
  }
  else
  {
    gp_Pnt aLeftPoint (gp::Origin());
    gp_Pnt aRightPoint (gp::Origin());
    const gp_Dir& aPlane = GetPlane().Axis().Direction();

    PointsForArrow (theLocation, theDirection, aPlane, aLength, anAngle, aLeftPoint, aRightPoint);

    Handle(Graphic3d_ArrayOfTriangles) anArrow = new Graphic3d_ArrayOfTriangles(3);

    anArrow->AddVertex (aLeftPoint);
    anArrow->AddVertex (theLocation);
    anArrow->AddVertex (aRightPoint);

    // Set aspect for arrow triangles
    Graphic3d_MaterialAspect aShadeMat (Graphic3d_NOM_DEFAULT);
    aShadeMat.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
    aShadeMat.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
    aShadeMat.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);

    Handle(Prs3d_ShadingAspect) aShadingStyle = new Prs3d_ShadingAspect();
    aShadingStyle->SetColor (myDrawer->DimensionAspect()->ArrowAspect()->Aspect()->Color());
    aShadingStyle->SetMaterial (aShadeMat);

    aGroup->SetPrimitivesAspect (aShadingStyle->Aspect());
    aGroup->AddPrimitiveArray (anArrow);
  }

  SelectionGeometry::Arrow& aSensitiveArrow = mySelectionGeom.NewArrow();
  aSensitiveArrow.Position  = theLocation;
  aSensitiveArrow.Direction = theDirection;
}

//=======================================================================
//function : drawText
//purpose  :
//=======================================================================
void AIS_Dimension::drawText (const Handle(Prs3d_Presentation)& thePresentation,
                              const gp_Pnt& theTextPos,
                              const gp_Dir& theTextDir,
                              const TCollection_ExtendedString& theText,
                              const Standard_Integer theLabelPosition)
{
  if (myDrawer->DimensionAspect()->IsText3d())
  {
    // getting font parameters
    Handle(Prs3d_TextAspect) aTextAspect = myDrawer->DimensionAspect()->TextAspect();
    Quantity_Color  aColor      = aTextAspect->Aspect()->Color();
    Font_FontAspect aFontAspect = aTextAspect->Aspect()->GetTextFontAspect();
    Standard_Real   aFontHeight = aTextAspect->Height();

    // creating TopoDS_Shape for text
    Font_BRepFont aFont (aTextAspect->Aspect()->Font().ToCString(),
                         aFontAspect, aFontHeight);
    NCollection_Utf8String anUTFString (theText.ToExtString());

    Font_BRepTextBuilder aBuilder;
    TopoDS_Shape aTextShape = aBuilder.Perform (aFont, anUTFString);

    // compute text width with kerning
    Standard_Real aTextWidth  = 0.0;
    Standard_Real aTextHeight = aFont.Ascender() + aFont.Descender();

    for (NCollection_Utf8Iter anIter = anUTFString.Iterator(); *anIter != 0; )
    {
      Standard_Utf32Char aCurrChar = *anIter;
      Standard_Utf32Char aNextChar = *(++anIter);
      aTextWidth += aFont.AdvanceX (aCurrChar, aNextChar);
    }

    // formating text position in XOY plane
    Standard_Integer aHLabelPos = theLabelPosition & LabelPosition_HMask;
    Standard_Integer aVLabelPos = theLabelPosition & LabelPosition_VMask;

    gp_Dir aTextDir (aHLabelPos == LabelPosition_Left ? -theTextDir : theTextDir);

    // compute label offsets
    Standard_Real aMarginSize    = aFontHeight * THE_3D_TEXT_MARGIN;
    Standard_Real aCenterHOffset = 0.0;
    Standard_Real aCenterVOffset = 0.0;
    switch (aHLabelPos)
    {
      case LabelPosition_HCenter : aCenterHOffset =  0.0; break;
      case LabelPosition_Right   : aCenterHOffset =  aTextWidth / 2.0 + aMarginSize; break;
      case LabelPosition_Left    : aCenterHOffset = -aTextWidth / 2.0 - aMarginSize; break;
    }
    switch (aVLabelPos)
    {
      case LabelPosition_VCenter : aCenterVOffset =  0.0; break;
      case LabelPosition_Above   : aCenterVOffset =  aTextHeight / 2.0 + aMarginSize; break;
      case LabelPosition_Below   : aCenterVOffset = -aTextHeight / 2.0 - aMarginSize; break;
    }

    // compute shape offset transformation
    Standard_Real aShapeHOffset = aCenterHOffset - aTextWidth / 2.0;
    Standard_Real aShapeVOffset = aCenterVOffset - aTextHeight / 2.0;

    // center shape in its bounding box (suppress border spacing added by FT_Font)
    Bnd_Box aShapeBnd;
    BRepBndLib::AddClose (aTextShape, aShapeBnd);

    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    aShapeBnd.Get (aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

    Standard_Real aXalign = aTextWidth  * 0.5 - (aXmax + aXmin) * 0.5;
    Standard_Real aYalign = aTextHeight * 0.5 - (aYmax + aYmin) * 0.5;
    aShapeHOffset += aXalign;
    aShapeVOffset += aYalign;

    gp_Trsf anOffsetTrsf;
    anOffsetTrsf.SetTranslation (gp::Origin(), gp_Pnt (aShapeHOffset, aShapeVOffset, 0.0));
    aTextShape.Move (anOffsetTrsf);

    // transform text to myWorkingPlane coordinate system
    gp_Ax3 aTextCoordSystem (theTextPos, GetPlane().Axis().Direction(), aTextDir);
    gp_Trsf aTextPlaneTrsf;
    aTextPlaneTrsf.SetTransformation (aTextCoordSystem, gp_Ax3 (gp::XOY()));
    aTextShape.Move (aTextPlaneTrsf);

    // set text flipping anchors
    gp_Trsf aCenterOffsetTrsf;
    gp_Pnt aCenterOffset (aCenterHOffset, aCenterVOffset, 0.0);
    aCenterOffsetTrsf.SetTranslation (gp::Origin(), aCenterOffset);

    gp_Pnt aCenterOfLabel (gp::Origin());
    aCenterOfLabel.Transform (aCenterOffsetTrsf);
    aCenterOfLabel.Transform (aTextPlaneTrsf);

    gp_Ax2 aFlippingAxes (aCenterOfLabel, GetPlane().Axis().Direction(), aTextDir);
    Prs3d_Root::CurrentGroup (thePresentation)->SetFlippingOptions (Standard_True, aFlippingAxes);

    // draw text
    if (myDrawer->DimensionAspect()->IsTextShaded())
    {
      // Setting text shading and color parameters
      if (!myDrawer->HasOwnShadingAspect())
      {
        myDrawer->SetShadingAspect (new Prs3d_ShadingAspect());
      }

      Graphic3d_MaterialAspect aShadeMat (Graphic3d_NOM_DEFAULT);
      aShadeMat.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
      aShadeMat.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
      aShadeMat.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
      myDrawer->ShadingAspect()->Aspect()->SetInteriorColor (aColor);
      myDrawer->ShadingAspect()->Aspect()->SetBackInteriorColor (aColor);
      myDrawer->ShadingAspect()->SetMaterial (aShadeMat);

      // drawing text
      StdPrs_ShadedShape::Add (thePresentation, aTextShape, myDrawer);
    }
    else
    {
      // Setting color for text
      if (!myDrawer->HasOwnFreeBoundaryAspect())
      {
        myDrawer->SetFreeBoundaryAspect (new Prs3d_LineAspect (aColor, Aspect_TOL_SOLID, 1.0));
      }

      myDrawer->FreeBoundaryAspect()->Aspect()->SetColor (aColor);

      // drawing text
      StdPrs_WFShape::Add (thePresentation, aTextShape, myDrawer);
    }
    Prs3d_Root::CurrentGroup (thePresentation)->SetFlippingOptions (Standard_False, gp_Ax2());

    mySelectionGeom.TextPos    = aCenterOfLabel;
    mySelectionGeom.TextDir    = aTextDir;
    mySelectionGeom.TextWidth  = aTextWidth + aMarginSize * 2.0;
    mySelectionGeom.TextHeight = aTextHeight;

    return;
  }

  // generate primitives for 2D text
  myDrawer->DimensionAspect()->TextAspect()->Aspect()->SetDisplayType (Aspect_TODT_DIMENSION);

  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (thePresentation),
                    myDrawer->DimensionAspect()->TextAspect(),
                    theText,
                    theTextPos);

  mySelectionGeom.TextPos    = theTextPos;
  mySelectionGeom.TextDir    = theTextDir;
  mySelectionGeom.TextWidth  = 0.0;
  mySelectionGeom.TextHeight = 0.0;
}

//=======================================================================
//function : DrawExtension
//purpose  : 
//=======================================================================
void AIS_Dimension::DrawExtension (const Handle(Prs3d_Presentation)& thePresentation,
                                   const Standard_Real theExtensionSize,
                                   const gp_Pnt& theExtensionStart,
                                   const gp_Dir& theExtensionDir,
                                   const TCollection_ExtendedString& theLabelString,
                                   const Standard_Real theLabelWidth,
                                   const Standard_Integer theMode,
                                   const Standard_Integer theLabelPosition)
{
  // reference line for extension starting at its connection point
  gp_Lin anExtensionLine (theExtensionStart, theExtensionDir);

  Standard_Boolean hasLabel = theLabelString.Length() > 0;
  if (hasLabel && (theMode == ComputeMode_All || theMode == ComputeMode_Text))
  {
    // compute text primitives; get its model width
    gp_Pnt aTextPos = ElCLib::Value (theExtensionSize, anExtensionLine);
    gp_Dir aTextDir = theExtensionDir;

    drawText (thePresentation,
              aTextPos,
              aTextDir,
              theLabelString,
              theLabelPosition);
  }

  if (theMode != ComputeMode_All && theMode != ComputeMode_Line)
  {
    return;
  }

  Standard_Boolean isShortLine =  !myDrawer->DimensionAspect()->IsText3d()
                               || theLabelPosition & LabelPosition_VCenter;

  // compute graphical primitives and sensitives for extension line
  gp_Pnt anExtStart = theExtensionStart;
  gp_Pnt anExtEnd   = !hasLabel || isShortLine
    ? ElCLib::Value (theExtensionSize, anExtensionLine)
    : ElCLib::Value (theExtensionSize + theLabelWidth, anExtensionLine);

  // add graphical primitives
  Handle(Graphic3d_ArrayOfSegments) anExtPrimitive = new Graphic3d_ArrayOfSegments (2);
  anExtPrimitive->AddVertex (anExtStart);
  anExtPrimitive->AddVertex (anExtEnd);

  // add selection primitives
  SelectionGeometry::Curve& aSensitiveCurve = mySelectionGeom.NewCurve();
  aSensitiveCurve.Append (anExtStart);
  aSensitiveCurve.Append (anExtEnd);

  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == ComputeMode_All)
  {
    Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_True);
  }
  Handle(Graphic3d_AspectLine3d) aDimensionLineStyle = myDrawer->DimensionAspect()->LineAspect()->Aspect();
  Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionLineStyle);
  Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (anExtPrimitive);
  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == ComputeMode_All)
  {
    Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_False);
  }
}

//=======================================================================
//function : DrawLinearDimension
//purpose  : 
//=======================================================================
void AIS_Dimension::DrawLinearDimension (const Handle(Prs3d_Presentation)& thePresentation,
                                         const Standard_Integer theMode,
                                         const gp_Pnt& theFirstPoint,
                                         const gp_Pnt& theSecondPoint,
                                         const Standard_Boolean theIsOneSide)
{
  // do not build any dimension for equal points
  if (theFirstPoint.IsEqual (theSecondPoint, Precision::Confusion()))
  {
    throw Standard_ProgramError("Can not build presentation for equal points.");
  }

  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();

  // For extensions we need to know arrow size, text size and extension size: get it from aspect
  Standard_Real anArrowLength   = aDimensionAspect->ArrowAspect()->Length();
  Standard_Real anExtensionSize = aDimensionAspect->ExtensionSize();
  // prepare label string and compute its geometrical width
  Standard_Real aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString (aLabelWidth);

  // add margins to cut dimension lines for 3d text
  if (aDimensionAspect->IsText3d())
  {
    aLabelWidth += aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN * 2.0;
  }

  // handle user-defined and automatic arrow placement
  Standard_Boolean isArrowsExternal = Standard_False;
  Standard_Integer aLabelPosition = LabelPosition_None;

  Prs3d_DimensionTextHorizontalPosition aHorisontalTextPos = aDimensionAspect->TextHorizontalPosition();
  if (IsTextPositionCustom())
  {
    if (!AdjustParametersForLinear (myFixedTextPosition, theFirstPoint, theSecondPoint,
                                    anExtensionSize, aHorisontalTextPos, myFlyout, myPlane, myIsPlaneCustom))
    {
      throw Standard_ProgramError("Can not adjust plane to the custom label position.");
    }
  }

  FitTextAlignmentForLinear (theFirstPoint, theSecondPoint, theIsOneSide, aHorisontalTextPos,
                             aLabelPosition, isArrowsExternal);

  // compute dimension line points
  gp_Pnt aLineBegPoint, aLineEndPoint;
  ComputeFlyoutLinePoints (theFirstPoint, theSecondPoint, aLineBegPoint, aLineEndPoint);
  gp_Lin aDimensionLine = gce_MakeLin (aLineBegPoint, aLineEndPoint);

  // compute arrows positions and directions
  gp_Dir aFirstArrowDir       = aDimensionLine.Direction().Reversed();
  gp_Dir aSecondArrowDir      = aDimensionLine.Direction();
  gp_Dir aFirstExtensionDir   = aDimensionLine.Direction().Reversed();
  gp_Dir aSecondExtensionDir  = aDimensionLine.Direction();

  gp_Pnt aFirstArrowBegin  (0.0, 0.0, 0.0);
  gp_Pnt aFirstArrowEnd    (0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowBegin (0.0, 0.0, 0.0);
  gp_Pnt aSecondArrowEnd   (0.0, 0.0, 0.0);

  if (isArrowsExternal)
  {
    aFirstArrowDir.Reverse();
    aSecondArrowDir.Reverse();
  }

  aFirstArrowBegin  = aLineBegPoint;
  aSecondArrowBegin = aLineEndPoint;
  aFirstArrowEnd    = aLineBegPoint.Translated (-gp_Vec (aFirstArrowDir).Scaled (anArrowLength));
  aSecondArrowEnd   = aLineEndPoint.Translated (-gp_Vec (aSecondArrowDir).Scaled (anArrowLength));

  gp_Pnt aCenterLineBegin = isArrowsExternal 
    ? aLineBegPoint : aFirstArrowEnd;

  gp_Pnt aCenterLineEnd = isArrowsExternal || theIsOneSide
    ? aLineEndPoint : aSecondArrowEnd;


  switch (aLabelPosition & LabelPosition_HMask)
  {
    // ------------------------------------------------------------------------ //
    //                                CENTER                                    //
    // -------------------------------------------------------------------------//
    case LabelPosition_HCenter:
    {
      // add label on dimension or extension line to presentation
      Prs3d_Root::NewGroup (thePresentation);

      gp_Pnt aTextPos = IsTextPositionCustom() ? myFixedTextPosition
                                              : (aCenterLineBegin.XYZ() + aCenterLineEnd.XYZ()) * 0.5;
      gp_Dir aTextDir = aDimensionLine.Direction();

      // add text primitives
      if (theMode == ComputeMode_All || theMode == ComputeMode_Text)
      {
        drawText (thePresentation,
                  aTextPos,
                  aTextDir,
                  aLabelString,
                  aLabelPosition);
      }

      // add dimension line primitives
      if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
      {
        Standard_Boolean isLineBreak = aDimensionAspect->TextVerticalPosition() == Prs3d_DTVP_Center
                                    && aDimensionAspect->IsText3d();

        Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments (isLineBreak ? 4 : 2);

        // compute continuous or sectioned main line segments
        if (isLineBreak)
        {
          Standard_Real aPTextPosition = ElCLib::Parameter (aDimensionLine, aTextPos);
          gp_Pnt aSection1Beg = aCenterLineBegin;
          gp_Pnt aSection1End = ElCLib::Value (aPTextPosition - (aLabelWidth * 0.5), aDimensionLine);
          gp_Pnt aSection2Beg = ElCLib::Value (aPTextPosition + (aLabelWidth * 0.5), aDimensionLine);
          gp_Pnt aSection2End = aCenterLineEnd;

          aPrimSegments->AddVertex (aSection1Beg);
          aPrimSegments->AddVertex (aSection1End);
          aPrimSegments->AddVertex (aSection2Beg);
          aPrimSegments->AddVertex (aSection2End);

          SelectionGeometry::Curve& aLeftSensitiveCurve  = mySelectionGeom.NewCurve();
          SelectionGeometry::Curve& aRightSensitiveCurve = mySelectionGeom.NewCurve();
          aLeftSensitiveCurve.Append (aSection1Beg);
          aLeftSensitiveCurve.Append (aSection1End);
          aRightSensitiveCurve.Append (aSection2Beg);
          aRightSensitiveCurve.Append (aSection2End);
        }
        else
        {
          aPrimSegments->AddVertex (aCenterLineBegin);
          aPrimSegments->AddVertex (aCenterLineEnd);

          SelectionGeometry::Curve& aSensitiveCurve = mySelectionGeom.NewCurve();
          aSensitiveCurve.Append (aCenterLineBegin);
          aSensitiveCurve.Append (aCenterLineEnd);
        }

        // set text label justification
        Graphic3d_VerticalTextAlignment aTextJustificaton = Graphic3d_VTA_BOTTOM;
        switch (aLabelPosition & LabelPosition_VMask)
        {
          case LabelPosition_Above   :
          case LabelPosition_VCenter : aTextJustificaton = Graphic3d_VTA_BOTTOM; break;
          case LabelPosition_Below   : aTextJustificaton = Graphic3d_VTA_TOP;    break;
        }
        aDimensionAspect->TextAspect()->SetVerticalJustification (aTextJustificaton);

        // main dimension line, short extension
        if (!aDimensionAspect->IsText3d() && theMode == ComputeMode_All)
        {
          Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_True);
        }
        Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());
        Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
        if (!aDimensionAspect->IsText3d() && theMode == ComputeMode_All)
        {
          Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_False);
        }

        // add arrows to presentation
        Prs3d_Root::NewGroup (thePresentation);

        DrawArrow (thePresentation, aFirstArrowBegin, aFirstArrowDir);
        if (!theIsOneSide)
        {
          DrawArrow (thePresentation, aSecondArrowBegin, aSecondArrowDir);
        }

        if (!isArrowsExternal)
        {
          break;
        }

        // add arrow extension lines to presentation
        Prs3d_Root::NewGroup (thePresentation);

        DrawExtension (thePresentation, aDimensionAspect->ArrowTailSize(),
                       aFirstArrowEnd, aFirstExtensionDir,
                       THE_EMPTY_LABEL, 0.0, theMode, LabelPosition_None);
        if (!theIsOneSide)
        {
          DrawExtension (thePresentation, aDimensionAspect->ArrowTailSize(),
                         aSecondArrowEnd, aSecondExtensionDir,
                         THE_EMPTY_LABEL, 0.0, theMode, LabelPosition_None);
        }
      }

      break;
    }
    // ------------------------------------------------------------------------ //
    //                                LEFT                                      //
    // -------------------------------------------------------------------------//

    case LabelPosition_Left:
    {
      // add label on dimension or extension line to presentation
      Prs3d_Root::NewGroup (thePresentation);

      // Left extension with the text
      DrawExtension (thePresentation, anExtensionSize,
                     isArrowsExternal
                       ? aFirstArrowEnd
                       : aFirstArrowBegin,
                     aFirstExtensionDir,
                     aLabelString,
                     aLabelWidth,
                     theMode,
                     aLabelPosition);

      // add dimension line primitives
      if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
      {
        // add central dimension line
        Prs3d_Root::NewGroup (thePresentation);

        // add graphical primitives
        Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments (2);
        aPrimSegments->AddVertex (aCenterLineBegin);
        aPrimSegments->AddVertex (aCenterLineEnd);

        Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());
        Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);

        // add selection primitives
        SelectionGeometry::Curve& aSensitiveCurve = mySelectionGeom.NewCurve();
        aSensitiveCurve.Append (aCenterLineBegin);
        aSensitiveCurve.Append (aCenterLineEnd);

        // add arrows to presentation
        Prs3d_Root::NewGroup (thePresentation);

        DrawArrow (thePresentation, aFirstArrowBegin, aFirstArrowDir);
        if (!theIsOneSide)
        {
          DrawArrow (thePresentation, aSecondArrowBegin, aSecondArrowDir);
        }

        if (!isArrowsExternal || theIsOneSide)
        {
          break;
        }

        // add extension lines for external arrows
        Prs3d_Root::NewGroup (thePresentation);

        DrawExtension (thePresentation, aDimensionAspect->ArrowTailSize(),
                       aSecondArrowEnd, aSecondExtensionDir,
                       THE_EMPTY_LABEL, 0.0, theMode, LabelPosition_None);
      }

      break;
    }
    // ------------------------------------------------------------------------ //
    //                                RIGHT                                     //
    // -------------------------------------------------------------------------//

    case LabelPosition_Right:
    {
      // add label on dimension or extension line to presentation
      Prs3d_Root::NewGroup (thePresentation);

      // Right extension with text
      DrawExtension (thePresentation, anExtensionSize,
                     isArrowsExternal
                       ? aSecondArrowEnd
                       : aSecondArrowBegin,
                     aSecondExtensionDir,
                     aLabelString, aLabelWidth,
                     theMode,
                     aLabelPosition);

      if (theMode == ComputeMode_All || theMode == ComputeMode_Line)
      {
        // add central dimension line
        Prs3d_Root::NewGroup (thePresentation);

        // add graphical primitives
        Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments (2);
        aPrimSegments->AddVertex (aCenterLineBegin);
        aPrimSegments->AddVertex (aCenterLineEnd);
        Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());
        Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);

        // add selection primitives
        SelectionGeometry::Curve& aSensitiveCurve = mySelectionGeom.NewCurve();
        aSensitiveCurve.Append (aCenterLineBegin);
        aSensitiveCurve.Append (aCenterLineEnd);

        // add arrows to presentation
        Prs3d_Root::NewGroup (thePresentation);

        DrawArrow (thePresentation, aSecondArrowBegin, aSecondArrowDir);
        if (!theIsOneSide)
        {
          DrawArrow (thePresentation, aFirstArrowBegin, aFirstArrowDir);
        }

        if (!isArrowsExternal || theIsOneSide)
        {
          break;
        }

        // add extension lines for external arrows
        Prs3d_Root::NewGroup (thePresentation);

        DrawExtension (thePresentation, aDimensionAspect->ArrowTailSize(),
                       aFirstArrowEnd, aFirstExtensionDir,
                       THE_EMPTY_LABEL, 0.0, theMode, LabelPosition_None);
      }

      break;
    }
  }

  // add flyout lines to presentation
  if (theMode == ComputeMode_All)
  {
    Prs3d_Root::NewGroup (thePresentation);

    Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments(4);
    aPrimSegments->AddVertex (theFirstPoint);
    aPrimSegments->AddVertex (aLineBegPoint);

    aPrimSegments->AddVertex (theSecondPoint);
    aPrimSegments->AddVertex (aLineEndPoint);

    Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
  }

  mySelectionGeom.IsComputed = Standard_True;
}

//=======================================================================
//function : ComputeFlyoutLinePoints
//purpose  :
//=======================================================================
void AIS_Dimension::ComputeFlyoutLinePoints (const gp_Pnt& theFirstPoint, const gp_Pnt& theSecondPoint,
                                             gp_Pnt& theLineBegPoint, gp_Pnt& theLineEndPoint)
{
  // compute dimension line points
  gp_Ax1 aPlaneNormal = GetPlane().Axis();
  // compute flyout direction vector
  gp_Dir aTargetPointsVector = gce_MakeDir (theFirstPoint, theSecondPoint);
  gp_Dir aFlyoutVector = aPlaneNormal.Direction() ^ aTargetPointsVector;
  // create lines for layouts
  gp_Lin aLine1 (theFirstPoint, aFlyoutVector);
  gp_Lin aLine2 (theSecondPoint, aFlyoutVector);

  // Get flyout end points
  theLineBegPoint = ElCLib::Value (ElCLib::Parameter (aLine1, theFirstPoint)  + GetFlyout(), aLine1);
  theLineEndPoint = ElCLib::Value (ElCLib::Parameter (aLine2, theSecondPoint) + GetFlyout(), aLine2);
}

//=======================================================================
//function : ComputeLinearFlyouts
//purpose  :
//=======================================================================
void AIS_Dimension::ComputeLinearFlyouts (const Handle(SelectMgr_Selection)& theSelection,
                                          const Handle(SelectMgr_EntityOwner)& theOwner,
                                          const gp_Pnt& theFirstPoint,
                                          const gp_Pnt& theSecondPoint)
{
  // count flyout direction
  gp_Ax1 aPlaneNormal = GetPlane().Axis();
  gp_Dir aTargetPointsVector = gce_MakeDir (theFirstPoint, theSecondPoint);

  // count a flyout direction vector.
  gp_Dir aFlyoutVector = aPlaneNormal.Direction() ^ aTargetPointsVector;

  // create lines for layouts
  gp_Lin aLine1 (theFirstPoint,  aFlyoutVector);
  gp_Lin aLine2 (theSecondPoint, aFlyoutVector);

  // get flyout end points
  gp_Pnt aFlyoutEnd1 = ElCLib::Value (ElCLib::Parameter (aLine1, theFirstPoint) + GetFlyout(), aLine1);
  gp_Pnt aFlyoutEnd2 = ElCLib::Value (ElCLib::Parameter (aLine2, theSecondPoint) + GetFlyout(), aLine2);

  // fill sensitive entity for flyouts
  Handle(Select3D_SensitiveGroup) aSensitiveEntity = new Select3D_SensitiveGroup (theOwner);
  aSensitiveEntity->Add (new Select3D_SensitiveSegment (theOwner, theFirstPoint, aFlyoutEnd1));
  aSensitiveEntity->Add (new Select3D_SensitiveSegment (theOwner, theSecondPoint, aFlyoutEnd2));
  theSelection->Add (aSensitiveEntity);
}

//=======================================================================
//function : CircleFromPlanarFace
//purpose  : if possible computes circle from planar face
//=======================================================================
Standard_Boolean AIS_Dimension::CircleFromPlanarFace (const TopoDS_Face& theFace,
                                                      Handle(Geom_Curve)& theCurve,
                                                      gp_Pnt& theFirstPoint,
                                                      gp_Pnt& theLastPoint)
{
  TopExp_Explorer anIt (theFace, TopAbs_EDGE);
  for ( ; anIt.More(); anIt.Next())
  {
    TopoDS_Edge aCurEdge =  TopoDS::Edge (anIt.Current());
    if (AIS::ComputeGeometry (aCurEdge, theCurve, theFirstPoint, theLastPoint))
    {
      if (theCurve->IsInstance (STANDARD_TYPE(Geom_Circle)))
      {
        return Standard_True;
      }
    }
  }
  return Standard_False;
}

//=======================================================================
//function : CircleFromEdge
//purpose  : if possible computes circle from edge
//=======================================================================
Standard_Boolean AIS_Dimension::CircleFromEdge (const TopoDS_Edge& theEdge,
                                                gp_Circ&           theCircle,
                                                gp_Pnt&            theFirstPoint,
                                                gp_Pnt&            theLastPoint)
{
  BRepAdaptor_Curve anAdaptedCurve (theEdge);
  switch (anAdaptedCurve.GetType())
  {
    case GeomAbs_Circle:
    {
      theCircle = anAdaptedCurve.Circle();
      break;
    }
    case GeomAbs_Ellipse:
    {
      gp_Elips anEll = anAdaptedCurve.Ellipse();
      if ((anEll.MinorRadius() - anEll.MajorRadius()) >= Precision::Confusion())
      {
        return Standard_False;
      }
      theCircle = gp_Circ(anEll.Position(),anEll.MinorRadius());
      break;
    }
    case GeomAbs_Line:
    case GeomAbs_Hyperbola:
    case GeomAbs_Parabola:
    case GeomAbs_BezierCurve:
    case GeomAbs_BSplineCurve:
    case GeomAbs_OtherCurve:
    default:
      return Standard_False;
  }

  theFirstPoint = anAdaptedCurve.Value (anAdaptedCurve.FirstParameter());
  theLastPoint  = anAdaptedCurve.Value (anAdaptedCurve.LastParameter());
  return Standard_True;
}

//=======================================================================
//function : InitCircularDimension
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Dimension::InitCircularDimension (const TopoDS_Shape& theShape,
                                                       gp_Circ& theCircle,
                                                       gp_Pnt& theMiddleArcPoint,
                                                       Standard_Boolean& theIsClosed)
{
  gp_Pln aPln;
  Handle(Geom_Surface) aBasisSurf;
  AIS_KindOfSurface aSurfType = AIS_KOS_OtherSurface;
  gp_Pnt aFirstPoint, aLastPoint;
  Standard_Real anOffset    = 0.0;
  Standard_Real aFirstParam = 0.0;
  Standard_Real aLastParam  = 0.0;

  // Discover circular geometry
  switch (theShape.ShapeType())
  {
    case TopAbs_FACE:
    {
      AIS::GetPlaneFromFace (TopoDS::Face (theShape), aPln, aBasisSurf, aSurfType, anOffset);

      if (aSurfType == AIS_KOS_Plane)
      {
        Handle(Geom_Curve) aCurve;
        if (!CircleFromPlanarFace (TopoDS::Face (theShape), aCurve, aFirstPoint, aLastPoint))
        {
          return Standard_False;
        }

        theCircle = Handle(Geom_Circle)::DownCast (aCurve)->Circ();
      }
      else
      {
        gp_Pnt aCurPos;
        BRepAdaptor_Surface aSurf1 (TopoDS::Face (theShape));
        Standard_Real aFirstU = aSurf1.FirstUParameter();
        Standard_Real aLastU  = aSurf1.LastUParameter();
        Standard_Real aFirstV = aSurf1.FirstVParameter();
        Standard_Real aLastV  = aSurf1.LastVParameter();
        Standard_Real aMidU   = (aFirstU + aLastU) * 0.5;
        Standard_Real aMidV   = (aFirstV + aLastV) * 0.5;
        aSurf1.D0 (aMidU, aMidV, aCurPos);
        Handle (Adaptor3d_HCurve) aBasisCurve;
        Standard_Boolean isExpectedType = Standard_False;
        if (aSurfType == AIS_KOS_Cylinder)
        {
          isExpectedType = Standard_True;
        }
        else
        {
          if (aSurfType == AIS_KOS_Revolution)
          {
            aBasisCurve = aSurf1.BasisCurve();
            if (aBasisCurve->GetType() == GeomAbs_Line)
            {
              isExpectedType = Standard_True;
            }
          }
          else if (aSurfType == AIS_KOS_Extrusion)
          {
            aBasisCurve = aSurf1.BasisCurve();
            if (aBasisCurve->GetType() == GeomAbs_Circle)
            {
              isExpectedType = Standard_True;
            }
          }
        }

        if (!isExpectedType)
        {
          return Standard_False;
        }

        Handle(Geom_Curve) aCurve = aBasisSurf->VIso(aMidV);
        if (aCurve->DynamicType() == STANDARD_TYPE (Geom_Circle))
        {
          theCircle = Handle(Geom_Circle)::DownCast (aCurve)->Circ();
        }
        else if (aCurve->DynamicType() == STANDARD_TYPE (Geom_TrimmedCurve))
        {
          Handle(Geom_TrimmedCurve) aTrimmedCurve = Handle(Geom_TrimmedCurve)::DownCast (aCurve);
          aFirstU = aTrimmedCurve->FirstParameter();
          aLastU  = aTrimmedCurve->LastParameter();
          if (aTrimmedCurve->BasisCurve()->DynamicType() == STANDARD_TYPE (Geom_Circle))
          {
            theCircle = Handle(Geom_Circle)::DownCast(aTrimmedCurve->BasisCurve())->Circ();
          }
        }
        else
        {
          // Compute a circle from 3 points on "aCurve"
          gp_Pnt aP1, aP2;
          aSurf1.D0 (aFirstU, aMidV, aP1);
          aSurf1.D0 (aLastU, aMidV, aP2);
          GC_MakeCircle aMkCirc (aP1, aCurPos, aP2);
          theCircle = aMkCirc.Value()->Circ();
        }

        aFirstPoint = ElCLib::Value (aFirstU, theCircle);
        aLastPoint = ElCLib::Value (aLastU,  theCircle);
      }
      break;
    }
    case TopAbs_WIRE:
    {
      TopoDS_Edge anEdge;
      TopExp_Explorer anIt (theShape, TopAbs_EDGE);
      if (anIt.More())
      {
        anEdge = TopoDS::Edge (anIt.Current());
      }
      if (!AIS_Dimension::CircleFromEdge (anEdge, theCircle, aFirstPoint, aLastPoint))
      {
        return Standard_False;
      }
      break;
    }
    case TopAbs_EDGE:
    {
      TopoDS_Edge anEdge = TopoDS::Edge (theShape);
      if (!AIS_Dimension::CircleFromEdge (anEdge, theCircle, aFirstPoint, aLastPoint))
      {
        return Standard_False;
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
      return Standard_False;
  }

  theIsClosed = aFirstPoint.IsEqual (aLastPoint, Precision::Confusion());

  gp_Pnt aCenter = theCircle.Location();

  if (theIsClosed) // Circle
  {
    gp_Dir anXDir = theCircle.XAxis().Direction();
    theMiddleArcPoint = aCenter.Translated (gp_Vec (anXDir) * theCircle.Radius());
  }
  else // Arc
  {
    aFirstParam = ElCLib::Parameter (theCircle, aFirstPoint);
    aLastParam  = ElCLib::Parameter (theCircle, aLastPoint);
    if (aFirstParam > aLastParam)
    {
      aFirstParam -= 2.0 * M_PI;
    }

    Standard_Real aParCurPos = (aFirstParam + aLastParam) * 0.5;
    gp_Vec aVec = gp_Vec (aCenter, ElCLib::Value (aParCurPos, theCircle)).Normalized () * theCircle.Radius ();
    theMiddleArcPoint = aCenter.Translated (aVec);
  }

  return Standard_True;
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_Dimension::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                      const Standard_Integer theMode)
{
  if (!mySelectionGeom.IsComputed)
  {
    return;
  }

  AIS_DimensionSelectionMode aSelectionMode = (AIS_DimensionSelectionMode)theMode;

  // init appropriate entity owner
  Handle(SelectMgr_EntityOwner) aSensitiveOwner;

  switch (aSelectionMode)
  {
    // neutral selection owner
    case AIS_DSM_All :
      aSensitiveOwner = new SelectMgr_EntityOwner (this, THE_NEUTRAL_SEL_PRIORITY);
      break;

    // local selection owners
    case AIS_DSM_Line :
    case AIS_DSM_Text :
      aSensitiveOwner = new AIS_DimensionOwner (this, aSelectionMode, THE_LOCAL_SEL_PRIORITY);
      break;
  }

  if (aSelectionMode == AIS_DSM_All || aSelectionMode == AIS_DSM_Line)
  {
    // sensitives for dimension line segments
    Handle(Select3D_SensitiveGroup) aGroupOfSensitives = new Select3D_SensitiveGroup (aSensitiveOwner);

    SelectionGeometry::SeqOfCurves::Iterator aCurveIt (mySelectionGeom.DimensionLine);
    for (; aCurveIt.More(); aCurveIt.Next())
    {
      const SelectionGeometry::HCurve& aCurveData = aCurveIt.Value();

      TColgp_Array1OfPnt aSensitivePnts (1, aCurveData->Length());
      for (Standard_Integer aPntIt = 1; aPntIt <= aCurveData->Length(); ++aPntIt)
      {
        aSensitivePnts.ChangeValue (aPntIt) = aCurveData->Value (aPntIt);
      }

      aGroupOfSensitives->Add (new Select3D_SensitiveCurve (aSensitiveOwner, aSensitivePnts));
    }

    Standard_Real anArrowLength = myDrawer->DimensionAspect()->ArrowAspect()->Length();
    Standard_Real anArrowAngle  = myDrawer->DimensionAspect()->ArrowAspect()->Angle();

    // sensitives for arrows
    SelectionGeometry::SeqOfArrows::Iterator anArrowIt (mySelectionGeom.Arrows);
    for (; anArrowIt.More(); anArrowIt.Next())
    {
      const SelectionGeometry::HArrow& anArrow = anArrowIt.Value();

      gp_Pnt aSidePnt1 (gp::Origin());
      gp_Pnt aSidePnt2 (gp::Origin());
      const gp_Dir& aPlane = GetPlane().Axis().Direction();
      const gp_Pnt& aPeak  = anArrow->Position;
      const gp_Dir& aDir   = anArrow->Direction;

      // compute points for arrow in plane
      PointsForArrow (aPeak, aDir, aPlane, anArrowLength, anArrowAngle, aSidePnt1, aSidePnt2);

      aGroupOfSensitives->Add (new Select3D_SensitiveTriangle (aSensitiveOwner, aPeak, aSidePnt1, aSidePnt2));

      if (!myDrawer->DimensionAspect()->IsArrows3d())
      {
        continue;
      }

      // compute points for orthogonal sensitive plane
      gp_Dir anOrthoPlane = anArrow->Direction.Crossed (aPlane);

      PointsForArrow (aPeak, aDir, anOrthoPlane, anArrowLength, anArrowAngle, aSidePnt1, aSidePnt2);

      aGroupOfSensitives->Add (new Select3D_SensitiveTriangle (aSensitiveOwner, aPeak, aSidePnt1, aSidePnt2));
    }

    theSelection->Add (aGroupOfSensitives);
  }

  // sensitives for text element
  if (aSelectionMode == AIS_DSM_All || aSelectionMode == AIS_DSM_Text)
  {
    Handle(Select3D_SensitiveEntity) aTextSensitive;

    gp_Ax2 aTextAxes (mySelectionGeom.TextPos,
                      GetPlane().Axis().Direction(),
                      mySelectionGeom.TextDir);

    if (myDrawer->DimensionAspect()->IsText3d())
    {
      // sensitive planar rectangle for text
      Standard_Real aDx = mySelectionGeom.TextWidth  * 0.5;
      Standard_Real aDy = mySelectionGeom.TextHeight * 0.5;

      gp_Trsf aLabelPlane;
      aLabelPlane.SetTransformation (aTextAxes, gp::XOY());

      TColgp_Array1OfPnt aRectanglePoints(1, 4);
      aRectanglePoints.ChangeValue(1) = gp_Pnt (-aDx, -aDy, 0.0).Transformed (aLabelPlane);
      aRectanglePoints.ChangeValue(2) = gp_Pnt (-aDx,  aDy, 0.0).Transformed (aLabelPlane);
      aRectanglePoints.ChangeValue(3) = gp_Pnt ( aDx,  aDy, 0.0).Transformed (aLabelPlane);
      aRectanglePoints.ChangeValue(4) = gp_Pnt ( aDx, -aDy, 0.0).Transformed (aLabelPlane);

      Poly_Array1OfTriangle aTriangles(1, 2);
      aTriangles.ChangeValue(1) = Poly_Triangle(1, 2, 3);
      aTriangles.ChangeValue(2) = Poly_Triangle(1, 3, 4);

      Handle(Poly_Triangulation) aRectanglePoly = 
        new Poly_Triangulation(aRectanglePoints, aTriangles);

      aTextSensitive =
        new Select3D_SensitiveTriangulation (aSensitiveOwner, aRectanglePoly, TopLoc_Location(), Standard_True);
    }
    else
    {
      gp_Circ aTextGeom (aTextAxes, mySelToleranceForText2d != 0.0 
                                      ? mySelToleranceForText2d : 1.0);

      Handle(Geom_Circle) aSensGeom = new Geom_Circle (aTextGeom);

      aTextSensitive = new Select3D_SensitiveCircle (aSensitiveOwner, aSensGeom, Standard_True);
    }

    theSelection->Add (aTextSensitive);
  }

  // callback for flyout sensitive calculation
  if (aSelectionMode == AIS_DSM_All)
  {
    ComputeFlyoutSelection (theSelection, aSensitiveOwner);
  }
}

//=======================================================================
//function : PointsForArrow
//purpose  : 
//=======================================================================
void AIS_Dimension::PointsForArrow (const gp_Pnt& thePeakPnt,
                                    const gp_Dir& theDirection,
                                    const gp_Dir& thePlane,
                                    const Standard_Real theArrowLength,
                                    const Standard_Real theArrowAngle,
                                    gp_Pnt& theSidePnt1,
                                    gp_Pnt& theSidePnt2)
{
  gp_Lin anArrowLin (thePeakPnt, theDirection.Reversed());
  gp_Pnt anArrowEnd = ElCLib::Value (theArrowLength, anArrowLin);
  gp_Lin anEdgeLin (anArrowEnd, theDirection.Crossed (thePlane));

  Standard_Real anEdgeLength = Tan (theArrowAngle) * theArrowLength;

  theSidePnt1 = ElCLib::Value ( anEdgeLength, anEdgeLin);
  theSidePnt2 = ElCLib::Value (-anEdgeLength, anEdgeLin);
}

//=======================================================================
//function : GetTextPositionForLinear
//purpose  : 
//=======================================================================
gp_Pnt AIS_Dimension::GetTextPositionForLinear (const gp_Pnt& theFirstPoint,
                                                const gp_Pnt& theSecondPoint,
                                                const Standard_Boolean theIsOneSide) const
{
  if (!IsValid())
  {
    return gp::Origin();
  }

  gp_Pnt aTextPosition (gp::Origin());

  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();

  // Get label alignment and arrow orientation.
  Standard_Integer aLabelPosition = 0;
  Standard_Boolean isArrowsExternal = Standard_False;
  FitTextAlignmentForLinear (theFirstPoint, theSecondPoint, theIsOneSide,
                             aDimensionAspect->TextHorizontalPosition(),
                             aLabelPosition, isArrowsExternal);

  // Compute dimension line points.
  gp_Dir aPlaneNormal = GetPlane().Axis().Direction();
  gp_Vec aTargetPointsVec (theFirstPoint, theSecondPoint);

  // Compute flyout direction vector
  gp_Dir aFlyoutVector = aPlaneNormal ^ gp_Dir (aTargetPointsVec);

  // create lines for layouts
  gp_Lin aLine1 (theFirstPoint, aFlyoutVector);
  gp_Lin aLine2 (theSecondPoint, aFlyoutVector);
  // Get flyout end points
  gp_Pnt aLineBegPoint = ElCLib::Value (ElCLib::Parameter (aLine1, theFirstPoint)  + GetFlyout(), aLine1);
  gp_Pnt aLineEndPoint = ElCLib::Value (ElCLib::Parameter (aLine2, theSecondPoint) + GetFlyout(), aLine2);

  // Get text position.
  switch (aLabelPosition & LabelPosition_HMask)
  {
  case LabelPosition_Left:
    {
      gp_Dir aTargetPointsDir = gce_MakeDir (theFirstPoint, theSecondPoint);
      Standard_Real anExtensionSize = aDimensionAspect->ExtensionSize();

      Standard_Real anOffset = isArrowsExternal
                                 ? anExtensionSize + aDimensionAspect->ArrowAspect()->Length()
                                 : anExtensionSize;
      gp_Vec anExtensionVec = gp_Vec (aTargetPointsDir) * -anOffset;
      aTextPosition = aLineEndPoint.Translated (anExtensionVec);
    }
    break;
  case LabelPosition_Right:
    {
      gp_Dir aTargetPointsDir = gce_MakeDir (theFirstPoint, theSecondPoint);
      Standard_Real anExtensionSize = aDimensionAspect->ExtensionSize();

      Standard_Real anOffset = isArrowsExternal
                                 ? anExtensionSize + aDimensionAspect->ArrowAspect()->Length()
                                 : anExtensionSize;
      gp_Vec anExtensionVec = gp_Vec (aTargetPointsDir) * anOffset;
      aTextPosition = aLineBegPoint.Translated (anExtensionVec);
    }
    break;
  case LabelPosition_HCenter:
    {
      aTextPosition = (aLineBegPoint.XYZ() + aLineEndPoint.XYZ()) * 0.5;
    }
    break;
  }

  return aTextPosition;
}

//=======================================================================
//function : AdjustParametersForLinear
//purpose  : 
//=======================================================================
Standard_Boolean AIS_Dimension::AdjustParametersForLinear (const gp_Pnt& theTextPos,
                                                           const gp_Pnt& theFirstPoint,
                                                           const gp_Pnt& theSecondPoint,
                                                           Standard_Real& theExtensionSize,
                                                           Prs3d_DimensionTextHorizontalPosition& theAlignment,
                                                           Standard_Real& theFlyout,
                                                           gp_Pln& thePlane,
                                                           Standard_Boolean& theIsPlaneOld) const
{
  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();
  Standard_Real anArrowLength = aDimensionAspect->ArrowAspect()->Length();

  gp_Dir aTargetPointsDir = gce_MakeDir (theFirstPoint, theSecondPoint);
  gp_Vec aTargetPointsVec (theFirstPoint, theSecondPoint);

  // Don't set new plane if the text position lies on the attachment points line.
  gp_Lin aTargetPointsLin (theFirstPoint, aTargetPointsDir);
  if (!aTargetPointsLin.Contains (theTextPos, Precision::Confusion()))
  {
    //Set new automatic plane.
    thePlane = gce_MakePln (theTextPos, theFirstPoint, theSecondPoint);
    theIsPlaneOld = Standard_False;
  }

  // Compute flyout direction vector.
  gp_Dir aPlaneNormal = GetPlane().Axis().Direction();
  gp_Dir aPositiveFlyout = aPlaneNormal ^ aTargetPointsDir;

  // Additional check of collinearity of the plane normal and attachment points vector.
  if (aPlaneNormal.IsParallel (aTargetPointsDir, Precision::Angular()))
  {
    return Standard_False;
  }

  // Set flyout.
  gp_Vec aFirstToTextVec (theFirstPoint, theTextPos);

  Standard_Real aCos = aFirstToTextVec.Normalized() * gp_Vec (aTargetPointsDir);

  gp_Pnt aTextPosProj = theFirstPoint.Translated
    (gp_Vec (aTargetPointsDir) * aFirstToTextVec.Magnitude() * aCos);

  // Compute flyout value and direction.
  gp_Vec aFlyoutVector = gp_Vec (aTextPosProj, theTextPos);

  theFlyout = 0.0;
  if (aFlyoutVector.Magnitude() > Precision::Confusion())
  {
    theFlyout = gp_Dir (aFlyoutVector).IsOpposite (aPositiveFlyout, Precision::Angular())
                ? -aFlyoutVector.Magnitude()
                :  aFlyoutVector.Magnitude();
  }
  
  // Compute attach points (through which main dimension line passes).
  gp_Pnt aFirstAttach  = theFirstPoint.Translated (aFlyoutVector);
  gp_Pnt aSecondAttach = theSecondPoint.Translated (aFlyoutVector);

  // Set horizontal text alignment.
  if (aCos < 0.0)
  {
    theAlignment = Prs3d_DTHP_Left;

    Standard_Real aNewExtSize = theTextPos.Distance (aFirstAttach) - anArrowLength;
    theExtensionSize = aNewExtSize < 0.0 ? 0.0 : aNewExtSize;
  }
  else if (aTextPosProj.Distance (theFirstPoint) > theFirstPoint.Distance (theSecondPoint))
  {
    theAlignment = Prs3d_DTHP_Right;

    Standard_Real aNewExtSize = theTextPos.Distance (aSecondAttach) - anArrowLength;
    theExtensionSize = aNewExtSize < 0.0 ? 0.0 : aNewExtSize;
  }
  else
  {
    theAlignment = Prs3d_DTHP_Center;
  }
  return Standard_True;
}

//=======================================================================
//function : FitTextAlignmentForLinear
//purpose  : 
//=======================================================================
void AIS_Dimension::FitTextAlignmentForLinear (const gp_Pnt& theFirstPoint,
                                               const gp_Pnt& theSecondPoint,
                                               const Standard_Boolean theIsOneSide,
                                               const Prs3d_DimensionTextHorizontalPosition& theHorizontalTextPos,
                                               Standard_Integer& theLabelPosition,
                                               Standard_Boolean& theIsArrowsExternal) const
{
  theLabelPosition = LabelPosition_None;
  theIsArrowsExternal = Standard_False;

  // Compute dimension line points
  gp_Ax1 aPlaneNormal = GetPlane().Axis();
  gp_Dir aTargetPointsVector = gce_MakeDir (theFirstPoint, theSecondPoint);

  // compute flyout direction vector
  gp_Dir aFlyoutVector = aPlaneNormal.Direction() ^ aTargetPointsVector;

  // create lines for layouts
  gp_Lin aLine1 (theFirstPoint, aFlyoutVector);
  gp_Lin aLine2 (theSecondPoint, aFlyoutVector);

  // Get flyout end points
  gp_Pnt aLineBegPoint = ElCLib::Value (ElCLib::Parameter (aLine1, theFirstPoint)  + GetFlyout(), aLine1);
  gp_Pnt aLineEndPoint = ElCLib::Value (ElCLib::Parameter (aLine2, theSecondPoint) + GetFlyout(), aLine2);

  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();

  // For extensions we need to know arrow size, text size and extension size: get it from aspect
  Standard_Real anArrowLength = aDimensionAspect->ArrowAspect()->Length();

  // prepare label string and compute its geometrical width
  Standard_Real aLabelWidth;
  TCollection_ExtendedString aLabelString = GetValueString (aLabelWidth);

  // Add margins to cut dimension lines for 3d text
  if (aDimensionAspect->IsText3d())
  {
    aLabelWidth += aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN * 2.0;
  }

  // Handle user-defined and automatic arrow placement
  switch (aDimensionAspect->ArrowOrientation())
  {
    case Prs3d_DAO_External: theIsArrowsExternal = true; break;
    case Prs3d_DAO_Internal: theIsArrowsExternal = false; break;
    case Prs3d_DAO_Fit:
    {
      // Add margin to ensure a small tail between text and arrow
      Standard_Real anArrowMargin   = aDimensionAspect->IsText3d() 
                                    ? aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN
                                    : 0.0;

      Standard_Real aDimensionWidth = aLineBegPoint.Distance (aLineEndPoint);
      Standard_Real anArrowsWidth   = theIsOneSide 
                                      ?  anArrowLength + anArrowMargin
                                      : (anArrowLength + anArrowMargin) * 2.0;

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
      Standard_Real aDimensionWidth = aLineBegPoint.Distance (aLineEndPoint);
      Standard_Real anArrowsWidth   = theIsOneSide ? anArrowLength : 2.0 * anArrowLength;
      Standard_Real aContentWidth   = theIsArrowsExternal ? aLabelWidth : aLabelWidth + anArrowsWidth;

      theLabelPosition |= aDimensionWidth < aContentWidth ? LabelPosition_Left : LabelPosition_HCenter;
      break;
    }
  }

  // Handle vertical text placement options
  switch (aDimensionAspect->TextVerticalPosition())
  {
    case Prs3d_DTVP_Above  : theLabelPosition |= LabelPosition_Above; break;
    case Prs3d_DTVP_Below  : theLabelPosition |= LabelPosition_Below; break;
    case Prs3d_DTVP_Center : theLabelPosition |= LabelPosition_VCenter; break;
  }
}

//=======================================================================
//function : UnsetFixedTextPosition
//purpose  : 
//=======================================================================
void AIS_Dimension::UnsetFixedTextPosition()
{
  myIsTextPositionFixed = Standard_False;
  myFixedTextPosition = gp::Origin();
}
