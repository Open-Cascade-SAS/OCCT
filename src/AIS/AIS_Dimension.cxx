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

#include <AIS.hxx>
#include <AIS_Dimension.hxx>
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

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_Dimension::AIS_Dimension (const Standard_Real theExtensionSize /*= 1.0*/)
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
  myTextOffset (DimensionAspect()->ArrowAspect()->Length()),
  myIsInitialized (Standard_False),
  myKindOfDimension (AIS_KOD_NONE),
  myExtensionSize (theExtensionSize)
{
  ResetWorkingPlane();
  // Units default settings
  UnitsAPI::SetLocalSystem (UnitsAPI_SI);
  myModelUnits = Units::DictionaryOfUnits()->ActiveUnit (myUnitsQuantity.ToCString());
  myDisplayUnits = Units::DictionaryOfUnits()->ActiveUnit (myUnitsQuantity.ToCString());
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

AIS_Dimension::AIS_Dimension (const Handle(Prs3d_DimensionAspect)& theAspect,
                              const Standard_Real theExtensionSize /*= 1.0*/)
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
  myTextOffset (DimensionAspect()->ArrowAspect()->Length()),
  myIsInitialized (Standard_False),
  myKindOfDimension (AIS_KOD_NONE),
  myExtensionSize (theExtensionSize)
{
  ResetWorkingPlane();
  // Units default settings
  UnitsAPI::SetLocalSystem (UnitsAPI_SI);
  myModelUnits = Units::DictionaryOfUnits()->ActiveUnit (myUnitsQuantity.ToCString());
  myDisplayUnits = Units::DictionaryOfUnits()->ActiveUnit (myUnitsQuantity.ToCString());
  SetDimensionAspect (theAspect);
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
//function : SetExtensionSize
//purpose  : 
//=======================================================================

void AIS_Dimension::SetExtensionSize (const Standard_Real theExtensionSize)
{
  myExtensionSize = theExtensionSize;
}
   
//=======================================================================
//function : GetExtensionSize
//purpose  : 
//=======================================================================

Standard_Real AIS_Dimension::GetExtensionSize() const
{
  return myExtensionSize;
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

  // Get font length
  // Get expansion ratio for getting a width of symbols
  Quantity_Color aColor; 
  Standard_CString aFont;
  Standard_Real aFactor;
  Standard_Real aSpace;
  myDrawer->DimensionAspect()->TextAspect()->Aspect()->Values (aColor, aFont, aFactor, aSpace);
  theWidth = (myDrawer->DimensionAspect()->TextAspect()->Height() / aFactor) * theString.Length();
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
                       theDirection.Reversed(),
                       myDrawer->DimensionAspect()->ArrowAspect()->Angle(),
                       anArrowLength);
  }
  else
  {
    gp_Vec anArrowDir (theDirection);
    Quantity_Length theCathetusLength = anArrowLength / Cos (M_PI / 9.0);
    Handle(Graphic3d_ArrayOfTriangles) anArrow = new Graphic3d_ArrayOfTriangles(3);
    gp_Pnt aLeftPoint (theLocation.Translated (anArrowDir.Rotated (myWorkingPlane.Axis(), M_PI / 9.0) * theCathetusLength));
    gp_Pnt aRightPoint (theLocation.Translated (anArrowDir.Rotated (myWorkingPlane.Axis(), M_PI * 17.0 / 9.0) * theCathetusLength));

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
    myDrawer->ShadingAspect()->Aspect()->SetInteriorColor (aColor);
    myDrawer->ShadingAspect()->Aspect()->SetBackInteriorColor (aColor);
    myDrawer->ShadingAspect()->SetMaterial (aShadeMat);
    Prs3d_Root::CurrentGroup(thePresentation)->SetGroupPrimitivesAspect (myDrawer->ShadingAspect()->Aspect());
    Prs3d_Root::CurrentGroup(thePresentation)->AddPrimitiveArray (anArrow);
  }
}

//=======================================================================
//function : drawText
//purpose  : 
//=======================================================================

Standard_Real AIS_Dimension::drawText (const Handle(Prs3d_Presentation)& thePresentation,
                                       const gp_Dir& theTextDir,
                                       const TCollection_ExtendedString theText,
                                       const AIS_DimensionDisplayMode theMode)
{
  Standard_Real aTextWidth (0.0), aTextHeight (0.0);
  if (theMode == AIS_DDM_Line)
    return aTextWidth;
  // Creating new group for text
  Prs3d_Root::NewGroup (thePresentation);

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
    gp_Dir aTextDir (theTextDir);
    Standard_Real aHorizontalOffset (0.0), aVerticalOffset (0.0);
    switch (myDrawer->DimensionAspect()->HorizontalTextAlignment())
    {
      case Prs3d_HTA_Left:
        aTextDir.Reverse();
        aHorizontalOffset = -aTextWidth;
        break;
      case Prs3d_HTA_Center:
        aHorizontalOffset = -(aTextWidth / 2.0);
        break;
      case Prs3d_HTA_Right:
        aHorizontalOffset = 0.0;
        break;
    }
    switch (myDrawer->DimensionAspect()->VerticalTextAlignment())
    {
      case Prs3d_VTA_Top:
        aVerticalOffset = 0.0;
        break;
      case Prs3d_VTA_Center:
        aVerticalOffset = -(aTextHeight / 2.0);
        break;
      case Prs3d_VTA_Bottom:
        aVerticalOffset = -aTextHeight;
        break;
    }
    gp_Trsf aTrsf;
    aTrsf.SetTranslation (gp_Pnt (), gp_Pnt (aHorizontalOffset, aVerticalOffset, 0.0));
    aTextShape.Move (aTrsf);

    // Transform text to myWorkingPlane coordinate system
    gp_Ax3 aPenAx3 (myGeom.myTextPosition, myWorkingPlane.Axis().Direction(), aTextDir);
    aTrsf.SetTransformation (aPenAx3, gp_Ax3 (gp::XOY()));
    aTextShape.Move (aTrsf);

    // Set display parameters for advanced selection
    BRepBndLib::AddClose (aTextShape, myGeom.myTextBndBox);
    // Drawing text
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
    // Creating new group for lines
    Prs3d_Root::NewGroup (thePresentation);
  }
  else
  {
    myDrawer->DimensionAspect()->TextAspect()->Aspect()->SetDisplayType (Aspect_TODT_DIMENSION);
    Prs3d_Text::Draw (thePresentation,
                      myDrawer->DimensionAspect()->TextAspect(),
                      theText,
                      myGeom.myTextPosition);

    // For 2d text we don not create new group for lines and draw them in the same group with text
    // for the proper handling of stencil test buffer.
  }

  return aTextWidth;
}

  //=======================================================================
//function : drawExtensionWithText
//purpose  : 
//=======================================================================

void AIS_Dimension::drawExtensionWithText (const Handle(Prs3d_Presentation)& thePresentation,
                                           const gp_Pnt& theStartPoint,
                                           const gp_Lin& theDimensionLine,
                                           const TCollection_ExtendedString& theValueString,
                                           const AIS_DimensionDisplayMode theMode)
{
  Handle(SelectMgr_EntityOwner) anEmptyOwner;
  Standard_Boolean isGapInCenter = (myDrawer->DimensionAspect()->VerticalTextAlignment() == Prs3d_VTA_Center
                                    && myDrawer->DimensionAspect()->IsText3d());

  Handle(Graphic3d_ArrayOfSegments) aPrimSegments = new Graphic3d_ArrayOfSegments (isGapInCenter ? 4 : 2);

  gp_Dir anAttachPointsVector = myWorkingPlane.Axis().Direction() ^ gce_MakeDir (myFirstPoint, mySecondPoint);
  Standard_Real aGap = 1.;
  Standard_Real aStartParam = ElCLib::Parameter (theDimensionLine, theStartPoint);

  // Text
  Standard_Real aTextParam = isGapInCenter ? aStartParam + myTextOffset + aGap : aStartParam + myTextOffset;
  myGeom.myTextPosition = ElCLib::Value (aTextParam, theDimensionLine);
  Standard_Real aTextWidth = drawText (thePresentation,
                                       myIsTextReversed ? theDimensionLine.Direction().Reversed()
                                                        : theDimensionLine.Direction(),
                                       theValueString,
                                       theMode);
  gp_Pnt aFirstPoint, aLastPoint;
  aFirstPoint = theStartPoint;
  Standard_Real aParam = isGapInCenter ? aTextParam + aTextWidth + aGap : aTextParam + aTextWidth;

  // If text separates dimension line into two parts (4 points)
  if (isGapInCenter)
  {
    aLastPoint = ElCLib::Value (aStartParam + myTextOffset, theDimensionLine);
    aPrimSegments->AddVertex (aFirstPoint);
    aPrimSegments->AddVertex (aLastPoint);
    myGeom.mySensitiveSegments.Append (new Select3D_SensitiveSegment (anEmptyOwner, aFirstPoint, aLastPoint));
    aFirstPoint = ElCLib::Value (aParam, theDimensionLine);
  }

  // Draw additional line segment only after 3D text
  if (myDrawer->DimensionAspect()->IsText3d())
  {
    aParam += myTextOffset;
  }

  aLastPoint = ElCLib::Value (aParam, theDimensionLine);
  aPrimSegments->AddVertex (aFirstPoint);
  aPrimSegments->AddVertex (aLastPoint);
  myGeom.mySensitiveSegments.Append (new Select3D_SensitiveSegment (anEmptyOwner, aFirstPoint, aLastPoint));

  // Extension line in the same group
  if (theMode != AIS_DDM_Text)
  {
    if (!myDrawer->DimensionAspect()->IsText3d() && theMode == AIS_DDM_All)
    {
      Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_True);
    }
    Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (myDrawer->DimensionAspect()->LineAspect()->Aspect());
    Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
    if (!myDrawer->DimensionAspect()->IsText3d() && theMode == AIS_DDM_All)
    {
      Prs3d_Root::CurrentGroup (thePresentation)->SetStencilTestOptions (Standard_False);
    }
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
//function : SetTextOffset
//purpose  : 
//=======================================================================

void AIS_Dimension::SetTextOffset (const Standard_Real theOffset)
{
  myTextOffset = theOffset;
}

//=======================================================================
//function : TextOffset
//purpose  : 
//=======================================================================

Standard_Real AIS_Dimension::TextOffset() const
{
  return myTextOffset;
}

//=======================================================================
//function : drawLinearDimension
//purpose  : 
//=======================================================================

void AIS_Dimension::drawLinearDimension (const Handle(Prs3d_Presentation)& thePresentation,
                                         const gp_Pnt& theFirstAttach,
                                         const gp_Pnt& theSecondAttach,
                                         const AIS_DimensionDisplayMode theMode,
                                         const Standard_Boolean isOneSideDimension/* = Standard_False*/)
{
  // Don't build any dimension for equal points
  if (myFirstPoint.IsEqual (mySecondPoint, Precision::Confusion()))
  {
    setComputed (Standard_False);
    return;
  }
  Handle(Prs3d_DimensionAspect) aDimensionAspect = myDrawer->DimensionAspect();
  Handle(SelectMgr_EntityOwner) anEmptyOwner;
  myGeom.mySensitiveSegments.Clear();

  gp_Dir aAttachPointsVector = GetWorkingPlane().Axis().Direction()^gce_MakeDir (myFirstPoint, mySecondPoint);
  // Get line of the dimension
  gp_Lin aDimensionLine = gce_MakeLin (theFirstAttach, theSecondAttach);

  // Get parameters on dimension line of two layout points
  Standard_Real aParam1 = ElCLib::Parameter (aDimensionLine, theFirstAttach);
  Standard_Real aParam2 = ElCLib::Parameter (aDimensionLine, theSecondAttach);

  // For extensions we need to know arrow size and text size, get it from aspect
  Quantity_Length anArrowLength = aDimensionAspect->ArrowAspect()->Length();
  // Set line parameters
  Standard_Real aGap = 0.; // gap between line and text if AIS_VTA_Center
  if (!myIsValueCustom)
  {
   computeValue();
  }

  TCollection_ExtendedString aValueString;
  Standard_Real aTextLength;
  getTextWidthAndString (aTextLength, aValueString);

  // Automatical text and arrow placement
  Standard_Real aValue = myFirstPoint.Distance (mySecondPoint);
  if (aDimensionAspect->HorizontalTextAlignment() == Prs3d_HTA_Center)
  {
    aDimensionAspect->SetArrowOrientation (Prs3d_DAO_Internal);
    if (aValue < aTextLength + (isOneSideDimension ? anArrowLength : 2.0 * anArrowLength))
    {
      aDimensionAspect->SetArrowOrientation (Prs3d_DAO_External);
      aDimensionAspect->SetHorizontalTextAlignment (Prs3d_HTA_Left);
    }
  }
  else
  {
    aDimensionAspect->SetArrowOrientation (Prs3d_DAO_External);
  }

  // Arrows positions and directions
  gp_Pnt aFirstArrowPosition = ElCLib::Value (aParam1, aDimensionLine);
  gp_Pnt aSecondArrowPosition = ElCLib::Value (aParam2, aDimensionLine);
  gp_Dir aFirstArrowDir = aDimensionLine.Direction();
  gp_Dir aSecondArrowDir = aDimensionLine.Direction().Reversed();
  Standard_Real aFirstArrowBegin, aFirstArrowEnd, aSecondArrowBegin, aSecondArrowEnd;

  if (aDimensionAspect->GetArrowOrientation() == Prs3d_DAO_External)
  {
    aFirstArrowDir.Reverse();
    aSecondArrowDir.Reverse();

    aFirstArrowBegin  = aParam1 - anArrowLength;
    aFirstArrowEnd    = aParam1;
    aSecondArrowBegin = aParam2;
    aSecondArrowEnd   = aParam2 + anArrowLength;
  }
  else
  {
    aFirstArrowBegin  = aParam1;
    aFirstArrowEnd    = aParam1 + anArrowLength;
    aSecondArrowBegin = aParam2 - anArrowLength;
    aSecondArrowEnd   = aParam2;
  }

  Handle(Graphic3d_ArrayOfSegments) aPrimSegments;
  gp_Pnt aFirstPoint, aLastPoint;
  // Take into account vertical text alignment:
  // only for 3D text! subtract the text length if it is in the center.
  Standard_Boolean isGapInCenter = (aDimensionAspect->VerticalTextAlignment() == Prs3d_VTA_Center
                                     && aDimensionAspect->IsText3d());
  if (isGapInCenter)
  {
    aGap = 1.0;
  }

  switch (aDimensionAspect->HorizontalTextAlignment())
  {
    // Default case - text is to be in the center of length dimension line
    case Prs3d_HTA_Center:
    {
      // Group1: arrows
      if (theMode != AIS_DDM_Text)
      {
        drawArrow (thePresentation, aFirstArrowPosition, aFirstArrowDir);
        if (!isOneSideDimension)
        {
          drawArrow (thePresentation, aSecondArrowPosition, aSecondArrowDir);
        }
      }

      // Group 2: Text and dimension line
      aPrimSegments = new Graphic3d_ArrayOfSegments (isGapInCenter ? 4 : 2);
      myGeom.myTextPosition = ElCLib::Value ((aParam1 + aParam2) / 2.0, aDimensionLine);

      gp_Vec aTextDir (myFirstPoint, mySecondPoint);
      Standard_Real aTextWidth = drawText (thePresentation,
                                           myIsTextReversed ? aTextDir.Reversed() : aTextDir,
                                           aValueString,
                                           theMode);

      aFirstPoint = ElCLib::Value (aFirstArrowEnd, aDimensionLine);
      if (isGapInCenter)
      {
        aLastPoint = ElCLib::Value (ElCLib::Parameter (aDimensionLine,myGeom.myTextPosition) - aGap - (aTextWidth / 2.0), aDimensionLine);
        aPrimSegments->AddVertex (aFirstPoint);
        aPrimSegments->AddVertex (aLastPoint);
        myGeom.mySensitiveSegments.Append (new Select3D_SensitiveSegment (anEmptyOwner,aFirstPoint,aLastPoint));
        aFirstPoint = ElCLib::Value (ElCLib::Parameter(aDimensionLine,myGeom.myTextPosition) + (aTextWidth / 2.0) + aGap, aDimensionLine);
      }
      else if (aDimensionAspect->VerticalTextAlignment() == Prs3d_VTA_Top)
      {
        aDimensionAspect->TextAspect()->SetVerticalJustification (Graphic3d_VTA_BOTTOM);
      }
      else if (aDimensionAspect->VerticalTextAlignment() == Prs3d_VTA_Bottom)
      {
        aDimensionAspect->TextAspect()->SetVerticalJustification(Graphic3d_VTA_TOP);
      }

      aLastPoint = isOneSideDimension ? theSecondAttach : ElCLib::Value (aSecondArrowBegin, aDimensionLine);

      aPrimSegments->AddVertex (aFirstPoint);
      aPrimSegments->AddVertex (aLastPoint);
      myGeom.mySensitiveSegments.Append (new Select3D_SensitiveSegment (anEmptyOwner, aFirstPoint, aLastPoint));

      // Main dimension line, short extension
      if (theMode != AIS_DDM_Text)
      {
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
      }
      break;
    }
    // Text is disposed from the left side of length dimension (after the left flyout line)
    // Needs to create extensions: left for text and right for proper view of dimensions.
    case Prs3d_HTA_Left:
    {
      aPrimSegments = new Graphic3d_ArrayOfSegments (4);

      gp_Pnt aFirstArrowBeginPnt = ElCLib::Value (aFirstArrowBegin, aDimensionLine);
      gp_Lin aLongExtLine (aDimensionLine.Location(), aDimensionLine.Direction().Reversed());
      gp_Pnt aStartPoint = ElCLib::Value (aFirstArrowBegin, aDimensionLine);
      // Left extension with the text
      drawExtensionWithText (thePresentation, aStartPoint, aLongExtLine, aValueString, theMode);

      // Central(main) dimension line
      aFirstPoint = ElCLib::Value (aFirstArrowEnd, aDimensionLine);
      aLastPoint = isOneSideDimension ? theSecondAttach : ElCLib::Value (aSecondArrowBegin, aDimensionLine);
      aPrimSegments->AddVertex (aFirstPoint);
      aPrimSegments->AddVertex (aLastPoint);
      myGeom.mySensitiveSegments.Append (new Select3D_SensitiveSegment (anEmptyOwner, aFirstPoint, aLastPoint));

      // Right extension
      if (!isOneSideDimension)
      {
        aFirstPoint = ElCLib::Value (aSecondArrowEnd, aDimensionLine);
        aLastPoint = ElCLib::Value (aSecondArrowEnd + anArrowLength, aDimensionLine);
        aPrimSegments->AddVertex (aFirstPoint);
        aPrimSegments->AddVertex (aLastPoint);
        myGeom.mySensitiveSegments.Append(new Select3D_SensitiveSegment (anEmptyOwner, aFirstPoint, aLastPoint));
      }
      if (theMode != AIS_DDM_Text)
      {
        // Main dimension line, short extension
        Prs3d_Root::NewGroup (thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());
        Prs3d_Root::CurrentGroup (thePresentation)->AddPrimitiveArray (aPrimSegments);
        // Group1: Add arrows to a group
        drawArrow (thePresentation, aFirstArrowPosition, aFirstArrowDir);
        if (!isOneSideDimension)
        {
          drawArrow (thePresentation, aSecondArrowPosition, aSecondArrowDir);
        }
      }
      break;
    }
    case Prs3d_HTA_Right:
    {
      aPrimSegments = new Graphic3d_ArrayOfSegments (4);
      // Left extension
      if (!isOneSideDimension)
      {
        aFirstPoint = ElCLib::Value (aFirstArrowBegin - anArrowLength, aDimensionLine);
        aLastPoint = ElCLib::Value (aFirstArrowEnd, aDimensionLine);
        aPrimSegments->AddVertex (aFirstPoint);
        aPrimSegments->AddVertex (aLastPoint);
        myGeom.mySensitiveSegments.Append (new Select3D_SensitiveSegment (anEmptyOwner, aFirstPoint, aLastPoint));
      }

      // Central(main) dimension line
      aFirstPoint = isOneSideDimension ? theFirstAttach :  ElCLib::Value (aFirstArrowEnd, aDimensionLine);
      aLastPoint = ElCLib::Value (aSecondArrowBegin, aDimensionLine);
      aPrimSegments->AddVertex (aFirstPoint);
      aPrimSegments->AddVertex (aLastPoint);
      myGeom.mySensitiveSegments.Append (new Select3D_SensitiveSegment (anEmptyOwner, aFirstPoint, aLastPoint));

      // Right extension with text
      aFirstPoint = ElCLib::Value (aSecondArrowEnd, aDimensionLine);
      drawExtensionWithText (thePresentation, aFirstPoint, aDimensionLine, aValueString, theMode);

      if (theMode != AIS_DDM_Text)
      {
        // Main dimension line, short extension
        Prs3d_Root::NewGroup(thePresentation)->SetPrimitivesAspect (aDimensionAspect->LineAspect()->Aspect());
        Prs3d_Root::CurrentGroup(thePresentation)->AddPrimitiveArray (aPrimSegments);
        // Group1, 2: Add arrows to a group
        if (!isOneSideDimension)
        {
          drawArrow (thePresentation, aFirstArrowPosition, aFirstArrowDir);
        }

        drawArrow (thePresentation, aSecondArrowPosition, aSecondArrowDir);
      }
      break;
    }
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

  Handle( Select3D_SensitiveGroup) aSensitiveForLine;
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

  theSelection->Add (aSensitiveForLine);
  theSelection->Add (aSensitiveForText);
}
