// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#include <AIS_Dimension.hxx>

#include <AIS.hxx>
#include <AIS_DimensionOwner.hxx>
#include <AIS_Drawer.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <ElCLib.hxx>
#include <Font_BRepFont.hxx>
#include <GC_MakeCircle.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gce_MakeDir.hxx>
#include <gce_MakeLin.hxx>
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
#include <Select3D_ListIteratorOfListOfSensitive.hxx>
#include <Select3D_ListOfSensitive.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <Select3D_SensitiveSegment.hxx>
#include <Select3D_SensitiveFace.hxx>
#include <Select3D_SensitiveTriangle.hxx>
#include <Standard_CString.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <StdPrs_WFShape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <Units.hxx>
#include <Units_UnitsDictionary.hxx>
#include <UnitsAPI.hxx>
#include <UnitsAPI_SystemUnits.hxx>

IMPLEMENT_STANDARD_HANDLE(AIS_Dimension, AIS_InteractiveObject)
IMPLEMENT_STANDARD_RTTIEXT(AIS_Dimension, AIS_InteractiveObject)

namespace
{
  // default text strings
  static const Standard_Utf32Char THE_FILL_CHARACTER ('0');
  static const TCollection_ExtendedString THE_EMPTY_LABEL;

  // default text margin and resolution
  static const Standard_Real THE_3D_TEXT_MARGIN    = 0.1;
  static const unsigned int  THE_2D_TEXT_RESOLUTION = 72;

  // default selection priorities
  static const Standard_Integer THE_NEUTRAL_SEL_PRIORITY = 5;
  static const Standard_Integer THE_LOCAL_SEL_PRIORITY   = 6;
};

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
AIS_Dimension::AIS_Dimension()
: AIS_InteractiveObject(),
  myDefaultPlane (gp_Pln (gp::XOY())),
  myIsWorkingPlaneCustom (Standard_False),
  myValue (0.0),
  myIsValueCustom (Standard_False),
  myUnitsQuantity (TCollection_AsciiString("LENGTH")),
  myToDisplayUnits (Standard_False),
  mySpecialSymbol (' '),
  myDisplaySpecialSymbol (AIS_DSS_No),
  myIsTextReversed (Standard_False),
  myIsInitialized (Standard_False),
  myFlyout (0.0),
  myKindOfDimension (AIS_KOD_NONE)
{
  ResetWorkingPlane();
  // Units default settings
  UnitsAPI::SetLocalSystem (UnitsAPI_SI);
  myModelUnits = Units::DictionaryOfUnits()->ActiveUnit (myUnitsQuantity.ToCString());
  myDisplayUnits = Units::DictionaryOfUnits()->ActiveUnit (myUnitsQuantity.ToCString());
}

//=======================================================================
//function : AcceptDisplayMode
//purpose  : Checks if display mode <theMode> is allowed to display object.
//=======================================================================
Standard_Boolean AIS_Dimension::AcceptDisplayMode (const Standard_Integer theMode) const
{
  return theMode == ComputeMode_All;
}

//=======================================================================
//function : computeValue
//purpose  : Computes dimension value in display units.
//=======================================================================
void AIS_Dimension::computeValue()
{
  UnitsAPI::SetCurrentUnit (myUnitsQuantity.ToCString(), myModelUnits.ToCString());
  myValue = UnitsAPI::CurrentFromLS (myValue, myUnitsQuantity.ToCString());
  myValue = valueToDisplayUnits();
}

//=======================================================================
//function : countDefaultPlane
//purpose  : 
//=======================================================================
void AIS_Dimension::countDefaultPlane()
{
}

//=======================================================================
//function : GetWorkingPlane
//purpose  : 
//=======================================================================
const gp_Pln& AIS_Dimension::GetWorkingPlane() const
{
  return myWorkingPlane;
}

//=======================================================================
//function : SetWorkingPlane
//purpose  : 
//=======================================================================
void AIS_Dimension::SetWorkingPlane (const gp_Pln& thePlane)
{
  myWorkingPlane = thePlane;
  myIsWorkingPlaneCustom = Standard_True;
}

//=======================================================================
//function : ResetWorkingPlane
//purpose  : Set default value of working plane
//=======================================================================
void AIS_Dimension::ResetWorkingPlane()
{
  myWorkingPlane = myDefaultPlane;
  myIsWorkingPlaneCustom = Standard_False;
}

//=======================================================================
//function : resetWorkingPlane
//purpose  : Set default value of working plane
//           Only for internal use.
//=======================================================================
void AIS_Dimension::resetWorkingPlane (const gp_Pln& theNewDefaultPlane)
{
  myDefaultPlane = theNewDefaultPlane;
  ResetWorkingPlane();
}

//=======================================================================
//function : valueInDisplayUnits
//purpose  :
//=======================================================================
Standard_Real AIS_Dimension::valueToDisplayUnits()
{
  return  UnitsAPI::AnyToAny (myValue,
                              myModelUnits.ToCString(),
                              myDisplayUnits.ToCString());
}

//=======================================================================
//function : KindOfDimension
//purpose  : 
//=======================================================================
AIS_KindOfDimension AIS_Dimension::KindOfDimension() const 
{
  return myKindOfDimension;
}

//=======================================================================
//function : SetKindOfDimension
//purpose  : 
//=======================================================================
void AIS_Dimension::SetKindOfDimension (const AIS_KindOfDimension theKindOfDimension)
{
  myKindOfDimension = theKindOfDimension;
}

//=======================================================================
//function : GetValue
//purpose  : 
//=======================================================================
Standard_Real AIS_Dimension::GetValue() const
 {
  return myValue;
 }

//=======================================================================
//function : SetCustomValue
//purpose  : 
//=======================================================================
void AIS_Dimension::SetCustomValue (const Standard_Real theValue)
{
  myValue = theValue;
  myIsValueCustom = Standard_True;
}

//=======================================================================
//function : SetFirstShape
//purpose  : 
//=======================================================================
void AIS_Dimension::SetFirstShape (const TopoDS_Shape& theShape)
{
  myFirstShape = theShape;
  myIsInitialized = Standard_False;
  resetGeom();
}

//=======================================================================
//function : SetSecondShape
//purpose  : 
//=======================================================================
void AIS_Dimension::SetSecondShape (const TopoDS_Shape& theShape)
{
  mySecondShape = theShape;
  myIsInitialized = Standard_False;
  resetGeom();
}

//=======================================================================
//function : getTextWidthAndString
//purpose  : 
//=======================================================================
void AIS_Dimension::getTextWidthAndString (Quantity_Length& theWidth,
                                           TCollection_ExtendedString& theString) const
{
  char aValueSimpleStr[25];
  sprintf (aValueSimpleStr, "%g", GetValue());
  theString = TCollection_ExtendedString (aValueSimpleStr);

  if (IsUnitsDisplayed())
  {
    theString += " ";
    theString += TCollection_ExtendedString (myDisplayUnits);
  }

  if (myDisplaySpecialSymbol == AIS_DSS_Before)
  {
    theString = TCollection_ExtendedString (mySpecialSymbol) + theString;
  }
  else if (myDisplaySpecialSymbol == AIS_DSS_After)
  {
    theString += TCollection_ExtendedString (mySpecialSymbol);
  }

  // Get text style parameters
  Quantity_Color aColor; 
  Standard_CString aFontName;
  Standard_Real aFactor;
  Standard_Real aSpace;
  myDrawer->DimensionAspect()->TextAspect()->Aspect()->Values (aColor, aFontName, aFactor, aSpace);
  Font_FontAspect aFontAspect = myDrawer->DimensionAspect()->TextAspect()->Aspect()->GetTextFontAspect();
  Standard_Real   aFontHeight = myDrawer->DimensionAspect()->TextAspect()->Height();

  NCollection_Utf8String anUTFString = (Standard_Utf16Char* )theString.ToExtString();

  theWidth = 0.0;

  if (myDrawer->DimensionAspect()->IsText3d())
  {
    // text width produced by BRepFont
    Font_BRepFont aFont (aFontName, aFontAspect, aFontHeight);

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
    Handle(Font_FTFont) aFont = new Font_FTFont ();
    aFont->Init (aFontName, aFontAspect, (const unsigned int)aFontHeight, THE_2D_TEXT_RESOLUTION);

    for (NCollection_Utf8Iter anIter = anUTFString.Iterator(); *anIter != 0; )
    {
      Standard_Utf32Char aCurrChar = *anIter;
      Standard_Utf32Char aNextChar = *(++anIter);
      theWidth += (Standard_Real) aFont->AdvanceX (aCurrChar, aNextChar);
    }
  }
}

//=======================================================================
//function : drawArrow
//purpose  : 
//=======================================================================
void AIS_Dimension::drawArrow (const Handle(Prs3d_Presentation)& thePresentation,
                               const gp_Pnt& theLocation,
                               const gp_Dir& theDirection)
{
  Prs3d_Root::NewGroup (thePresentation);

  Quantity_Length aLength = myDrawer->DimensionAspect()->ArrowAspect()->Length();
  Standard_Real   anAngle = myDrawer->DimensionAspect()->ArrowAspect()->Angle();

  if (myDrawer->DimensionAspect()->IsArrows3d())
  {
    Prs3d_Arrow::Draw (thePresentation,
                       theLocation,
                       theDirection,
                       anAngle,
                       aLength);
  }
  else
  {
    gp_Pnt aLeftPoint (gp::Origin());
    gp_Pnt aRightPoint (gp::Origin());
    const gp_Dir& aPlane = myWorkingPlane.Axis().Direction();

    PointsForArrow (theLocation, theDirection, aPlane, aLength, anAngle, aLeftPoint, aRightPoint);

    Handle(Graphic3d_ArrayOfTriangles) anArrow = new Graphic3d_ArrayOfTriangles(3);

    anArrow->AddVertex (aLeftPoint);
    anArrow->AddVertex (theLocation);
    anArrow->AddVertex (aRightPoint);

    // Set aspect for arrow triangles
    Quantity_Color aColor;
    Aspect_TypeOfLine aTOL;
    Standard_Real aWidth;
    myDrawer->DimensionAspect()->ArrowAspect()->Aspect()->Values (aColor, aTOL, aWidth);
    Graphic3d_MaterialAspect aShadeMat (Graphic3d_NOM_DEFAULT);
    aShadeMat.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
    aShadeMat.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
    aShadeMat.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);

    Handle(Prs3d_ShadingAspect) aShadingStyle = new Prs3d_ShadingAspect();
    aShadingStyle->SetColor (aColor);
    aShadingStyle->SetMaterial (aShadeMat);

    Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aShadingStyle->Aspect());
    Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (anArrow);
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
    Quantity_Color aColor;
    Standard_CString aFontName;
    Standard_Real anExpansionFactor;
    Standard_Real aSpace;
    myDrawer->DimensionAspect()->TextAspect()->Aspect()->Values (aColor, aFontName, anExpansionFactor, aSpace);
    Font_FontAspect aFontAspect = myDrawer->DimensionAspect()->TextAspect()->Aspect()->GetTextFontAspect();
    Standard_Real aFontHeight = myDrawer->DimensionAspect()->TextAspect()->Height();

    // creating TopoDS_Shape for text
    Font_BRepFont aFont (aFontName, aFontAspect, aFontHeight);
    NCollection_Utf8String anUTFString = (Standard_Utf16Char* )theText.ToExtString();
    TopoDS_Shape aTextShape = aFont.RenderText (anUTFString);

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
    gp_Ax3 aTextCoordSystem (theTextPos, myWorkingPlane.Axis().Direction(), aTextDir);
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

    gp_Ax2 aFlippingAxes (aCenterOfLabel, myWorkingPlane.Axis().Direction(), aTextDir);
    Prs3d_Root::CurrentGroup (thePresentation)->SetFlippingOptions (Standard_True, aFlippingAxes);

    // draw text
    if (myDrawer->DimensionAspect()->IsTextShaded())
    {
      // Setting text shading and color parameters
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
      // setting color for text
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

  Prs3d_Text::Draw (thePresentation,
                    myDrawer->DimensionAspect()->TextAspect(),
                    theText,
                    theTextPos);

  mySelectionGeom.TextPos    = theTextPos;
  mySelectionGeom.TextDir    = theTextDir;
  mySelectionGeom.TextWidth  = 0.0;
  mySelectionGeom.TextHeight = 0.0;
}

//=======================================================================
//function : drawExtension
//purpose  : 
//=======================================================================
void AIS_Dimension::drawExtension (const Handle(Prs3d_Presentation)& thePresentation,
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
    gp_Dir aTextDir = myIsTextReversed ? -theExtensionDir : theExtensionDir;

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
//function : SetDimensionAspect
//purpose  : 
//=======================================================================
void AIS_Dimension::SetDimensionAspect (const Handle(Prs3d_DimensionAspect)& theDimensionAspect)
{
  myDrawer->SetDimensionAspect (theDimensionAspect);
}

//=======================================================================
//function : DimensionAspect
//purpose  : 
//=======================================================================
Handle(Prs3d_DimensionAspect) AIS_Dimension::DimensionAspect() const
{
  return myDrawer->DimensionAspect();
}

//=======================================================================
//function : drawLinearDimension
//purpose  : 
//=======================================================================
void AIS_Dimension::drawLinearDimension (const Handle(Prs3d_Presentation)& thePresentation,
                                         const Standard_Integer theMode,
                                         const Standard_Boolean isOneSideDimension/* = Standard_False*/)
{
  // don not build any dimension for equal points
  if (myFirstPoint.IsEqual (mySecondPoint, Precision::Confusion()))
  {
    setComputed (Standard_False);
    return;
  }

  // compute dimension line points
  gp_Ax1 aWorkingPlaneNormal = GetWorkingPlane().Axis();
  gp_Dir aTargetPointsVector = gce_MakeDir (myFirstPoint, mySecondPoint);

  // compute flyout direction vector
  gp_Dir aFlyoutVector = aWorkingPlaneNormal.Direction() ^ aTargetPointsVector;

  // create lines for layouts
  gp_Lin aLine1 (myFirstPoint, aFlyoutVector);
  gp_Lin aLine2 (mySecondPoint, aFlyoutVector);

  // Get flyout end points
  gp_Pnt aLineBegPoint = ElCLib::Value (ElCLib::Parameter (aLine1, myFirstPoint)  + GetFlyout(), aLine1);
  gp_Pnt aLineEndPoint = ElCLib::Value (ElCLib::Parameter (aLine2, mySecondPoint) + GetFlyout(), aLine2);

  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();
  Handle(SelectMgr_EntityOwner) anEmptyOwner;

  gp_Lin aDimensionLine = gce_MakeLin (aLineBegPoint, aLineEndPoint);

  // For extensions we need to know arrow size, text size and extension size: get it from aspect
  Quantity_Length anArrowLength   = aDimensionAspect->ArrowAspect()->Length();
  Standard_Real   anExtensionSize = aDimensionAspect->ExtensionSize();

  if (!myIsValueCustom)
  {
   computeValue();
  }

  // prepare label string and compute its geometrical width
  Standard_Real aLabelWidth;
  TCollection_ExtendedString aLabelString;
  getTextWidthAndString (aLabelWidth, aLabelString);

  // add margins to cut dimension lines for 3d text
  if (aDimensionAspect->IsText3d())
  {
    aLabelWidth += aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN * 2.0;
  }

  // handle user-defined and automatic arrow placement
  bool isArrowsExternal = false;
  switch (aDimensionAspect->ArrowOrientation())
  {
    case Prs3d_DAO_External: isArrowsExternal = true; break;
    case Prs3d_DAO_Internal: isArrowsExternal = false; break;
    case Prs3d_DAO_Fit:
    {
      // add margin to ensure a small tail between text and arrow
      Standard_Real anArrowMargin   = aDimensionAspect->IsText3d() 
                                    ? aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN
                                    : 0.0;

      Standard_Real aDimensionWidth = aLineBegPoint.Distance (aLineEndPoint);
      Standard_Real anArrowsWidth   = isOneSideDimension 
                                    ?  anArrowLength + anArrowMargin
                                    : (anArrowLength + anArrowMargin) * 2.0;

      isArrowsExternal = aDimensionWidth < aLabelWidth + anArrowsWidth;
      break;
    }
  }

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
    ? aLineBegPoint  : aFirstArrowEnd;

  gp_Pnt aCenterLineEnd   = isArrowsExternal || isOneSideDimension
    ? aLineEndPoint : aSecondArrowEnd;

  Standard_Integer aLabelPosition = LabelPosition_None;

  // handle user-defined and automatic text placement
  switch (aDimensionAspect->TextHorizontalPosition())
  {
    case Prs3d_DTHP_Left  : aLabelPosition |= LabelPosition_Left; break;
    case Prs3d_DTHP_Right : aLabelPosition |= LabelPosition_Right; break;
    case Prs3d_DTHP_Center: aLabelPosition |= LabelPosition_HCenter; break;
    case Prs3d_DTHP_Fit:
    {
      Standard_Real aDimensionWidth = aLineBegPoint.Distance (aLineEndPoint);
      Standard_Real anArrowsWidth   = isOneSideDimension ? anArrowLength : 2.0 * anArrowLength;
      Standard_Real aContentWidth   = isArrowsExternal ? aLabelWidth : aLabelWidth + anArrowsWidth;

      aLabelPosition |= aDimensionWidth < aContentWidth ? LabelPosition_Left : LabelPosition_HCenter;
      break;
    }
  }

  // handle vertical text placement options
  switch (aDimensionAspect->TextVerticalPosition())
  {
    case Prs3d_DTVP_Above  : aLabelPosition |= LabelPosition_Above; break;
    case Prs3d_DTVP_Below  : aLabelPosition |= LabelPosition_Below; break;
    case Prs3d_DTVP_Center : aLabelPosition |= LabelPosition_VCenter; break;
  }

  switch (aLabelPosition & LabelPosition_HMask)
  {
    // ------------------------------------------------------------------------ //
    //                                CENTER                                    //
    // -------------------------------------------------------------------------//
    case LabelPosition_HCenter:
    {
      // add label on dimension or extension line to presentation
      Prs3d_Root::NewGroup (thePresentation);

      gp_Pnt aTextPos = (aCenterLineBegin.XYZ() + aCenterLineEnd.XYZ()) * 0.5;
      gp_Dir aTextDir = myIsTextReversed 
                      ? -aDimensionLine.Direction()
                      :  aDimensionLine.Direction();

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

        drawArrow (thePresentation, aFirstArrowBegin, aFirstArrowDir);
        if (!isOneSideDimension)
        {
          drawArrow (thePresentation, aSecondArrowBegin, aSecondArrowDir);
        }

        if (!isArrowsExternal)
        {
          break;
        }

        // add arrow extension lines to presentation
        Prs3d_Root::NewGroup (thePresentation);

        drawExtension (thePresentation, anExtensionSize,
                       aFirstArrowEnd, aFirstExtensionDir,
                       THE_EMPTY_LABEL, 0.0, theMode, LabelPosition_None);
        if (!isOneSideDimension)
        {
          drawExtension (thePresentation, anExtensionSize,
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
      drawExtension (thePresentation, anExtensionSize,
                     isArrowsExternal ? aFirstArrowEnd : aFirstArrowBegin,
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

        drawArrow (thePresentation, aFirstArrowBegin, aFirstArrowDir);
        if (!isOneSideDimension)
        {
          drawArrow (thePresentation, aSecondArrowBegin, aSecondArrowDir);
        }

        if (!isArrowsExternal || isOneSideDimension)
        {
          break;
        }

        // add extension lines for external arrows
        Prs3d_Root::NewGroup (thePresentation);

        drawExtension (thePresentation, anExtensionSize,
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
      drawExtension (thePresentation, anExtensionSize,
                     isArrowsExternal ? aSecondArrowEnd : aSecondArrowBegin,
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

        drawArrow (thePresentation, aSecondArrowBegin, aSecondArrowDir);
        if (!isOneSideDimension)
        {
          drawArrow (thePresentation, aFirstArrowBegin, aFirstArrowDir);
        }

        if (!isArrowsExternal || isOneSideDimension)
        {
          break;
        }

        // add extension lines for external arrows
        Prs3d_Root::NewGroup (thePresentation);

        drawExtension (thePresentation, anExtensionSize,
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
    aPrimSegments->AddVertex (myFirstPoint);
    aPrimSegments->AddVertex (aLineBegPoint);

    aPrimSegments->AddVertex (mySecondPoint);
    aPrimSegments->AddVertex (aLineEndPoint);

    Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();
    Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
  }

  setComputed (Standard_True);
}

//=======================================================================
//function : SetFirstPoint
//purpose  : 
//=======================================================================
void AIS_Dimension::SetFirstPoint (const gp_Pnt& thePoint)
{
  myFirstPoint = thePoint;
}

//=======================================================================
//function : SetSecondPoint
//purpose  : 
//=======================================================================
void AIS_Dimension::SetSecondPoint (const gp_Pnt& thePoint)
{
  mySecondPoint = thePoint;
}

//=======================================================================
//function : Type
//purpose  :
//=======================================================================
AIS_KindOfInteractive AIS_Dimension::Type() const
{
  return AIS_KOI_Relation;
}

//=======================================================================
//function : circleFromPlanarFace
//purpose  : if possible computes circle from planar face
//=======================================================================
Standard_Boolean AIS_Dimension::circleFromPlanarFace (const TopoDS_Face& theFace,
                                                      Handle(Geom_Curve)& theCurve,
                                                      gp_Pnt & theFirstPoint,
                                                      gp_Pnt & theLastPoint)
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
//function : initCircularDimension
//purpose  : if it's possible computes circle from planar face
//=======================================================================
Standard_Boolean AIS_Dimension::initCircularDimension (const TopoDS_Shape& theShape,
                                                       gp_Circ& theCircle,
                                                       gp_Pnt& theMiddleArcPoint,
                                                       gp_Pnt& theOppositeDiameterPoint)
{
  gp_Pln aPln;
  Handle(Geom_Surface) aBasisSurf;
  AIS_KindOfSurface aSurfType = AIS_KOS_OtherSurface;
  gp_Pnt aFirstPoint, aLastPoint;
  Standard_Real anOffset    = 0.0;
  Standard_Real aFirstParam = 0.0;
  Standard_Real aLastParam  = 0.0;
  Standard_Boolean isAnArc  = Standard_False;

  if (theShape.ShapeType() == TopAbs_FACE)
  {
    AIS::GetPlaneFromFace (TopoDS::Face (theShape), aPln, aBasisSurf, aSurfType, anOffset);

    if (aSurfType == AIS_KOS_Plane)
    {
      Handle(Geom_Curve) aCurve;
      if (!circleFromPlanarFace (TopoDS::Face (theShape), aCurve, aFirstPoint, aLastPoint))
      {
        Standard_ConstructionError::Raise ("AIS_Dimension:: Curve is not a circle or is Null") ;
        return Standard_False;
      }

      theCircle = Handle(Geom_Circle)::DownCast (aCurve)->Circ();
      isAnArc = !(aFirstPoint.IsEqual (aLastPoint, Precision::Confusion()));
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
      aSurf1.D0(aMidU, aMidV, aCurPos);
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
        Standard_ConstructionError::Raise ("AIS_Dimension:: Unexpected type of surface") ;
        return Standard_False;
      }
      Handle(Geom_Curve) aCurve;
      aCurve = aBasisSurf->VIso(aMidV);
      if (aCurve->DynamicType() == STANDARD_TYPE (Geom_Circle))
      {
        theCircle = Handle(Geom_Circle)::DownCast (aCurve)->Circ();
      }
      else if (aCurve->DynamicType() == STANDARD_TYPE (Geom_TrimmedCurve))
      {
        Handle(Geom_TrimmedCurve) aTrimmedCurve = Handle(Geom_TrimmedCurve)::DownCast (aCurve);
        aFirstU = aTrimmedCurve->FirstParameter();
        aLastU  = aTrimmedCurve->LastParameter();
        if (aTrimmedCurve->DynamicType() == STANDARD_TYPE (Geom_Circle))
        {
          theCircle = Handle(Geom_Circle)::DownCast(aTrimmedCurve)->Circ();
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

      gp_Vec aVec = gp_Vec (theCircle.Location(), aCurPos).Normalized();
      aFirstPoint = ElCLib::Value (aFirstU, theCircle);
      aLastPoint = ElCLib::Value (aLastU,  theCircle);
    }
  }
  else // TopAbs_EDGE | TopAbs_WIRE
  {
    TopoDS_Edge anEdge;
    if (theShape.ShapeType() == TopAbs_WIRE)
    {
      TopExp_Explorer anIt (theShape, TopAbs_EDGE);
      if (anIt.More())
      {
        anEdge = TopoDS::Edge (anIt.Current());
      }
    }
    else if (theShape.ShapeType() == TopAbs_EDGE)
    {
      anEdge = TopoDS::Edge (theShape);
    }
    else // Unexpected type of shape
    {
      Standard_ConstructionError::Raise ("AIS_Dimension:: Unexpected type of shape");
      return Standard_False;
    }
    BRepAdaptor_Curve anAdaptedCurve (anEdge);
    if (!anAdaptedCurve.GetType() == GeomAbs_Circle)
    {
      return Standard_False;
    }
    theCircle = anAdaptedCurve.Circle();
    aFirstPoint = anAdaptedCurve.Value (anAdaptedCurve.FirstParameter());
    aLastPoint = anAdaptedCurve.Value (anAdaptedCurve.LastParameter());
  }
  // Get <theMiddleArcPoint> and <theOppositeDiameterPoint> values from <theCircle>
  isAnArc = !(aFirstPoint.IsEqual (aLastPoint, Precision::Confusion()));
  gp_Pnt aCenter = theCircle.Location();
  if (!isAnArc)
  {
    // Circle
    gp_Dir anXDir = theCircle.XAxis().Direction();
    theMiddleArcPoint = aCenter.Translated (gp_Vec (anXDir) * theCircle.Radius());
    theOppositeDiameterPoint = aCenter.Translated (-gp_Vec (anXDir) * theCircle.Radius());
  }
  else
  {
    // Arc
    aFirstParam = ElCLib::Parameter (theCircle, aFirstPoint);
    aLastParam  = ElCLib::Parameter (theCircle, aLastPoint);
    if (aFirstParam > aLastParam)
    {
      aFirstParam -= 2.0 * M_PI;
    }
    Standard_Real aParCurPos = (aFirstParam + aLastParam) * 0.5;
    gp_Vec aVec = gp_Vec (aCenter, ElCLib::Value (aParCurPos, theCircle)).Normalized () * theCircle.Radius ();
    theMiddleArcPoint = aCenter.Translated (aVec);
    theOppositeDiameterPoint = aCenter.Translated (-aVec);
  }

  return Standard_True;
}

//=======================================================================
//function : SetDisplaySpecialSymbol
//purpose  : specifies dimension special symbol display options
//=======================================================================
void AIS_Dimension::SetDisplaySpecialSymbol (const AIS_DisplaySpecialSymbol theDisplaySpecSymbol)
{
  myDisplaySpecialSymbol = theDisplaySpecSymbol;
}

//=======================================================================
//function : DisplaySpecialSymbol
//purpose  : shows dimension special symbol display options
//=======================================================================
AIS_DisplaySpecialSymbol AIS_Dimension::DisplaySpecialSymbol() const
{
  return myDisplaySpecialSymbol;
}

//=======================================================================
//function : SetSpecialSymbol
//purpose  : specifies special symbol
//=======================================================================
void AIS_Dimension::SetSpecialSymbol (const Standard_ExtCharacter theSpecialSymbol)
{
  mySpecialSymbol = theSpecialSymbol;
}

//=======================================================================
//function : SpecialSymbol
//purpose  : returns special symbol
//=======================================================================
Standard_ExtCharacter AIS_Dimension::SpecialSymbol() const
{
  return mySpecialSymbol;
}

//=======================================================================
//function : IsUnitsDisplayed
//purpose  : shows if Units are to be displayed along with dimension value
//=======================================================================
Standard_Boolean AIS_Dimension::IsUnitsDisplayed() const
{
  return myToDisplayUnits;
}

//=======================================================================
//function : MakeUnitsDisplayed
//purpose  : sets to display units along with the dimension value or no
//=======================================================================
void AIS_Dimension::MakeUnitsDisplayed (const Standard_Boolean toDisplayUnits)
{
  myToDisplayUnits = toDisplayUnits;
}

//=======================================================================
//function : MakeUnitsDisplayed
//purpose  : returns the current type of units
//=======================================================================
TCollection_AsciiString AIS_Dimension::UnitsQuantity() const
{
  return myUnitsQuantity;
}

//=======================================================================
//function : SetUnitsQuantity
//purpose  : sets the current type of units
//=======================================================================
void AIS_Dimension::SetUnitsQuantity (const TCollection_AsciiString& theUnitsQuantity)
{
  myUnitsQuantity = theUnitsQuantity;
}

//=======================================================================
//function : ModelUnits
//purpose  : returns the current model units
//=======================================================================
TCollection_AsciiString AIS_Dimension::ModelUnits() const
{
  return myModelUnits;
}

//=======================================================================
//function : SetModelUnits
//purpose  : sets the current model units
//=======================================================================
void AIS_Dimension::SetModelUnits (const TCollection_AsciiString& theUnits)
{
  myModelUnits = theUnits;
}

//=======================================================================
//function : DisplayUnits
//purpose  : returns the current display units
//=======================================================================
TCollection_AsciiString AIS_Dimension::DisplayUnits() const
{
  return myDisplayUnits;
}

//=======================================================================
//function : SetDisplayUnits
//purpose  : sets the current display units
//=======================================================================
void AIS_Dimension::SetDisplayUnits (const TCollection_AsciiString& theUnits)
{
  myDisplayUnits = theUnits;
}

//=======================================================================
//function : isComputed
//purpose  :
//=======================================================================
Standard_Boolean AIS_Dimension::isComputed() const
{
  return myIsComputed;
}

//=======================================================================
//function : setComputed
//purpose  :
//=======================================================================
void AIS_Dimension::setComputed (Standard_Boolean isComputed)
{
  myIsComputed = isComputed;
}

//=======================================================================
//function : resetGeom
//purpose  :
//=======================================================================
void AIS_Dimension::resetGeom()
{
  mySelectionGeom.Clear (ComputeMode_All);
}

//=======================================================================
//function : IsTextReversed
//purpose  :
//=======================================================================
Standard_Boolean AIS_Dimension::IsTextReversed() const
{
  return myIsTextReversed;
}

//=======================================================================
//function : MakeTextReversed
//purpose  :
//=======================================================================
void AIS_Dimension::MakeTextReversed (const Standard_Boolean isTextReversed)
{
  myIsTextReversed = isTextReversed;
}

//=======================================================================
//function : SetSelToleranceForText2d
//purpose  :
//=======================================================================
void AIS_Dimension::SetSelToleranceForText2d (const Standard_Real theTol)
{
  mySelToleranceForText2d = theTol;
}

//=======================================================================
//function : SelToleranceForText2d
//purpose  :
//=======================================================================
Standard_Real AIS_Dimension::SelToleranceForText2d() const
{
  return mySelToleranceForText2d;
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
  SetToUpdate();
}

//=======================================================================
//function : computeFlyoutSelection
//purpose  : computes selection for flyouts
//=======================================================================
void AIS_Dimension::computeFlyoutSelection (const Handle(SelectMgr_Selection)& theSelection,
                                            const Handle(SelectMgr_EntityOwner)& theOwner)
{
  //Count flyout direction
  gp_Ax1 aWorkingPlaneNormal = GetWorkingPlane().Axis();
  gp_Dir aTargetPointsVector = gce_MakeDir (myFirstPoint, mySecondPoint);

  // Count a flyout direction vector.
  gp_Dir aFlyoutVector = aWorkingPlaneNormal.Direction() ^ aTargetPointsVector;

  // Create lines for layouts
  gp_Lin aLine1 (myFirstPoint, aFlyoutVector);
  gp_Lin aLine2 (mySecondPoint, aFlyoutVector);

  // Get flyout end points
  gp_Pnt aFlyoutEnd1 = ElCLib::Value (ElCLib::Parameter (aLine1, myFirstPoint) + GetFlyout(), aLine1);
  gp_Pnt aFlyoutEnd2 = ElCLib::Value (ElCLib::Parameter (aLine2, mySecondPoint) + GetFlyout(), aLine2);

  // Fill sensitive entity for flyouts
  Handle(Select3D_SensitiveGroup) aSensitiveEntity = new Select3D_SensitiveGroup (theOwner);
  aSensitiveEntity->Add (new Select3D_SensitiveSegment (theOwner, myFirstPoint, aFlyoutEnd1));
  aSensitiveEntity->Add (new Select3D_SensitiveSegment (theOwner, mySecondPoint, aFlyoutEnd2));
  theSelection->Add (aSensitiveEntity);
}

//=======================================================================
//function : ComputeSelection
//purpose  : 
//=======================================================================
void AIS_Dimension::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                      const Standard_Integer theMode)
{
  if (!isComputed())
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

    Quantity_Length anArrowLength = myDrawer->DimensionAspect()->ArrowAspect()->Length();
    Standard_Real   anArrowAngle  = myDrawer->DimensionAspect()->ArrowAspect()->Angle();

    // sensitives for arrows
    SelectionGeometry::SeqOfArrows::Iterator anArrowIt (mySelectionGeom.Arrows);
    for (; anArrowIt.More(); anArrowIt.Next())
    {
      const SelectionGeometry::HArrow& anArrow = anArrowIt.Value();

      gp_Pnt aSidePnt1 (gp::Origin());
      gp_Pnt aSidePnt2 (gp::Origin());
      const gp_Dir& aPlane = myWorkingPlane.Axis().Direction();
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
                      myWorkingPlane.Axis().Direction(),
                      mySelectionGeom.TextDir);

    if (myDrawer->DimensionAspect()->IsText3d())
    {
      // sensitive planar rectangle for text
      Standard_Real aDx = mySelectionGeom.TextWidth  * 0.5;
      Standard_Real aDy = mySelectionGeom.TextHeight * 0.5;

      gp_Trsf aLabelPlane;
      aLabelPlane.SetTransformation (aTextAxes, gp::XOY());

      TColgp_Array1OfPnt aRectanglePoints (1, 4);
      aRectanglePoints.ChangeValue (1) = gp_Pnt (-aDx, -aDy, 0.0).Transformed (aLabelPlane);
      aRectanglePoints.ChangeValue (2) = gp_Pnt (-aDx,  aDy, 0.0).Transformed (aLabelPlane);
      aRectanglePoints.ChangeValue (3) = gp_Pnt ( aDx,  aDy, 0.0).Transformed (aLabelPlane);
      aRectanglePoints.ChangeValue (4) = gp_Pnt ( aDx, -aDy, 0.0).Transformed (aLabelPlane);

      aTextSensitive = new Select3D_SensitiveFace (aSensitiveOwner, aRectanglePoints);
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
    computeFlyoutSelection (theSelection, aSensitiveOwner);
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
