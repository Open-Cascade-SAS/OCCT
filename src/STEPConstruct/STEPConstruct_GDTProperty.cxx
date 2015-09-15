// Created on: 1999-09-09
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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

#include <STEPConstruct_GDTProperty.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
   
//=======================================================================
//function : STEPConstruct_GDTProperty
//purpose  : 
//=======================================================================

STEPConstruct_GDTProperty::STEPConstruct_GDTProperty ()
{
}

//=======================================================================
//function : getDimModifiers
//purpose  : 
//=======================================================================
void STEPConstruct_GDTProperty::GetDimModifiers(const Handle(StepRepr_CompoundRepresentationItem)& theCRI,
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
void STEPConstruct_GDTProperty::GetDimClassOfTolerance(const Handle(StepShape_LimitsAndFits)& theLAF,
                                   Standard_Boolean theHolle,
                                   XCAFDimTolObjects_DimensionFormVariance theFV,
                                   XCAFDimTolObjects_DimensionGrade theG)
{
  Handle(TCollection_HAsciiString) aFormV = theLAF->FormVariance();
  Handle(TCollection_HAsciiString) aGrade = theLAF->Grade();
  theFV = XCAFDimTolObjects_DimensionFormVariance_None;
  Standard_Boolean aFound;
  theHolle = Standard_False;
  //it is not verified information
  for(Standard_Integer c = 0; c <= 1; c++)
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
  if(aGrade->IsSameString(aStr))
  {
    theG = XCAFDimTolObjects_DimensionGrade_IT01;
  }
  else
  {
    theG = (XCAFDimTolObjects_DimensionGrade)(aGrade->IntegerValue()+1);
  }
}

//=======================================================================
//function : getDimType
//purpose  : 
//=======================================================================
Standard_Boolean STEPConstruct_GDTProperty::GetDimType(const Handle(TCollection_HAsciiString)& theName,
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

    if(theType != XCAFDimTolObjects_DimensionType_Location_None)
    {
      return Standard_True;
    }
    return Standard_False;
}


//=======================================================================
//function : DatumTargetType
//purpose  : 
//=======================================================================
Standard_Boolean STEPConstruct_GDTProperty::GetDatumTargetType(const Handle(TCollection_HAsciiString)& theDescription,
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
Standard_Boolean STEPConstruct_GDTProperty::GetDimQualifierType(const Handle(TCollection_HAsciiString)& theDescription,
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
Standard_Boolean STEPConstruct_GDTProperty::GetTolValueType(const Handle(TCollection_HAsciiString)& theDescription,
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