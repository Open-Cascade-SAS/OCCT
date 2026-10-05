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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
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
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_TagSource.hxx>
#include <TDocStd_Document.hxx>
#include <TFunction_Driver.hxx>
#include <TFunction_DriverTable.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

namespace
{
static occ::handle<TDocStd_Document> NewDocument()
{
  return new TDocStd_Document("BinOcaf");
}

static occ::handle<TDataStd_UAttribute> AddObject(const occ::handle<TDocStd_Document>& theDocument)
{
  occ::handle<TDataStd_TreeNode>   aRootNode = TDataStd_TreeNode::Set(theDocument->Main());
  const TDF_Label                  aLabel    = TDF_TagSource::NewChild(theDocument->Main());
  occ::handle<TDataStd_UAttribute> anObject  = TDataStd_UAttribute::Set(aLabel, GEOMOBJECT_GUID);
  occ::handle<TDataStd_TreeNode>   aNode     = TDataStd_TreeNode::Set(aLabel);
  aRootNode->Append(aNode);
  return anObject;
}

static void SetObjectShape(const occ::handle<TDataStd_UAttribute>& theObject,
                           const TopoDS_Shape&                     theShape)
{
  const TDF_Label aShapeLabel = TDF_TagSource::NewChild(theObject->Label());
  TNaming_Builder aBuilder(aShapeLabel);
  aBuilder.Generated(theShape);
  TDF_Reference::Set(theObject->Label(), aShapeLabel);
}

static occ::handle<TFunction_Function> AddFunction(
  const occ::handle<TDataStd_UAttribute>& theObject,
  const Standard_GUID&                    theDriverGuid)
{
  const TDF_Label                 aFunctionLabel = TDF_TagSource::NewChild(theObject->Label());
  occ::handle<TFunction_Function> aFunction =
    TFunction_Function::Set(aFunctionLabel, theDriverGuid);
  occ::handle<TDataStd_TreeNode> aFunctionNode = TDataStd_TreeNode::Set(aFunctionLabel);
  occ::handle<TDataStd_TreeNode> anObjectNode;
  theObject->Label().FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), anObjectNode);
  anObjectNode->Append(aFunctionNode);

  const TDF_Label                anArgumentsLabel = TDF_TagSource::NewChild(aFunctionLabel);
  occ::handle<TDataStd_TreeNode> anArgumentsNode  = TDataStd_TreeNode::Set(anArgumentsLabel);
  if (!aFunctionNode.IsNull())
  {
    aFunctionNode->Append(anArgumentsNode);
  }

  const TDF_Label                aResultLabel = TDF_TagSource::NewChild(aFunctionLabel);
  occ::handle<TDataStd_TreeNode> aResultNode  = TDataStd_TreeNode::Set(aResultLabel);
  if (!aFunctionNode.IsNull())
  {
    aFunctionNode->Append(aResultNode);
  }

  TDF_Reference::Set(theObject->Label(), aResultLabel);
  return aFunction;
}

static int ExecuteFunction(const occ::handle<TDocStd_Document>&   theDocument,
                           const occ::handle<TFunction_Function>& theFunction,
                           const occ::handle<TFunction_Driver>&   theDriver)
{
  (void)theDocument;
  occ::handle<TFunction_Logbook> aLogbook = TFunction_Logbook::Set(theFunction->Label());
  aLogbook->Clear();
  theDriver->Init(theFunction->Label());
  return theDriver->Execute(aLogbook);
}

static TopoDS_Shape FunctionShape(const occ::handle<TFunction_Function>& theFunction)
{
  const occ::handle<TNaming_NamedShape> aNamedShape = DNaming::GetFunctionResult(theFunction);
  return aNamedShape.IsNull() ? TopoDS_Shape() : TNaming_Tool::GetShape(aNamedShape);
}

static occ::handle<TDataStd_UAttribute> MakePointObject(
  const occ::handle<TDocStd_Document>& theDocument,
  const gp_Pnt&                        thePoint)
{
  occ::handle<TDataStd_UAttribute> anObject = AddObject(theDocument);
  SetObjectShape(anObject, BRepBuilderAPI_MakeVertex(thePoint).Vertex());
  return anObject;
}

static occ::handle<TFunction_Function> MakeBoxFunction(
  const occ::handle<TDocStd_Document>&    theDocument,
  const occ::handle<TDataStd_UAttribute>& theObject,
  const double                            theDx,
  const double                            theDy,
  const double                            theDz)
{
  occ::handle<TFunction_Function> aFunction = AddFunction(theObject, BOX_GUID);
  DNaming::GetReal(aFunction, BOX_DX)->Set(theDx);
  DNaming::GetReal(aFunction, BOX_DY)->Set(theDy);
  DNaming::GetReal(aFunction, BOX_DZ)->Set(theDz);
  EXPECT_EQ(ExecuteFunction(theDocument, aFunction, new DNaming_BoxDriver()), 0);
  return aFunction;
}

static occ::handle<TFunction_Function> MakeSphereFunction(
  const occ::handle<TDocStd_Document>&    theDocument,
  const occ::handle<TDataStd_UAttribute>& theObject,
  const occ::handle<TDataStd_UAttribute>& theCenter,
  const double                            theRadius)
{
  occ::handle<TFunction_Function> aFunction = AddFunction(theObject, SPH_GUID);
  DNaming::SetObjectArg(aFunction, SPHERE_CENTER, theCenter);
  DNaming::GetReal(aFunction, SPHERE_RADIUS)->Set(theRadius);
  EXPECT_EQ(ExecuteFunction(theDocument, aFunction, new DNaming_SphereDriver()), 0);
  return aFunction;
}

static occ::handle<TFunction_Function> AddFuseFunction(
  const occ::handle<TDocStd_Document>&    theDocument,
  const occ::handle<TDataStd_UAttribute>& theObject,
  const occ::handle<TDataStd_UAttribute>& theTool)
{
  occ::handle<TFunction_Function> aFunction = AddFunction(theObject, FUSE_GUID);
  DNaming::SetObjectArg(aFunction, BOOL_TOOL, theTool);
  EXPECT_EQ(ExecuteFunction(theDocument, aFunction, new DNaming_BooleanOperationDriver()), 0);
  return aFunction;
}

static occ::handle<TFunction_Function> AddCutFunction(
  const occ::handle<TDocStd_Document>&    theDocument,
  const occ::handle<TDataStd_UAttribute>& theObject,
  const occ::handle<TDataStd_UAttribute>& theTool)
{
  occ::handle<TFunction_Function> aFunction = AddFunction(theObject, CUT_GUID);
  DNaming::SetObjectArg(aFunction, BOOL_TOOL, theTool);
  EXPECT_EQ(ExecuteFunction(theDocument, aFunction, new DNaming_BooleanOperationDriver()), 0);
  return aFunction;
}

static occ::handle<TFunction_Function> AddSectionFunction(
  const occ::handle<TDocStd_Document>&    theDocument,
  const occ::handle<TDataStd_UAttribute>& theObject,
  const occ::handle<TDataStd_UAttribute>& theTool)
{
  occ::handle<TFunction_Function> aFunction = AddFunction(theObject, SECTION_GUID);
  DNaming::SetObjectArg(aFunction, BOOL_TOOL, theTool);
  EXPECT_EQ(ExecuteFunction(theDocument, aFunction, new DNaming_BooleanOperationDriver()), 0);
  return aFunction;
}

static occ::handle<TFunction_Function> AddTranslationFunction(
  const occ::handle<TDocStd_Document>&    theDocument,
  const occ::handle<TDataStd_UAttribute>& theObject,
  const double                            theDx,
  const double                            theDy,
  const double                            theDz)
{
  occ::handle<TFunction_Function> aFunction = AddFunction(theObject, PTXYZ_GUID);
  DNaming::GetReal(aFunction, PTRANSF_DX)->Set(theDx);
  DNaming::GetReal(aFunction, PTRANSF_DY)->Set(theDy);
  DNaming::GetReal(aFunction, PTRANSF_DZ)->Set(theDz);
  EXPECT_EQ(ExecuteFunction(theDocument, aFunction, new DNaming_TransformationDriver()), 0);
  return aFunction;
}

static void AddDriversToTable()
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
} // namespace

// caf/driver/A2: recompute a box through the transformation and fillet driver chain.
TEST(DNaming_Driver_Test, CafDriver_A2_TransformationChain)
{
  AddDriversToTable();
  occ::handle<TDocStd_Document>    aDocument  = NewDocument();
  occ::handle<TDataStd_UAttribute> aBoxObject = AddObject(aDocument);
  occ::handle<TFunction_Function>  aBoxFunction =
    MakeBoxFunction(aDocument, aBoxObject, 190.0, 290.0, 390.0);
  ASSERT_FALSE(FunctionShape(aBoxFunction).IsNull());

  occ::handle<TFunction_Function> aTranslate =
    AddTranslationFunction(aDocument, aBoxObject, 150.0, 40.0, 90.0);
  ASSERT_FALSE(FunctionShape(aTranslate).IsNull());

  occ::handle<TDataStd_UAttribute> aLineObject = AddObject(aDocument);
  SetObjectShape(
    aLineObject,
    BRepBuilderAPI_MakeEdge(gp_Lin(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0))).Edge());
  occ::handle<TFunction_Function> aAlongLine = AddFunction(aBoxObject, PTALINE_GUID);
  DNaming::SetObjectArg(aAlongLine, PTRANSF_LINE, aLineObject);
  DNaming::GetReal(aAlongLine, PTRANSF_OFF)->Set(210.0);
  ASSERT_EQ(ExecuteFunction(aDocument, aAlongLine, new DNaming_TransformationDriver()), 0);

  occ::handle<TFunction_Function> aRotate = AddFunction(aBoxObject, PRRLINE_GUID);
  DNaming::SetObjectArg(aRotate, PTRANSF_LINE, aLineObject);
  DNaming::GetReal(aRotate, PTRANSF_ANG)->Set(120.0);
  ASSERT_EQ(ExecuteFunction(aDocument, aRotate, new DNaming_TransformationDriver()), 0);

  occ::handle<TDataStd_UAttribute> aPlaneObject = AddObject(aDocument);
  SetObjectShape(aPlaneObject,
                 BRepBuilderAPI_MakeFace(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)),
                                         -100.0,
                                         100.0,
                                         -100.0,
                                         100.0)
                   .Face());
  occ::handle<TFunction_Function> aMirror = AddFunction(aBoxObject, PMIRR_GUID);
  DNaming::SetObjectArg(aMirror, PTRANSF_PLANE, aPlaneObject);
  ASSERT_EQ(ExecuteFunction(aDocument, aMirror, new DNaming_TransformationDriver()), 0);

  TopExp_Explorer anEdgeExplorer(FunctionShape(aMirror), TopAbs_EDGE);
  ASSERT_TRUE(anEdgeExplorer.More());
  occ::handle<TDataStd_UAttribute> aPathObject = AddObject(aDocument);
  SetObjectShape(aPathObject, anEdgeExplorer.Current());
  occ::handle<TFunction_Function> aFillet = AddFunction(aBoxObject, FILLT_GUID);
  DNaming::GetReal(aFillet, FILLET_RADIUS)->Set(25.0);
  DNaming::GetInteger(aFillet, FILLET_SURFTYPE)->Set(0);
  DNaming::SetObjectArg(aFillet, FILLET_PATH, aPathObject);
  ASSERT_EQ(ExecuteFunction(aDocument, aFillet, new DNaming_FilletDriver()), 0);
  EXPECT_FALSE(FunctionShape(aFillet).IsNull());
}

// bugs/caf/bug24869: the section driver preserves the DRAW naming counts for
// the generated section edges and modified source faces.
TEST(DNaming_Driver_Test, CafBug_24869_SectionNamingCounts)
{
  AddDriversToTable();
  const occ::handle<TDocStd_Document> aDocument = NewDocument();

  const occ::handle<TDataStd_UAttribute> aFirstObject = AddObject(aDocument);
  MakeBoxFunction(aDocument, aFirstObject, 100.0, 200.0, 300.0);

  const occ::handle<TDataStd_UAttribute> aSecondObject = AddObject(aDocument);
  MakeBoxFunction(aDocument, aSecondObject, 150.0, 200.0, 300.0);
  AddTranslationFunction(aDocument, aSecondObject, 30.0, 40.0, 50.0);

  const occ::handle<TFunction_Function> aSection =
    AddSectionFunction(aDocument, aFirstObject, aSecondObject);
  ASSERT_FALSE(aSection.IsNull());
  const TopoDS_Shape aSectionShape = FunctionShape(aSection);
  ASSERT_FALSE(aSectionShape.IsNull());

  const TDF_Label aSectionNamingLabel =
    aSection->Label().FindChild(FUNCTION_RESULT_LABEL).FindChild(1);
  int aNbNewShapes = 0;
  int aNbOldShapes = 0;
  for (TNaming_Iterator anIterator(aSectionNamingLabel); anIterator.More(); anIterator.Next())
  {
    aNbNewShapes += anIterator.NewShape().IsNull() ? 0 : 1;
    aNbOldShapes += anIterator.OldShape().IsNull() ? 0 : 1;
  }
  EXPECT_EQ(aNbNewShapes, 18);
  EXPECT_EQ(aNbOldShapes, 18);
}

// caf/driver/A9: create spheres, fuse them with a box, and recompute after a box edit.
TEST(DNaming_Driver_Test, CafDriver_A9_SpheresAndFuses)
{
  AddDriversToTable();
  occ::handle<TDocStd_Document>    aDocument  = NewDocument();
  occ::handle<TDataStd_UAttribute> aBoxObject = AddObject(aDocument);
  occ::handle<TFunction_Function>  aBoxFunction =
    MakeBoxFunction(aDocument, aBoxObject, 130.0, 140.0, 150.0);
  occ::handle<TDataStd_UAttribute> aCenterObject =
    MakePointObject(aDocument, gp_Pnt(0.0, 0.0, 0.0));

  occ::handle<TDataStd_UAttribute> aSphereObject1 = AddObject(aDocument);
  occ::handle<TDataStd_UAttribute> aSphereObject2 = AddObject(aDocument);
  occ::handle<TFunction_Function>  aSphere1 =
    MakeSphereFunction(aDocument, aSphereObject1, aCenterObject, 25.0);
  occ::handle<TFunction_Function> aSphere2 =
    MakeSphereFunction(aDocument, aSphereObject2, aCenterObject, 25.0);
  ASSERT_FALSE(FunctionShape(aSphere1).IsNull());
  ASSERT_FALSE(FunctionShape(aSphere2).IsNull());

  occ::handle<TFunction_Function> aFuse1 = AddFuseFunction(aDocument, aBoxObject, aSphereObject1);
  occ::handle<TFunction_Function> aFuse2 = AddFuseFunction(aDocument, aBoxObject, aSphereObject2);
  ASSERT_FALSE(FunctionShape(aFuse1).IsNull());
  ASSERT_FALSE(FunctionShape(aFuse2).IsNull());

  DNaming::GetReal(aBoxFunction, BOX_DX)->Set(210.0);
  DNaming::GetReal(aBoxFunction, BOX_DZ)->Set(270.0);
  ASSERT_EQ(ExecuteFunction(aDocument, aBoxFunction, new DNaming_BoxDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aSphere1, new DNaming_SphereDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aSphere2, new DNaming_SphereDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aFuse1, new DNaming_BooleanOperationDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aFuse2, new DNaming_BooleanOperationDriver()), 0);
  EXPECT_FALSE(FunctionShape(aFuse2).IsNull());
}

// caf/driver/B2: recompute two translated spheres and their fuse after editing the box driver.
TEST(DNaming_Driver_Test, CafDriver_B2_RecomputeFuse)
{
  AddDriversToTable();
  occ::handle<TDocStd_Document>    aDocument  = NewDocument();
  occ::handle<TDataStd_UAttribute> aBoxObject = AddObject(aDocument);
  occ::handle<TFunction_Function>  aBoxFunction =
    MakeBoxFunction(aDocument, aBoxObject, 130.0, 140.0, 150.0);
  occ::handle<TDataStd_UAttribute> aCenterObject =
    MakePointObject(aDocument, gp_Pnt(0.0, 0.0, 0.0));
  occ::handle<TDataStd_UAttribute> aSphereObject1 = AddObject(aDocument);
  occ::handle<TDataStd_UAttribute> aSphereObject2 = AddObject(aDocument);
  occ::handle<TFunction_Function>  aSphere1 =
    MakeSphereFunction(aDocument, aSphereObject1, aCenterObject, 250.0);
  occ::handle<TFunction_Function> aSphere2 =
    MakeSphereFunction(aDocument, aSphereObject2, aCenterObject, 30.0);
  occ::handle<TFunction_Function> aTranslate =
    AddTranslationFunction(aDocument, aSphereObject2, 0.0, -253.0, 0.0);
  occ::handle<TFunction_Function> aFuse =
    AddFuseFunction(aDocument, aSphereObject1, aSphereObject2);
  ASSERT_FALSE(FunctionShape(aTranslate).IsNull());
  ASSERT_FALSE(FunctionShape(aFuse).IsNull());

  DNaming::GetReal(aBoxFunction, BOX_DZ)->Set(270.0);
  ASSERT_EQ(ExecuteFunction(aDocument, aBoxFunction, new DNaming_BoxDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aSphere1, new DNaming_SphereDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aSphere2, new DNaming_SphereDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aTranslate, new DNaming_TransformationDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aFuse, new DNaming_BooleanOperationDriver()), 0);
  EXPECT_FALSE(FunctionShape(aFuse).IsNull());
}

// caf/driver/B3: cut several translated spheres from a common sphere and recompute.
TEST(DNaming_Driver_Test, CafDriver_B3_RecomputeCuts)
{
  AddDriversToTable();
  occ::handle<TDocStd_Document>    aDocument  = NewDocument();
  occ::handle<TDataStd_UAttribute> aBoxObject = AddObject(aDocument);
  occ::handle<TFunction_Function>  aBoxFunction =
    MakeBoxFunction(aDocument, aBoxObject, 130.0, 140.0, 150.0);
  occ::handle<TDataStd_UAttribute> aCenterObject =
    MakePointObject(aDocument, gp_Pnt(0.0, 0.0, 0.0));
  occ::handle<TDataStd_UAttribute> aMainSphereObject = AddObject(aDocument);
  occ::handle<TFunction_Function>  aMainSphere =
    MakeSphereFunction(aDocument, aMainSphereObject, aCenterObject, 250.0);

  occ::handle<TDataStd_UAttribute> aToolObject1 = AddObject(aDocument);
  occ::handle<TDataStd_UAttribute> aToolObject2 = AddObject(aDocument);
  occ::handle<TDataStd_UAttribute> aToolObject3 = AddObject(aDocument);
  occ::handle<TDataStd_UAttribute> aToolObject4 = AddObject(aDocument);
  occ::handle<TFunction_Function>  aTool1 =
    MakeSphereFunction(aDocument, aToolObject1, aCenterObject, 30.0);
  occ::handle<TFunction_Function> aTool2 =
    MakeSphereFunction(aDocument, aToolObject2, aCenterObject, 30.0);
  occ::handle<TFunction_Function> aTool3 =
    MakeSphereFunction(aDocument, aToolObject3, aCenterObject, 30.0);
  occ::handle<TFunction_Function> aTool4 =
    MakeSphereFunction(aDocument, aToolObject4, aCenterObject, 30.0);
  AddTranslationFunction(aDocument, aToolObject1, 0.0, -251.0, 0.0);
  AddTranslationFunction(aDocument, aToolObject2, 0.0, 251.0, 0.0);
  AddTranslationFunction(aDocument, aToolObject3, -251.0, 0.0, 0.0);
  AddTranslationFunction(aDocument, aToolObject4, 251.0, -40.0, 0.0);
  occ::handle<TFunction_Function> aCut1 =
    AddCutFunction(aDocument, aMainSphereObject, aToolObject1);
  occ::handle<TFunction_Function> aCut2 =
    AddCutFunction(aDocument, aMainSphereObject, aToolObject2);
  occ::handle<TFunction_Function> aCut3 =
    AddCutFunction(aDocument, aMainSphereObject, aToolObject3);
  occ::handle<TFunction_Function> aCut4 =
    AddCutFunction(aDocument, aMainSphereObject, aToolObject4);
  (void)aMainSphere;
  (void)aTool1;
  (void)aTool2;
  (void)aTool3;
  (void)aTool4;
  ASSERT_FALSE(FunctionShape(aCut4).IsNull());

  DNaming::GetReal(aBoxFunction, BOX_DZ)->Set(270.0);
  ASSERT_EQ(ExecuteFunction(aDocument, aBoxFunction, new DNaming_BoxDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aCut1, new DNaming_BooleanOperationDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aCut2, new DNaming_BooleanOperationDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aCut3, new DNaming_BooleanOperationDriver()), 0);
  ASSERT_EQ(ExecuteFunction(aDocument, aCut4, new DNaming_BooleanOperationDriver()), 0);
  EXPECT_FALSE(FunctionShape(aCut4).IsNull());
}
