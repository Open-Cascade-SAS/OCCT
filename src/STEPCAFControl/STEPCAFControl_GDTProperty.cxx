// Created on: 2015-09-10
// Created by: Irina Krylova
// Copyright (c) 1999-2015 OPEN CASCADE SAS
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

#include <BRep_Tool.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <ShapeConstruct_Curve.hxx>
#include <STEPCAFControl_GDTProperty.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_Direction.hxx>
#include <StepDimTol_CylindricityTolerance.hxx>
#include <StepDimTol_FlatnessTolerance.hxx>
#include <StepDimTol_LineProfileTolerance.hxx>
#include <StepDimTol_PositionTolerance.hxx>
#include <StepDimTol_RoundnessTolerance.hxx>
#include <StepDimTol_StraightnessTolerance.hxx>
#include <StepDimTol_SurfaceProfileTolerance.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_TessellatedCurveSet.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDimTolObjects_DatumModifiersSequence.hxx>
#include <XCAFDimTolObjects_DatumModifWithValue.hxx>

//=======================================================================
//function : STEPCAFControl_GDTProperty
//purpose  : 
//=======================================================================

STEPCAFControl_GDTProperty::STEPCAFControl_GDTProperty ()
{
}

//=======================================================================
//function : getDimModifiers
//purpose  : 
//=======================================================================
void STEPCAFControl_GDTProperty::GetDimModifiers(const Handle(StepRepr_CompoundRepresentationItem)& theCRI,
                            XCAFDimTolObjects_DimensionModifiersSequence& theModifiers)
{
  for (Standard_Integer l = 1; l <= theCRI->ItemElement()->Length(); l++)
  {
    Handle(StepRepr_DescriptiveRepresentationItem) aDRI =
      Handle(StepRepr_DescriptiveRepresentationItem)::DownCast(theCRI->ItemElement()->Value(l));
    if(aDRI.IsNull()) continue;
    XCAFDimTolObjects_DimensionModif aModifier = XCAFDimTolObjects_DimensionModif_ControlledRadius;
    const TCollection_AsciiString aModifStr = aDRI->Description()->String();
    Standard_Boolean aFound = Standard_False;
    if(aModifStr.IsEqual("controlled radius"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_ControlledRadius;
    }
    else if(aModifStr.IsEqual("square"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_Square;
    }
    else if(aModifStr.IsEqual("statistical"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_StatisticalTolerance;
    }
    else if(aModifStr.IsEqual("continuous feature"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_ContinuousFeature;
    }
    else if(aModifStr.IsEqual("two point size"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_TwoPointSize;
    }
    else if(aModifStr.IsEqual("local size defined by a sphere"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_LocalSizeDefinedBySphere;
    }
    else if(aModifStr.IsEqual("least squares association criteria"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_LeastSquaresAssociationCriterion;
    }
    else if(aModifStr.IsEqual("maximum inscribed association criteria"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_MaximumInscribedAssociation;
    }
    else if(aModifStr.IsEqual("minimum circumscribed association criteria"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_MinimumCircumscribedAssociation;
    }
    else if(aModifStr.IsEqual("circumference diameter calculated size"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_CircumferenceDiameter;
    }
    else if(aModifStr.IsEqual("area diameter calculated size"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_AreaDiameter;
    }
    else if(aModifStr.IsEqual("volume diameter calculated size"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_VolumeDiameter;
    }
    else if(aModifStr.IsEqual("maximum rank order size"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_MaximumSize;
    }
    else if(aModifStr.IsEqual("minimum rank order size"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_MinimumSize;
    }
    else if(aModifStr.IsEqual("average rank order size"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_AverageSize;
    }
    else if(aModifStr.IsEqual("median rank order size"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_MedianSize;
    }
    else if(aModifStr.IsEqual("mid range rank order size"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_MidRangeSize;
    }
    else if(aModifStr.IsEqual("range rank order size"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_RangeOfSizes;
    }
    else if(aModifStr.IsEqual("any part of the feature"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_AnyRestrictedPortionOfFeature;
    }
    else if(aModifStr.IsEqual("any cross section"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_AnyCrossSection;
    }
    else if(aModifStr.IsEqual("specific fixed cross section"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_SpecificFixedCrossSection;
    }           
    else if(aModifStr.IsEqual("common tolerance"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_CommonTolerance;
    }             
    else if(aModifStr.IsEqual("free state condition"))
    {
      aFound = Standard_True;
      aModifier = XCAFDimTolObjects_DimensionModif_FreeStateCondition;
    }
    if (aFound)
      theModifiers.Append(aModifier);
  }
}

//=======================================================================
//function : getClassOfTolerance
//purpose  : 
//=======================================================================
void STEPCAFControl_GDTProperty::GetDimClassOfTolerance(const Handle(StepShape_LimitsAndFits)& theLAF,
                                   Standard_Boolean& theHolle,
                                   XCAFDimTolObjects_DimensionFormVariance& theFV,
                                   XCAFDimTolObjects_DimensionGrade& theG)
{
  Handle(TCollection_HAsciiString) aFormV = theLAF->FormVariance();
  Handle(TCollection_HAsciiString) aGrade = theLAF->Grade();
  theFV = XCAFDimTolObjects_DimensionFormVariance_None;
  Standard_Boolean aFound;
  theHolle = Standard_False;
  //it is not verified information
  for(Standard_Integer c = 0; c <= 1 && !aFormV.IsNull(); c++)
  {
    aFound = Standard_False;
    Standard_Boolean aCaseSens = Standard_False;
    if (c == 1)
      aCaseSens = Standard_True;
    Handle(TCollection_HAsciiString) aStr = new TCollection_HAsciiString("a");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_A;
      continue;
    }
    aStr = new TCollection_HAsciiString("b");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_B;
      continue;
    }
    aStr = new TCollection_HAsciiString("c");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_C;
      continue;
    }
    aStr = new TCollection_HAsciiString("cd");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_CD;
      continue;
    }
    aStr = new TCollection_HAsciiString("d");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_D;
      continue;
    }
    aStr = new TCollection_HAsciiString("e");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_E;
      continue;
    }
    aStr = new TCollection_HAsciiString("ef");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_EF;
      continue;
    }
    aStr = new TCollection_HAsciiString("f");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_F;
      continue;
    }
    aStr = new TCollection_HAsciiString("fg");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_FG;
      continue;
    }
    aStr = new TCollection_HAsciiString("g");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_G;
      continue;
    }
    aStr = new TCollection_HAsciiString("h");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_H;
      continue;
    }
    aStr = new TCollection_HAsciiString("js");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_JS;
      continue;
    }
    aStr = new TCollection_HAsciiString("k");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_K;
      continue;
    }
    aStr = new TCollection_HAsciiString("m");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_M;
      continue;
    }
    aStr = new TCollection_HAsciiString("n");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_N;
      continue;
    }
    aStr = new TCollection_HAsciiString("p");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_P;
      continue;
    }
    aStr = new TCollection_HAsciiString("r");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_R;
      continue;
    }
    aStr = new TCollection_HAsciiString("s");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_S;
      continue;
    }
    aStr = new TCollection_HAsciiString("t");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_T;
      continue;
    }
    aStr = new TCollection_HAsciiString("u");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_U;
      continue;
    }
    aStr = new TCollection_HAsciiString("v");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_V;
      continue;
    }
    aStr = new TCollection_HAsciiString("x");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_X;
      continue;
    }
    aStr = new TCollection_HAsciiString("y");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_Y;
      continue;
    }
    aStr = new TCollection_HAsciiString("b");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_B;
      continue;
    }
    aStr = new TCollection_HAsciiString("z");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_Z;
      continue;
    }
    aStr = new TCollection_HAsciiString("za");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_ZA;
      continue;
    }
    aStr = new TCollection_HAsciiString("zb");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_ZB;
      continue;
    }
    aStr = new TCollection_HAsciiString("zc");
    if(aFormV->IsSameString(aStr, aCaseSens))
    {
      aFound = Standard_True;
      theFV = XCAFDimTolObjects_DimensionFormVariance_ZC;
      continue;
    }

    if (c == 1 && !aFound)
      theHolle = Standard_True;
  }
  Handle(TCollection_HAsciiString) aStr = new TCollection_HAsciiString("01");
  theG = XCAFDimTolObjects_DimensionGrade_IT01;
  if (!aGrade.IsNull()
    && !aGrade->String().IsEqual("01")
    && aGrade->IsIntegerValue())
  {
    theG = (XCAFDimTolObjects_DimensionGrade)(aGrade->IntegerValue() + 1);
  }
}

//=======================================================================
//function : getDimType
//purpose  : 
//=======================================================================
Standard_Boolean STEPCAFControl_GDTProperty::GetDimType(const Handle(TCollection_HAsciiString)& theName,
                       XCAFDimTolObjects_DimensionType& theType)
{
    TCollection_AsciiString aName = theName->String();
    aName.LowerCase();
    theType = XCAFDimTolObjects_DimensionType_Location_None;
    if(aName.IsEqual("curve length"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_CurveLength;
    }
    else if(aName.IsEqual("diameter"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_Diameter;
    }
    else if(aName.IsEqual("spherical diameter"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_SphericalDiameter;
    }
    else if(aName.IsEqual("radius"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_Radius;
    }
    else if(aName.IsEqual("spherical radius"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_SphericalRadius;
    }
    else if(aName.IsEqual("toroidal minor diameter"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_ToroidalMinorDiameter;
    }
    else if(aName.IsEqual("toroidal major diameter"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_ToroidalMajorDiameter;
    }
    else if(aName.IsEqual("toroidal minor radius"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_ToroidalMinorRadius;
    }
    else if(aName.IsEqual("toroidal major radius"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_ToroidalMajorRadius;
    }
    else if(aName.IsEqual("toroidal high major diameter"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorDiameter;
    }
    else if(aName.IsEqual("toroidal low major diameter"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorDiameter;
    }
    else if(aName.IsEqual("toroidal high major radius"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorRadius;
    }
    else if(aName.IsEqual("toroidal low major radius"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorRadius;
    }
    else if(aName.IsEqual("thickness"))
    {
      theType = XCAFDimTolObjects_DimensionType_Size_Thickness;
    }
    else if(aName.IsEqual("curved distance"))
    {
      theType = XCAFDimTolObjects_DimensionType_Location_CurvedDistance;
    }
    else if(aName.IsEqual("linear distance"))
    {
      theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance;
    }
    else if(aName.IsEqual("linear distance centre outer"))
    {
      theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToOuter;
    }
    else if(aName.IsEqual("linear distance centre inner"))
    {
      theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToInner;
    }
    else if(aName.IsEqual("linear distance outer centre"))
    {
      theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToCenter;
    }
    else if(aName.IsEqual("linear distance outer outer"))
    {
      theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToOuter;
    }
    else if(aName.IsEqual("linear distance outer inner"))
    {
      theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToInner;
    }
    else if(aName.IsEqual("linear distance inner centre"))
    {
      theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToCenter;
    }
    else if(aName.IsEqual("linear distance inner outer"))
    {
      theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToOuter;
    }
    else if(aName.IsEqual("linear distance inner inner"))
    {
      theType = XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToInner;
    }

    if(theType != XCAFDimTolObjects_DimensionType_Location_None &&
       theType != XCAFDimTolObjects_DimensionType_CommonLabel)
    {
      return Standard_True;
    }
    return Standard_False;
}


//=======================================================================
//function : DatumTargetType
//purpose  : 
//=======================================================================
Standard_Boolean STEPCAFControl_GDTProperty::GetDatumTargetType(const Handle(TCollection_HAsciiString)& theDescription,
                       XCAFDimTolObjects_DatumTargetType& theType)
{
    TCollection_AsciiString aName = theDescription->String();
    aName.LowerCase();
    if(aName.IsEqual("area"))
    {
      theType = XCAFDimTolObjects_DatumTargetType_Area;
      return Standard_True;
    }
    else if(aName.IsEqual("line"))
    {
      theType = XCAFDimTolObjects_DatumTargetType_Line;
      return Standard_True;
    }
    else if(aName.IsEqual("circle"))
    {
      theType = XCAFDimTolObjects_DatumTargetType_Circle;
      return Standard_True;
    }
    else if(aName.IsEqual("rectangle"))
    {
      theType = XCAFDimTolObjects_DatumTargetType_Rectangle;
      return Standard_True;
    }
    else if(aName.IsEqual("point"))
    {
      theType = XCAFDimTolObjects_DatumTargetType_Point;
      return Standard_True;
    }
    return Standard_False;
}

//=======================================================================
//function : GetDimQualifierType
//purpose  : 
//=======================================================================
Standard_Boolean STEPCAFControl_GDTProperty::GetDimQualifierType(const Handle(TCollection_HAsciiString)& theDescription,
                       XCAFDimTolObjects_DimensionQualifier& theType)
{
    TCollection_AsciiString aName = theDescription->String();
    aName.LowerCase();
    theType = XCAFDimTolObjects_DimensionQualifier_None;
    if(aName.IsEqual("maximum"))
    {
      theType = XCAFDimTolObjects_DimensionQualifier_Max;
    }
    else if(aName.IsEqual("minimum"))
    {
      theType = XCAFDimTolObjects_DimensionQualifier_Min;
    }
    else if(aName.IsEqual("average"))
    {
      theType = XCAFDimTolObjects_DimensionQualifier_Avg;
    }
    if(theType != XCAFDimTolObjects_DimensionQualifier_None)
    {
      return Standard_True;
    }
    return Standard_False;
}

//=======================================================================
//function : GetTolValueType
//purpose  : 
//=======================================================================
Standard_Boolean STEPCAFControl_GDTProperty::GetTolValueType(const Handle(TCollection_HAsciiString)& theDescription,
                       XCAFDimTolObjects_GeomToleranceTypeValue& theType)
{
    TCollection_AsciiString aName = theDescription->String();
    aName.LowerCase();
    theType = XCAFDimTolObjects_GeomToleranceTypeValue_None;
    if(aName.IsEqual("cylindrical or circular"))
    {
      theType = XCAFDimTolObjects_GeomToleranceTypeValue_Diameter;
    }
    else if(aName.IsEqual("spherical"))
    {
      theType = XCAFDimTolObjects_GeomToleranceTypeValue_SphericalDiameter;
    }
    if(theType != XCAFDimTolObjects_GeomToleranceTypeValue_None)
    {
      return Standard_True;
    }
    return Standard_False;
}


//=======================================================================
//function : GetDimTypeName
//purpose  : 
//=======================================================================
Handle(TCollection_HAsciiString) STEPCAFControl_GDTProperty::GetDimTypeName(const XCAFDimTolObjects_DimensionType theType)
{
  Handle(TCollection_HAsciiString) aName;
        switch (theType) {
        // Dimensional_Location
        case XCAFDimTolObjects_DimensionType_Location_CurvedDistance:
          aName = new TCollection_HAsciiString("curved distance");
          break;
        case XCAFDimTolObjects_DimensionType_Location_LinearDistance:
          aName = new TCollection_HAsciiString("linear distance");
          break;
        case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToOuter:
          aName = new TCollection_HAsciiString("linear distance centre outer");
          break;
        case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToInner:
          aName = new TCollection_HAsciiString("linear distance centre inner");
          break;
        case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToCenter:
          aName = new TCollection_HAsciiString("linear distance outer centre");
          break;
        case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToOuter:
          aName = new TCollection_HAsciiString("linear distance outer outer");
          break;
        case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToInner:
          aName = new TCollection_HAsciiString("linear distance outer inner");
          break;
        case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToCenter:
          aName = new TCollection_HAsciiString("linear distance inner centre");
          break;
        case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToOuter:
          aName = new TCollection_HAsciiString("linear distance inner outer");
          break;
        case XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToInner:
          aName = new TCollection_HAsciiString("linear distance inner inner");
          break;
        //Dimensional_Size
        case XCAFDimTolObjects_DimensionType_Size_CurveLength:
          aName = new TCollection_HAsciiString("curve length");
          break;
        case XCAFDimTolObjects_DimensionType_Size_Diameter:
          aName = new TCollection_HAsciiString("diameter");
          break;
        case XCAFDimTolObjects_DimensionType_Size_SphericalDiameter:
          aName = new TCollection_HAsciiString("spherical diameter");
          break;
        case XCAFDimTolObjects_DimensionType_Size_Radius:
          aName = new TCollection_HAsciiString("radius");
          break;
        case XCAFDimTolObjects_DimensionType_Size_SphericalRadius:
          aName = new TCollection_HAsciiString("spherical radius");
          break;
        case XCAFDimTolObjects_DimensionType_Size_ToroidalMinorDiameter:
          aName = new TCollection_HAsciiString("toroidal minor diameter");
          break;
        case XCAFDimTolObjects_DimensionType_Size_ToroidalMajorDiameter:
          aName = new TCollection_HAsciiString("toroidal major diameter");
          break;
        case XCAFDimTolObjects_DimensionType_Size_ToroidalMinorRadius:
          aName = new TCollection_HAsciiString("toroidal minor radius");
          break;
        case XCAFDimTolObjects_DimensionType_Size_ToroidalMajorRadius:
          aName = new TCollection_HAsciiString("toroidal major radius");
          break;
        case XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorDiameter:
          aName = new TCollection_HAsciiString("toroidal high major diameter");
          break;
        case XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorDiameter:
          aName = new TCollection_HAsciiString("toroidal low major diameter");
          break;
        case XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorRadius:
          aName = new TCollection_HAsciiString("toroidal high major radius");
          break;
        case XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorRadius:
          aName = new TCollection_HAsciiString("toroidal low major radius");
          break;
        case XCAFDimTolObjects_DimensionType_Size_Thickness:
          aName = new TCollection_HAsciiString("thickness");
          break;
        // Other entities
        default:
          aName = new TCollection_HAsciiString();
      }
  return aName;
}

//=======================================================================
//function : GetDimQualifierName
//purpose  : 
//=======================================================================
Handle(TCollection_HAsciiString) STEPCAFControl_GDTProperty::GetDimQualifierName(const XCAFDimTolObjects_DimensionQualifier theQualifier)
{
  Handle(TCollection_HAsciiString) aName;
  switch (theQualifier) {
    case XCAFDimTolObjects_DimensionQualifier_Min:
      aName = new TCollection_HAsciiString("minimum");
      break;
    case XCAFDimTolObjects_DimensionQualifier_Avg:
      aName = new TCollection_HAsciiString("average");
      break;
    case XCAFDimTolObjects_DimensionQualifier_Max:
      aName = new TCollection_HAsciiString("maximum");
      break;
    default:
      aName = new TCollection_HAsciiString();
  }
  return aName;
}

//=======================================================================
//function : GetDimModifierName
//purpose  : 
//=======================================================================
Handle(TCollection_HAsciiString) STEPCAFControl_GDTProperty::GetDimModifierName(const XCAFDimTolObjects_DimensionModif theModifier)
{
  Handle(TCollection_HAsciiString) aName;
  switch (theModifier) {
    case XCAFDimTolObjects_DimensionModif_ControlledRadius:
      aName = new TCollection_HAsciiString("controlled radius");
      break;
    case XCAFDimTolObjects_DimensionModif_Square:
      aName = new TCollection_HAsciiString("square");
      break;
    case XCAFDimTolObjects_DimensionModif_StatisticalTolerance:
      aName = new TCollection_HAsciiString("statistical");
      break;
    case XCAFDimTolObjects_DimensionModif_ContinuousFeature:
      aName = new TCollection_HAsciiString("continuous feature");
      break;
    case XCAFDimTolObjects_DimensionModif_TwoPointSize:
      aName = new TCollection_HAsciiString("two point size");
      break;
    case XCAFDimTolObjects_DimensionModif_LocalSizeDefinedBySphere:
      aName = new TCollection_HAsciiString("local size defined by a sphere");
      break;
    case XCAFDimTolObjects_DimensionModif_LeastSquaresAssociationCriterion:
      aName = new TCollection_HAsciiString("least squares association criteria");
      break;
    case XCAFDimTolObjects_DimensionModif_MaximumInscribedAssociation:
      aName = new TCollection_HAsciiString("maximum inscribed association criteria");
      break;
    case XCAFDimTolObjects_DimensionModif_MinimumCircumscribedAssociation:
      aName = new TCollection_HAsciiString("minimum circumscribed association criteria");
      break;
    case XCAFDimTolObjects_DimensionModif_CircumferenceDiameter:
      aName = new TCollection_HAsciiString("circumference diameter calculated size");
      break;
    case XCAFDimTolObjects_DimensionModif_AreaDiameter:
      aName = new TCollection_HAsciiString("area diameter calculated size");
      break;
    case XCAFDimTolObjects_DimensionModif_VolumeDiameter:
      aName = new TCollection_HAsciiString("volume diameter calculated size");
      break;
    case XCAFDimTolObjects_DimensionModif_MaximumSize:
      aName = new TCollection_HAsciiString("maximum rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_MinimumSize:
      aName = new TCollection_HAsciiString("minimum rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_AverageSize:
      aName = new TCollection_HAsciiString("average rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_MedianSize:
      aName = new TCollection_HAsciiString("median rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_MidRangeSize:
      aName = new TCollection_HAsciiString("mid range rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_RangeOfSizes:
      aName = new TCollection_HAsciiString("range rank order size");
      break;
    case XCAFDimTolObjects_DimensionModif_AnyRestrictedPortionOfFeature:
      aName = new TCollection_HAsciiString("any part of the feature");
      break;
    case XCAFDimTolObjects_DimensionModif_AnyCrossSection:
      aName = new TCollection_HAsciiString("any cross section");
      break;
    case XCAFDimTolObjects_DimensionModif_SpecificFixedCrossSection:
      aName = new TCollection_HAsciiString("specific fixed cross section");
      break;
    case XCAFDimTolObjects_DimensionModif_CommonTolerance:
      aName = new TCollection_HAsciiString("common tolerance");
      break;
    case XCAFDimTolObjects_DimensionModif_FreeStateCondition:
      aName = new TCollection_HAsciiString("free state condition");
      break;
    default: aName = new TCollection_HAsciiString();
  }
  return aName;
}

//=======================================================================
//function : GetLimitsAndFits
//purpose  : 
//=======================================================================
Handle(StepShape_LimitsAndFits) STEPCAFControl_GDTProperty::GetLimitsAndFits(Standard_Boolean theHole,
                       XCAFDimTolObjects_DimensionFormVariance theFormVariance,
                       XCAFDimTolObjects_DimensionGrade theGrade)
{
  Handle(StepShape_LimitsAndFits) aLAF = new StepShape_LimitsAndFits();
  Handle(TCollection_HAsciiString) aGradeStr, aFormStr, aHoleStr;
  
  if (theGrade == XCAFDimTolObjects_DimensionGrade_IT01)
      aGradeStr = new TCollection_HAsciiString("01");
    else
      aGradeStr = new TCollection_HAsciiString((Standard_Integer)theGrade + 1);
  
  switch (theFormVariance) {
    case XCAFDimTolObjects_DimensionFormVariance_None:
      aFormStr = new TCollection_HAsciiString("");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_A:
      aFormStr = new TCollection_HAsciiString("A");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_B:
      aFormStr = new TCollection_HAsciiString("B");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_C:
      aFormStr = new TCollection_HAsciiString("C");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_CD:
      aFormStr = new TCollection_HAsciiString("CD");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_D:
      aFormStr = new TCollection_HAsciiString("D");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_E:
      aFormStr = new TCollection_HAsciiString("E");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_EF:
      aFormStr = new TCollection_HAsciiString("EF");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_F:
      aFormStr = new TCollection_HAsciiString("F");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_FG:
      aFormStr = new TCollection_HAsciiString("FG");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_G:
      aFormStr = new TCollection_HAsciiString("G");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_H:
      aFormStr = new TCollection_HAsciiString("H");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_JS:
      aFormStr = new TCollection_HAsciiString("JS");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_J:
      aFormStr = new TCollection_HAsciiString("J");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_K:
      aFormStr = new TCollection_HAsciiString("K");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_M:
      aFormStr = new TCollection_HAsciiString("M");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_N:
      aFormStr = new TCollection_HAsciiString("N");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_P:
      aFormStr = new TCollection_HAsciiString("P");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_R:
      aFormStr = new TCollection_HAsciiString("R");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_S:
      aFormStr = new TCollection_HAsciiString("S");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_T:
      aFormStr = new TCollection_HAsciiString("T");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_U:
      aFormStr = new TCollection_HAsciiString("U");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_V:
      aFormStr = new TCollection_HAsciiString("V");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_X:
      aFormStr = new TCollection_HAsciiString("X");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_Y:
      aFormStr = new TCollection_HAsciiString("Y");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_Z:
      aFormStr = new TCollection_HAsciiString("Z");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_ZA:
      aFormStr = new TCollection_HAsciiString("ZA");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_ZB:
      aFormStr = new TCollection_HAsciiString("ZB");
      break;
    case XCAFDimTolObjects_DimensionFormVariance_ZC:
      aFormStr = new TCollection_HAsciiString("ZC");
      break;
  }

  if (theHole) {
    aHoleStr = new TCollection_HAsciiString("hole");
  }
  else {
    aHoleStr = new TCollection_HAsciiString("shaft");
    aFormStr->LowerCase();
  }
  aLAF->Init(aFormStr, aHoleStr, aGradeStr, new TCollection_HAsciiString);
  return aLAF;
}

//=======================================================================
//function : GetDatumTargetName
//purpose  : 
//=======================================================================
Handle(TCollection_HAsciiString) STEPCAFControl_GDTProperty::GetDatumTargetName(const XCAFDimTolObjects_DatumTargetType theDatumType)
{
  Handle(TCollection_HAsciiString) aName;
  switch (theDatumType) {
    case XCAFDimTolObjects_DatumTargetType_Point:
      aName = new TCollection_HAsciiString("point");
      break;
    case XCAFDimTolObjects_DatumTargetType_Line:
      aName = new TCollection_HAsciiString("line");
      break;
    case XCAFDimTolObjects_DatumTargetType_Rectangle:
      aName = new TCollection_HAsciiString("rectangle");
      break;
    case XCAFDimTolObjects_DatumTargetType_Circle:
      aName = new TCollection_HAsciiString("circle");
      break;
    case XCAFDimTolObjects_DatumTargetType_Area:
      aName = new TCollection_HAsciiString("area");
      break;
    default: aName = new TCollection_HAsciiString();
  }
  return aName;
}

//=======================================================================
//function : IsDimensionalSize
//purpose  : 
//=======================================================================
Standard_Boolean STEPCAFControl_GDTProperty::IsDimensionalLocation(const XCAFDimTolObjects_DimensionType theType)
{
  if (theType == XCAFDimTolObjects_DimensionType_Location_None ||
      theType == XCAFDimTolObjects_DimensionType_Location_CurvedDistance  ||
      theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance ||
      theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToOuter ||
      theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromCenterToInner ||
      theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToCenter ||
      theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToOuter ||
      theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromOuterToInner ||
      theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToCenter ||
      theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToOuter ||
      theType == XCAFDimTolObjects_DimensionType_Location_LinearDistance_FromInnerToInner ||
      theType == XCAFDimTolObjects_DimensionType_Location_Oriented)
    return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : IsDimensionalSize
//purpose  : 
//=======================================================================
Standard_Boolean STEPCAFControl_GDTProperty::IsDimensionalSize(const XCAFDimTolObjects_DimensionType theType)
{
  if (theType == XCAFDimTolObjects_DimensionType_Size_CurveLength ||
      theType == XCAFDimTolObjects_DimensionType_Size_Diameter ||
      theType == XCAFDimTolObjects_DimensionType_Size_SphericalDiameter ||
      theType == XCAFDimTolObjects_DimensionType_Size_Radius ||
      theType == XCAFDimTolObjects_DimensionType_Size_SphericalRadius ||
      theType == XCAFDimTolObjects_DimensionType_Size_ToroidalMinorDiameter ||
      theType == XCAFDimTolObjects_DimensionType_Size_ToroidalMajorDiameter ||
      theType == XCAFDimTolObjects_DimensionType_Size_ToroidalMinorRadius ||
      theType == XCAFDimTolObjects_DimensionType_Size_ToroidalMajorRadius ||
      theType == XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorDiameter ||
      theType == XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorDiameter ||
      theType == XCAFDimTolObjects_DimensionType_Size_ToroidalHighMajorRadius ||
      theType == XCAFDimTolObjects_DimensionType_Size_ToroidalLowMajorRadius ||
      theType == XCAFDimTolObjects_DimensionType_Size_Thickness)
    return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : GetGeomToleranceType
//purpose  : 
//=======================================================================
StepDimTol_GeometricToleranceType STEPCAFControl_GDTProperty::GetGeomToleranceType(const XCAFDimTolObjects_GeomToleranceType theType)
{
  switch (theType) {
    case XCAFDimTolObjects_GeomToleranceType_Angularity:
      return StepDimTol_GTTAngularityTolerance;
    case XCAFDimTolObjects_GeomToleranceType_CircularRunout:
      return StepDimTol_GTTCircularRunoutTolerance;
    case XCAFDimTolObjects_GeomToleranceType_CircularityOrRoundness:
      return StepDimTol_GTTRoundnessTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Coaxiality:
      return StepDimTol_GTTCoaxialityTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Concentricity:
      return StepDimTol_GTTConcentricityTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Cylindricity:
      return StepDimTol_GTTCylindricityTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Flatness:
      return StepDimTol_GTTFlatnessTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Parallelism:
      return StepDimTol_GTTParallelismTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Perpendicularity:
      return StepDimTol_GTTPerpendicularityTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Position:
      return StepDimTol_GTTPositionTolerance;
    case XCAFDimTolObjects_GeomToleranceType_ProfileOfLine:
      return StepDimTol_GTTLineProfileTolerance;
    case XCAFDimTolObjects_GeomToleranceType_ProfileOfSurface:
      return StepDimTol_GTTSurfaceProfileTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Straightness:
      return StepDimTol_GTTStraightnessTolerance;
    case XCAFDimTolObjects_GeomToleranceType_Symmetry:
      return StepDimTol_GTTSymmetryTolerance;
    case XCAFDimTolObjects_GeomToleranceType_TotalRunout:
      return StepDimTol_GTTTotalRunoutTolerance;
    default:
      return StepDimTol_GTTPositionTolerance;
  }
}

//=======================================================================
//function : GetGeomToleranceType
//purpose  : 
//=======================================================================
XCAFDimTolObjects_GeomToleranceType STEPCAFControl_GDTProperty::GetGeomToleranceType(const StepDimTol_GeometricToleranceType theType)
{
  switch (theType) {
    case StepDimTol_GTTAngularityTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Angularity;
    case StepDimTol_GTTCircularRunoutTolerance:
      return XCAFDimTolObjects_GeomToleranceType_CircularRunout;
    case StepDimTol_GTTRoundnessTolerance:
      return XCAFDimTolObjects_GeomToleranceType_CircularityOrRoundness;
    case StepDimTol_GTTCoaxialityTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Coaxiality;
    case StepDimTol_GTTConcentricityTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Concentricity;
    case StepDimTol_GTTCylindricityTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Cylindricity;
    case StepDimTol_GTTFlatnessTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Flatness;
    case StepDimTol_GTTParallelismTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Parallelism;
    case StepDimTol_GTTPerpendicularityTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Perpendicularity;
    case StepDimTol_GTTPositionTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Position;
    case StepDimTol_GTTLineProfileTolerance:
      return XCAFDimTolObjects_GeomToleranceType_ProfileOfLine;
    case StepDimTol_GTTSurfaceProfileTolerance:
      return XCAFDimTolObjects_GeomToleranceType_ProfileOfSurface;
    case StepDimTol_GTTStraightnessTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Straightness;
    case StepDimTol_GTTSymmetryTolerance:
      return XCAFDimTolObjects_GeomToleranceType_Symmetry;
    case StepDimTol_GTTTotalRunoutTolerance:
      return XCAFDimTolObjects_GeomToleranceType_TotalRunout;
    default:
      return XCAFDimTolObjects_GeomToleranceType_Position;
  }
}

//=======================================================================
//function : GetGeomTolerance
//purpose  : 
//=======================================================================
Handle(StepDimTol_GeometricTolerance) STEPCAFControl_GDTProperty::
  GetGeomTolerance(const XCAFDimTolObjects_GeomToleranceType theType)
{
  switch (theType) {
    case XCAFDimTolObjects_GeomToleranceType_CircularityOrRoundness:
      return new StepDimTol_RoundnessTolerance();
    case XCAFDimTolObjects_GeomToleranceType_Cylindricity:
      return new StepDimTol_CylindricityTolerance();
    case XCAFDimTolObjects_GeomToleranceType_Flatness:
      return new StepDimTol_FlatnessTolerance();
    case XCAFDimTolObjects_GeomToleranceType_Position:
      return new StepDimTol_PositionTolerance();
    case XCAFDimTolObjects_GeomToleranceType_ProfileOfLine:
      return new StepDimTol_LineProfileTolerance();
    case XCAFDimTolObjects_GeomToleranceType_ProfileOfSurface:
      return new StepDimTol_SurfaceProfileTolerance();
    case XCAFDimTolObjects_GeomToleranceType_Straightness:
      return new StepDimTol_StraightnessTolerance();
    default:
      return NULL;
  }
}

//=======================================================================
//function : GetGeomToleranceModifier
//purpose  : 
//=======================================================================
StepDimTol_GeometricToleranceModifier STEPCAFControl_GDTProperty::
  GetGeomToleranceModifier(const XCAFDimTolObjects_GeomToleranceModif theModifier)
{
  switch (theModifier) {
    case XCAFDimTolObjects_GeomToleranceModif_Any_Cross_Section:
      return StepDimTol_GTMAnyCrossSection;
    case XCAFDimTolObjects_GeomToleranceModif_Common_Zone:
      return StepDimTol_GTMCommonZone;
    case XCAFDimTolObjects_GeomToleranceModif_Each_Radial_Element:
      return StepDimTol_GTMEachRadialElement;
    case XCAFDimTolObjects_GeomToleranceModif_Free_State:
      return StepDimTol_GTMFreeState;
    case XCAFDimTolObjects_GeomToleranceModif_Least_Material_Requirement:
      return StepDimTol_GTMLeastMaterialRequirement;
    case XCAFDimTolObjects_GeomToleranceModif_Line_Element:
      return StepDimTol_GTMLineElement;
    case XCAFDimTolObjects_GeomToleranceModif_Major_Diameter:
      return StepDimTol_GTMMajorDiameter;
    case XCAFDimTolObjects_GeomToleranceModif_Maximum_Material_Requirement:
      return StepDimTol_GTMMaximumMaterialRequirement;
    case XCAFDimTolObjects_GeomToleranceModif_Minor_Diameter:
      return StepDimTol_GTMMinorDiameter;
    case XCAFDimTolObjects_GeomToleranceModif_Not_Convex:
      return StepDimTol_GTMNotConvex;
    case XCAFDimTolObjects_GeomToleranceModif_Pitch_Diameter:
      return StepDimTol_GTMPitchDiameter;
    case XCAFDimTolObjects_GeomToleranceModif_Reciprocity_Requirement:
      return StepDimTol_GTMReciprocityRequirement;
    case XCAFDimTolObjects_GeomToleranceModif_Separate_Requirement:
      return StepDimTol_GTMSeparateRequirement;
    case XCAFDimTolObjects_GeomToleranceModif_Statistical_Tolerance:
      return StepDimTol_GTMStatisticalTolerance;
    case XCAFDimTolObjects_GeomToleranceModif_Tangent_Plane:
      return StepDimTol_GTMTangentPlane;
    default:
      return StepDimTol_GTMMaximumMaterialRequirement;
  }
}

//=======================================================================
//function : GetDatumRefModifiers
//purpose  : Note: this function does not add anything to model
//=======================================================================
Handle(StepDimTol_HArray1OfDatumReferenceModifier) STEPCAFControl_GDTProperty::
  GetDatumRefModifiers(const XCAFDimTolObjects_DatumModifiersSequence theModifiers,
                       const XCAFDimTolObjects_DatumModifWithValue theModifWithVal,
                       const Standard_Real theValue,
                       const StepBasic_Unit theUnit)
{
  if ((theModifiers.Length() == 0) && (theModifWithVal == XCAFDimTolObjects_DatumModifWithValue_None))
    return NULL;
  Standard_Integer aModifNb = theModifiers.Length();
  if (theModifWithVal != XCAFDimTolObjects_DatumModifWithValue_None)
    aModifNb++;
  Handle(StepDimTol_HArray1OfDatumReferenceModifier) aModifiers =
    new StepDimTol_HArray1OfDatumReferenceModifier(1, aModifNb);

  // Modifier with value
  if (theModifWithVal != XCAFDimTolObjects_DatumModifWithValue_None) {
    StepDimTol_DatumReferenceModifierType aType;
    switch (theModifWithVal) {
      case XCAFDimTolObjects_DatumModifWithValue_CircularOrCylindrical:
        aType = StepDimTol_CircularOrCylindrical;
        break;
      case XCAFDimTolObjects_DatumModifWithValue_Distance:
        aType = StepDimTol_Distance;
        break;
      case XCAFDimTolObjects_DatumModifWithValue_Projected:
        aType = StepDimTol_Projected;
        break;
      case XCAFDimTolObjects_DatumModifWithValue_Spherical:
        aType = StepDimTol_Spherical;
        break;
      default:
        aType = StepDimTol_Distance;
    }
    Handle(StepBasic_LengthMeasureWithUnit) aLMWU = new StepBasic_LengthMeasureWithUnit();
    Handle(StepBasic_MeasureValueMember) aValueMember = new StepBasic_MeasureValueMember();
    aValueMember->SetName("LENGTH_MEASURE");
    aValueMember->SetReal(theValue);
    aLMWU->Init(aValueMember, theUnit);
    Handle(StepDimTol_DatumReferenceModifierWithValue) aModifWithVal = new StepDimTol_DatumReferenceModifierWithValue();
    aModifWithVal->Init(aType, aLMWU);
    StepDimTol_DatumReferenceModifier aModif;
    aModif.SetValue(aModifWithVal);
    aModifiers->SetValue(aModifNb, aModif);
  }

  // Simple modifiers
  for (Standard_Integer i = 1; i <= theModifiers.Length(); i++) {
    Handle(StepDimTol_SimpleDatumReferenceModifierMember) aSimpleModifMember = 
      new StepDimTol_SimpleDatumReferenceModifierMember();
    switch (theModifiers.Value(i)) {
      case XCAFDimTolObjects_DatumSingleModif_AnyCrossSection:
        aSimpleModifMember->SetEnumText(0, ".ANY_CROSS_SECTION.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Any_LongitudinalSection:
        aSimpleModifMember->SetEnumText(0, ".ANY_LONGITUDINAL_SECTION.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Basic:
        aSimpleModifMember->SetEnumText(0, ".BASIC.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_ContactingFeature:
        aSimpleModifMember->SetEnumText(0, ".CONTACTING_FEATURE.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintU:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_U.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintV:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_V.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintW:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_W.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintX:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_X.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintY:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_Y.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DegreeOfFreedomConstraintZ:
        aSimpleModifMember->SetEnumText(0, ".DEGREE_OF_FREEDOM_CONSTRAINT_Z.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_DistanceVariable:
        aSimpleModifMember->SetEnumText(0, ".DISTANCE_VARIABLE.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_FreeState:
        aSimpleModifMember->SetEnumText(0, ".FREE_STATE.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_LeastMaterialRequirement:
        aSimpleModifMember->SetEnumText(0, ".LEAST_MATERIAL_REQUIREMENT.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Line:
        aSimpleModifMember->SetEnumText(0, ".LINE.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_MajorDiameter:
        aSimpleModifMember->SetEnumText(0, ".MAJOR_DIAMETER.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_MaximumMaterialRequirement:
        aSimpleModifMember->SetEnumText(0, ".MAXIMUM_MATERIAL_REQUIREMENT.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_MinorDiameter:
        aSimpleModifMember->SetEnumText(0, ".MINOR_DIAMETER.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Orientation:
        aSimpleModifMember->SetEnumText(0, ".ORIENTATION.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_PitchDiameter:
        aSimpleModifMember->SetEnumText(0, ".PITCH_DIAMETER.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Plane:
        aSimpleModifMember->SetEnumText(0, ".PLANE.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Point:
        aSimpleModifMember->SetEnumText(0, ".POINT.");
        break;
      case XCAFDimTolObjects_DatumSingleModif_Translation:
        aSimpleModifMember->SetEnumText(0, ".TRANSLATION.");
        break;
    }
    StepDimTol_DatumReferenceModifier aModif;
    aModif.SetValue(aSimpleModifMember);
    aModifiers->SetValue(i, aModif);
  }

  return aModifiers;
}

//=======================================================================
//function : GetTolValueType
//purpose  : 
//=======================================================================
Handle(TCollection_HAsciiString) STEPCAFControl_GDTProperty::GetTolValueType(const XCAFDimTolObjects_GeomToleranceTypeValue& theType)
{
  switch (theType) {
    case XCAFDimTolObjects_GeomToleranceTypeValue_Diameter:
      return new TCollection_HAsciiString("cylindrical or circular");
    case XCAFDimTolObjects_GeomToleranceTypeValue_SphericalDiameter:
      return new TCollection_HAsciiString("spherical");
    default:
      return new TCollection_HAsciiString("unknown");
  }
}

//=======================================================================
//function : GetTessellation
//purpose  : 
//=======================================================================
Handle(StepVisual_TessellatedGeometricSet) STEPCAFControl_GDTProperty::GetTessellation(const TopoDS_Shape theShape)
{
  // Build coordinate list and curves
  NCollection_Handle<StepVisual_VectorOfHSequenceOfInteger> aCurves = new StepVisual_VectorOfHSequenceOfInteger;
  NCollection_Vector<gp_XYZ> aCoords;
  Standard_Integer aPntNb = 1;
  for (TopExp_Explorer aCurveIt(theShape, TopAbs_EDGE); aCurveIt.More(); aCurveIt.Next()) {
    Handle(TColStd_HSequenceOfInteger) aCurve = new TColStd_HSequenceOfInteger;
    // Find out type of edge curve
    Standard_Real aFirst = 0, aLast = 0;
    Handle(Geom_Curve) anEdgeCurve = BRep_Tool::Curve(TopoDS::Edge(aCurveIt.Current()), aFirst, aLast);
    if (anEdgeCurve.IsNull())
      continue;
    // Line
    if (anEdgeCurve->IsKind(STANDARD_TYPE(Geom_Line))) {
      for (TopExp_Explorer aVertIt(aCurveIt.Current(), TopAbs_VERTEX); aVertIt.More(); aVertIt.Next()) {
        aCoords.Append(BRep_Tool::Pnt(TopoDS::Vertex(aVertIt.Current())).XYZ());
        aCurve->Append(aPntNb);
        aPntNb++;
      }
    }
    // BSpline
    else {
      ShapeConstruct_Curve aSCC;
      Handle(Geom_BSplineCurve) aBSCurve = aSCC.ConvertToBSpline(anEdgeCurve,
          aFirst, aLast, Precision::Confusion());
      for (Standard_Integer i = 1; i <= aBSCurve->NbPoles(); i++) {
        aCoords.Append(aBSCurve->Pole(i).XYZ());
        aCurve->Append(aPntNb);
        aPntNb++;
      }
    }
    aCurves->Append(aCurve);
  }

  Handle(TColgp_HArray1OfXYZ) aPoints = new TColgp_HArray1OfXYZ(1, aCoords.Length());
  for (Standard_Integer i = 1; i <= aPoints->Length(); i++) {
    aPoints->SetValue(i, aCoords.Value(i - 1));
  }
  // STEP entities
  Handle(StepVisual_CoordinatesList) aCoordList = new StepVisual_CoordinatesList();
  aCoordList->Init(new TCollection_HAsciiString(), aPoints);
  Handle(StepVisual_TessellatedCurveSet) aCurveSet = new StepVisual_TessellatedCurveSet();
  aCurveSet->Init(new TCollection_HAsciiString(), aCoordList, aCurves);
  NCollection_Handle<StepVisual_Array1OfTessellatedItem> aTessItems = new StepVisual_Array1OfTessellatedItem(1, 1);
  aTessItems->SetValue(1, aCurveSet);
  Handle(StepVisual_TessellatedGeometricSet) aGeomSet = new StepVisual_TessellatedGeometricSet();
  aGeomSet->Init(new TCollection_HAsciiString(), aTessItems);
  return aGeomSet;
}
