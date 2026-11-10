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

#include <STEPCAFControl_GDTProperty.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom_BezierCurve.hxx>
#include <NCollection_Array1.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Triangle.hxx>
#include <StepVisual_CoordinatesList.hxx>
#include <StepVisual_ComplexTriangulatedSurfaceSet.hxx>
#include <StepVisual_TessellatedCurveSet.hxx>
#include <StepVisual_TessellatedGeometricSet.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <STEPCAFControl_Controller.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <STEPControl_Writer.hxx>
#include <StepData_StepModel.hxx>
#include <DESTEP_Parameters.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_DimTolTool.hxx>
#include <XCAFDoc_Dimension.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDimTolObjects_DimensionObject.hxx>
#include <StepVisual_AnnotationPlane.hxx>
#include <StepGeom_Plane.hxx>
#include <StepGeom_Axis2Placement3d.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepAP242_DraughtingModelItemAssociation.hxx>
#include <gp_Ax2.hxx>
#include <Standard_DomainError.hxx>

#include <gtest/gtest.h>

#include <limits>

TEST(STEPCAFControl_GDTPropertyTest, Tessellation_SamplesLocatedQuadraticInOutputUnits)
{
  NCollection_Array1<gp_Pnt> aPoles(size_t{3});
  aPoles.ChangeAt(0)                         = gp_Pnt(0, 0, 0);
  aPoles.ChangeAt(1)                         = gp_Pnt(1, 2, 0);
  aPoles.ChangeAt(2)                         = gp_Pnt(2, 0, 0);
  const occ::handle<Geom_BezierCurve> aCurve = new Geom_BezierCurve(aPoles);
  BRepBuilderAPI_MakeEdge             aMaker(aCurve);
  ASSERT_TRUE(aMaker.IsDone());
  TopoDS_Edge anEdge = aMaker.Edge();
  gp_Trsf     aLocation;
  aLocation.SetTranslation(gp_Vec(10, 20, 30));
  anEdge.Location(TopLoc_Location(aLocation));
  const double                                          aLengthFactor = 25.4;
  const double                                          aDeflection   = 0.01;
  const occ::handle<StepVisual_TessellatedGeometricSet> aSet =
    STEPCAFControl_GDTProperty::GetTessellation(anEdge, aLengthFactor, aDeflection);
  ASSERT_FALSE(aSet.IsNull());
  ASSERT_EQ(aSet->Items()->Length(), 1);
  const occ::handle<StepVisual_TessellatedCurveSet> aStrip =
    occ::down_cast<StepVisual_TessellatedCurveSet>(aSet->Items()->At(0));
  ASSERT_FALSE(aStrip.IsNull());
  const occ::handle<NCollection_HArray1<gp_XYZ>> aPoints = aStrip->CoordList()->Points();
  ASSERT_GT(aPoints->Length(), 3);
  for (size_t anIndex = 0; anIndex < aPoints->Size(); ++anIndex)
  {
    const gp_XYZ aPoint = aPoints->At(anIndex) * aLengthFactor - gp_XYZ(10, 20, 30);
    EXPECT_NEAR(aPoint.Y(), 2 * aPoint.X() - aPoint.X() * aPoint.X(), 1.e-12);
    EXPECT_NEAR(aPoint.Z(), 0, 1.e-12);
    if (anIndex != 0)
    {
      const gp_Pnt aFirst(aPoints->At(anIndex - 1) * aLengthFactor);
      const gp_Pnt aLast(aPoints->At(anIndex) * aLengthFactor);
      const double aParameter = (aFirst.X() + aLast.X() - 20) / 4;
      const gp_Pnt aMid       = aCurve->EvalD0(aParameter).Transformed(aLocation);
      EXPECT_LE(gp_Lin(aFirst, gp_Dir(gp_Vec(aFirst, aLast))).Distance(aMid), aDeflection + 1.e-12);
    }
  }
}

//==================================================================================================

TEST(STEPCAFControl_GDTPropertyTest, Tessellation_ScalesLocatedMeshWithoutScalingNormals)
{
  BRep_Builder aBuilder;
  TopoDS_Face  aFace;
  aBuilder.MakeFace(aFace);
  const occ::handle<Poly_Triangulation> aMesh = new Poly_Triangulation(3, 1, false, true);
  aMesh->SetNode(1, gp_Pnt(0, 0, 0));
  aMesh->SetNode(2, gp_Pnt(25.4, 0, 0));
  aMesh->SetNode(3, gp_Pnt(0, 25.4, 0));
  aMesh->SetTriangle(1, Poly_Triangle(1, 2, 3));
  for (int anIndex = 1; anIndex <= 3; ++anIndex)
  {
    aMesh->SetNormal(anIndex, gp_Dir(0, 0, 1));
  }
  aBuilder.UpdateFace(aFace, aMesh);
  gp_Trsf aLocation;
  aLocation.SetTranslation(gp_Vec(254, 508, 762));
  aFace.Location(TopLoc_Location(aLocation));
  const occ::handle<StepVisual_TessellatedGeometricSet> aSet =
    STEPCAFControl_GDTProperty::GetTessellation(aFace, 25.4);
  ASSERT_FALSE(aSet.IsNull());
  ASSERT_EQ(aSet->Items()->Length(), 1);
  const occ::handle<StepVisual_ComplexTriangulatedSurfaceSet> aTriangles =
    occ::down_cast<StepVisual_ComplexTriangulatedSurfaceSet>(aSet->Items()->At(0));
  ASSERT_FALSE(aTriangles.IsNull());
  const gp_XYZ& aFirst = aTriangles->Coordinates()->Points()->At(0);
  EXPECT_NEAR(aFirst.X(), 10, 1.e-12);
  EXPECT_NEAR(aFirst.Y(), 20, 1.e-12);
  EXPECT_NEAR(aFirst.Z(), 30, 1.e-12);
  EXPECT_NEAR(aTriangles->Coordinates()->Points()->At(1).X(), 11, 1.e-12);
  ASSERT_FALSE(aTriangles->Normals().IsNull());
  EXPECT_DOUBLE_EQ(aTriangles->Normals()->Value(1, 3), 1);
}

//==================================================================================================

TEST(STEPCAFControl_GDTPropertyTest, Writer_PresentationUsesTargetContextLengthUnit)
{
  STEPCAFControl_Controller::Init();
  for (const bool hasPriorRepresentation : {false, true})
  {
    SCOPED_TRACE(hasPriorRepresentation);
    const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
    XCAFDoc_DocumentTool::SetLengthUnit(aDocument, 0.001);
    BRepBuilderAPI_MakeEdge aMaker(gp_Pnt(0, 0, 0), gp_Pnt(25.4, 0, 0));
    ASSERT_TRUE(aMaker.IsDone());
    const TopoDS_Edge                    anEdge = aMaker.Edge();
    const occ::handle<XCAFDoc_ShapeTool> aShapeTool =
      XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
    const TDF_Label                       aShapeLabel = aShapeTool->AddShape(anEdge);
    const occ::handle<XCAFDoc_DimTolTool> aDimTool =
      XCAFDoc_DocumentTool::DimTolTool(aDocument->Main());
    const TDF_Label aDimension = aDimTool->AddDimension();
    aDimTool->SetDimension(aShapeLabel, aDimension);
    const occ::handle<XCAFDimTolObjects_DimensionObject> anObject =
      new XCAFDimTolObjects_DimensionObject;
    anObject->SetType(XCAFDimTolObjects_DimensionType_DimensionPresentation);
    anObject->SetPresentation(anEdge, new TCollection_HAsciiString("length"));
    XCAFDoc_Dimension::Set(aDimension)->SetObject(anObject);
    DESTEP_Parameters aParameters;
    aParameters.WriteSchema = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
    aParameters.WriteUnit   = UnitsMethods_LengthUnit_Inch;
    STEPCAFControl_Writer aWriter;
    aWriter.SetDimTolMode(true);
    if (hasPriorRepresentation)
    {
      BRepBuilderAPI_MakeEdge aPriorMaker(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0));
      ASSERT_TRUE(aPriorMaker.IsDone());
      const occ::handle<StepData_StepModel> aPriorModel = aWriter.ChangeWriter().Model();
      aPriorModel->InternalParameters.WriteUnit         = UnitsMethods_LengthUnit_Millimeter;
      ASSERT_EQ(aWriter.ChangeWriter().Transfer(aPriorMaker.Shape(), STEPControl_AsIs),
                IFSelect_RetDone);
      aPriorModel->SetWriteLengthUnit(25.4);
    }
    ASSERT_TRUE(aWriter.Transfer(aDocument, aParameters));
    const occ::handle<StepData_StepModel> aModel          = aWriter.ChangeWriter().Model();
    bool                                  hasPresentation = false;
    for (int anIndex = 1; anIndex <= aModel->NbEntities(); ++anIndex)
    {
      const occ::handle<StepVisual_TessellatedCurveSet> aCurveSet =
        occ::down_cast<StepVisual_TessellatedCurveSet>(aModel->Entity(anIndex));
      if (aCurveSet.IsNull())
      {
        continue;
      }
      hasPresentation                                        = true;
      const occ::handle<NCollection_HArray1<gp_XYZ>> aPoints = aCurveSet->CoordList()->Points();
      ASSERT_EQ(aPoints->Length(), 2);
      EXPECT_NEAR(aPoints->At(0).X(), 0, 1.e-12);
      EXPECT_NEAR(aPoints->At(1).X(), 1, 1.e-12);
    }
    EXPECT_TRUE(hasPresentation);
  }
}

//==================================================================================================

TEST(STEPCAFControl_GDTPropertyTest, Writer_PresentationOnlyPreservesAnnotationPlane)
{
  STEPCAFControl_Controller::Init();
  for (const XCAFDimTolObjects_DimensionType aType :
       {XCAFDimTolObjects_DimensionType_DimensionPresentation,
        XCAFDimTolObjects_DimensionType_CommonLabel})
  {
    SCOPED_TRACE(static_cast<int>(aType));
    const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
    XCAFDoc_DocumentTool::SetLengthUnit(aDocument, 0.001);
    BRepBuilderAPI_MakeEdge aMaker(gp_Pnt(0, 0, 0), gp_Pnt(25.4, 0, 0));
    ASSERT_TRUE(aMaker.IsDone());
    const TopoDS_Edge                    anEdge = aMaker.Edge();
    const occ::handle<XCAFDoc_ShapeTool> aShapeTool =
      XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
    const TDF_Label                       aShapeLabel = aShapeTool->AddShape(anEdge);
    const occ::handle<XCAFDoc_DimTolTool> aDimTool =
      XCAFDoc_DocumentTool::DimTolTool(aDocument->Main());
    const TDF_Label aDimension = aDimTool->AddDimension();
    if (aType == XCAFDimTolObjects_DimensionType_DimensionPresentation)
    {
      aDimTool->SetDimension(aShapeLabel, aDimension);
    }
    const occ::handle<XCAFDimTolObjects_DimensionObject> anObject =
      new XCAFDimTolObjects_DimensionObject;
    anObject->SetType(aType);
    anObject->SetPresentation(anEdge, new TCollection_HAsciiString("length"));
    anObject->SetPlane(gp_Ax2(gp_Pnt(), gp_Dir(0, 0, 1)));
    anObject->SetPointTextAttach(gp_Pnt(25.4, 50.8, 76.2));
    XCAFDoc_Dimension::Set(aDimension)->SetObject(anObject);
    DESTEP_Parameters aParameters;
    aParameters.WriteSchema = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
    aParameters.WriteUnit   = UnitsMethods_LengthUnit_Inch;
    STEPCAFControl_Writer aWriter;
    aWriter.SetDimTolMode(true);
    ASSERT_TRUE(aWriter.Transfer(aDocument, aParameters));
    const occ::handle<StepData_StepModel> aModel      = aWriter.ChangeWriter().Model();
    int                                   aPlaneCount = 0;
    for (int anIndex = 1; anIndex <= aModel->NbEntities(); ++anIndex)
    {
      if (const occ::handle<StepVisual_AnnotationPlane> anAnnotation =
            occ::down_cast<StepVisual_AnnotationPlane>(aModel->Entity(anIndex)))
      {
        ++aPlaneCount;
        const occ::handle<StepGeom_Plane> aPlane =
          occ::down_cast<StepGeom_Plane>(anAnnotation->Item());
        ASSERT_FALSE(aPlane.IsNull());
        const occ::handle<StepGeom_CartesianPoint>& aPoint = aPlane->Position()->Location();
        EXPECT_NEAR(aPoint->CoordinatesValue(1), 1, 1.e-12);
        EXPECT_NEAR(aPoint->CoordinatesValue(2), 2, 1.e-12);
        EXPECT_NEAR(aPoint->CoordinatesValue(3), 3, 1.e-12);
      }
      if (const occ::handle<StepAP242_DraughtingModelItemAssociation> anAssociation =
            occ::down_cast<StepAP242_DraughtingModelItemAssociation>(aModel->Entity(anIndex)))
      {
        EXPECT_TRUE(anAssociation->Name()->IsEmpty());
      }
    }
    EXPECT_EQ(aPlaneCount, 1);
  }
}

//==================================================================================================

TEST(STEPCAFControl_GDTPropertyTest, TessellationRejectsInvalidUnitsAndDeflections)
{
  const TopoDS_Shape aShape;
  for (const double aValue : {0.0,
                              -1.0,
                              std::numeric_limits<double>::infinity(),
                              std::numeric_limits<double>::quiet_NaN()})
  {
    EXPECT_THROW(STEPCAFControl_GDTProperty::GetTessellation(aShape, aValue), Standard_DomainError);
    EXPECT_THROW(STEPCAFControl_GDTProperty::GetTessellation(aShape, 1.0, aValue),
                 Standard_DomainError);
    EXPECT_THROW(STEPCAFControl_GDTProperty::GetTessellation(aShape, 1.0, 0.01, aValue),
                 Standard_DomainError);
  }
  EXPECT_TRUE(STEPCAFControl_GDTProperty::GetTessellation(aShape).IsNull());
}

//==================================================================================================

TEST(STEPCAFControl_GDTPropertyTest, Writer_RepeatedTransferPreservesCommonLabelUnits)
{
  STEPCAFControl_Controller::Init();
  enum class RootType
  {
    Shape,
    Assembly,
    Reference
  };
  for (const RootType aRootType : {RootType::Shape, RootType::Assembly, RootType::Reference})
  {
    SCOPED_TRACE(static_cast<int>(aRootType));
    const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
    XCAFDoc_DocumentTool::SetLengthUnit(aDocument, 0.001);
    BRepBuilderAPI_MakeEdge aMaker(gp_Pnt(0, 0, 0), gp_Pnt(25.4, 0, 0));
    ASSERT_TRUE(aMaker.IsDone());
    const TopoDS_Edge                    anEdge = aMaker.Edge();
    const occ::handle<XCAFDoc_ShapeTool> aShapeTool =
      XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
    const TDF_Label aShapeLabel = aShapeTool->AddShape(anEdge);
    TDF_Label       aTransferLabel;
    if (aRootType == RootType::Assembly)
    {
      const TDF_Label anAssembly = aShapeTool->NewShape();
      const TDF_Label aComponent =
        aShapeTool->AddComponent(anAssembly, aShapeLabel, TopLoc_Location());
      ASSERT_FALSE(aComponent.IsNull());
      aShapeTool->UpdateAssemblies();
      ASSERT_TRUE(XCAFDoc_ShapeTool::IsAssembly(anAssembly));
    }
    else if (aRootType == RootType::Reference)
    {
      gp_Trsf aLocation;
      aLocation.SetTranslation(gp_Vec(10, 0, 0));
      ASSERT_TRUE(aShapeTool->SetLocation(aShapeLabel, TopLoc_Location(aLocation), aTransferLabel));
      ASSERT_TRUE(XCAFDoc_ShapeTool::IsReference(aTransferLabel));
      ASSERT_TRUE(XCAFDoc_ShapeTool::IsFree(aTransferLabel));
    }
    const occ::handle<XCAFDoc_DimTolTool> aDimTool =
      XCAFDoc_DocumentTool::DimTolTool(aDocument->Main());
    const TDF_Label                                      aDimension = aDimTool->AddDimension();
    const occ::handle<XCAFDimTolObjects_DimensionObject> anObject =
      new XCAFDimTolObjects_DimensionObject;
    anObject->SetType(XCAFDimTolObjects_DimensionType_CommonLabel);
    anObject->SetPresentation(anEdge, new TCollection_HAsciiString("length"));
    anObject->SetPlane(gp_Ax2(gp_Pnt(), gp_Dir(0, 0, 1)));
    anObject->SetPointTextAttach(gp_Pnt(25.4, 50.8, 76.2));
    XCAFDoc_Dimension::Set(aDimension)->SetObject(anObject);
    DESTEP_Parameters aParameters;
    aParameters.WriteSchema = DESTEP_Parameters::WriteMode_StepSchema_AP242DIS;
    aParameters.WriteUnit   = UnitsMethods_LengthUnit_Inch;
    STEPCAFControl_Writer aWriter;
    aWriter.SetDimTolMode(true);
    const occ::handle<StepData_StepModel> aModel = aWriter.ChangeWriter().Model();
    for (int aTransferIndex = 0; aTransferIndex < 2; ++aTransferIndex)
    {
      SCOPED_TRACE(aTransferIndex);
      const int aFirstNewEntity = aModel->NbEntities() + 1;
      ASSERT_TRUE(aRootType == RootType::Reference ? aWriter.Transfer(aTransferLabel, aParameters)
                                                   : aWriter.Transfer(aDocument, aParameters));
      int aCurveCount = 0;
      int aPlaneCount = 0;
      for (int anIndex = aFirstNewEntity; anIndex <= aModel->NbEntities(); ++anIndex)
      {
        if (const occ::handle<StepVisual_TessellatedCurveSet> aCurveSet =
              occ::down_cast<StepVisual_TessellatedCurveSet>(aModel->Entity(anIndex)))
        {
          ++aCurveCount;
          const occ::handle<NCollection_HArray1<gp_XYZ>> aPoints = aCurveSet->CoordList()->Points();
          ASSERT_EQ(aPoints->Length(), 2);
          EXPECT_NEAR(aPoints->At(0).X(), 0, 1.e-12);
          EXPECT_NEAR(aPoints->At(1).X(), 1, 1.e-12);
        }
        if (const occ::handle<StepVisual_AnnotationPlane> anAnnotation =
              occ::down_cast<StepVisual_AnnotationPlane>(aModel->Entity(anIndex)))
        {
          ++aPlaneCount;
          const occ::handle<StepGeom_Plane> aPlane =
            occ::down_cast<StepGeom_Plane>(anAnnotation->Item());
          ASSERT_FALSE(aPlane.IsNull());
          const occ::handle<StepGeom_CartesianPoint>& aPoint = aPlane->Position()->Location();
          EXPECT_NEAR(aPoint->CoordinatesValue(1), 1, 1.e-12);
          EXPECT_NEAR(aPoint->CoordinatesValue(2), 2, 1.e-12);
          EXPECT_NEAR(aPoint->CoordinatesValue(3), 3, 1.e-12);
        }
      }
      EXPECT_EQ(aCurveCount, 1);
      EXPECT_EQ(aPlaneCount, 1);
    }
  }
}
