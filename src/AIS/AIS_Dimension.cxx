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
#include <AIS_DimensionDisplayMode.hxx>
#include <AIS_DimensionOwner.hxx>
#include <AIS_Drawer.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <BRepBndLib.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <ElCLib.hxx>
#include <Font_BRepFont.hxx>
#include <GC_MakeCircle.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Plane.hxx>
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
#include <Select3D_SensitiveBox.hxx>
#include <Select3D_SensitiveCircle.hxx>
#include <Select3D_SensitiveGroup.hxx>
#include <Select3D_SensitiveSegment.hxx>
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
  static const Standard_Utf32Char THE_FILL_CHARACTER ('0');
  static const TCollection_ExtendedString THE_EMPTY_LABEL;
  static const Standard_Real THE_3D_TEXT_MARGIN = 0.1;
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
  return theMode == 0 ? Standard_True : Standard_False;
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
  // Init font instance
  Handle(Font_FTFont) aFont = new Font_FTFont ();
  aFont->Init (aFontName,
               myDrawer->DimensionAspect()->TextAspect()->Aspect()->GetTextFontAspect(),
               (Standard_Integer) myDrawer->DimensionAspect()->TextAspect()->Height(), 72);

  TCollection_ExtendedString aString (theString);
  aString += ".";
  Standard_PCharacter aUtf8String = new Standard_Character[aString.Length()];
  Standard_Integer aStrLength = aString.ToUTF8CString(aUtf8String);
  theWidth = 0.0;
  for (Standard_Integer anIt = 0; anIt < aStrLength - 1; ++anIt)
  {
    Standard_Real anAdvance = aFont->AdvanceX (aUtf8String[anIt], aUtf8String[anIt + 1]);
    theWidth += anAdvance;
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
  Quantity_Length anArrowLength = myDrawer->DimensionAspect()->ArrowAspect()->Length();

  if (myDrawer->DimensionAspect()->IsArrows3d())
  {
    Prs3d_Arrow::Draw (thePresentation,
                       theLocation,
                       theDirection,
                       myDrawer->DimensionAspect()->ArrowAspect()->Angle(),
                       anArrowLength);
  }
  else
  {
    gp_Vec aBackDir (theDirection.Reversed());
    Quantity_Length theCathetusLength = anArrowLength / Cos (M_PI / 9.0);
    Handle(Graphic3d_ArrayOfTriangles) anArrow = new Graphic3d_ArrayOfTriangles(3);
    gp_Pnt aLeftPoint (theLocation.Translated (aBackDir.Rotated (myWorkingPlane.Axis(), M_PI / 9.0) * theCathetusLength));
    gp_Pnt aRightPoint (theLocation.Translated (aBackDir.Rotated (myWorkingPlane.Axis(), M_PI * 17.0 / 9.0) * theCathetusLength));

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
}

//=======================================================================
//function : drawText
//purpose  : 
//=======================================================================
Standard_Real AIS_Dimension::drawText (const Handle(Prs3d_Presentation)& thePresentation,
                                       const gp_Dir& theTextDir,
                                       const TCollection_ExtendedString theText,
                                       const AIS_DimensionDisplayMode theMode,
                                       const Standard_Integer theLabelPosition)
{
  Standard_Real aTextWidth (0.0), aTextHeight (0.0);
  if (theMode == AIS_DDM_Line)
  {
    return 0.0;
  }

  if (myDrawer->DimensionAspect()->IsText3d())
  {
    // Getting font parameters
    Quantity_Color aColor;
    Standard_CString aFontName;
    Standard_Real anExpansionFactor;
    Standard_Real aSpace;
    myDrawer->DimensionAspect()->TextAspect()->Aspect()->Values (aColor, aFontName, anExpansionFactor, aSpace);
    Font_FontAspect aFontAspect = myDrawer->DimensionAspect()->TextAspect()->Aspect()->GetTextFontAspect();
    Standard_Real aHeight = myDrawer->DimensionAspect()->TextAspect()->Height();

    // Creating TopoDS_Shape for text
    Font_BRepFont aFont (aFontName, aFontAspect, aHeight);
    NCollection_String aText = (Standard_Utf16Char* )theText.ToExtString();
    TopoDS_Shape aTextShape = aFont.RenderText (aText);

    // Formating text position in XOY plane
    Bnd_Box aTextBndBox;
    BRepBndLib::AddClose (aTextShape, aTextBndBox);
    Standard_Real aXMin, anYMin, aZMin, aXMax, anYMax, aZMax;
    aTextBndBox.Get (aXMin, anYMin, aZMin, aXMax, anYMax, aZMax);
    aTextWidth  = aXMax  - aXMin;
    aTextHeight = anYMax - anYMin;

    Standard_Integer aHLabelPos = theLabelPosition & LabelPosition_HMask;
    Standard_Integer aVLabelPos = theLabelPosition & LabelPosition_VMask;

    gp_Dir aTextDir (aHLabelPos == LabelPosition_Left ? -theTextDir : theTextDir);

    // compute label offsets
    Standard_Real aMarginSize    = aHeight * THE_3D_TEXT_MARGIN;
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
    Standard_Real aShapeHOffset = aCenterHOffset -  aTextWidth / 2.0;
    Standard_Real aShapeVOffset = aCenterVOffset - aTextHeight / 2.0;

    gp_Trsf anOffsetTrsf;
    anOffsetTrsf.SetTranslation (gp::Origin(), gp_Pnt (aShapeHOffset, aShapeVOffset, 0.0));
    aTextShape.Move (anOffsetTrsf);

    // Transform text to myWorkingPlane coordinate system
    gp_Ax3 aTextCoordSystem (myGeom.myTextPosition, myWorkingPlane.Axis().Direction(), aTextDir);
    gp_Trsf aTextPlaneTrsf;
    aTextPlaneTrsf.SetTransformation (aTextCoordSystem, gp_Ax3 (gp::XOY()));
    aTextShape.Move (aTextPlaneTrsf);

    // Set display parameters for advanced selection
    BRepBndLib::AddClose (aTextShape, myGeom.myTextBndBox);

    // Set text flipping anchors
    gp_Trsf aCenterOffsetTrsf;
    gp_Pnt aCenterOffset (aCenterHOffset, aCenterVOffset, 0.0);
    aCenterOffsetTrsf.SetTranslation (gp::Origin(), aCenterOffset);

    gp_Pnt aCenterOfFlip (gp::Origin());
    aCenterOfFlip.Transform (aCenterOffsetTrsf);
    aCenterOfFlip.Transform (aTextPlaneTrsf);

    gp_Ax2 aFlippingAxes (aCenterOfFlip, myWorkingPlane.Axis().Direction(), aTextDir);
    Prs3d_Root::CurrentGroup (thePresentation)->SetFlippingOptions (Standard_True, aFlippingAxes);

    // Draw text
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

      // Drawing text
      StdPrs_ShadedShape::Add (thePresentation, aTextShape, myDrawer);
    }
    else
    {
      // Setting color for text
      myDrawer->FreeBoundaryAspect()->Aspect()->SetColor (aColor);
      // Drawing text
      StdPrs_WFShape::Add (thePresentation, aTextShape, myDrawer);
    }
    Prs3d_Root::CurrentGroup (thePresentation)->SetFlippingOptions (Standard_False, gp_Ax2());

    return aTextWidth + aMarginSize * 2.0;
  }

  myDrawer->DimensionAspect()->TextAspect()->Aspect()->SetDisplayType (Aspect_TODT_DIMENSION);

  Prs3d_Text::Draw (thePresentation,
                    myDrawer->DimensionAspect()->TextAspect(),
                    theText,
                    myGeom.myTextPosition);

  // For 2d text we don not create new group for lines and draw them in the same group with text
  // for the proper handling of stencil test buffer.
  return 0.0;
}

//=======================================================================
//function : drawExtension
//purpose  : 
//=======================================================================
void AIS_Dimension::drawExtension (const Handle(Prs3d_Presentation)& thePresentation,
                                   const Standard_Real theExtensionSize,
                                   const gp_Pnt& theExtensionStart,
                                   const gp_Dir& theExtensionDir,
                                   const TCollection_ExtendedString& theValueString,
                                   const AIS_DimensionDisplayMode theMode,
                                   const Standard_Integer theLabelPosition)
{
  Standard_Real aTextWidth = 0.0;

  Standard_Boolean isLabel = theValueString.Length() > 0;
  if (isLabel)
  {
    // compute text primitives; get its model width
    myGeom.myTextPosition = theExtensionStart.Translated (
      gp_Vec (theExtensionDir).Scaled (theExtensionSize));

    aTextWidth = drawText (thePresentation,
                           myIsTextReversed ? -theExtensionDir : theExtensionDir,
                           theValueString,
                           theMode,
                           theLabelPosition);
  }

  if (theMode == AIS_DDM_Text)
  {
    return;
  }

  // compute graphical primitives and sensitives for extension line
  gp_Pnt anExtStart = theExtensionStart;
  gp_Pnt anExtEnd   = !isLabel || (theLabelPosition & LabelPosition_VCenter) != 0
    ? theExtensionStart.Translated (gp_Vec (theExtensionDir) * theExtensionSize)
    : theExtensionStart.Translated (gp_Vec (theExtensionDir) * (theExtensionSize + aTextWidth));

  Handle(Graphic3d_ArrayOfSegments) anExtPrimitive = new Graphic3d_ArrayOfSegments (2);
  anExtPrimitive->AddVertex (anExtStart);
  anExtPrimitive->AddVertex (anExtEnd);

  Handle(SelectMgr_EntityOwner) aDummyOwner;

  myGeom.mySensitiveSegments.Append (new Select3D_SensitiveSegment (aDummyOwner, anExtStart, anExtEnd));

  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == AIS_DDM_All)
  {
    Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_True);
  }
  Handle(Graphic3d_AspectLine3d) aDimensionLineStyle = myDrawer->DimensionAspect()->LineAspect()->Aspect();
  Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionLineStyle);
  Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (anExtPrimitive);
  if (!myDrawer->DimensionAspect()->IsText3d() && theMode == AIS_DDM_All)
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
                                         const AIS_DimensionDisplayMode theMode,
                                         const Standard_Boolean isOneSideDimension/* = Standard_False*/)
{
  // Don't build any dimension for equal points
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
  myGeom.mySensitiveSegments.Clear();

  gp_Lin aDimensionLine = gce_MakeLin (aLineBegPoint, aLineEndPoint);

  // For extensions we need to know arrow size, text size and extension size: get it from aspect
  Quantity_Length anArrowLength   = aDimensionAspect->ArrowAspect()->Length();
  Standard_Real   anExtensionSize = aDimensionAspect->ExtensionSize();

  if (!myIsValueCustom)
  {
   computeValue();
  }

  TCollection_ExtendedString aValueString;
  Standard_Real aTextLength;
  getTextWidthAndString (aTextLength, aValueString);

  // Handle user-defined and automatic arrow placement
  bool isArrowsExternal = false;
  switch (aDimensionAspect->ArrowOrientation())
  {
    case Prs3d_DAO_External: isArrowsExternal = true; break;
    case Prs3d_DAO_Internal: isArrowsExternal = false; break;
    case Prs3d_DAO_Fit:
    {
      Standard_Real aDimensionWidth = aLineBegPoint.Distance (aLineEndPoint);
      Standard_Real anArrowsWidth   = isOneSideDimension ? anArrowLength : 2.0 * anArrowLength;
      isArrowsExternal = aDimensionWidth < aTextLength + anArrowsWidth;
      break;
    }
  }

  // Arrows positions and directions
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

  // Handle user-defined and automatic text placement
  switch (aDimensionAspect->TextHorizontalPosition())
  {
    case Prs3d_DTHP_Left  : aLabelPosition |= LabelPosition_Left; break;
    case Prs3d_DTHP_Right : aLabelPosition |= LabelPosition_Right; break;
    case Prs3d_DTHP_Center: aLabelPosition |= LabelPosition_HCenter; break;
    case Prs3d_DTHP_Fit:
    {
      Standard_Real aDimensionWidth = aLineBegPoint.Distance (aLineEndPoint);
      Standard_Real anArrowsWidth   = isOneSideDimension ? anArrowLength : 2.0 * anArrowLength;
      Standard_Real aContentWidth   = isArrowsExternal ? aTextLength : aTextLength + anArrowsWidth;

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

  switch (aLabelPosition & LabelPosition_HMask)
  {
    // ------------------------------------------------------------------------ //
    //                                CENTER                                    //
    // -------------------------------------------------------------------------//

    case LabelPosition_HCenter:
    {
      // add label on dimension or extension line to presentation
      Prs3d_Root::NewGroup (thePresentation);

      gp_Vec aTextDir (aLineBegPoint, aLineEndPoint);

      myGeom.myTextPosition = gp_XYZ (aLineBegPoint.XYZ() + aLineEndPoint.XYZ()) * 0.5;

      Standard_Real aTextWidth = drawText (thePresentation,
                                           myIsTextReversed ? -aTextDir : aTextDir,
                                           aValueString,
                                           theMode,
                                           aLabelPosition);

      if (theMode == AIS_DDM_Text)
      {
        break;
      }

      Standard_Real aLabelMargin = 
        aDimensionAspect->IsText3d() ? aDimensionAspect->TextAspect()->Height() * THE_3D_TEXT_MARGIN : 0.0;

      Standard_Boolean isLineBreak = aDimensionAspect->TextVerticalPosition() == Prs3d_DTVP_Center
                                  && aDimensionAspect->IsText3d();

      Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments (isLineBreak ? 4 : 2);

      // compute dimension continuous or sectioned dimension line
      if (isLineBreak)
      {
        Standard_Real aPTextPosition = ElCLib::Parameter (aDimensionLine, myGeom.myTextPosition);
        gp_Pnt aSection1Beg = aCenterLineBegin;
        gp_Pnt aSection1End = ElCLib::Value (aPTextPosition - aLabelMargin - (aTextWidth * 0.5), aDimensionLine);
        gp_Pnt aSection2Beg = ElCLib::Value (aPTextPosition + aLabelMargin + (aTextWidth * 0.5), aDimensionLine);
        gp_Pnt aSection2End = aCenterLineEnd;

        aPrimSegments->AddVertex (aSection1Beg);
        aPrimSegments->AddVertex (aSection1End);
        aPrimSegments->AddVertex (aSection2Beg);
        aPrimSegments->AddVertex (aSection2End);

        myGeom.mySensitiveSegments.Append (new Select3D_SensitiveSegment (anEmptyOwner, aSection1Beg, aSection1End));
        myGeom.mySensitiveSegments.Append (new Select3D_SensitiveSegment (anEmptyOwner, aSection2Beg, aSection2End));
      }
      else
      {
        aPrimSegments->AddVertex (aCenterLineBegin);
        aPrimSegments->AddVertex (aCenterLineEnd);

        myGeom.mySensitiveSegments.Append (
          new Select3D_SensitiveSegment (anEmptyOwner, aCenterLineBegin, aCenterLineEnd));
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

      // Main dimension line, short extension
      if (!aDimensionAspect->IsText3d() && theMode == AIS_DDM_All)
      {
        Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_True);
      }
      Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());
      Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
      if (!aDimensionAspect->IsText3d() && theMode == AIS_DDM_All)
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
                     THE_EMPTY_LABEL, theMode, LabelPosition_None);
      if (!isOneSideDimension)
      {
        drawExtension (thePresentation, anExtensionSize,
                       aSecondArrowEnd, aSecondExtensionDir,
                       THE_EMPTY_LABEL, theMode, LabelPosition_None);
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
                     isArrowsExternal ? aFirstArrowEnd : aLineBegPoint,
                     aFirstExtensionDir,
                     aValueString,
                     theMode,
                     aLabelPosition);

      if (theMode == AIS_DDM_Text)
      {
        break;
      }

      // add central dimension line
      Prs3d_Root::NewGroup (thePresentation);

      Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments (2);
      aPrimSegments->AddVertex (aCenterLineBegin);
      aPrimSegments->AddVertex (aCenterLineEnd);
      Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());
      Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);

      myGeom.mySensitiveSegments.Append (
        new Select3D_SensitiveSegment (anEmptyOwner, aCenterLineBegin, aCenterLineEnd));

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
                     THE_EMPTY_LABEL, theMode, LabelPosition_None);

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
                     aValueString,
                     theMode,
                     aLabelPosition);

      if (theMode == AIS_DDM_Text)
      {
        break;
      }

      // add central dimension line
      Prs3d_Root::NewGroup (thePresentation);

      Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments (2);
      aPrimSegments->AddVertex (aCenterLineBegin);
      aPrimSegments->AddVertex (aCenterLineEnd);
      Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());
      Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);

      myGeom.mySensitiveSegments.Append (
        new Select3D_SensitiveSegment (anEmptyOwner, aCenterLineBegin, aCenterLineEnd));

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
                     THE_EMPTY_LABEL, theMode, LabelPosition_None);
      break;
    }
  }

  // add flyout lines to presentation
  if (theMode == AIS_DDM_All)
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
  return myGeom.myIsComputed;
}

//=======================================================================
//function : setComputed
//purpose  :
//=======================================================================
void AIS_Dimension::setComputed (Standard_Boolean isComputed)
{
  myGeom.myIsComputed = isComputed;
}

//=======================================================================
//function : textPosition
//purpose  :
//=======================================================================
gp_Pnt AIS_Dimension::textPosition() const
{
  return myGeom.myTextPosition;
}

//=======================================================================
//function : setTextPosition
//purpose  :
//=======================================================================
void AIS_Dimension::setTextPosition (const gp_Pnt thePosition)
{
  myGeom.myTextPosition = thePosition;
}

//=======================================================================
//function : resetGeom
//purpose  :
//=======================================================================
void AIS_Dimension::resetGeom()
{
  setComputed (Standard_False);
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
  myGeom.mySelToleranceForText2d = theTol;
}

//=======================================================================
//function : SelToleranceForText2d
//purpose  :
//=======================================================================
Standard_Real AIS_Dimension::SelToleranceForText2d() const
{
  return myGeom.mySelToleranceForText2d;
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
                                            const Handle(AIS_DimensionOwner)& theOwner)
{
  //Count flyout direction
  gp_Ax1 aWorkingPlaneNormal = GetWorkingPlane().Axis();
  gp_Dir aTargetPointsVector = gce_MakeDir (myFirstPoint, mySecondPoint);

  // Count a flyout direction vector.
  gp_Dir aFlyoutVector = aWorkingPlaneNormal.Direction()^aTargetPointsVector;

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

  Handle(Select3D_SensitiveGroup) aSensitiveForLine;
  Handle(Select3D_SensitiveEntity) aSensitiveForText;
  Select3D_ListOfSensitive aSensitiveList;
  aSensitiveList.Assign (myGeom.mySensitiveSegments);

  // Full dimension selection
  Handle(AIS_DimensionOwner) anOwner = new AIS_DimensionOwner (this, AIS_DDM_All, theMode == 0 ? 5 : 6);
  for (Select3D_ListIteratorOfListOfSensitive anIt (aSensitiveList); anIt.More(); anIt.Next())
  {
    anIt.Value()->Set (anOwner);
  }
  aSensitiveForLine = new Select3D_SensitiveGroup (anOwner, aSensitiveList);

  // Text
  if (myDrawer->DimensionAspect()->IsText3d())
  {
    aSensitiveForText = new Select3D_SensitiveBox (anOwner,myGeom.myTextBndBox);
  }
  else
  {
    Handle(Geom_Circle) aSensitiveGeom = new Geom_Circle (gp_Circ (gp_Ax2 (myGeom.myTextPosition,
                                                            myWorkingPlane.Position().XDirection()),
                                                            myGeom.mySelToleranceForText2d != 0
                                                          ? myGeom.mySelToleranceForText2d : 1.0));
    aSensitiveForText = new Select3D_SensitiveCircle (anOwner, aSensitiveGeom, Standard_True);
  }
  if (theMode > 0)
  {
    anOwner->SetDisplayMode (AIS_DDM_Line);
    Handle(AIS_DimensionOwner) aTextOwner = new AIS_DimensionOwner (this, AIS_DDM_Text, 7);
    aSensitiveForText->Set (aTextOwner);
  }
  else
  {
    computeFlyoutSelection (theSelection, anOwner);
  }

  theSelection->Add (aSensitiveForLine);
  theSelection->Add (aSensitiveForText);
}
