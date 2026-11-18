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

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopLoc_Location.hxx>
#include <VrmlAPI_CafReader.hxx>
#include <VrmlData_Scene.hxx>
#include <VrmlData_Node.hxx>
#include <gp_Trsf.hxx>
#include <DE_ValidationUtils.hxx>
#include <DEVRML_ConfigurationNode.hxx>
#include <DEVRML_Provider.hxx>
#include <Image_Texture.hxx>
#include <NCollection_Buffer.hxx>
#include <NCollection_Sequence.hxx>
#include <Poly_Triangulation.hxx>
#include <Quantity_Color.hxx>
#include <Standard_ArrayStreamBuffer.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <istream>

namespace
{
constexpr char THE_MODEL[] =
  "#VRML V2.0 utf8\n"
  "DEF Face Shape { appearance Appearance { material Material { diffuseColor 1 0 0 "
  "ambientIntensity 0.3 shininess 0.6 transparency 0.25 } texture ImageTexture { url "
  "\"../images/paint.png\" } } "
  "geometry IndexedFaceSet { coord Coordinate { point [ 0 0 0, 1 0 0, 0 2 0 ] } "
  "coordIndex [ 0, 1, 2, -1 ] } }\n";
constexpr char THE_PLAIN[] =
  "#VRML V2.0 utf8\n"
  "DEF Face Shape { geometry IndexedFaceSet { coord Coordinate { point [ 0 0 0, 1 0 0, 0 2 0 ] } "
  "coordIndex [ 0, 1, 2, -1 ] } }\n";
constexpr uint8_t THE_IMAGE[] = {
  137, 80, 78, 71, 13, 10, 26, 10, 0,   0,   0, 13, 73, 72, 68, 82, 0,  0,   0,   1,   0,  0, 0,
  1,   8,  6,  0,  0,  0,  31, 21, 196, 137, 0, 0,  0,  10, 73, 68, 65, 84,  120, 156, 99, 0, 1,
  0,   0,  5,  0,  1,  13, 10, 45, 180, 0,   0, 0,  0,  73, 69, 78, 68, 174, 66,  96,  130};

//==================================================================================================

int triangleCount(const TopoDS_Shape& theShape)
{
  int aCount = 0;
  for (TopExp_Explorer anIt(theShape, TopAbs_FACE); anIt.More(); anIt.Next())
  {
    TopLoc_Location                       aLocation;
    const occ::handle<Poly_Triangulation> aMesh =
      BRep_Tool::Triangulation(TopoDS::Face(anIt.Current()), aLocation);
    if (!aMesh.IsNull())
    {
      aCount += aMesh->NbTriangles();
    }
  }
  return aCount;
}
} // namespace

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, NamedTextureAndMaterialSurviveInputLifetime)
{
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  {
    Standard_ArrayStreamBuffer  aModelBuffer(THE_MODEL, sizeof(THE_MODEL) - 1);
    Standard_ArrayStreamBuffer  anImageBuffer(reinterpret_cast<const char*>(THE_IMAGE),
                                             sizeof(THE_IMAGE));
    std::istream                aModel(&aModelBuffer), anImage(&anImageBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("models/main.wrl", aModel));
    aStreams.Append(DE_Provider::ReadStreamNode("images/paint.png", anImage));
    DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
    ASSERT_TRUE(aProvider.Read(aStreams, aDocument));
  }
  NCollection_Sequence<TDF_Label>            aLabels;
  const occ::handle<XCAFDoc_VisMaterialTool> aMaterials =
    XCAFDoc_DocumentTool::VisMaterialTool(aDocument->Main());
  aMaterials->GetMaterials(aLabels);
  ASSERT_EQ(aLabels.Length(), 1);
  const occ::handle<XCAFDoc_VisMaterial> aStyle = aMaterials->GetMaterial(aLabels.First());
  ASSERT_FALSE(aStyle.IsNull());
  ASSERT_TRUE(aStyle->HasCommonMaterial());
  const XCAFDoc_VisMaterialCommon& aMaterial = aStyle->CommonMaterial();
  EXPECT_NEAR(aMaterial.AmbientColor.Red(), 0.3, 1.0e-7);
  EXPECT_FLOAT_EQ(aMaterial.Shininess, 0.6f);
  EXPECT_FLOAT_EQ(aMaterial.Transparency, 0.25f);
  EXPECT_NEAR(aMaterial.DiffuseColor.Red(), 1, 1.0e-7);
  ASSERT_FALSE(aMaterial.DiffuseTexture.IsNull());
  const occ::handle<NCollection_Buffer>& aBytes = aMaterial.DiffuseTexture->DataBuffer();
  ASSERT_FALSE(aBytes.IsNull());
  ASSERT_EQ(aBytes->Size(), sizeof(THE_IMAGE));
  EXPECT_EQ(std::memcmp(aBytes->Data(), THE_IMAGE, sizeof(THE_IMAGE)), 0);
  const occ::handle<XCAFDoc_ColorTool> aColors = XCAFDoc_DocumentTool::ColorTool(aDocument->Main());
  NCollection_Sequence<TDF_Label>      aColorLabels;
  aColors->GetColors(aColorLabels);
  ASSERT_EQ(aColorLabels.Length(), 1);
  Quantity_Color aColor;
  ASSERT_TRUE(XCAFDoc_ColorTool::GetColor(aColorLabels.First(), aColor));
  EXPECT_EQ(aColor, aMaterial.DiffuseColor);
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, InlineUsesNamedSceneAndNestedRelativeNames)
{
  const char aRoot[]  = "#VRML V2.0 utf8\nTransform { translation 3 0 0 children [ Inline { url "
                        "\"../parts/child.wrl\" } ] }\n";
  const char aChild[] = "#VRML V2.0 utf8\nInline { url \"mesh/triangle.wrl\" }\n";
  Standard_ArrayStreamBuffer aRootBuffer(aRoot, sizeof(aRoot) - 1),
    aChildBuffer(aChild, sizeof(aChild) - 1), aPartBuffer(THE_PLAIN, sizeof(THE_PLAIN) - 1);
  std::istream aRootInput(&aRootBuffer), aChildInput(&aChildBuffer), aPartInput(&aPartBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/main.wrl", aRootInput));
  aStreams.Append(DE_Provider::ReadStreamNode("parts/child.wrl", aChildInput));
  aStreams.Append(DE_Provider::ReadStreamNode("parts/mesh/triangle.wrl", aPartInput));
  DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
  TopoDS_Shape    aShape;
  ASSERT_TRUE(aProvider.Read(aStreams, aShape));
  EXPECT_EQ(triangleCount(aShape), 1);

  aRootInput.clear();
  aRootInput.seekg(0);
  VrmlData_Scene aScene;
  aScene.SetResourceStreams(&aStreams, "models/main.wrl");
  aScene << aRootInput;
  ASSERT_EQ(aScene.Status(), VrmlData_StatusOK);
  gp_Trsf                          aLocation;
  const occ::handle<VrmlData_Node> aNode = aScene.FindNode("_mesh/triangle_Face", aLocation);
  ASSERT_FALSE(aNode.IsNull());
  EXPECT_EQ(&aNode->Scene(), &aScene);
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, ConfiguredLengthAndCoordinateSystemAreApplied)
{
  Standard_ArrayStreamBuffer  aBuffer(THE_PLAIN, sizeof(THE_PLAIN) - 1);
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/main.wrl", anInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.wrl", anInput));
  DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
  TopoDS_Shape    aShape;
  ASSERT_TRUE(aProvider.Read(aStreams, aShape));
  TopExp_Explorer anIt(aShape, TopAbs_FACE);
  ASSERT_TRUE(anIt.More());
  TopLoc_Location                       aLocation;
  const occ::handle<Poly_Triangulation> aMesh =
    BRep_Tool::Triangulation(TopoDS::Face(anIt.Current()), aLocation);
  ASSERT_FALSE(aMesh.IsNull());
  bool hasX = false, hasZ = false;
  for (int anIndex = 1; anIndex <= aMesh->NbNodes(); ++anIndex)
  {
    const gp_Pnt aPoint = aMesh->Node(anIndex).Transformed(aLocation.Transformation());
    EXPECT_NEAR(aPoint.Y(), 0, 1.0e-9);
    hasX = hasX || std::abs(aPoint.X() - 1000) < 1.0e-9;
    hasZ = hasZ || std::abs(aPoint.Z() - 2000) < 1.0e-9;
  }
  EXPECT_TRUE(hasX);
  EXPECT_TRUE(hasZ);
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, MissingInlineDoesNotReplaceShapeOrFillDocument)
{
  const char aModel[] =
    "#VRML V2.0 utf8\nShape { geometry Box {} } Inline { url \"missing.wrl\" }\n";
  Standard_ArrayStreamBuffer  aBuffer(aModel, sizeof(aModel) - 1);
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("main.wrl", anInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.wrl", anInput));
  DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
  TopoDS_Compound aSentinel;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aSentinel);
  TopoDS_Shape aShape = aSentinel;
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
  EXPECT_TRUE(aShape.IsSame(aSentinel));
  anInput.clear();
  anInput.seekg(0);
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  EXPECT_FALSE(aProvider.Read(aStreams, aDocument));
  NCollection_Sequence<TDF_Label> aRoots;
  XCAFDoc_DocumentTool::ShapeTool(aDocument->Main())->GetFreeShapes(aRoots);
  EXPECT_TRUE(aRoots.IsEmpty());
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, InlineURLListSelectsOneAdmittedAlternative)
{
  const char aRoot[] =
    "#VRML V2.0 utf8\nInline { url [ \"missing.wrl\", \"one.wrl\", \"two.wrl\" ] }\n";
  Standard_ArrayStreamBuffer aRootBuffer(aRoot, sizeof(aRoot) - 1),
    aFirstBuffer(THE_PLAIN, sizeof(THE_PLAIN) - 1), aSecondBuffer(THE_PLAIN, sizeof(THE_PLAIN) - 1);
  std::istream aRootInput(&aRootBuffer), aFirstInput(&aFirstBuffer), aSecondInput(&aSecondBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("main.wrl", aRootInput));
  aStreams.Append(DE_Provider::ReadStreamNode("one.wrl", aFirstInput));
  aStreams.Append(DE_Provider::ReadStreamNode("two.wrl", aSecondInput));
  DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
  TopoDS_Shape    aShape;
  ASSERT_TRUE(aProvider.Read(aStreams, aShape));
  EXPECT_EQ(triangleCount(aShape), 1);
  EXPECT_EQ(aSecondInput.tellg(), 0);
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, SelfAndMutualInlineCyclesFail)
{
  const char aSelf[] = "#VRML V2.0 utf8\nInline { url \"main.wrl\" }\n";
  const char aRoot[] = "#VRML V2.0 utf8\nInline { url \"part.wrl\" }\n";
  for (const char* aModel : {aSelf, aRoot})
  {
    Standard_ArrayStreamBuffer aRootBuffer(aModel, std::strlen(aModel)),
      aPartBuffer(aSelf, sizeof(aSelf) - 1);
    std::istream                aRootInput(&aRootBuffer), aPartInput(&aPartBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("main.wrl", aRootInput));
    aStreams.Append(DE_Provider::ReadStreamNode("part.wrl", aPartInput));
    DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
    TopoDS_Shape    aShape;
    EXPECT_FALSE(aProvider.Read(aStreams, aShape));
    EXPECT_TRUE(aShape.IsNull());
  }
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, InvalidReferencesAndDuplicateNamesFail)
{
  for (const char* aReference : {"/part.wrl", "https://example/part.wrl", "../part.wrl"})
  {
    TCollection_AsciiString aRoot("#VRML V2.0 utf8\nInline { url \"");
    aRoot += aReference;
    aRoot += "\" }\n";
    Standard_ArrayStreamBuffer aRootBuffer(aRoot.ToCString(), aRoot.Length()),
      aPartBuffer(THE_PLAIN, sizeof(THE_PLAIN) - 1);
    std::istream                aRootInput(&aRootBuffer), aPartInput(&aPartBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("main.wrl", aRootInput));
    aStreams.Append(DE_Provider::ReadStreamNode("part.wrl", aPartInput));
    DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
    TopoDS_Shape    aShape;
    EXPECT_FALSE(aProvider.Read(aStreams, aShape));
  }
  Standard_ArrayStreamBuffer  aBuffer(THE_PLAIN, sizeof(THE_PLAIN) - 1);
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("main.wrl", anInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.wrl", anInput));
  aStreams.Append(DE_Provider::ReadStreamNode("./main.wrl", anInput));
  DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
  TopoDS_Shape    aShape;
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, MissingAndEmptyImagesFail)
{
  for (const bool hasImage : {false, true})
  {
    Standard_ArrayStreamBuffer  aRootBuffer(THE_MODEL, sizeof(THE_MODEL) - 1), anImageBuffer("", 0);
    std::istream                aRootInput(&aRootBuffer), anImageInput(&anImageBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("models/main.wrl", aRootInput));
    aStreams.Append(DE_Provider::ReadStreamNode("unused.wrl", aRootInput));
    if (hasImage)
    {
      aStreams.Append(DE_Provider::ReadStreamNode("images/paint.png", anImageInput));
    }
    DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
    TopoDS_Shape    aShape;
    EXPECT_FALSE(aProvider.Read(aStreams, aShape));
    EXPECT_TRUE(aShape.IsNull());
  }
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, NestedScenesOwnAndShareResolvedImageBytes)
{
  const char aRoot[] =
    "#VRML V2.0 utf8\nInline { url \"../models/part.wrl\" } "
    "Transform { translation 3 0 0 children [ Inline { url \"../models/part.wrl\" } ] }\n";
  Standard_ArrayStreamBuffer aRootBuffer(aRoot, sizeof(aRoot) - 1),
    aPartBuffer(THE_MODEL, sizeof(THE_MODEL) - 1),
    anImageBuffer(reinterpret_cast<const char*>(THE_IMAGE), sizeof(THE_IMAGE));
  std::istream aRootInput(&aRootBuffer), aPartInput(&aPartBuffer), anImageInput(&anImageBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("root/main.wrl", aRootInput));
  aStreams.Append(DE_Provider::ReadStreamNode("models/part.wrl", aPartInput));
  aStreams.Append(DE_Provider::ReadStreamNode("images/paint.png", anImageInput));
  DEVRML_Provider                     aProvider(new DEVRML_ConfigurationNode());
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  ASSERT_TRUE(aProvider.Read(aStreams, aDocument));
  NCollection_Sequence<TDF_Label>            aLabels;
  const occ::handle<XCAFDoc_VisMaterialTool> aMaterials =
    XCAFDoc_DocumentTool::VisMaterialTool(aDocument->Main());
  aMaterials->GetMaterials(aLabels);
  ASSERT_FALSE(aLabels.IsEmpty());
  const occ::handle<XCAFDoc_VisMaterial> aStyle = aMaterials->GetMaterial(aLabels.First());
  ASSERT_FALSE(aStyle.IsNull());
  ASSERT_TRUE(aStyle->HasCommonMaterial());
  const occ::handle<Image_Texture> anImage = aStyle->CommonMaterial().DiffuseTexture;
  ASSERT_FALSE(anImage.IsNull());
  for (const TDF_Label& aLabel : aLabels)
  {
    const occ::handle<XCAFDoc_VisMaterial> aMaterial = aMaterials->GetMaterial(aLabel);
    ASSERT_FALSE(aMaterial.IsNull());
    ASSERT_TRUE(aMaterial->HasCommonMaterial());
    EXPECT_EQ(aMaterial->CommonMaterial().DiffuseTexture, anImage);
  }
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, ReaderMayBeReusedWithIndependentResourceSets)
{
  VrmlAPI_CafReader aReader;
  for (int anIteration = 0; anIteration < 2; ++anIteration)
  {
    Standard_ArrayStreamBuffer  aBuffer(THE_PLAIN, sizeof(THE_PLAIN) - 1);
    std::istream                anInput(&aBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("main.wrl", anInput));
    aStreams.Append(DE_Provider::ReadStreamNode("unused.wrl", anInput));
    aReader.SetResourceStreams(&aStreams);
    const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
    aReader.SetDocument(aDocument);
    ASSERT_TRUE(aReader.Perform(anInput, Message_ProgressRange(), "main.wrl"));
    NCollection_Sequence<TDF_Label> aRoots;
    XCAFDoc_DocumentTool::ShapeTool(aDocument->Main())->GetFreeShapes(aRoots);
    ASSERT_EQ(aRoots.Length(), anIteration + 1);
    for (const TDF_Label& aRoot : aRoots)
    {
      EXPECT_EQ(triangleCount(XCAFDoc_ShapeTool::GetShape(aRoot)), 1);
    }
    aReader.SetResourceStreams(nullptr);
  }
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, DocumentUsesConfiguredOutputUnit)
{
  Standard_ArrayStreamBuffer  aBuffer(THE_PLAIN, sizeof(THE_PLAIN) - 1);
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("main.wrl", anInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.wrl", anInput));
  const occ::handle<DEVRML_ConfigurationNode> aConfig = new DEVRML_ConfigurationNode();
  aConfig->InternalParameters.ReadFileUnit            = 0.001;
  aConfig->GlobalParameters.LengthUnit                = 1000.0;
  aConfig->InternalParameters.ReadSystemCoordinateSys =
    aConfig->InternalParameters.ReadFileCoordinateSys;
  DEVRML_Provider                     aProvider(aConfig);
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  ASSERT_TRUE(aProvider.Read(aStreams, aDocument));
  double aUnit = 0.0;
  ASSERT_TRUE(XCAFDoc_DocumentTool::GetLengthUnit(aDocument, aUnit));
  EXPECT_DOUBLE_EQ(aUnit, 1.0);
  NCollection_Sequence<TDF_Label> aRoots;
  XCAFDoc_DocumentTool::ShapeTool(aDocument->Main())->GetFreeShapes(aRoots);
  ASSERT_EQ(aRoots.Length(), 1);
  TopExp_Explorer anIt(XCAFDoc_ShapeTool::GetShape(aRoots.First()), TopAbs_FACE);
  ASSERT_TRUE(anIt.More());
  TopLoc_Location                       aLocation;
  const occ::handle<Poly_Triangulation> aMesh =
    BRep_Tool::Triangulation(TopoDS::Face(anIt.Current()), aLocation);
  ASSERT_FALSE(aMesh.IsNull());
  double aMaxY = 0.0;
  for (int anIndex = 1; anIndex <= aMesh->NbNodes(); ++anIndex)
  {
    aMaxY = std::max(aMaxY, aMesh->Node(anIndex).Transformed(aLocation.Transformation()).Y());
  }
  EXPECT_NEAR(aMaxY, 0.002, 1.0e-12);
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, EmptyInlineURLListDoesNotLoadACompanion)
{
  const char aModel[] = "#VRML V2.0 utf8\nShape { geometry Box {} } Inline { url [] }\n";
  Standard_ArrayStreamBuffer  aBuffer(aModel, sizeof(aModel) - 1);
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("main.wrl", anInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.wrl", anInput));
  DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
  TopoDS_Shape    aShape;
  EXPECT_TRUE(aProvider.Read(aStreams, aShape));
  EXPECT_FALSE(aShape.IsNull());
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, MissingHeaderAndEmbeddedNullFail)
{
  Standard_ArrayStreamBuffer anEmptyBuffer("", 0);
  std::istream               anEmptyInput(&anEmptyBuffer);
  anEmptyInput.exceptions(std::ios::badbit | std::ios::failbit);
  VrmlAPI_CafReader aReader;
  EXPECT_THROW(aReader.Perform(anEmptyInput, Message_ProgressRange(), "empty.wrl"),
               std::ios_base::failure);

  const char aNoHeader[] = "Shape { geometry Box {} }\n";
  const char aNull[] =
    "#VRML V2.0 utf8\nShape { geometry Box {} }\0Inline { url \"missing.wrl\" }\n";
  for (const bool hasNull : {false, true})
  {
    const char*                 aModel = hasNull ? aNull : aNoHeader;
    const size_t                aSize  = hasNull ? sizeof(aNull) - 1 : sizeof(aNoHeader) - 1;
    Standard_ArrayStreamBuffer  aBuffer(aModel, aSize);
    std::istream                anInput(&aBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("main.wrl", anInput));
    aStreams.Append(DE_Provider::ReadStreamNode("unused.wrl", anInput));
    DEVRML_Provider aProvider(new DEVRML_ConfigurationNode());
    TopoDS_Shape    aShape;
    EXPECT_FALSE(aProvider.Read(aStreams, aShape));
    EXPECT_TRUE(aShape.IsNull());
  }
}

//==================================================================================================

TEST(VrmlAPI_CafReaderTest, ReusedAppearanceHasOneDocumentMaterial)
{
  const char aModel[] =
    "#VRML V2.0 utf8\n"
    "Shape { appearance DEF App Appearance { material DEF Red Material { diffuseColor 1 0 0 } } "
    "geometry Box {} } "
    "Transform { translation 3 0 0 children [ Shape { appearance USE App geometry Box {} } ] }\n";
  Standard_ArrayStreamBuffer  aBuffer(aModel, sizeof(aModel) - 1);
  std::istream                anInput(&aBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("main.wrl", anInput));
  aStreams.Append(DE_Provider::ReadStreamNode("unused.wrl", anInput));
  DEVRML_Provider                     aProvider(new DEVRML_ConfigurationNode());
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  ASSERT_TRUE(aProvider.Read(aStreams, aDocument));
  NCollection_Sequence<TDF_Label>            aMaterials;
  const occ::handle<XCAFDoc_VisMaterialTool> aTool =
    XCAFDoc_DocumentTool::VisMaterialTool(aDocument->Main());
  aTool->GetMaterials(aMaterials);
  ASSERT_EQ(aMaterials.Length(), 1);
  const occ::handle<XCAFDoc_VisMaterial> aMaterial = aTool->GetMaterial(aMaterials.First());
  ASSERT_FALSE(aMaterial.IsNull());
  ASSERT_FALSE(aMaterial->RawName().IsNull());
  EXPECT_STREQ(aMaterial->RawName()->ToCString(), "Red");
}
