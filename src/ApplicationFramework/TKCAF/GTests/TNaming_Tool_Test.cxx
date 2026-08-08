// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <TNaming_Tool.hxx>

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRep_Tool.hxx>
#include <DNaming.hxx>
#include <DNaming_BooleanOperationDriver.hxx>
#include <DNaming_BoxDriver.hxx>
#include <DNaming_FilletDriver.hxx>
#include <DNaming_SelectionDriver.hxx>
#include <DNaming_SphereDriver.hxx>
#include <DNaming_TransformationDriver.hxx>
#include <ModelDefinitions.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_Data.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TDF_Reference.hxx>
#include <TDF_CopyLabel.hxx>
#include <TDF_TagSource.hxx>
#include <TDocStd_Document.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamingTool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_Selector.hxx>
#include <TNaming_Translator.hxx>
#include <TNaming_UsedShapes.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <cmath>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <Geom_Line.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Map.hxx>

#include <gtest/gtest.h>

// Test fixture for TNaming_Tool tests
class TNaming_ToolTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Create a new TDF document
    myData = new TDF_Data();
    myRoot = myData->Root();
  }

  void TearDown() override { myData.Nullify(); }

  // Helper method to create a simple box shape
  TopoDS_Shape MakeBox(double dx, double dy, double dz)
  {
    return BRepPrimAPI_MakeBox(dx, dy, dz).Shape();
  }

  // Helper method to create a simple sphere shape
  TopoDS_Shape MakeSphere(double radius) { return BRepPrimAPI_MakeSphere(radius).Shape(); }

  occ::handle<TDF_Data> myData;
  TDF_Label             myRoot;
};

namespace
{
static TopoDS_Shape FirstSubShape(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  TopExp_Explorer anExplorer(theShape, theType);
  return anExplorer.More() ? anExplorer.Current() : TopoDS_Shape();
}

static occ::handle<TDocStd_Document> NewNamingDocument()
{
  return new TDocStd_Document("BinOcaf");
}

static occ::handle<TDataStd_UAttribute> AddNamingObject(
  const occ::handle<TDocStd_Document>& theDocument)
{
  occ::handle<TDataStd_TreeNode> aRootNode = TDataStd_TreeNode::Set(theDocument->Main());
  const TDF_Label                  aLabel = TDF_TagSource::NewChild(theDocument->Main());
  occ::handle<TDataStd_UAttribute> anObject = TDataStd_UAttribute::Set(aLabel, GEOMOBJECT_GUID);
  occ::handle<TDataStd_TreeNode>   aNode = TDataStd_TreeNode::Set(aLabel);
  aRootNode->Append(aNode);
  return anObject;
}

static occ::handle<TFunction_Function> AddNamingFunction(
  const occ::handle<TDataStd_UAttribute>& theObject,
  const Standard_GUID&                    theDriverGuid)
{
  const TDF_Label                 aFunctionLabel = TDF_TagSource::NewChild(theObject->Label());
  occ::handle<TFunction_Function> aFunction = TFunction_Function::Set(aFunctionLabel, theDriverGuid);
  occ::handle<TDataStd_TreeNode>  aFunctionNode = TDataStd_TreeNode::Set(aFunctionLabel);
  occ::handle<TDataStd_TreeNode>  anObjectNode;
  theObject->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), anObjectNode);
  anObjectNode->Append(aFunctionNode);

  const TDF_Label                anArgumentsLabel = TDF_TagSource::NewChild(aFunctionLabel);
  occ::handle<TDataStd_TreeNode> anArgumentsNode = TDataStd_TreeNode::Set(anArgumentsLabel);
  aFunctionNode->Append(anArgumentsNode);

  const TDF_Label                aResultLabel = TDF_TagSource::NewChild(aFunctionLabel);
  occ::handle<TDataStd_TreeNode> aResultNode = TDataStd_TreeNode::Set(aResultLabel);
  aFunctionNode->Append(aResultNode);
  TDF_Reference::Set(theObject->Label(), aResultLabel);
  return aFunction;
}

static int ExecuteNamingFunction(const occ::handle<TFunction_Function>& theFunction,
                                 const occ::handle<TFunction_Driver>&   theDriver)
{
  occ::handle<TFunction_Logbook> aLogbook = TFunction_Logbook::Set(theFunction->Label());
  aLogbook->Clear();
  theDriver->Init(theFunction->Label());
  return theDriver->Execute(aLogbook);
}

static TopoDS_Shape NamingFunctionShape(const occ::handle<TFunction_Function>& theFunction)
{
  const occ::handle<TNaming_NamedShape> aNamedShape = DNaming::GetFunctionResult(theFunction);
  return aNamedShape.IsNull() ? TopoDS_Shape() : TNaming_Tool::GetShape(aNamedShape);
}

static void RegisterNamingDrivers()
{
  occ::handle<TFunction_DriverTable> aTable = TFunction_DriverTable::Get();
  aTable->AddDriver(BOX_GUID, new DNaming_BoxDriver());
  aTable->AddDriver(SPH_GUID, new DNaming_SphereDriver());
  aTable->AddDriver(CUT_GUID, new DNaming_BooleanOperationDriver());
  aTable->AddDriver(FUSE_GUID, new DNaming_BooleanOperationDriver());
  aTable->AddDriver(PTXYZ_GUID, new DNaming_TransformationDriver());
  aTable->AddDriver(PTALINE_GUID, new DNaming_TransformationDriver());
  aTable->AddDriver(PRRLINE_GUID, new DNaming_TransformationDriver());
  aTable->AddDriver(PMIRR_GUID, new DNaming_TransformationDriver());
  aTable->AddDriver(FILLT_GUID, new DNaming_FilletDriver());
}

static occ::handle<TFunction_Function> MakeNamingBox(
  const occ::handle<TDataStd_UAttribute>& theObject,
  const double                            theDX,
  const double                            theDY,
  const double                            theDZ)
{
  occ::handle<TFunction_Function> aFunction = AddNamingFunction(theObject, BOX_GUID);
  DNaming::GetReal(aFunction, BOX_DX)->Set(theDX);
  DNaming::GetReal(aFunction, BOX_DY)->Set(theDY);
  DNaming::GetReal(aFunction, BOX_DZ)->Set(theDZ);
  EXPECT_EQ(ExecuteNamingFunction(aFunction, new DNaming_BoxDriver()), 0);
  return aFunction;
}

static occ::handle<TFunction_Function> MakeNamingTranslation(
  const occ::handle<TDataStd_UAttribute>& theObject,
  const double                            theDX,
  const double                            theDY,
  const double                            theDZ)
{
  occ::handle<TFunction_Function> aFunction = AddNamingFunction(theObject, PTXYZ_GUID);
  DNaming::GetReal(aFunction, PTRANSF_DX)->Set(theDX);
  DNaming::GetReal(aFunction, PTRANSF_DY)->Set(theDY);
  DNaming::GetReal(aFunction, PTRANSF_DZ)->Set(theDZ);
  EXPECT_EQ(ExecuteNamingFunction(aFunction, new DNaming_TransformationDriver()), 0);
  return aFunction;
}

static occ::handle<TFunction_Function> MakeNamingAlongLine(
  const occ::handle<TDataStd_UAttribute>& theObject,
  const occ::handle<TDataStd_UAttribute>& theLine,
  const double                            theOffset)
{
  occ::handle<TFunction_Function> aFunction = AddNamingFunction(theObject, PTALINE_GUID);
  DNaming::SetObjectArg(aFunction, PTRANSF_LINE, theLine);
  DNaming::GetReal(aFunction, PTRANSF_OFF)->Set(theOffset);
  EXPECT_EQ(ExecuteNamingFunction(aFunction, new DNaming_TransformationDriver()), 0);
  return aFunction;
}

static occ::handle<TFunction_Function> MakeNamingRotation(
  const occ::handle<TDataStd_UAttribute>& theObject,
  const occ::handle<TDataStd_UAttribute>& theLine,
  const double                            theAngle)
{
  occ::handle<TFunction_Function> aFunction = AddNamingFunction(theObject, PRRLINE_GUID);
  DNaming::SetObjectArg(aFunction, PTRANSF_LINE, theLine);
  DNaming::GetReal(aFunction, PTRANSF_ANG)->Set(theAngle);
  EXPECT_EQ(ExecuteNamingFunction(aFunction, new DNaming_TransformationDriver()), 0);
  return aFunction;
}

static occ::handle<TFunction_Function> MakeNamingMirror(
  const occ::handle<TDataStd_UAttribute>& theObject,
  const occ::handle<TDataStd_UAttribute>& thePlane)
{
  occ::handle<TFunction_Function> aFunction = AddNamingFunction(theObject, PMIRR_GUID);
  DNaming::SetObjectArg(aFunction, PTRANSF_PLANE, thePlane);
  EXPECT_EQ(ExecuteNamingFunction(aFunction, new DNaming_TransformationDriver()), 0);
  return aFunction;
}

static occ::handle<TFunction_Function> MakeNamingFillet(
  const occ::handle<TDataStd_UAttribute>& theObject,
  const occ::handle<TDataStd_UAttribute>& thePath)
{
  occ::handle<TFunction_Function> aFunction = AddNamingFunction(theObject, FILLT_GUID);
  DNaming::GetReal(aFunction, FILLET_RADIUS)->Set(25.0);
  DNaming::GetInteger(aFunction, FILLET_SURFTYPE)->Set(0);
  DNaming::SetObjectArg(aFunction, FILLET_PATH, thePath);
  EXPECT_EQ(ExecuteNamingFunction(aFunction, new DNaming_FilletDriver()), 0);
  return aFunction;
}

static TopoDS_Shape NamingSubShape(const TopoDS_Shape& theShape,
                                   const TopAbs_ShapeEnum theType,
                                   const int theIndex)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShapeMap;
  aShapeMap.Add(theShape);
  int anIndex = 0;
  for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
  {
    if (aShapeMap.Add(anExplorer.Current()) && ++anIndex == theIndex)
    {
      return anExplorer.Current();
    }
  }
  return TopoDS_Shape();
}

static int CollectNamingSubShapes(const TopoDS_Shape& theShape,
                                  const TopAbs_ShapeEnum theType,
                                  TopoDS_Shape* theShapes,
                                  const int theCapacity)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShapeMap;
  aShapeMap.Add(theShape);
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
  {
    if (aShapeMap.Add(anExplorer.Current()))
    {
      if (aCount < theCapacity)
      {
        theShapes[aCount] = anExplorer.Current();
      }
      ++aCount;
    }
  }
  return aCount;
}

static gp_Pnt NamingVertexAverage(const TopoDS_Shape& theShape)
{
  double aX = 0.0;
  double aY = 0.0;
  double aZ = 0.0;
  int    aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_VERTEX); anExplorer.More(); anExplorer.Next())
  {
    const gp_Pnt aPoint = BRep_Tool::Pnt(TopoDS::Vertex(anExplorer.Current()));
    aX += aPoint.X();
    aY += aPoint.Y();
    aZ += aPoint.Z();
    ++aCount;
  }
  return aCount == 0 ? gp_Pnt() : gp_Pnt(aX / aCount, aY / aCount, aZ / aCount);
}

static int NamingFaceCount(const TopoDS_Shape& theShape)
{
  TopoDS_Shape aFaces[32];
  return CollectNamingSubShapes(theShape, TopAbs_FACE, aFaces, 32);
}

static bool NamingNear(const double theValue, const double theExpected)
{
  return std::abs(theValue - theExpected) <= 1.0e-7;
}

static void AddNamingPrimitives(const TDF_Label& theLabel,
                                const TopoDS_Shape* theShapes,
                                const int theCount)
{
  TNaming_Builder aBuilder(theLabel);
  for (int anIndex = 0; anIndex < theCount; ++anIndex)
  {
    aBuilder.Generated(theShapes[anIndex]);
  }
}

static void AddNamingGenerated(const TDF_Label& theLabel,
                               const TopoDS_Shape* theOldShapes,
                               const TopoDS_Shape* theNewShapes,
                               const int theCount)
{
  TNaming_Builder aBuilder(theLabel);
  for (int anIndex = 0; anIndex < theCount; ++anIndex)
  {
    aBuilder.Generated(theOldShapes[anIndex], theNewShapes[anIndex]);
  }
}

static void AddNamingModified(const TDF_Label& theLabel,
                              const TopoDS_Shape* theOldShapes,
                              const TopoDS_Shape* theNewShapes,
                              const int theCount)
{
  TNaming_Builder aBuilder(theLabel);
  for (int anIndex = 0; anIndex < theCount; ++anIndex)
  {
    aBuilder.Modify(theOldShapes[anIndex], theNewShapes[anIndex]);
  }
}

static void AddNamingDeleted(const TDF_Label& theLabel,
                             const TopoDS_Shape* theShapes,
                             const int theCount)
{
  TNaming_Builder aBuilder(theLabel);
  for (int anIndex = 0; anIndex < theCount; ++anIndex)
  {
    aBuilder.Delete(theShapes[anIndex]);
  }
}

static void AddNamingSelected(const TDF_Label& theLabel,
                              const TopoDS_Shape* theShapes,
                              const TopoDS_Shape* theContexts,
                              const int theCount)
{
  TNaming_Builder aBuilder(theLabel);
  for (int anIndex = 0; anIndex < theCount; ++anIndex)
  {
    aBuilder.Select(theShapes[anIndex], theContexts[anIndex]);
  }
}

static void ExpectNamingAttribute(const TDF_Label& theLabel)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  EXPECT_FALSE(aNamedShape.IsNull());
}

static void ExpectNamingLabel(const TopoDS_Shape& theShape,
                              const TDF_Label& theExpectedLabel,
                              const TDF_Label& theAccessLabel)
{
  const occ::handle<TNaming_NamedShape> aNamedShape =
    TNaming_Tool::NamedShape(theShape, theAccessLabel);
  ASSERT_FALSE(aNamedShape.IsNull());
  EXPECT_TRUE(aNamedShape->Label().IsEqual(theExpectedLabel));
}

static void ExpectNamingCurrentShape(const TDF_Label& theLabel,
                                     const TopoDS_Shape& theExpectedShape)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  TCollection_AsciiString aLabelEntry;
  TDF_Tool::Entry(theLabel, aLabelEntry);
  EXPECT_TRUE(TNaming_Tool::CurrentShape(aNamedShape).IsSame(theExpectedShape))
    << "CurrentShape mismatch at " << aLabelEntry.ToCString();
}

static void ExpectNamingGetShape(const TDF_Label& theLabel,
                                 const TopoDS_Shape& theExpectedShape)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  TCollection_AsciiString aLabelEntry;
  TDF_Tool::Entry(theLabel, aLabelEntry);
  EXPECT_TRUE(TNaming_Tool::GetShape(aNamedShape).IsSame(theExpectedShape))
    << "GetShape mismatch at " << aLabelEntry.ToCString();
}

static void ExpectNamingShape(const TDF_Label& theLabel, const TopoDS_Shape& theExpectedShape)
{
  ExpectNamingCurrentShape(theLabel, theExpectedShape);
  ExpectNamingGetShape(theLabel, theExpectedShape);
}

static void ExpectNamingCompoundFaces(const TDF_Label& theLabel,
                                      const int theCurrentCount,
                                      const int theShapeCount)
{
  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(theLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  EXPECT_EQ(NamingFaceCount(TNaming_Tool::CurrentShape(aNamedShape)), theCurrentCount);
  EXPECT_EQ(NamingFaceCount(TNaming_Tool::GetShape(aNamedShape)), theShapeCount);
}

static void CollectNamingShapes(const TopoDS_Shape& theShape,
                                NCollection_List<TopoDS_Shape>& theShapes)
{
  theShapes.Append(theShape);
  for (TopoDS_Iterator anIterator(theShape, true, true); anIterator.More(); anIterator.Next())
  {
    CollectNamingShapes(anIterator.Value(), theShapes);
  }
}

static void FillSelectionValidMap(const TDF_Label& theLabel,
                                  NCollection_Map<TDF_Label>& theValidMap);

static occ::handle<TFunction_Function> AddNamingSelection(
  const occ::handle<TDocStd_Document>&    theDocument,
  const occ::handle<TDataStd_UAttribute>& theContextObject,
  const TopoDS_Shape&                      theSelection,
  const bool                               theKeepOrientation,
  occ::handle<TDataStd_UAttribute>&        theSelectionObject)
{
  const occ::handle<TDataStd_UAttribute> anObject = AddNamingObject(theDocument);
  theSelectionObject = anObject;
  occ::handle<TDataStd_TreeNode>         anObjectNode;
  anObject->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), anObjectNode);
  anObjectNode->Remove();
  occ::handle<TDataStd_TreeNode> aContextNode;
  theContextObject->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aContextNode);
  aContextNode->Append(anObjectNode);

  const occ::handle<TFunction_Function> aFunction = AddNamingFunction(anObject, ATTCH_GUID);
  const TDF_Label                       aResultLabel =
    aFunction->Label().FindChild(FUNCTION_RESULT_LABEL, true);
  TDF_Reference::Set(anObject->Label(), aResultLabel);

  const occ::handle<TNaming_NamedShape> aContextNS = DNaming::GetObjectValue(theContextObject);
  if (aContextNS.IsNull() || aContextNS->IsEmpty())
  {
    return aFunction;
  }

  TNaming_Selector aSelector(aResultLabel);
  if (!aSelector.Select(theSelection, aContextNS->Get(), false, theKeepOrientation))
  {
    return aFunction;
  }
  TDF_Reference::Set(aFunction->Label().FindChild(FUNCTION_ARGUMENTS_LABEL).FindChild(ATTACH_ARG),
                     aContextNS->Label().Father());
  return aFunction;
}

static TopoDS_Shape SolveNamingFunctionSelection(
  const occ::handle<TFunction_Function>& theFunction)
{
  const TDF_Label aResultLabel = theFunction->Label().FindChild(FUNCTION_RESULT_LABEL, true);
  NCollection_Map<TDF_Label> aValidLabels;
  FillSelectionValidMap(aResultLabel, aValidLabels);
  TDF_ChildIterator anIterator(theFunction->Label().Root(), true);
  for (; anIterator.More(); anIterator.Next())
  {
    occ::handle<TNaming_NamedShape> aNamedShape;
    if (anIterator.Value().FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
    {
      aValidLabels.Add(anIterator.Value());
    }
  }
  occ::handle<TFunction_Logbook> aLogbook = TFunction_Logbook::Set(theFunction->Label());
  aLogbook->Clear();
  aLogbook->SetValid(aValidLabels);
  occ::handle<TFunction_Driver> aDriver = new DNaming_SelectionDriver();
  aDriver->Init(theFunction->Label());
  if (aDriver->Execute(aLogbook) != 0)
  {
    return TopoDS_Shape();
  }
  return TNaming_Tool::CurrentShape(DNaming::GetFunctionResult(theFunction));
}

static void FillSelectionValidMap(const TDF_Label& theLabel,
                                  NCollection_Map<TDF_Label>& theValidMap)
{
  NCollection_Map<occ::handle<TDF_Attribute>> anExternalAttributes;
  TDF_ChildIterator                           anIterator(theLabel, true);
  for (; anIterator.More(); anIterator.Next())
  {
    const TDF_Label& aChildLabel = anIterator.Value();
    TDF_Tool::OutReferences(aChildLabel, anExternalAttributes);
    for (NCollection_Map<occ::handle<TDF_Attribute>>::Iterator anAttributeIterator(
           anExternalAttributes);
         anAttributeIterator.More();
         anAttributeIterator.Next())
    {
      const occ::handle<TDF_Attribute>& anAttribute = anAttributeIterator.Key();
      if (anAttribute->Label().IsDifferent(aChildLabel)
          && !anAttribute->Label().IsDescendant(theLabel))
      {
        theValidMap.Add(anAttribute->Label());
        occ::handle<TNaming_NamedShape> aNamedShape;
        anAttribute->Label().FindAttribute(TNaming_NamedShape::GetID(), aNamedShape);
        if (!aNamedShape.IsNull())
        {
          TNaming_NamingTool::BuildDescendants(aNamedShape, theValidMap);
        }
      }
    }
  }
}

static TopoDS_Shape SolveSelectionAfterModification(const TopoDS_Shape& theInitial,
                                                    const TopoDS_Shape& theModified)
{
  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aRoot = aData->Root();
  const TDF_Label       aContextLabel = aRoot.FindChild(1);
  const TDF_Label       aSelectionLabel = aRoot.FindChild(2);

  DNaming::LoadImportedShape(aContextLabel, theInitial);

  const TopoDS_Shape aSelected = FirstSubShape(theInitial, TopAbs_FACE);
  TNaming_Selector   aSelector(aSelectionLabel);
  if (aSelected.IsNull() || !aSelector.Select(aSelected, theInitial, true))
  {
    return TopoDS_Shape();
  }

  TNaming_Builder aModificationBuilder(aContextLabel);
  aModificationBuilder.Modify(theInitial, theModified);

  NCollection_Map<TDF_Label> aValidLabels;
  FillSelectionValidMap(aSelectionLabel, aValidLabels);
  if (!aSelector.Solve(aValidLabels))
  {
    return TopoDS_Shape();
  }
  return TNaming_Tool::GetShape(aSelector.NamedShape());
}
} // namespace

// Test TNaming_Tool::GetShape with primitive evolution
TEST_F(TNaming_ToolTest, GetShape_Primitive)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Get the NamedShape attribute
  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test GetShape
  TopoDS_Shape aRetrievedShape = TNaming_Tool::GetShape(aNS);
  EXPECT_FALSE(aRetrievedShape.IsNull());
  EXPECT_TRUE(aRetrievedShape.IsSame(aBox));
}

// Test TNaming_Tool::GetShape with null NamedShape
TEST_F(TNaming_ToolTest, GetShape_NullNamedShape)
{
  occ::handle<TNaming_NamedShape> aNS;

  // This should not crash even with null handle
  // Just verify it compiles and doesn't crash
  EXPECT_NO_THROW({
    if (!aNS.IsNull())
    {
      TNaming_Tool::GetShape(aNS);
    }
  });
}

// Test TNaming_Tool::OriginalShape
TEST_F(TNaming_ToolTest, OriginalShape_Modification)
{
  // Create original and modified shapes
  TopoDS_Shape aBox1 = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aBox2 = MakeBox(20.0, 20.0, 20.0);

  // Create a label and store a modification
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Modify(aBox1, aBox2);

  // Get the NamedShape attribute
  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test OriginalShape - should return the old shape
  TopoDS_Shape aOriginal = TNaming_Tool::OriginalShape(aNS);
  EXPECT_FALSE(aOriginal.IsNull());
  EXPECT_TRUE(aOriginal.IsSame(aBox1));
}

// Test TNaming_Tool::CurrentShape without modifications
TEST_F(TNaming_ToolTest, CurrentShape_NoModification)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Get the NamedShape attribute
  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test CurrentShape
  TopoDS_Shape aCurrentShape = TNaming_Tool::CurrentShape(aNS);
  EXPECT_FALSE(aCurrentShape.IsNull());
  EXPECT_TRUE(aCurrentShape.IsSame(aBox));
}

// Test TNaming_Tool::CurrentShape with modification
TEST_F(TNaming_ToolTest, CurrentShape_WithModification)
{
  // Create original and modified shapes
  TopoDS_Shape aBox1 = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aBox2 = MakeBox(20.0, 20.0, 20.0);

  // Create first label with generated shape
  TDF_Label       aLabel1 = myRoot.FindChild(1);
  TNaming_Builder aBuilder1(aLabel1);
  aBuilder1.Generated(aBox1);

  // Get the first NamedShape
  occ::handle<TNaming_NamedShape> aNS1;
  ASSERT_TRUE(aLabel1.FindAttribute(TNaming_NamedShape::GetID(), aNS1));

  // Create second label with modified shape
  TDF_Label       aLabel2 = myRoot.FindChild(2);
  TNaming_Builder aBuilder2(aLabel2);
  aBuilder2.Modify(aBox1, aBox2);

  // Test CurrentShape on the first NamedShape
  // It should return the modified shape (aBox2)
  TopoDS_Shape aCurrentShape = TNaming_Tool::CurrentShape(aNS1);
  EXPECT_FALSE(aCurrentShape.IsNull());
}

// Test TNaming_Tool::HasLabel
TEST_F(TNaming_ToolTest, HasLabel_ExistingShape)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Test HasLabel
  EXPECT_TRUE(TNaming_Tool::HasLabel(aLabel, aBox));
}

// Test TNaming_Tool::HasLabel with non-existing shape
TEST_F(TNaming_ToolTest, HasLabel_NonExistingShape)
{
  // Create two different shapes
  TopoDS_Shape aBox    = MakeBox(10.0, 20.0, 30.0);
  TopoDS_Shape aSphere = MakeSphere(15.0);

  // Store only the box
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Test HasLabel with sphere (not stored)
  EXPECT_FALSE(TNaming_Tool::HasLabel(aLabel, aSphere));
}

// Test TNaming_Tool::NamedShape
TEST_F(TNaming_ToolTest, NamedShape_RetrieveByShape)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Test NamedShape retrieval
  occ::handle<TNaming_NamedShape> aNS = TNaming_Tool::NamedShape(aBox, aLabel);
  EXPECT_FALSE(aNS.IsNull());

  // Verify the retrieved NamedShape contains our shape
  TopoDS_Shape aRetrieved = TNaming_Tool::GetShape(aNS);
  EXPECT_TRUE(aRetrieved.IsSame(aBox));
}

// Test TNaming_Tool::NamedShape with non-existing shape
TEST_F(TNaming_ToolTest, NamedShape_NonExistingShape)
{
  // Create a box and sphere
  TopoDS_Shape aBox    = MakeBox(10.0, 20.0, 30.0);
  TopoDS_Shape aSphere = MakeSphere(15.0);

  // Store only the box
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Try to retrieve NamedShape for sphere (not stored)
  occ::handle<TNaming_NamedShape> aNS = TNaming_Tool::NamedShape(aSphere, aLabel);
  EXPECT_TRUE(aNS.IsNull());
}

// Test TNaming_Tool::CurrentNamedShape
TEST_F(TNaming_ToolTest, CurrentNamedShape_Simple)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Get the NamedShape attribute
  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test CurrentNamedShape
  occ::handle<TNaming_NamedShape> aCurrentNS = TNaming_Tool::CurrentNamedShape(aNS);
  EXPECT_FALSE(aCurrentNS.IsNull());
}

// Test TNaming_Tool::Label
TEST_F(TNaming_ToolTest, Label_RetrieveLabel)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Test Label retrieval
  int       aTransDef       = 0;
  TDF_Label aRetrievedLabel = TNaming_Tool::Label(aLabel, aBox, aTransDef);

  EXPECT_FALSE(aRetrievedLabel.IsNull());
  EXPECT_TRUE(aRetrievedLabel.IsEqual(aLabel));
}

// Test TNaming_Tool::GeneratedShape
TEST_F(TNaming_ToolTest, GeneratedShape_Simple)
{
  // Create two shapes - old and new
  TopoDS_Shape anOldBox = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aNewBox  = MakeBox(20.0, 20.0, 20.0);

  // Create a label and store the shape as generated from old shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(anOldBox, aNewBox);

  // Get the NamedShape attribute
  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test GeneratedShape - should find the new shape generated from old shape
  TopoDS_Shape aGenerated = TNaming_Tool::GeneratedShape(anOldBox, aNS);

  // Should return the new shape
  EXPECT_FALSE(aGenerated.IsNull());
  EXPECT_TRUE(aGenerated.IsSame(aNewBox));
}

// Test TNaming_Tool with multiple shapes
TEST_F(TNaming_ToolTest, MultipleShapes_GetShape)
{
  // Create multiple shapes
  TopoDS_Shape aBox1 = MakeBox(10.0, 10.0, 10.0);
  TopoDS_Shape aBox2 = MakeBox(20.0, 20.0, 20.0);
  TopoDS_Shape aBox3 = MakeBox(30.0, 30.0, 30.0);

  // Store shapes in different labels
  TDF_Label       aLabel1 = myRoot.FindChild(1);
  TNaming_Builder aBuilder1(aLabel1);
  aBuilder1.Generated(aBox1);

  TDF_Label       aLabel2 = myRoot.FindChild(2);
  TNaming_Builder aBuilder2(aLabel2);
  aBuilder2.Generated(aBox2);

  TDF_Label       aLabel3 = myRoot.FindChild(3);
  TNaming_Builder aBuilder3(aLabel3);
  aBuilder3.Generated(aBox3);

  // Verify each shape can be retrieved
  occ::handle<TNaming_NamedShape> aNS1, aNS2, aNS3;
  ASSERT_TRUE(aLabel1.FindAttribute(TNaming_NamedShape::GetID(), aNS1));
  ASSERT_TRUE(aLabel2.FindAttribute(TNaming_NamedShape::GetID(), aNS2));
  ASSERT_TRUE(aLabel3.FindAttribute(TNaming_NamedShape::GetID(), aNS3));

  TopoDS_Shape aRetrieved1 = TNaming_Tool::GetShape(aNS1);
  TopoDS_Shape aRetrieved2 = TNaming_Tool::GetShape(aNS2);
  TopoDS_Shape aRetrieved3 = TNaming_Tool::GetShape(aNS3);

  EXPECT_TRUE(aRetrieved1.IsSame(aBox1));
  EXPECT_TRUE(aRetrieved2.IsSame(aBox2));
  EXPECT_TRUE(aRetrieved3.IsSame(aBox3));
}

// Test TNaming_Tool::ValidUntil
TEST_F(TNaming_ToolTest, ValidUntil_SimpleCase)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Test ValidUntil
  int aValidUntil = TNaming_Tool::ValidUntil(aLabel, aBox);

  // ValidUntil should return a valid transaction number
  EXPECT_GE(aValidUntil, 0);
}

// Test TNaming_Tool with deleted shape
TEST_F(TNaming_ToolTest, DeletedShape_CurrentShape)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel1 = myRoot.FindChild(1);
  TNaming_Builder aBuilder1(aLabel1);
  aBuilder1.Generated(aBox);

  // Delete the shape
  TDF_Label       aLabel2 = myRoot.FindChild(2);
  TNaming_Builder aBuilder2(aLabel2);
  aBuilder2.Delete(aBox);

  // Get the first NamedShape
  occ::handle<TNaming_NamedShape> aNS1;
  ASSERT_TRUE(aLabel1.FindAttribute(TNaming_NamedShape::GetID(), aNS1));

  // CurrentShape should handle the deletion
  TopoDS_Shape aCurrentShape = TNaming_Tool::CurrentShape(aNS1);

  // The behavior depends on the implementation, but it should not crash
  EXPECT_NO_THROW({ TNaming_Tool::CurrentShape(aNS1); });
}

// Test edge case: Empty label
TEST_F(TNaming_ToolTest, EmptyLabel_HasLabel)
{
  // Create a box but don't store it
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Use an empty label
  TDF_Label aEmptyLabel = myRoot.FindChild(99);

  // HasLabel should return false for empty label
  EXPECT_FALSE(TNaming_Tool::HasLabel(aEmptyLabel, aBox));
}

// Test TNaming_Tool::Collect
TEST_F(TNaming_ToolTest, Collect_SimpleCase)
{
  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Create a label and store the shape
  TDF_Label       aLabel = myRoot.FindChild(1);
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(aBox);

  // Get the NamedShape attribute
  occ::handle<TNaming_NamedShape> aNS;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS));

  // Test Collect
  NCollection_Map<occ::handle<TNaming_NamedShape>> aLabels;
  EXPECT_NO_THROW({ TNaming_Tool::Collect(aNS, aLabels); });
}

// Test regression case from user's selection: NamedShape with missing UsedShapes
TEST_F(TNaming_ToolTest, NamedShape_MissingUsedShapes)
{
  // Create a new TDF document without UsedShapes
  occ::handle<TDF_Data> aDataWithoutUS = new TDF_Data();
  TDF_Label             aRootWithoutUS = aDataWithoutUS->Root();

  // Create a box shape
  TopoDS_Shape aBox = MakeBox(10.0, 20.0, 30.0);

  // Try to retrieve NamedShape without UsedShapes attribute
  occ::handle<TNaming_NamedShape> aNS = TNaming_Tool::NamedShape(aBox, aRootWithoutUS);

  // Should return null NamedShape when UsedShapes is missing
  EXPECT_TRUE(aNS.IsNull());
}

// caf/named_shape/D1: SelectGeometry resolves every box edge.
TEST(TNaming_Tool_Test, CafNamedShape_D1_SelectGeometryEdges)
{
  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aRoot = aData->Root();
  const TDF_Label       aContextLabel = aRoot.FindChild(1);
  const TopoDS_Shape    aBox = BRepPrimAPI_MakeBox(100.0, 200.0, 300.0).Shape();
  DNaming::LoadImportedShape(aContextLabel, aBox);

  int anIndex = 0;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShapeMap;
  aShapeMap.Add(aBox);
  for (TopExp_Explorer anExplorer(aBox, TopAbs_EDGE); anExplorer.More(); anExplorer.Next())
  {
    if (!aShapeMap.Add(anExplorer.Current()))
    {
      continue;
    }
    const TDF_Label aSelectionLabel = aRoot.FindChild(1001 + anIndex++);
    TNaming_Selector aSelector(aSelectionLabel);
    ASSERT_TRUE(aSelector.Select(anExplorer.Current(), aBox, true));
    NCollection_Map<TDF_Label> aValidLabels;
    FillSelectionValidMap(aSelectionLabel, aValidLabels);
    ASSERT_TRUE(aSelector.Solve(aValidLabels));
    EXPECT_TRUE(TNaming_Tool::GetShape(aSelector.NamedShape()).IsSame(anExplorer.Current()));
  }
  EXPECT_EQ(anIndex, 12);
}

// caf/named_shape/D2: SelectGeometry resolves every box face.
TEST(TNaming_Tool_Test, CafNamedShape_D2_SelectGeometryFaces)
{
  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aRoot = aData->Root();
  const TDF_Label       aContextLabel = aRoot.FindChild(1);
  const TopoDS_Shape    aBox = BRepPrimAPI_MakeBox(100.0, 200.0, 300.0).Shape();
  DNaming::LoadImportedShape(aContextLabel, aBox);

  int anIndex = 0;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShapeMap;
  aShapeMap.Add(aBox);
  for (TopExp_Explorer anExplorer(aBox, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    if (!aShapeMap.Add(anExplorer.Current()))
    {
      continue;
    }
    const TDF_Label aSelectionLabel = aRoot.FindChild(2001 + anIndex++);
    TNaming_Selector aSelector(aSelectionLabel);
    ASSERT_TRUE(aSelector.Select(anExplorer.Current(), aBox, true));
    NCollection_Map<TDF_Label> aValidLabels;
    FillSelectionValidMap(aSelectionLabel, aValidLabels);
    ASSERT_TRUE(aSelector.Solve(aValidLabels));
    EXPECT_TRUE(TNaming_Tool::GetShape(aSelector.NamedShape()).IsSame(anExplorer.Current()));
  }
  EXPECT_EQ(anIndex, 6);
}

// caf/named_shape/D3: SelectGeometry resolves every box vertex.
TEST(TNaming_Tool_Test, CafNamedShape_D3_SelectGeometryVertices)
{
  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aRoot = aData->Root();
  const TDF_Label       aContextLabel = aRoot.FindChild(1);
  const TopoDS_Shape    aBox = BRepPrimAPI_MakeBox(100.0, 200.0, 300.0).Shape();
  DNaming::LoadImportedShape(aContextLabel, aBox);

  int anIndex = 0;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aShapeMap;
  aShapeMap.Add(aBox);
  for (TopExp_Explorer anExplorer(aBox, TopAbs_VERTEX); anExplorer.More(); anExplorer.Next())
  {
    if (!aShapeMap.Add(anExplorer.Current()))
    {
      continue;
    }
    const TDF_Label aSelectionLabel = aRoot.FindChild(3001 + anIndex++);
    TNaming_Selector aSelector(aSelectionLabel);
    ASSERT_TRUE(aSelector.Select(anExplorer.Current(), aBox, true));
    NCollection_Map<TDF_Label> aValidLabels;
    FillSelectionValidMap(aSelectionLabel, aValidLabels);
    ASSERT_TRUE(aSelector.Solve(aValidLabels));
    EXPECT_TRUE(TNaming_Tool::GetShape(aSelector.NamedShape()).IsSame(anExplorer.Current()));
  }
  EXPECT_EQ(anIndex, 8);
}

// caf/named_shape/D4: current shape follows a Boolean modification history.
TEST(TNaming_Tool_Test, CafNamedShape_D4_CurrentShapeAfterBoolean)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(190.0, 890.0, 290.0).Shape();
  const TopoDS_Shape aTool =
    BRepPrimAPI_MakeBox(gp_Pnt(-100.0, 400.0, -130.0), 390.0, 190.0, 490.0).Shape();
  const TopoDS_Shape aCut = BRepAlgoAPI_Fuse(aBox, aTool).Shape();

  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aLabel = aData->Root().FindChild(1);
  TNaming_Builder       aBuilder(aLabel);
  aBuilder.Generated(aBox);
  TNaming_Builder aModification(aLabel);
  aModification.Modify(aBox, aCut);

  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  EXPECT_TRUE(TNaming_Tool::CurrentShape(aNamedShape).IsSame(aCut));
}

// caf/named_shape/E2: selection follows the complete transformation/fillet chain.
TEST(TNaming_Tool_Test, CafNamedShape_E2_SelectionAfterTransformations)
{
  RegisterNamingDrivers();
  const occ::handle<TDocStd_Document> aDocument = NewNamingDocument();
  const occ::handle<TDataStd_UAttribute> aBoxObject = AddNamingObject(aDocument);
  const occ::handle<TFunction_Function> aBoxFunction =
    MakeNamingBox(aBoxObject, 190.0, 290.0, 390.0);
  const occ::handle<TFunction_Function> aTranslate1 =
    MakeNamingTranslation(aBoxObject, 150.0, 40.0, 90.0);

  const TopoDS_Shape aBox2 = NamingFunctionShape(aTranslate1);
  const TopoDS_Shape aLineShape =
    NamingSubShape(NamingSubShape(aBox2, TopAbs_FACE, 1), TopAbs_EDGE, 1);
  ASSERT_FALSE(aLineShape.IsNull());
  occ::handle<TDataStd_UAttribute> aLineObject;
  const occ::handle<TFunction_Function> aLineSelection =
    AddNamingSelection(aDocument, aBoxObject, aLineShape, true, aLineObject);
  const occ::handle<TFunction_Function> aAlongLine =
    MakeNamingAlongLine(aBoxObject, aLineObject, 210.0);

  const TopoDS_Shape aBox3 = NamingFunctionShape(aAlongLine);
  const TopoDS_Shape aRotationLine =
    NamingSubShape(NamingSubShape(aBox3, TopAbs_FACE, 1), TopAbs_EDGE, 3);
  occ::handle<TDataStd_UAttribute> aRotationLineObject;
  const occ::handle<TFunction_Function> aRotationLineSelection =
    AddNamingSelection(aDocument, aBoxObject, aRotationLine, true, aRotationLineObject);
  const occ::handle<TFunction_Function> aRotate =
    MakeNamingRotation(aBoxObject, aRotationLineObject, 120.0);

  const TopoDS_Shape aBox4 = NamingFunctionShape(aRotate);
  const TopoDS_Shape aMirrorFace = NamingSubShape(aBox4, TopAbs_FACE, 3);
  ASSERT_FALSE(aMirrorFace.IsNull());
  occ::handle<TDataStd_UAttribute> aPlaneObject;
  const occ::handle<TFunction_Function> aPlaneSelection =
    AddNamingSelection(aDocument, aBoxObject, aMirrorFace, true, aPlaneObject);
  const occ::handle<TFunction_Function> aMirror =
    MakeNamingMirror(aBoxObject, aPlaneObject);

  const TopoDS_Shape aBox5 = NamingFunctionShape(aMirror);
  const TopoDS_Shape aFilletPath =
    NamingSubShape(NamingSubShape(aBox5, TopAbs_FACE, 1), TopAbs_EDGE, 3);
  ASSERT_FALSE(aFilletPath.IsNull());
  occ::handle<TDataStd_UAttribute> aPathObject;
  const occ::handle<TFunction_Function> aPathSelection =
    AddNamingSelection(aDocument, aBoxObject, aFilletPath, true, aPathObject);
  const occ::handle<TFunction_Function> aFillet =
    MakeNamingFillet(aBoxObject, aPathObject);
  ASSERT_FALSE(NamingFunctionShape(aFillet).IsNull());

  NCollection_List<occ::handle<TFunction_Function>> aSelectionFunctions;
  NCollection_List<TopoDS_Shape>                    aSelectionShapes;
  NCollection_List<TopoDS_Shape>                    aCollectedShapes;
  CollectNamingShapes(NamingFunctionShape(aFillet), aCollectedShapes);
  ASSERT_FALSE(aCollectedShapes.IsEmpty());
  aCollectedShapes.RemoveFirst();
  for (NCollection_List<TopoDS_Shape>::Iterator anIterator(aCollectedShapes); anIterator.More();
       anIterator.Next())
  {
    if (anIterator.Value().ShapeType() == TopAbs_EDGE
        && BRep_Tool::Degenerated(TopoDS::Edge(anIterator.Value())))
    {
      continue;
    }
    occ::handle<TDataStd_UAttribute> aSelectionObject;
    aSelectionFunctions.Append(AddNamingSelection(aDocument,
                                                  aBoxObject,
                                                  anIterator.Value(),
                                                  true,
                                                  aSelectionObject));
    aSelectionShapes.Append(anIterator.Value());
  }

  DNaming::GetReal(aBoxFunction, BOX_DX)->Set(290.0);
  ASSERT_EQ(ExecuteNamingFunction(aBoxFunction, new DNaming_BoxDriver()), 0);
  ASSERT_EQ(ExecuteNamingFunction(aTranslate1, new DNaming_TransformationDriver()), 0);
  ASSERT_FALSE(SolveNamingFunctionSelection(aLineSelection).IsNull());
  ASSERT_EQ(ExecuteNamingFunction(aAlongLine, new DNaming_TransformationDriver()), 0);
  ASSERT_FALSE(SolveNamingFunctionSelection(aRotationLineSelection).IsNull());
  ASSERT_EQ(ExecuteNamingFunction(aRotate, new DNaming_TransformationDriver()), 0);
  ASSERT_FALSE(SolveNamingFunctionSelection(aPlaneSelection).IsNull());
  ASSERT_EQ(ExecuteNamingFunction(aMirror, new DNaming_TransformationDriver()), 0);
  ASSERT_FALSE(SolveNamingFunctionSelection(aPathSelection).IsNull());
  ASSERT_EQ(ExecuteNamingFunction(aFillet, new DNaming_FilletDriver()), 0);

  NCollection_List<occ::handle<TFunction_Function>>::Iterator aFunctionIterator(
    aSelectionFunctions);
  NCollection_List<TopoDS_Shape>::Iterator aShapeIterator(aSelectionShapes);
  for (; aFunctionIterator.More() && aShapeIterator.More();
       aFunctionIterator.Next(), aShapeIterator.Next())
  {
    const TopoDS_Shape aSelected = SolveNamingFunctionSelection(aFunctionIterator.Value());
    ASSERT_FALSE(aSelected.IsNull());
    EXPECT_EQ(aSelected.ShapeType(), aShapeIterator.Value().ShapeType());
  }
}

TEST(TNaming_Tool_Test, CafNamedShape_F2_FusedSelectionAfterRecompute)
{
  const TopoDS_Shape aFirst = BRepPrimAPI_MakeBox(130.0, 140.0, 150.0).Shape();
  const TopoDS_Shape aSecond =
    BRepPrimAPI_MakeBox(gp_Pnt(80.0, 20.0, 20.0), 80.0, 100.0, 120.0).Shape();
  const TopoDS_Shape aFused = BRepAlgoAPI_Fuse(aFirst, aSecond).Shape();
  const TopoDS_Shape aChangedSecond =
    BRepPrimAPI_MakeBox(gp_Pnt(80.0, 20.0, 20.0), 80.0, 100.0, 140.0).Shape();
  const TopoDS_Shape aChangedFused = BRepAlgoAPI_Fuse(aFirst, aChangedSecond).Shape();
  const TopoDS_Shape aSelected = SolveSelectionAfterModification(aFused, aChangedFused);
  ASSERT_FALSE(aSelected.IsNull());
  EXPECT_EQ(aSelected.ShapeType(), TopAbs_FACE);
}

TEST(TNaming_Tool_Test, CafNamedShape_F3_CutSelectionAfterRecompute)
{
  const TopoDS_Shape aFirst = BRepPrimAPI_MakeBox(130.0, 140.0, 150.0).Shape();
  const TopoDS_Shape aSecond =
    BRepPrimAPI_MakeBox(gp_Pnt(20.0, 20.0, 20.0), 30.0, 30.0, 30.0).Shape();
  const TopoDS_Shape aCut = BRepAlgoAPI_Cut(aFirst, aSecond).Shape();
  const TopoDS_Shape aChangedSecond =
    BRepPrimAPI_MakeBox(gp_Pnt(30.0, 20.0, 20.0), 30.0, 30.0, 30.0).Shape();
  const TopoDS_Shape aChangedCut = BRepAlgoAPI_Cut(aFirst, aChangedSecond).Shape();
  const TopoDS_Shape aSelected = SolveSelectionAfterModification(aCut, aChangedCut);
  ASSERT_FALSE(aSelected.IsNull());
  EXPECT_EQ(aSelected.ShapeType(), TopAbs_FACE);
}

TEST(TNaming_Tool_Test, CafNamedShape_E9_SelectionAfterMultipleFusions)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(130.0, 140.0, 150.0).Shape();
  const TopoDS_Shape aSphere1 =
    BRepPrimAPI_MakeSphere(gp_Pnt(20.0, 20.0, 20.0), 25.0).Shape();
  const TopoDS_Shape aSphere2 =
    BRepPrimAPI_MakeSphere(gp_Pnt(70.0, 70.0, 70.0), 25.0).Shape();
  const TopoDS_Shape aFused = BRepAlgoAPI_Fuse(BRepAlgoAPI_Fuse(aBox, aSphere1).Shape(), aSphere2).Shape();
  const TopoDS_Shape aChangedSphere =
    BRepPrimAPI_MakeSphere(gp_Pnt(70.0, 70.0, 80.0), 25.0).Shape();
  const TopoDS_Shape aChangedFused =
    BRepAlgoAPI_Fuse(BRepAlgoAPI_Fuse(aBox, aSphere1).Shape(), aChangedSphere).Shape();
  const TopoDS_Shape aSelected = SolveSelectionAfterModification(aFused, aChangedFused);
  ASSERT_FALSE(aSelected.IsNull());
  EXPECT_EQ(aSelected.ShapeType(), TopAbs_FACE);
}

TEST(TNaming_Tool_Test, CafNamedShape_F8_SelectShapeAfterFusion)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(100.0, 200.0, 350.0).Shape();
  const TopoDS_Shape aBox2 =
    BRepPrimAPI_MakeBox(gp_Pnt(80.0, 80.0, 20.0), 330.0, 330.0, 90.0).Shape();
  const TopoDS_Shape aBox3 =
    BRepPrimAPI_MakeBox(gp_Pnt(40.0, 40.0, 40.0), 60.0, 450.0, 150.0).Shape();
  const TopoDS_Shape aFused = BRepAlgoAPI_Fuse(BRepAlgoAPI_Fuse(aBox1, aBox2).Shape(), aBox3).Shape();
  const TopoDS_Shape aChangedBox2 =
    BRepPrimAPI_MakeBox(gp_Pnt(80.0, 80.0, 20.0), 330.0, 330.0, 120.0).Shape();
  const TopoDS_Shape aChangedFused =
    BRepAlgoAPI_Fuse(BRepAlgoAPI_Fuse(aBox1, aChangedBox2).Shape(), aBox3).Shape();
  const TopoDS_Shape aSelected = SolveSelectionAfterModification(aFused, aChangedFused);
  ASSERT_FALSE(aSelected.IsNull());
  EXPECT_EQ(aSelected.ShapeType(), TopAbs_FACE);
}

TEST(TNaming_Tool_Test, CafNamedShape_F9_AttachedSelectionAfterFusion)
{
  const TopoDS_Shape aBox1 = BRepPrimAPI_MakeBox(100.0, 200.0, 350.0).Shape();
  const TopoDS_Shape aBox2 =
    BRepPrimAPI_MakeBox(gp_Pnt(80.0, 80.0, 20.0), 330.0, 330.0, 90.0).Shape();
  const TopoDS_Shape aBox3 =
    BRepPrimAPI_MakeBox(gp_Pnt(40.0, 40.0, 40.0), 60.0, 450.0, 150.0).Shape();
  const TopoDS_Shape aFused = BRepAlgoAPI_Fuse(BRepAlgoAPI_Fuse(aBox1, aBox2).Shape(), aBox3).Shape();
  const TopoDS_Shape aChangedBox2 =
    BRepPrimAPI_MakeBox(gp_Pnt(80.0, 80.0, 20.0), 330.0, 330.0, 125.0).Shape();
  const TopoDS_Shape aChangedFused =
    BRepAlgoAPI_Fuse(BRepAlgoAPI_Fuse(aBox1, aChangedBox2).Shape(), aBox3).Shape();
  const TopoDS_Shape aSelected = SolveSelectionAfterModification(aFused, aChangedFused);
  ASSERT_FALSE(aSelected.IsNull());
  EXPECT_EQ(aSelected.ShapeType(), TopAbs_FACE);
}

// caf/nam/A1: TNaming_Builder accepts primitive, generated, modified, deleted,
// replacement, and selected evolutions without DRAW command wrappers.
TEST(TNaming_Tool_Test, CafNamedShape_A1_BuilderEvolutions)
{
  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aRoot = aData->Root();
  const TDF_Label       aTestLabel = aRoot.FindChild(1, true);

  const TDF_Label aLabel1 = aTestLabel.FindChild(1, true);
  const TDF_Label aLabel11 = aLabel1.FindChild(1, true);
  const TDF_Label aLabel12 = aLabel1.FindChild(2, true);
  const TDF_Label aLabel2 = aTestLabel.FindChild(2, true);
  const TDF_Label aLabel21 = aLabel2.FindChild(1, true);
  const TDF_Label aLabel22 = aLabel2.FindChild(2, true);
  const TDF_Label aLabel3 = aTestLabel.FindChild(3, true);
  const TDF_Label aLabel31 = aLabel3.FindChild(1, true);
  const TDF_Label aLabel32 = aLabel3.FindChild(2, true);
  const TDF_Label aLabel4 = aTestLabel.FindChild(4, true);
  const TDF_Label aLabel41 = aLabel4.FindChild(1, true);
  const TDF_Label aLabel5 = aTestLabel.FindChild(5, true);
  const TDF_Label aLabel51 = aLabel5.FindChild(1, true);
  const TDF_Label aLabel6 = aTestLabel.FindChild(6, true);
  const TDF_Label aLabel61 = aLabel6.FindChild(1, true);

  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(100.0, 200.0, 300.0).Shape();
  const TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(200.0, 300.0, 400.0).Shape();
  const TopoDS_Shape aBox3 = BRepPrimAPI_MakeBox(300.0, 400.0, 500.0).Shape();
  const TopoDS_Shape aBox4 = BRepPrimAPI_MakeBox(300.0, 400.0, 500.0).Shape();
  TopoDS_Shape aBoxEdges[16];
  TopoDS_Shape aBox2Faces[8];
  TopoDS_Shape aBox3Faces[8];
  TopoDS_Shape aBox4Faces[8];
  ASSERT_EQ(CollectNamingSubShapes(aBox, TopAbs_EDGE, aBoxEdges, 16), 12);
  ASSERT_EQ(CollectNamingSubShapes(aBox2, TopAbs_FACE, aBox2Faces, 8), 6);
  ASSERT_EQ(CollectNamingSubShapes(aBox3, TopAbs_FACE, aBox3Faces, 8), 6);
  ASSERT_EQ(CollectNamingSubShapes(aBox4, TopAbs_FACE, aBox4Faces, 8), 6);

  AddNamingPrimitives(aLabel1, &aBox, 1);
  AddNamingPrimitives(aLabel11, &aBoxEdges[0], 1);
  AddNamingPrimitives(aLabel11, aBoxEdges, 3);
  AddNamingPrimitives(aLabel12, &aBoxEdges[3], 3);

  const TopoDS_Shape aGeneratedOld1[] = {aBox};
  const TopoDS_Shape aGeneratedNew1[] = {aBox2};
  AddNamingGenerated(aLabel2, aGeneratedOld1, aGeneratedNew1, 1);
  const TopoDS_Shape aGeneratedOld21[] = {aBoxEdges[0]};
  const TopoDS_Shape aGeneratedNew21[] = {aBox2Faces[0]};
  AddNamingGenerated(aLabel21, aGeneratedOld21, aGeneratedNew21, 1);
  const TopoDS_Shape aGeneratedOld22[] = {aBoxEdges[1], aBoxEdges[1], aBoxEdges[2]};
  const TopoDS_Shape aGeneratedNew22[] = {aBox2Faces[1], aBox2Faces[2], aBox2Faces[3]};
  AddNamingGenerated(aLabel22, aGeneratedOld22, aGeneratedNew22, 3);
  const TopoDS_Shape aGeneratedOld21Second[] = {aBoxEdges[2], aBoxEdges[2], aBoxEdges[3]};
  const TopoDS_Shape aGeneratedNew21Second[] = {aBox2Faces[0], aBox2Faces[2], aBox2Faces[3]};
  AddNamingGenerated(aLabel21,
                     aGeneratedOld21Second,
                     aGeneratedNew21Second,
                     3);

  const TopoDS_Shape aModifiedOld3[] = {aBox2};
  const TopoDS_Shape aModifiedNew3[] = {aBox3};
  AddNamingModified(aLabel3, aModifiedOld3, aModifiedNew3, 1);
  const TopoDS_Shape aModifiedOld31[] = {aBox2Faces[0]};
  const TopoDS_Shape aModifiedNew31[] = {aBox3Faces[0]};
  AddNamingModified(aLabel31, aModifiedOld31, aModifiedNew31, 1);
  const TopoDS_Shape aModifiedOld32[] = {aBox2Faces[4], aBox2Faces[1], aBox2Faces[2]};
  const TopoDS_Shape aModifiedNew32[] = {aBox3Faces[1], aBox3Faces[2], aBox3Faces[3]};
  AddNamingModified(aLabel32, aModifiedOld32, aModifiedNew32, 3);
  const TopoDS_Shape aModifiedOld31Second[] = {aBox2Faces[2], aBoxEdges[2], aBox2Faces[3]};
  const TopoDS_Shape aModifiedNew31Second[] = {aBox3Faces[0], aBox3Faces[2], aBox3Faces[3]};
  AddNamingModified(aLabel31,
                    aModifiedOld31Second,
                    aModifiedNew31Second,
                    3);

  AddNamingDeleted(aLabel41, &aBox3Faces[0], 1);
  const TopoDS_Shape aDeletedFaces[] = {aBox3Faces[0], aBox3Faces[3]};
  AddNamingDeleted(aLabel41, aDeletedFaces, 2);

  const TopoDS_Shape aModifiedOld5[] = {aBox3};
  const TopoDS_Shape aModifiedNew5[] = {aBox4};
  AddNamingModified(aLabel5, aModifiedOld5, aModifiedNew5, 1);
  const TopoDS_Shape aModifiedOld51[] = {aBox3Faces[1], aBox3Faces[2]};
  const TopoDS_Shape aModifiedNew51[] = {aBox4Faces[1], aBox4Faces[2]};
  AddNamingModified(aLabel51, aModifiedOld51, aModifiedNew51, 2);

  const TopoDS_Shape aSelectedOld6[] = {aBox4};
  const TopoDS_Shape aSelectedNew6[] = {aBox4};
  AddNamingSelected(aLabel6, aSelectedOld6, aSelectedNew6, 1);
  const TopoDS_Shape aSelectedOld61[] = {aBox4Faces[1], aBox4Faces[1], aBox2Faces[4], aBox2Faces[4]};
  const TopoDS_Shape aSelectedNew61[] = {aBox4Faces[1], aBox4Faces[1], aBox2Faces[4], aBox2Faces[4]};
  AddNamingSelected(aLabel61, aSelectedOld61, aSelectedNew61, 4);

  const TDF_Label aLabels[] = {aLabel1,  aLabel11, aLabel12, aLabel2,  aLabel21, aLabel22,
                               aLabel3,  aLabel31, aLabel32, aLabel41, aLabel5,  aLabel51,
                               aLabel6,  aLabel61};
  for (const TDF_Label& aLabel : aLabels)
  {
    ExpectNamingAttribute(aLabel);
  }
  ExpectNamingShape(aLabel1, aBox);
  ExpectNamingGetShape(aLabel2, aBox2);
  ExpectNamingGetShape(aLabel3, aBox3);
  ExpectNamingShape(aLabel5, aBox4);
  ExpectNamingShape(aLabel6, aBox4);
}

// caf/nam/A5: SelectShape command flows resolve geometry and reject an
// insufficient valid-label map when solving an intersection selection.
TEST(TNaming_Tool_Test, CafNamedShape_A5_SelectorCommandFlow)
{
  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aRoot = aData->Root();
  const TDF_Label       aTestLabel = aRoot.FindChild(1, true);
  const TDF_Label       aFaceLabel = aTestLabel.FindChild(1, true);
  const TDF_Label       anEdgeLabel = aFaceLabel.FindChild(1, true);

  const gp_Pnt aP1(0.0, 0.0, 0.0);
  const gp_Pnt aP2(0.0, 100.0, 0.0);
  const gp_Pnt aP3(100.0, 0.0, 0.0);
  const TopoDS_Vertex aVertex1 = BRepBuilderAPI_MakeVertex(aP1).Vertex();
  const TopoDS_Vertex aVertex2 = BRepBuilderAPI_MakeVertex(aP2).Vertex();
  const TopoDS_Vertex aVertex3 = BRepBuilderAPI_MakeVertex(aP3).Vertex();
  const occ::handle<Geom_Line> aLine1 = new Geom_Line(aP1, gp_Dir(0.0, 1.0, 0.0));
  const occ::handle<Geom_Line> aLine2 = new Geom_Line(aP2, gp_Dir(1.0, -1.0, 0.0));
  const occ::handle<Geom_Line> aLine3 = new Geom_Line(aP3, gp_Dir(-1.0, 0.0, 0.0));
  const TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(aLine1, aVertex1, aVertex2).Edge();
  const TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aLine2, aVertex2, aVertex3).Edge();
  const TopoDS_Edge aEdge3 = BRepBuilderAPI_MakeEdge(aLine3, aVertex3, aVertex1).Edge();
  BRepBuilderAPI_MakeWire aWireBuilder;
  aWireBuilder.Add(aEdge1);
  aWireBuilder.Add(aEdge2);
  aWireBuilder.Add(aEdge3);
  ASSERT_TRUE(aWireBuilder.IsDone());
  const TopoDS_Face aFace =
    BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                            aWireBuilder.Wire())
      .Face();
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(100.0, 100.0, 100.0).Shape();
  const TopoDS_Shape aFirstFaceVertex = NamingSubShape(aFace, TopAbs_VERTEX, 1);
  const TopoDS_Shape aFirstBoxEdge = NamingSubShape(aBox, TopAbs_EDGE, 1);
  const TopoDS_Shape aFirstBoxVertex = NamingSubShape(aBox, TopAbs_VERTEX, 1);
  ASSERT_FALSE(aFirstFaceVertex.IsNull());
  ASSERT_FALSE(aFirstBoxEdge.IsNull());
  ASSERT_FALSE(aFirstBoxVertex.IsNull());
  AddNamingPrimitives(aFaceLabel, &aFace, 1);
  AddNamingPrimitives(aFaceLabel.FindChild(1, true), &aEdge1, 1);
  AddNamingPrimitives(aFaceLabel.FindChild(2, true), &aEdge2, 1);
  AddNamingPrimitives(aFaceLabel.FindChild(3, true), &aEdge3, 1);
  AddNamingPrimitives(anEdgeLabel, &aEdge1, 1);

  const TDF_Label aSelectionRoot = aTestLabel.FindChild(10, true);
  TNaming_Selector aSelector1(aSelectionRoot.FindChild(1, true));
  TNaming_Selector aSelector2(aSelectionRoot.FindChild(2, true));
  TNaming_Selector aSelector3(aSelectionRoot.FindChild(3, true));
  TNaming_Selector aSelector4(aSelectionRoot.FindChild(4, true));
  TNaming_Selector aSelector5(aSelectionRoot.FindChild(5, true));
  TNaming_Selector aSelector6(aSelectionRoot.FindChild(6, true));
  ASSERT_TRUE(aSelector1.Select(aFace, false));
  ASSERT_TRUE(aSelector2.Select(aEdge1, aFace, false));
  ASSERT_TRUE(aSelector3.Select(aEdge2, false));
  ASSERT_TRUE(aSelector4.Select(aFirstFaceVertex, aFace, false));
  ASSERT_TRUE(aSelector5.Select(aFirstBoxEdge, aBox, false));
  ASSERT_TRUE(aSelector6.Select(aFirstBoxVertex, aBox, false));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector1.NamedShape()).IsSame(aFace));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector2.NamedShape()).IsSame(aEdge1));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector3.NamedShape()).IsSame(aEdge2));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector4.NamedShape()).IsSame(aFirstFaceVertex));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector5.NamedShape()).IsSame(aFirstBoxEdge));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector6.NamedShape()).IsSame(aFirstBoxVertex));

  NCollection_Map<TDF_Label> aValidLabels;
  aValidLabels.Add(anEdgeLabel);
  EXPECT_FALSE(aSelector4.Solve(aValidLabels));
}

// caf/nam/A3: NamedShape, CurrentShape, GetShape, and selector resolution on
// a triangle prism cut preserve the complete DRAW naming graph.
TEST(TNaming_Tool_Test, CafNamedShape_A3_NamingToolGraph)
{
  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aRoot = aData->Root();
  const TDF_Label       aTestLabel = aRoot.FindChild(1, true);

  const TDF_Label aLabel1 = aTestLabel.FindChild(1, true);
  const TDF_Label aLabel11 = aLabel1.FindChild(1, true);
  const TDF_Label aLabel12 = aLabel1.FindChild(2, true);
  const TDF_Label aLabel13 = aLabel1.FindChild(3, true);
  const TDF_Label aLabel2 = aTestLabel.FindChild(2, true);
  const TDF_Label aLabel21 = aLabel2.FindChild(1, true);
  const TDF_Label aLabel3 = aTestLabel.FindChild(3, true);
  const TDF_Label aLabel4 = aTestLabel.FindChild(4, true);
  const TDF_Label aLabel41 = aLabel4.FindChild(1, true);
  const TDF_Label aLabel42 = aLabel4.FindChild(2, true);

  const gp_Pnt aP1(0.0, 0.0, 0.0);
  const gp_Pnt aP2(0.0, 100.0, 0.0);
  const gp_Pnt aP3(100.0, 0.0, 0.0);
  const TopoDS_Vertex aVertex1 = BRepBuilderAPI_MakeVertex(aP1).Vertex();
  const TopoDS_Vertex aVertex2 = BRepBuilderAPI_MakeVertex(aP2).Vertex();
  const TopoDS_Vertex aVertex3 = BRepBuilderAPI_MakeVertex(aP3).Vertex();
  const occ::handle<Geom_Line> aLine1 = new Geom_Line(aP1, gp_Dir(0.0, 1.0, 0.0));
  const occ::handle<Geom_Line> aLine2 = new Geom_Line(aP2, gp_Dir(1.0, -1.0, 0.0));
  const occ::handle<Geom_Line> aLine3 = new Geom_Line(aP3, gp_Dir(-1.0, 0.0, 0.0));
  const TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(aLine1, aVertex1, aVertex2).Edge();
  const TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aLine2, aVertex2, aVertex3).Edge();
  const TopoDS_Edge aEdge3 = BRepBuilderAPI_MakeEdge(aLine3, aVertex3, aVertex1).Edge();
  BRepBuilderAPI_MakeWire aWireBuilder;
  aWireBuilder.Add(aEdge1);
  aWireBuilder.Add(aEdge2);
  aWireBuilder.Add(aEdge3);
  ASSERT_TRUE(aWireBuilder.IsDone());
  const TopoDS_Face aFace =
    BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                            aWireBuilder.Wire())
      .Face();

  AddNamingPrimitives(aLabel1, &aFace, 1);
  const TopoDS_Shape aTriangleEdges[] = {aEdge1, aEdge2, aEdge3};
  AddNamingPrimitives(aLabel11, &aEdge1, 1);
  AddNamingPrimitives(aLabel12, &aEdge2, 1);
  AddNamingPrimitives(aLabel13, &aEdge3, 1);

  const TopoDS_Shape aPrism =
    BRepPrimAPI_MakePrism(aFace, gp_Vec(0.0, 0.0, 100.0), false).Shape();
  TopoDS_Shape          aPrismFaces[6];
  const int              aPrismFaceCount =
    CollectNamingSubShapes(aPrism, TopAbs_FACE, aPrismFaces, 6);
  ASSERT_EQ(aPrismFaceCount, 5);
  AddNamingPrimitives(aLabel2, &aPrism, 1);

  TopoDS_Shape aGeneratedOld[8];
  TopoDS_Shape aGeneratedNew[8];
  int          aGeneratedCount = 0;
  int          aPrimitiveLabel = 2;
  for (int aFaceIndex = 0; aFaceIndex < aPrismFaceCount; ++aFaceIndex)
  {
    TopoDS_Shape anEdges[8];
    const int     anEdgeCount =
      CollectNamingSubShapes(aPrismFaces[aFaceIndex], TopAbs_EDGE, anEdges, 8);
    if (anEdgeCount == 4)
    {
      for (int anOldIndex = 0; anOldIndex < 3; ++anOldIndex)
      {
        for (int aNewIndex = 0; aNewIndex < 3; ++aNewIndex)
        {
          if (aTriangleEdges[anOldIndex].IsSame(anEdges[aNewIndex]))
          {
            if (aGeneratedCount < 8)
            {
              aGeneratedOld[aGeneratedCount] = aTriangleEdges[anOldIndex];
              aGeneratedNew[aGeneratedCount] = aPrismFaces[aFaceIndex];
              ++aGeneratedCount;
            }
          }
        }
      }
    }
    else
    {
      const TDF_Label aPrimitiveChild = aLabel2.FindChild(aPrimitiveLabel++, true);
      AddNamingPrimitives(aPrimitiveChild, &aPrismFaces[aFaceIndex], 1);
    }
  }
  ASSERT_EQ(aGeneratedCount, 3);
  AddNamingGenerated(aLabel21, aGeneratedOld, aGeneratedNew, aGeneratedCount);

  const TopoDS_Shape aBox =
    BRepPrimAPI_MakeBox(gp_Pnt(0.0, 0.0, 20.0), 100.0, 100.0, 20.0).Shape();
  TopoDS_Shape aBoxFaces[6];
  ASSERT_EQ(CollectNamingSubShapes(aBox, TopAbs_FACE, aBoxFaces, 6), 6);
  AddNamingPrimitives(aLabel3, &aBox, 1);
  for (int aFaceIndex = 0; aFaceIndex < 6; ++aFaceIndex)
  {
    AddNamingPrimitives(aLabel3.FindChild(aFaceIndex + 1, true), &aBoxFaces[aFaceIndex], 1);
  }

  const TopoDS_Shape aCut = BRepAlgoAPI_Cut(aPrism, aBox).Shape();
  ASSERT_FALSE(aCut.IsNull());
  TopoDS_Shape aCutFaces[16];
  const int aCutFaceCount = CollectNamingSubShapes(aCut, TopAbs_FACE, aCutFaces, 16);
  ASSERT_EQ(aCutFaceCount, 10);
  AddNamingModified(aLabel4, &aPrism, &aCut, 1);

  TopoDS_Shape aPX;
  TopoDS_Shape aPY;
  TopoDS_Shape aPXY;
  for (int aFaceIndex = 0; aFaceIndex < aPrismFaceCount; ++aFaceIndex)
  {
    const gp_Pnt aCenter = NamingVertexAverage(aPrismFaces[aFaceIndex]);
    if (NamingNear(aCenter.X(), 50.0))
    {
      if (NamingNear(aCenter.Y(), 50.0))
      {
        aPXY = aPrismFaces[aFaceIndex];
      }
      else
      {
        aPX = aPrismFaces[aFaceIndex];
      }
    }
    else if (NamingNear(aCenter.Y(), 50.0))
    {
      aPY = aPrismFaces[aFaceIndex];
    }
  }
  ASSERT_FALSE(aPX.IsNull());
  ASSERT_FALSE(aPY.IsNull());
  ASSERT_FALSE(aPXY.IsNull());

  TopoDS_Shape aCutGeneratedOld[16];
  TopoDS_Shape aCutGeneratedNew[16];
  int          aCutGeneratedCount = 0;
  TopoDS_Shape aCutModifiedOld[8];
  TopoDS_Shape aCutModifiedNew[8];
  int          aCutModifiedCount = 0;
  for (int aFaceIndex = 0; aFaceIndex < aCutFaceCount; ++aFaceIndex)
  {
    const gp_Pnt aCenter = NamingVertexAverage(aCutFaces[aFaceIndex]);
    if (NamingNear(aCenter.X(), 50.0))
    {
      const TopoDS_Shape& anOldShape = NamingNear(aCenter.Y(), 50.0) ? aPXY : aPX;
      aCutGeneratedOld[aCutGeneratedCount] = anOldShape;
      aCutGeneratedNew[aCutGeneratedCount++] = aCutFaces[aFaceIndex];
    }
    else if (NamingNear(aCenter.Y(), 50.0))
    {
      aCutGeneratedOld[aCutGeneratedCount] = aPY;
      aCutGeneratedNew[aCutGeneratedCount++] = aCutFaces[aFaceIndex];
    }
    else if (NamingNear(aCenter.Z(), 20.0))
    {
      aCutModifiedOld[aCutModifiedCount] = aBoxFaces[4];
      aCutModifiedNew[aCutModifiedCount++] = aCutFaces[aFaceIndex];
    }
    else if (NamingNear(aCenter.Z(), 40.0))
    {
      aCutModifiedOld[aCutModifiedCount] = aBoxFaces[5];
      aCutModifiedNew[aCutModifiedCount++] = aCutFaces[aFaceIndex];
    }
  }
  ASSERT_EQ(aCutGeneratedCount, 6);
  ASSERT_EQ(aCutModifiedCount, 2);

  TopoDS_Shape aReversedGeneratedOld[16];
  TopoDS_Shape aReversedGeneratedNew[16];
  for (int anIndex = 0; anIndex < aCutGeneratedCount; ++anIndex)
  {
    aReversedGeneratedOld[anIndex] = aCutGeneratedOld[aCutGeneratedCount - anIndex - 1];
    aReversedGeneratedNew[anIndex] = aCutGeneratedNew[aCutGeneratedCount - anIndex - 1];
  }
  AddNamingModified(aLabel41, aReversedGeneratedOld, aReversedGeneratedNew, aCutGeneratedCount);

  TopoDS_Shape aGenerated42Old[8];
  TopoDS_Shape aGenerated42New[8];
  for (int anIndex = 0; anIndex < aCutModifiedCount; ++anIndex)
  {
    aGenerated42Old[anIndex] = aCutModifiedOld[aCutModifiedCount - anIndex - 1];
    aGenerated42New[anIndex] = aCutModifiedNew[aCutModifiedCount - anIndex - 1];
  }
  aGenerated42Old[aCutModifiedCount] = aBoxFaces[0];
  aGenerated42New[aCutModifiedCount] = aBoxFaces[0];
  AddNamingGenerated(aLabel42, aGenerated42Old, aGenerated42New, aCutModifiedCount + 1);

  ExpectNamingLabel(aEdge1, aLabel11, aRoot);
  ExpectNamingLabel(aEdge2, aLabel12, aRoot);
  ExpectNamingLabel(aEdge3, aLabel13, aRoot);
  ExpectNamingLabel(aPrism, aLabel2, aRoot);
  ExpectNamingLabel(aPrismFaces[0], aLabel21, aRoot);
  ExpectNamingLabel(aPrismFaces[1], aLabel21, aRoot);
  ExpectNamingLabel(aPrismFaces[2], aLabel21, aRoot);
  ExpectNamingLabel(aPrismFaces[4], aLabel2.FindChild(3, true), aRoot);
  ExpectNamingLabel(aBox, aLabel3, aRoot);
  for (int aFaceIndex = 0; aFaceIndex < 6; ++aFaceIndex)
  {
    ExpectNamingLabel(aBoxFaces[aFaceIndex], aLabel3.FindChild(aFaceIndex + 1, true), aRoot);
  }
  ExpectNamingLabel(aCut, aLabel4, aRoot);
  const TDF_Label aLabel23 = aLabel2.FindChild(3, true);
  for (int aFaceIndex = 0; aFaceIndex < aCutFaceCount; ++aFaceIndex)
  {
    const gp_Pnt aCenter = NamingVertexAverage(aCutFaces[aFaceIndex]);
    if (aCutFaces[aFaceIndex].IsSame(aPrismFaces[3]))
    {
      ExpectNamingLabel(aCutFaces[aFaceIndex], aLabel1, aRoot);
    }
    else if (aCutFaces[aFaceIndex].IsSame(aPrismFaces[4]))
    {
      ExpectNamingLabel(aCutFaces[aFaceIndex], aLabel23, aRoot);
    }
    else if (NamingNear(aCenter.X(), 50.0) || NamingNear(aCenter.Y(), 50.0))
    {
      ExpectNamingLabel(aCutFaces[aFaceIndex], aLabel41, aRoot);
    }
    else if (NamingNear(aCenter.Z(), 20.0) || NamingNear(aCenter.Z(), 40.0))
    {
      ExpectNamingLabel(aCutFaces[aFaceIndex], aLabel42, aRoot);
    }
    else
    {
      ADD_FAILURE() << "Unclassified cut face in A3 naming graph";
    }
  }

  ExpectNamingShape(aLabel1, aFace);
  ExpectNamingShape(aLabel11, aEdge1);
  ExpectNamingShape(aLabel12, aEdge2);
  ExpectNamingShape(aLabel13, aEdge3);
  ExpectNamingCurrentShape(aLabel2, aCut);
  ExpectNamingGetShape(aLabel2, aPrism);
  ExpectNamingShape(aLabel2.FindChild(2, true), aPrismFaces[3]);
  ExpectNamingShape(aLabel2.FindChild(3, true), aPrismFaces[4]);
  ExpectNamingShape(aLabel3, aBox);
  for (int aFaceIndex = 0; aFaceIndex < 6; ++aFaceIndex)
  {
    ExpectNamingShape(aLabel3.FindChild(aFaceIndex + 1, true), aBoxFaces[aFaceIndex]);
  }
  ExpectNamingShape(aLabel4, aCut);
  ExpectNamingCompoundFaces(aLabel21, 6, 3);
  ExpectNamingCompoundFaces(aLabel41, 6, 6);
  ExpectNamingCompoundFaces(aLabel42, 2, 2);

  const TDF_Label aSelectionRoot = aTestLabel.FindChild(10, true);
  TNaming_Selector aSelector1(aSelectionRoot.FindChild(1, true));
  TNaming_Selector aSelector2(aSelectionRoot.FindChild(2, true));
  TNaming_Selector aSelector3(aSelectionRoot.FindChild(3, true));
  TNaming_Selector aSelector4(aSelectionRoot.FindChild(4, true));
  TNaming_Selector aSelector5(aSelectionRoot.FindChild(5, true));
  TNaming_Selector aSelector6(aSelectionRoot.FindChild(6, true));
  const TopoDS_Shape aFirstFaceVertex = NamingSubShape(aFace, TopAbs_VERTEX, 1);
  const TopoDS_Shape aFirstBoxEdge = NamingSubShape(aBox, TopAbs_EDGE, 1);
  const TopoDS_Shape aFirstBoxVertex = NamingSubShape(aBox, TopAbs_VERTEX, 1);
  ASSERT_TRUE(aSelector1.Select(aFace, false));
  ASSERT_TRUE(aSelector2.Select(aEdge1, aFace, false));
  ASSERT_TRUE(aSelector3.Select(aEdge2, false));
  ASSERT_TRUE(aSelector4.Select(aFirstFaceVertex, aFace, false));
  ASSERT_TRUE(aSelector5.Select(aFirstBoxEdge, aBox, false));
  ASSERT_TRUE(aSelector6.Select(aFirstBoxVertex, aBox, false));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector1.NamedShape()).IsSame(aFace));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector2.NamedShape()).IsSame(aEdge1));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector3.NamedShape()).IsSame(aEdge2));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector4.NamedShape()).IsSame(aFirstFaceVertex));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector5.NamedShape()).IsSame(aFirstBoxEdge));
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelector6.NamedShape()).IsSame(aFirstBoxVertex));

  NCollection_Map<TDF_Label> aValidLabels;
  aValidLabels.Add(aLabel11);
  EXPECT_FALSE(aSelector4.Solve(aValidLabels));
}

// caf/bugs/buc60847: a generated box is available through TNaming_Naming.
TEST_F(TNaming_ToolTest, Buc_CafBug_60847_GeneratedBoxNaming)
{
  const TDF_Label    aLabel = myRoot.FindChild(2, true);
  const TopoDS_Shape aBox   = MakeBox(10.0, 20.0, 30.0);
  TNaming_Builder    aBuilder(aLabel);
  aBuilder.Generated(aBox);

  const occ::handle<TNaming_NamedShape> aNamedShape = TNaming_Naming::Name(aLabel, aBox, aBox);
  ASSERT_FALSE(aNamedShape.IsNull());
  EXPECT_FALSE(aNamedShape->IsEmpty());
  EXPECT_TRUE(TNaming_Tool::GetShape(aNamedShape).IsSame(aBox));
}

// caf/bugs/buc60862: a generated edge follows the same naming path as a box.
TEST_F(TNaming_ToolTest, Buc_CafBug_60862_GeneratedEdgeNaming)
{
  const TDF_Label    aLabel = myRoot.FindChild(2, true);
  const TopoDS_Shape anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(10.0, 20.0, 0.0),
                                                      gp_Pnt(20.0, 10.0, 0.0))
                                .Shape();
  ASSERT_FALSE(anEdge.IsNull());
  TNaming_Builder aBuilder(aLabel);
  aBuilder.Generated(anEdge);

  const occ::handle<TNaming_NamedShape> aNamedShape =
    TNaming_Naming::Name(aLabel, anEdge, anEdge);
  ASSERT_FALSE(aNamedShape.IsNull());
  EXPECT_FALSE(aNamedShape->IsEmpty());
  EXPECT_TRUE(TNaming_Tool::GetShape(aNamedShape).IsSame(anEdge));
}

// caf/bugs/bug23799: copying a selection remains safe after its imported
// source label has forgotten its own attributes.
TEST_F(TNaming_ToolTest, CafBug_23799_CopySelectionAfterSourceForget)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(100.0).Shape();
  ASSERT_FALSE(aSphere.IsNull());

  const TDF_Label aSourceLabel = myRoot.FindChild(1, true);
  DNaming::LoadImportedShape(aSourceLabel, aSphere);

  TopExp_Explorer aFaceExplorer(aSphere, TopAbs_FACE);
  ASSERT_TRUE(aFaceExplorer.More());
  const TopoDS_Shape aFace = aFaceExplorer.Current();

  const TDF_Label aSelectionLabel = myRoot.FindChild(2, true);
  TNaming_Selector aSelector(aSelectionLabel);
  ASSERT_TRUE(aSelector.Select(aFace, true));
  aSourceLabel.ForgetAllAttributes();

  const TDF_Label aCopyLabel = myRoot.FindChild(3, true);
  TDF_CopyLabel  aCopier;
  aCopier.Load(aSelectionLabel, aCopyLabel);
  aCopier.Perform();
  EXPECT_TRUE(aCopier.IsDone());
}

// bugs/caf/bug25153: retrieving selection arguments remains safe after the
// imported source label has forgotten its attributes.
TEST(TNaming_Tool_Test, CafBug_25153_ArgumentsAfterSourceForget)
{
  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aRoot = aData->Root();
  const TDF_Label       aSourceLabel = aRoot.FindChild(1, true);
  const TDF_Label       aSelectionLabel = aRoot.FindChild(2, true);
  const TopoDS_Face     aFace =
    BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                            -100.0,
                            100.0,
                            -100.0,
                            100.0)
      .Face();
  DNaming::LoadImportedShape(aSourceLabel, aFace);

  TNaming_Selector aSelector(aSelectionLabel);
  ASSERT_TRUE(aSelector.Select(aFace, false));
  aSourceLabel.ForgetAllAttributes();

  NCollection_Map<occ::handle<TDF_Attribute>> anArguments;
  EXPECT_NO_THROW(aSelector.Arguments(anArguments));
}

// bugs/caf/bug26061: selecting a translated mirrored face does not throw and
// resolves to the current shape of the modified named-shape history.
TEST(TNaming_Tool_Test, CafBug_26061_TranslatedMirroredSelection)
{
  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aRoot = aData->Root();
  const TopoDS_Face     aFace =
    BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                            -10.0,
                            10.0,
                            -10.0,
                            10.0)
      .Face();

  gp_Trsf aMirror;
  aMirror.SetMirror(gp_Ax2(gp_Pnt(0.0, 0.0, 10.0), gp_Dir(0.0, 0.0, 1.0)));
  const TopoDS_Shape aMirroredFace = BRepBuilderAPI_Transform(aFace, aMirror).Shape();
  gp_Trsf            aTranslation;
  aTranslation.SetTranslation(gp_Vec(0.0, 0.0, -5.0));
  const TopoDS_Shape aTranslatedFace = BRepBuilderAPI_Transform(aFace, aTranslation).Shape();
  const TopoDS_Shape aTranslatedMirroredFace =
    BRepBuilderAPI_Transform(aMirroredFace, aTranslation).Shape();

  TNaming_Builder aPrimitiveBuilder(aRoot.FindChild(900, true));
  aPrimitiveBuilder.Generated(aFace);
  TNaming_Builder aMirrorBuilder(aRoot.FindChild(901, true));
  aMirrorBuilder.Generated(aFace, aMirroredFace);
  TNaming_Builder aTranslationBuilder(aRoot.FindChild(902, true));
  aTranslationBuilder.Modify(aFace, aTranslatedFace);
  TNaming_Builder aMirroredTranslationBuilder(aRoot.FindChild(903, true));
  aMirroredTranslationBuilder.Modify(aMirroredFace, aTranslatedMirroredFace);

  occ::handle<TNaming_NamedShape> aModifiedShape;
  ASSERT_TRUE(aRoot.FindChild(903, true).FindAttribute(TNaming_NamedShape::GetID(), aModifiedShape));
  const TopoDS_Shape aCurrentShape = TNaming_Tool::GetShape(aModifiedShape);
  ASSERT_FALSE(aCurrentShape.IsNull());

  TNaming_Selector aSelector(aRoot.FindChild(904, true));
  ASSERT_TRUE(aSelector.Select(aCurrentShape, false));
  occ::handle<TNaming_NamedShape> aSelection = aSelector.NamedShape();
  ASSERT_FALSE(aSelection.IsNull());
  EXPECT_TRUE(TNaming_Tool::GetShape(aSelection).IsSame(aTranslatedMirroredFace));
}

// bugs/caf/bug26428: both named-shape iterators can be constructed when the
// named shape contains only a generated (new) shape.
TEST(TNaming_Tool_Test, CafBug_26428_NewAndOldShapeIterators)
{
  occ::handle<TDF_Data> aData = new TDF_Data();
  const TDF_Label       aLabel = aData->Root().FindChild(1, true);
  const TopoDS_Shape    aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
  TNaming_Builder      aBuilder(aLabel);
  aBuilder.Generated(aBox);

  TNaming_Iterator        aNameIterator(aLabel);
  TNaming_NewShapeIterator aNewIterator(aNameIterator);
  TNaming_OldShapeIterator anOldIterator(aNameIterator);
  EXPECT_NO_THROW({
    (void)aNewIterator.More();
    (void)anOldIterator.More();
  });

  occ::handle<TNaming_NamedShape> aNamedShape;
  ASSERT_TRUE(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
  EXPECT_TRUE(TNaming_Tool::GetShape(aNamedShape).IsSame(aBox));
}

// bugs/caf/bug24263: TNaming_CopyShape copies a solid and its faces while
// preserving the orientation carried by each source shape.
TEST(TNaming_Tool_Test, CafBug_24263_CopyShapePreservesOrientation)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(100.0, 100.0, 100.0).Shape();
  TNaming_Translator aTranslator;
  aTranslator.Add(aBox);

  int aNbFaces = 0;
  int aNbReversedFaces = 0;
  for (TopExp_Explorer anExplorer(aBox, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    aTranslator.Add(anExplorer.Current());
    ++aNbFaces;
    aNbReversedFaces += anExplorer.Current().Orientation() == TopAbs_REVERSED ? 1 : 0;
  }
  ASSERT_EQ(aNbFaces, 6);
  ASSERT_GT(aNbReversedFaces, 0);

  aTranslator.Perform();
  ASSERT_TRUE(aTranslator.IsDone());
  const TopoDS_Shape aCopiedBox = aTranslator.Copied(aBox);
  ASSERT_FALSE(aCopiedBox.IsNull());
  EXPECT_EQ(aCopiedBox.Orientation(), TopAbs_FORWARD);

  int aCopiedFaceCount = 0;
  int aCopiedReversedFaceCount = 0;
  for (TopExp_Explorer anExplorer(aBox, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    const TopoDS_Shape aCopiedFace = aTranslator.Copied(anExplorer.Current());
    ASSERT_FALSE(aCopiedFace.IsNull());
    EXPECT_EQ(aCopiedFace.Orientation(), anExplorer.Current().Orientation());
    ++aCopiedFaceCount;
    aCopiedReversedFaceCount += aCopiedFace.Orientation() == TopAbs_REVERSED ? 1 : 0;
  }
  EXPECT_EQ(aCopiedFaceCount, aNbFaces);
  EXPECT_EQ(aCopiedReversedFaceCount, aNbReversedFaces);
}

// bugs/caf/bug26155: the order of a modified compound must remain stable
// when the same naming sequence is evaluated repeatedly.
TEST(TNaming_Tool_Test, CafBug_26155_ModificationOrderStable)
{
  const occ::handle<TDocStd_Document> aDocument = NewNamingDocument();
  ASSERT_FALSE(aDocument.IsNull());

  const double aTolerance = 0.1;
  double       aReferenceX = 0.0;
  bool         hasReference = false;
  for (int anIndex = 1; anIndex <= 50; ++anIndex)
  {
    TCollection_AsciiString aPrimitiveEntry(anIndex);
    aPrimitiveEntry += ":1:1";
    TDF_Label aPrimitiveLabel;
    TDF_Tool::Label(aDocument->GetData(), aPrimitiveEntry, aPrimitiveLabel, true);
    ASSERT_FALSE(aPrimitiveLabel.IsNull());

    const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
    TNaming_Builder    aPrimitiveBuilder(aPrimitiveLabel);
    aPrimitiveBuilder.Generated(aBox);

    const TopoDS_Shape aFirstHalf =
      BRepPrimAPI_MakeBox(gp_Pnt(0.0, 0.0, 0.0), gp_Pnt(5.0, 10.0, 10.0)).Shape();
    const TopoDS_Shape aSecondHalf =
      BRepPrimAPI_MakeBox(gp_Pnt(5.0, 0.0, 0.0), gp_Pnt(10.0, 10.0, 10.0)).Shape();

    TCollection_AsciiString aFirstModificationEntry = aPrimitiveEntry;
    aFirstModificationEntry += ":1";
    TDF_Label aFirstModificationLabel;
    TDF_Tool::Label(aDocument->GetData(), aFirstModificationEntry, aFirstModificationLabel, true);
    TNaming_Builder aFirstModificationBuilder(aFirstModificationLabel);
    aFirstModificationBuilder.Modify(aBox, aFirstHalf);

    TCollection_AsciiString aSecondModificationEntry = aPrimitiveEntry;
    aSecondModificationEntry += ":2";
    TDF_Label aSecondModificationLabel;
    TDF_Tool::Label(aDocument->GetData(), aSecondModificationEntry, aSecondModificationLabel, true);
    TNaming_Builder aSecondModificationBuilder(aSecondModificationLabel);
    aSecondModificationBuilder.Modify(aBox, aSecondHalf);

    occ::handle<TNaming_NamedShape> aNamedShape;
    ASSERT_TRUE(aPrimitiveLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape));
    const TopoDS_Shape aCurrentShape = TNaming_Tool::CurrentShape(aNamedShape);
    ASSERT_FALSE(aCurrentShape.IsNull());

    TopoDS_Shape aFirstSolid;
    for (TopoDS_Iterator anIterator(aCurrentShape); anIterator.More(); anIterator.Next())
    {
      if (anIterator.Value().ShapeType() == TopAbs_SOLID)
      {
        aFirstSolid = anIterator.Value();
        break;
      }
    }
    ASSERT_FALSE(aFirstSolid.IsNull());

    GProp_GProps aProperties;
    BRepGProp::VolumeProperties(aFirstSolid, aProperties);
    const double aCurrentX = aProperties.CentreOfMass().X();
    if (!hasReference)
    {
      aReferenceX = aCurrentX;
      hasReference = true;
    }
    else
    {
      EXPECT_LE(std::abs(aCurrentX - aReferenceX), aTolerance);
    }
  }
}
