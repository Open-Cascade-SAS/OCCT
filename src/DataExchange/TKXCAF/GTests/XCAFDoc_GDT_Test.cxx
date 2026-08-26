// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_Sequence.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <XCAFDimTolObjects_DatumObject.hxx>
#include <XCAFDimTolObjects_DatumSingleModif.hxx>
#include <XCAFDimTolObjects_DimensionFormVariance.hxx>
#include <XCAFDimTolObjects_DimensionGrade.hxx>
#include <XCAFDimTolObjects_DimensionModif.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <XCAFDimTolObjects_DimensionQualifier.hxx>
#include <XCAFDimTolObjects_DimensionType.hxx>
#include <XCAFDimTolObjects_GeomToleranceModif.hxx>
#include <XCAFDimTolObjects_GeomToleranceObject.hxx>
#include <XCAFDimTolObjects_GeomToleranceType.hxx>
#include <XCAFDimTolObjects_GeomToleranceTypeValue.hxx>
#include <XCAFDoc_Datum.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_GeomTolerance.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

namespace
{
struct GdtContext
{
  occ::handle<TDocStd_Application> Application;
  occ::handle<TDocStd_Document>    Document;
  occ::handle<XCAFDoc_ShapeTool>   ShapeTool;
  occ::handle<XCAFDoc_DimTolTool>  DimTolTool;
  TDF_Label                        Shape1;
  TDF_Label                        Shape2;
};

static GdtContext NewContext()
{
  GdtContext aContext;
  aContext.Application = new TDocStd_Application();
  aContext.Application->NewDocument("BinXCAF", aContext.Document);
  aContext.ShapeTool  = XCAFDoc_DocumentTool::ShapeTool(aContext.Document->Main());
  aContext.DimTolTool = XCAFDoc_DocumentTool::DimTolTool(aContext.Document->Main());
  aContext.Shape1     = aContext.ShapeTool->AddShape(BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape());
  aContext.Shape2     = aContext.ShapeTool->AddShape(BRepPrimAPI_MakeBox(20.0, 20.0, 20.0).Shape());
  return aContext;
}

static TDF_Label AddDimension(GdtContext& theContext, const bool theHasSecondShape)
{
  const TDF_Label aDimension = theContext.DimTolTool->AddDimension();
  if (theHasSecondShape)
  {
    theContext.DimTolTool->SetDimension(theContext.Shape1, theContext.Shape2, aDimension);
  }
  else
  {
    theContext.DimTolTool->SetDimension(theContext.Shape1, aDimension);
  }
  EXPECT_TRUE(theContext.DimTolTool->IsDimension(aDimension));
  return aDimension;
}

static occ::handle<XCAFDimTolObjects_DimensionObject> DimensionObject(const TDF_Label& theLabel)
{
  occ::handle<XCAFDoc_Dimension> anAttribute;
  EXPECT_TRUE(theLabel.FindAttribute(XCAFDoc_Dimension::GetID(), anAttribute));
  return anAttribute->GetObject();
}

static TDF_Label AddGeomTolerance(GdtContext& theContext)
{
  const TDF_Label aTolerance = theContext.DimTolTool->AddGeomTolerance();
  theContext.DimTolTool->SetGeomTolerance(theContext.Shape1, aTolerance);
  EXPECT_TRUE(theContext.DimTolTool->IsGeomTolerance(aTolerance));
  return aTolerance;
}

static occ::handle<XCAFDimTolObjects_GeomToleranceObject> GeomToleranceObject(
  const TDF_Label& theLabel)
{
  occ::handle<XCAFDoc_GeomTolerance> anAttribute;
  EXPECT_TRUE(theLabel.FindAttribute(XCAFDoc_GeomTolerance::GetID(), anAttribute));
  return anAttribute->GetObject();
}

static TDF_Label AddDatum(GdtContext&      theContext,
                          const TDF_Label& theTolerance,
                          const char*      theName,
                          const int        thePosition,
                          const bool       theAddDistanceModifier)
{
  const TDF_Label                 aDatum = theContext.DimTolTool->AddDatum();
  NCollection_Sequence<TDF_Label> aShapeLabels;
  aShapeLabels.Append(theContext.Shape1);
  theContext.DimTolTool->SetDatum(aShapeLabels, aDatum);

  occ::handle<XCAFDoc_Datum> anAttribute;
  EXPECT_TRUE(aDatum.FindAttribute(XCAFDoc_Datum::GetID(), anAttribute));
  occ::handle<XCAFDimTolObjects_DatumObject> anObject = anAttribute->GetObject();
  anObject->SetName(new TCollection_HAsciiString(theName));
  anObject->SetPosition(thePosition);
  if (theAddDistanceModifier)
  {
    anObject->AddModifier(XCAFDimTolObjects_DatumSingleModif_DistanceVariable);
  }
  anAttribute->SetObject(anObject);
  theContext.DimTolTool->SetDatumToGeomTol(aDatum, theTolerance);
  return aDatum;
}

static TDF_Label AddBareDatum(GdtContext& theContext,
                              const char* theName,
                              const bool  theWithPlane,
                              const bool  theWithPoint)
{
  const TDF_Label            aDatum = theContext.DimTolTool->AddDatum();
  occ::handle<XCAFDoc_Datum> anAttribute;
  EXPECT_TRUE(aDatum.FindAttribute(XCAFDoc_Datum::GetID(), anAttribute));
  occ::handle<XCAFDimTolObjects_DatumObject> anObject = anAttribute->GetObject();
  anObject->SetName(new TCollection_HAsciiString(theName));
  if (theWithPlane)
  {
    anObject->SetPlane(gp_Ax2(gp_Pnt(6.0, 6.0, 6.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)));
  }
  if (theWithPoint)
  {
    anObject->SetPoint(gp_Pnt(7.0, 7.0, 7.0));
  }
  anAttribute->SetObject(anObject);
  return aDatum;
}

static occ::handle<XCAFDimTolObjects_DatumObject> DatumObject(const TDF_Label& theLabel)
{
  occ::handle<XCAFDoc_Datum> anAttribute;
  EXPECT_TRUE(theLabel.FindAttribute(XCAFDoc_Datum::GetID(), anAttribute));
  return anAttribute->GetObject();
}

static NCollection_Sequence<TDF_Label> DatumReferences(const TDF_Label&  theTolerance,
                                                       const GdtContext& theContext)
{
  NCollection_Sequence<TDF_Label> aDatums;
  EXPECT_TRUE(theContext.DimTolTool->GetDatumOfTolerLabels(theTolerance, aDatums));
  return aDatums;
}
} // namespace

// gdt/dimensions/A1: a curve-length dimension stores its type and value.
TEST(XCAFDoc_GDT_Test, GdtDimensions_A1_CurveLength)
{
  GdtContext                                     aContext   = NewContext();
  const TDF_Label                                aDimension = AddDimension(aContext, false);
  occ::handle<XCAFDimTolObjects_DimensionObject> anObject   = DimensionObject(aDimension);
  anObject->SetType(XCAFDimTolObjects_DimensionType_Size_CurveLength);
  anObject->SetValue(1.5);
  XCAFDoc_Dimension::Set(aDimension)->SetObject(anObject);

  anObject = DimensionObject(aDimension);
  EXPECT_EQ(anObject->GetType(), XCAFDimTolObjects_DimensionType_Size_CurveLength);
  EXPECT_DOUBLE_EQ(anObject->GetValue(), 1.5);
}

// gdt/dimensions/A2: a diameter dimension stores its maximum qualifier.
TEST(XCAFDoc_GDT_Test, GdtDimensions_A2_DiameterQualifier)
{
  GdtContext                                     aContext   = NewContext();
  const TDF_Label                                aDimension = AddDimension(aContext, false);
  occ::handle<XCAFDimTolObjects_DimensionObject> anObject   = DimensionObject(aDimension);
  anObject->SetType(XCAFDimTolObjects_DimensionType_Size_Diameter);
  anObject->SetValue(2.5);
  anObject->SetQualifier(XCAFDimTolObjects_DimensionQualifier_Max);
  XCAFDoc_Dimension::Set(aDimension)->SetObject(anObject);

  anObject = DimensionObject(aDimension);
  EXPECT_EQ(anObject->GetType(), XCAFDimTolObjects_DimensionType_Size_Diameter);
  EXPECT_DOUBLE_EQ(anObject->GetValue(), 2.5);
  EXPECT_TRUE(anObject->HasQualifier());
  EXPECT_EQ(anObject->GetQualifier(), XCAFDimTolObjects_DimensionQualifier_Max);
}

// gdt/dimensions/A3: a dimension may store a lower and upper range.
TEST(XCAFDoc_GDT_Test, GdtDimensions_A3_Range)
{
  GdtContext                                     aContext   = NewContext();
  const TDF_Label                                aDimension = AddDimension(aContext, false);
  occ::handle<XCAFDimTolObjects_DimensionObject> anObject   = DimensionObject(aDimension);
  anObject->SetType(XCAFDimTolObjects_DimensionType_Size_CurveLength);
  anObject->SetLowerBound(1.5);
  anObject->SetUpperBound(2.5);
  XCAFDoc_Dimension::Set(aDimension)->SetObject(anObject);

  anObject = DimensionObject(aDimension);
  EXPECT_EQ(anObject->GetType(), XCAFDimTolObjects_DimensionType_Size_CurveLength);
  EXPECT_TRUE(anObject->IsDimWithRange());
  EXPECT_DOUBLE_EQ(anObject->GetLowerBound(), 1.5);
  EXPECT_DOUBLE_EQ(anObject->GetUpperBound(), 2.5);
}

// gdt/dimensions/A4: class-of-tolerance, decimal places, and modifiers are persisted.
TEST(XCAFDoc_GDT_Test, GdtDimensions_A4_ClassAndModifiers)
{
  GdtContext                                     aContext   = NewContext();
  const TDF_Label                                aDimension = AddDimension(aContext, false);
  occ::handle<XCAFDimTolObjects_DimensionObject> anObject   = DimensionObject(aDimension);
  anObject->SetType(XCAFDimTolObjects_DimensionType_Size_Diameter);
  anObject->SetValue(2.5);
  anObject->SetClassOfTolerance(true,
                                XCAFDimTolObjects_DimensionFormVariance_F,
                                XCAFDimTolObjects_DimensionGrade_IT6);
  anObject->SetNbOfDecimalPlaces(3, 3);
  anObject->AddModifier(XCAFDimTolObjects_DimensionModif_AverageSize);
  anObject->AddModifier(XCAFDimTolObjects_DimensionModif_ControlledRadius);
  XCAFDoc_Dimension::Set(aDimension)->SetObject(anObject);

  anObject                                       = DimensionObject(aDimension);
  bool                                    aHole  = false;
  XCAFDimTolObjects_DimensionFormVariance aForm  = XCAFDimTolObjects_DimensionFormVariance_None;
  XCAFDimTolObjects_DimensionGrade        aGrade = XCAFDimTolObjects_DimensionGrade_IT01;
  EXPECT_TRUE(anObject->GetClassOfTolerance(aHole, aForm, aGrade));
  EXPECT_TRUE(aHole);
  EXPECT_EQ(aForm, XCAFDimTolObjects_DimensionFormVariance_F);
  EXPECT_EQ(aGrade, XCAFDimTolObjects_DimensionGrade_IT6);

  int aLeft  = 0;
  int aRight = 0;
  anObject->GetNbOfDecimalPlaces(aLeft, aRight);
  EXPECT_EQ(aLeft, 3);
  EXPECT_EQ(aRight, 3);

  const NCollection_Sequence<XCAFDimTolObjects_DimensionModif> aModifiers =
    anObject->GetModifiers();
  ASSERT_EQ(aModifiers.Length(), 2);
  EXPECT_EQ(aModifiers.Value(1), XCAFDimTolObjects_DimensionModif_AverageSize);
  EXPECT_EQ(aModifiers.Value(2), XCAFDimTolObjects_DimensionModif_ControlledRadius);
}

// gdt/dimensions/A5: a linear-location dimension stores two points and plus/minus tolerances.
TEST(XCAFDoc_GDT_Test, GdtDimensions_A5_LinearDistance)
{
  GdtContext                                     aContext   = NewContext();
  const TDF_Label                                aDimension = AddDimension(aContext, true);
  occ::handle<XCAFDimTolObjects_DimensionObject> anObject   = DimensionObject(aDimension);
  anObject->SetType(XCAFDimTolObjects_DimensionType_Location_LinearDistance);
  anObject->SetValue(6.0);
  ASSERT_TRUE(anObject->SetLowerTolValue(-0.001));
  ASSERT_TRUE(anObject->SetUpperTolValue(0.002));
  anObject->SetPoint(gp_Pnt(10.0, 10.0, 10.0));
  anObject->SetPoint2(gp_Pnt(20.0, 20.0, 20.0));
  XCAFDoc_Dimension::Set(aDimension)->SetObject(anObject);

  anObject = DimensionObject(aDimension);
  EXPECT_EQ(anObject->GetType(), XCAFDimTolObjects_DimensionType_Location_LinearDistance);
  EXPECT_DOUBLE_EQ(anObject->GetValue(), 6.0);
  EXPECT_TRUE(anObject->IsDimWithPlusMinusTolerance());
  EXPECT_DOUBLE_EQ(anObject->GetLowerTolValue(), -0.001);
  EXPECT_DOUBLE_EQ(anObject->GetUpperTolValue(), 0.002);
  EXPECT_TRUE(anObject->HasPoint());
  EXPECT_TRUE(anObject->HasPoint2());
  EXPECT_TRUE(anObject->GetPoint().IsEqual(gp_Pnt(10.0, 10.0, 10.0), 0.0));
  EXPECT_TRUE(anObject->GetPoint2().IsEqual(gp_Pnt(20.0, 20.0, 20.0), 0.0));
}

// gdt/dimensions/A6: an oriented dimension stores direction and plus/minus tolerances.
TEST(XCAFDoc_GDT_Test, GdtDimensions_A6_Oriented)
{
  GdtContext                                     aContext   = NewContext();
  const TDF_Label                                aDimension = AddDimension(aContext, true);
  occ::handle<XCAFDimTolObjects_DimensionObject> anObject   = DimensionObject(aDimension);
  anObject->SetType(XCAFDimTolObjects_DimensionType_Location_Oriented);
  anObject->SetValue(6.0);
  ASSERT_TRUE(anObject->SetLowerTolValue(-0.001));
  ASSERT_TRUE(anObject->SetUpperTolValue(0.002));
  ASSERT_TRUE(anObject->SetDirection(gp_Dir(1.0, 1.0, 0.0)));
  XCAFDoc_Dimension::Set(aDimension)->SetObject(anObject);

  anObject = DimensionObject(aDimension);
  gp_Dir aDirection;
  EXPECT_EQ(anObject->GetType(), XCAFDimTolObjects_DimensionType_Location_Oriented);
  EXPECT_DOUBLE_EQ(anObject->GetValue(), 6.0);
  EXPECT_TRUE(anObject->IsDimWithPlusMinusTolerance());
  EXPECT_DOUBLE_EQ(anObject->GetLowerTolValue(), -0.001);
  EXPECT_DOUBLE_EQ(anObject->GetUpperTolValue(), 0.002);
  ASSERT_TRUE(anObject->GetDirection(aDirection));
  EXPECT_NEAR(aDirection.X(), 0.7071067811865476, 1.0e-15);
  EXPECT_NEAR(aDirection.Y(), 0.7071067811865476, 1.0e-15);
  EXPECT_DOUBLE_EQ(aDirection.Z(), 0.0);
}

// gdt/tolerances/A1: a tolerance stores datum, type, material, zone, and modifiers.
TEST(XCAFDoc_GDT_Test, GdtTolerances_A1_FullDefinition)
{
  GdtContext      aContext   = NewContext();
  const TDF_Label aTolerance = AddGeomTolerance(aContext);
  const TDF_Label aDatum     = AddDatum(aContext, aTolerance, "A", 1, true);

  occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObject = GeomToleranceObject(aTolerance);
  anObject->SetValue(0.5);
  anObject->SetType(XCAFDimTolObjects_GeomToleranceType_Angularity);
  anObject->SetTypeOfValue(XCAFDimTolObjects_GeomToleranceTypeValue_Diameter);
  anObject->SetMaterialRequirementModifier(XCAFDimTolObjects_GeomToleranceMatReqModif_M);
  anObject->SetZoneModifier(XCAFDimTolObjects_GeomToleranceZoneModif_Projected);
  anObject->SetValueOfZoneModifier(1.0);
  anObject->AddModifier(XCAFDimTolObjects_GeomToleranceModif_Major_Diameter);
  anObject->AddModifier(XCAFDimTolObjects_GeomToleranceModif_Common_Zone);
  anObject->SetMaxValueModifier(1.5);
  XCAFDoc_GeomTolerance::Set(aTolerance)->SetObject(anObject);

  anObject = GeomToleranceObject(aTolerance);
  EXPECT_DOUBLE_EQ(anObject->GetValue(), 0.5);
  EXPECT_EQ(anObject->GetType(), XCAFDimTolObjects_GeomToleranceType_Angularity);
  EXPECT_EQ(anObject->GetTypeOfValue(), XCAFDimTolObjects_GeomToleranceTypeValue_Diameter);
  EXPECT_EQ(anObject->GetMaterialRequirementModifier(),
            XCAFDimTolObjects_GeomToleranceMatReqModif_M);
  EXPECT_EQ(anObject->GetZoneModifier(), XCAFDimTolObjects_GeomToleranceZoneModif_Projected);
  EXPECT_DOUBLE_EQ(anObject->GetValueOfZoneModifier(), 1.0);
  EXPECT_DOUBLE_EQ(anObject->GetMaxValueModifier(), 1.5);
  const NCollection_Sequence<XCAFDimTolObjects_GeomToleranceModif> aModifiers =
    anObject->GetModifiers();
  ASSERT_EQ(aModifiers.Length(), 2);
  EXPECT_EQ(aModifiers.Value(1), XCAFDimTolObjects_GeomToleranceModif_Major_Diameter);
  EXPECT_EQ(aModifiers.Value(2), XCAFDimTolObjects_GeomToleranceModif_Common_Zone);

  const NCollection_Sequence<TDF_Label> aDatums = DatumReferences(aTolerance, aContext);
  ASSERT_EQ(aDatums.Length(), 1);
  EXPECT_EQ(aDatums.Value(1), aDatum);
  const occ::handle<XCAFDimTolObjects_DatumObject> aDatumObject = DatumObject(aDatum);
  ASSERT_FALSE(aDatumObject.IsNull());
  EXPECT_STREQ(aDatumObject->GetName()->ToCString(), "A");
  EXPECT_EQ(aDatumObject->GetPosition(), 1);
  const NCollection_Sequence<XCAFDimTolObjects_DatumSingleModif> aDatumModifiers =
    aDatumObject->GetModifiers();
  ASSERT_EQ(aDatumModifiers.Length(), 1);
  EXPECT_EQ(aDatumModifiers.Value(1), XCAFDimTolObjects_DatumSingleModif_DistanceVariable);
}

// gdt/tolerances/A2: two datums can reference one geometric tolerance.
TEST(XCAFDoc_GDT_Test, GdtTolerances_A2_TwoDatums)
{
  GdtContext      aContext   = NewContext();
  const TDF_Label aTolerance = AddGeomTolerance(aContext);
  const TDF_Label aDatumA    = AddDatum(aContext, aTolerance, "A", 1, true);
  const TDF_Label aDatumB    = AddDatum(aContext, aTolerance, "B", 2, false);

  occ::handle<XCAFDimTolObjects_GeomToleranceObject> anObject = GeomToleranceObject(aTolerance);
  anObject->SetValue(0.5);
  XCAFDoc_GeomTolerance::Set(aTolerance)->SetObject(anObject);

  anObject = GeomToleranceObject(aTolerance);
  EXPECT_DOUBLE_EQ(anObject->GetValue(), 0.5);
  const NCollection_Sequence<TDF_Label> aDatums = DatumReferences(aTolerance, aContext);
  ASSERT_EQ(aDatums.Length(), 2);
  EXPECT_EQ(aDatums.Value(1), aDatumA);
  EXPECT_EQ(aDatums.Value(2), aDatumB);

  const occ::handle<XCAFDimTolObjects_DatumObject> aDatumObjectA = DatumObject(aDatumA);
  const occ::handle<XCAFDimTolObjects_DatumObject> aDatumObjectB = DatumObject(aDatumB);
  EXPECT_STREQ(aDatumObjectA->GetName()->ToCString(), "A");
  EXPECT_EQ(aDatumObjectA->GetPosition(), 1);
  EXPECT_STREQ(aDatumObjectB->GetName()->ToCString(), "B");
  EXPECT_EQ(aDatumObjectB->GetPosition(), 2);
  EXPECT_EQ(aDatumObjectA->GetModifiers().Length(), 1);
  EXPECT_EQ(aDatumObjectB->GetModifiers().Length(), 0);
}

// The datum point is read back from its own array. It used to be built with the X of the
// annotation plane's location, which also dereferenced a null handle when there was no plane.
TEST(XCAFDoc_GDT_Test, GdtDatum_PointIsReadFromItsOwnArray)
{
  GdtContext      aContext = NewContext();
  const TDF_Label aDatum   = AddBareDatum(aContext, "A", true, true);

  const occ::handle<XCAFDimTolObjects_DatumObject> anObject = DatumObject(aDatum);
  ASSERT_TRUE(anObject->HasPlane());
  ASSERT_TRUE(anObject->HasPoint());
  EXPECT_DOUBLE_EQ(anObject->GetPlane().Location().X(), 6.0);
  EXPECT_DOUBLE_EQ(anObject->GetPoint().X(), 7.0);
  EXPECT_DOUBLE_EQ(anObject->GetPoint().Y(), 7.0);
  EXPECT_DOUBLE_EQ(anObject->GetPoint().Z(), 7.0);
}

// A datum may carry a point without an annotation plane, in which case the plane's array is never
// bound and must not be read.
TEST(XCAFDoc_GDT_Test, GdtDatum_PointWithoutPlane)
{
  GdtContext      aContext = NewContext();
  const TDF_Label aDatum   = AddBareDatum(aContext, "A", false, true);

  const occ::handle<XCAFDimTolObjects_DatumObject> anObject = DatumObject(aDatum);
  ASSERT_FALSE(anObject->HasPlane());
  ASSERT_TRUE(anObject->HasPoint());
  EXPECT_DOUBLE_EQ(anObject->GetPoint().X(), 7.0);
  EXPECT_DOUBLE_EQ(anObject->GetPoint().Y(), 7.0);
  EXPECT_DOUBLE_EQ(anObject->GetPoint().Z(), 7.0);
}

// A datum with a plane and no point is what a STEP import produces, and is unaffected.
TEST(XCAFDoc_GDT_Test, GdtDatum_PlaneWithoutPoint)
{
  GdtContext      aContext = NewContext();
  const TDF_Label aDatum   = AddBareDatum(aContext, "A", true, false);

  const occ::handle<XCAFDimTolObjects_DatumObject> anObject = DatumObject(aDatum);
  ASSERT_TRUE(anObject->HasPlane());
  EXPECT_FALSE(anObject->HasPoint());
  EXPECT_DOUBLE_EQ(anObject->GetPlane().Location().X(), 6.0);
  EXPECT_DOUBLE_EQ(anObject->GetPlane().Location().Y(), 6.0);
  EXPECT_DOUBLE_EQ(anObject->GetPlane().Location().Z(), 6.0);
}
