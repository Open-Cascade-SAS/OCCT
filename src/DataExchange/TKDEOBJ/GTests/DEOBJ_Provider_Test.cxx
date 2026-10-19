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

#include <gp_Pnt.hxx>
#include <DEOBJ_ConfigurationNode.hxx>
#include <DEOBJ_Provider.hxx>
#include <DE_Wrapper.hxx>
#include <BRep_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopLoc_Location.hxx>
#include <TopExp_Explorer.hxx>
#include <Poly_Triangulation.hxx>
#include <XSControl_WorkSession.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <FSD_Base64.hxx>
#include <Image_Texture.hxx>
#include <NCollection_Buffer.hxx>
#include <Standard_ArrayStreamBuffer.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>
#include <NCollection_Sequence.hxx>
#include <TDF_Label.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>

#include <gtest/gtest.h>

#include <sstream>
#include <cstring>

TEST(DEOBJ_ProviderTest, ReadsMainStreamShapeAndSession)
{
  for (const bool aUseSession : {false, true})
  {
    std::istringstream                         anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
    const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
    aNode->InternalParameters.FileCS                 = aNode->InternalParameters.SystemCS;
    aNode->InternalParameters.FileLengthUnit         = 0.001;
    const occ::handle<DE_Provider> aProvider         = new DEOBJ_Provider(aNode);
    EXPECT_TRUE(aNode->IsStreamSupported());
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("triangle.obj", anInput));
    TopoDS_Shape                       aShape;
    occ::handle<XSControl_WorkSession> aSession;
    ASSERT_TRUE(aUseSession ? aProvider->Read(aStreams, aShape, aSession)
                            : aProvider->Read(aStreams, aShape));
    ASSERT_FALSE(aShape.IsNull());
    TopExp_Explorer aFaces(aShape, TopAbs_FACE);
    ASSERT_TRUE(aFaces.More());
    TopLoc_Location                        aLocation;
    const occ::handle<Poly_Triangulation>& aMesh =
      BRep_Tool::Triangulation(TopoDS::Face(aFaces.Current()), aLocation);
    ASSERT_FALSE(aMesh.IsNull());
    EXPECT_EQ(aMesh->NbNodes(), 3);
    EXPECT_EQ(aMesh->NbTriangles(), 1);
    EXPECT_EQ(aMesh->Node(2).Distance(aMesh->Node(1)), 1.0);
    EXPECT_TRUE(aSession.IsNull());
  }
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, ReadsMainStreamDocumentAndWrapper)
{
  for (const bool aUseSession : {false, true})
  {
    std::istringstream          anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("triangle.obj", anInput));
    const occ::handle<DEOBJ_ConfigurationNode> aNode     = new DEOBJ_ConfigurationNode;
    const occ::handle<TDocStd_Document>        aDocument = new TDocStd_Document("BinXCAF");
    const occ::handle<DE_Provider>             aProvider = new DEOBJ_Provider(aNode);
    occ::handle<XSControl_WorkSession>         aSession;
    ASSERT_TRUE(aUseSession ? aProvider->Read(aStreams, aDocument, aSession)
                            : aProvider->Read(aStreams, aDocument));
    EXPECT_GT(aDocument->Main().NbChildren(), 0);
  }

  std::istringstream          anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("triangle.obj", anInput));
  DE_Wrapper aWrapper;
  ASSERT_TRUE(aWrapper.Bind(new DEOBJ_ConfigurationNode));
  TopoDS_Shape aShape;
  ASSERT_TRUE(aWrapper.Read(aStreams, aShape));
  EXPECT_FALSE(aShape.IsNull());
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, StreamShapeHonorsGroupingOption)
{
  for (const bool aCreateShapes : {false, true})
  {
    std::istringstream                         anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nv 1 1 0\n"
                                                       "g first\nf 1 2 3\ng second\nf 2 4 3\n");
    const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
    aNode->InternalParameters.ReadCreateShapes       = aCreateShapes;
    DEOBJ_Provider              aProvider(aNode);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("groups.obj", anInput));
    TopoDS_Shape aShape;
    ASSERT_TRUE(aProvider.Read(aStreams, aShape));
    int aFaceCount = 0;
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      ++aFaceCount;
    }
    EXPECT_EQ(aFaceCount, aCreateShapes ? 2 : 1);
  }
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, RejectsInvalidStreamInput)
{
  const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
  DEOBJ_Provider                             aProvider(aNode);
  DE_Provider::ReadStreamList                aStreams;
  TopoDS_Shape                               aShape;
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
  std::istringstream anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 4\n");
  aStreams.Append(DE_Provider::ReadStreamNode("invalid.obj", anInput));
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
  EXPECT_TRUE(aShape.IsNull());
  EXPECT_FALSE(aProvider.Read(aStreams, occ::handle<TDocStd_Document>()));
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, FailedStreamReadPreservesExistingShape)
{
  const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
  DEOBJ_Provider                             aProvider(aNode);
  const TopoDS_Shape anOriginal = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 2, 3)).Vertex();
  for (const char* aText : {"", "# no mesh\n", "v 0 0 0\nf 1 2 3\n"})
  {
    std::istringstream          anInput(aText);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("invalid.obj", anInput));
    TopoDS_Shape aShape = anOriginal;
    EXPECT_FALSE(aProvider.Read(aStreams, aShape));
    EXPECT_TRUE(aShape.IsSame(anOriginal));
  }
  std::istringstream anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
  anInput.setstate(std::ios::failbit);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("invalid.obj", anInput));
  TopoDS_Shape aShape = anOriginal;
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
  EXPECT_TRUE(aShape.IsSame(anOriginal));
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, InvalidMemoryLimitPreservesExistingShape)
{
  const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
  aNode->InternalParameters.ReadMemoryLimitMiB     = -2;
  DEOBJ_Provider              aProvider(aNode);
  std::istringstream          anInput("v 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("triangle.obj", anInput));
  const TopoDS_Shape anOriginal = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 2, 3)).Vertex();
  TopoDS_Shape       aShape     = anOriginal;
  EXPECT_FALSE(aProvider.Read(aStreams, aShape));
  EXPECT_TRUE(aShape.IsSame(anOriginal));
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, NamedMaterialRetainsDiffuseTexture)
{
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  {
    std::istringstream anObj(
      "mtllib ../materials/paint.mtl\nmtllib ../materials/other.mtl\n"
      "v 0 0 0\nv 1 0 0\nv 0 1 0\nusemtl paint\nf 1 2 3\nusemtl other\nf 3 2 1\n");
    std::istringstream aMaterial("newmtl paint\nKd 0.2 0.4 0.6\nmap_Kd ../images/paint.png\n");
    std::istringstream anOtherMaterial(
      "newmtl other\nKd 0.6 0.4 0.2\nmap_Kd ../images/paint.png\n");
    const char* anEncodedImage = "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mP8/"
                                 "x8AAwMCAO+aPzUAAAAASUVORK5CYII=";
    const occ::handle<NCollection_Buffer> anImage =
      FSD_Base64::Decode(anEncodedImage, std::strlen(anEncodedImage));
    ASSERT_FALSE(anImage.IsNull());
    Standard_ArrayStreamBuffer  anImageBuffer(reinterpret_cast<const char*>(anImage->Data()),
                                             anImage->Size());
    std::istream                anImageStream(&anImageBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("models/triangle.obj", anObj));
    aStreams.Append(DE_Provider::ReadStreamNode("materials/paint.mtl", aMaterial));
    aStreams.Append(DE_Provider::ReadStreamNode("materials/other.mtl", anOtherMaterial));
    aStreams.Append(DE_Provider::ReadStreamNode("images/paint.png", anImageStream));
    const occ::handle<DEOBJ_ConfigurationNode> aNode = new DEOBJ_ConfigurationNode;
    aNode->InternalParameters.FileCS                 = aNode->InternalParameters.SystemCS;
    aNode->InternalParameters.FileLengthUnit         = 0.001;
    DEOBJ_Provider aProvider(aNode);
    ASSERT_TRUE(aProvider.Read(aStreams, aDocument));
  }
  const occ::handle<XCAFDoc_VisMaterialTool> aTool =
    XCAFDoc_DocumentTool::VisMaterialTool(aDocument->Main());
  NCollection_Sequence<TDF_Label> aMaterials;
  aTool->GetMaterials(aMaterials);
  ASSERT_EQ(aMaterials.Size(), 2u);
  const occ::handle<XCAFDoc_VisMaterial> aMaterial = aTool->GetMaterial(aMaterials.First());
  ASSERT_FALSE(aMaterial.IsNull());
  ASSERT_TRUE(aMaterial->HasCommonMaterial());
  const occ::handle<Image_Texture>& aTexture = aMaterial->CommonMaterial().DiffuseTexture;
  ASSERT_FALSE(aTexture.IsNull());
  EXPECT_TRUE(aTexture->FilePath().IsEmpty());
  ASSERT_FALSE(aTexture->DataBuffer().IsNull());
  EXPECT_EQ(aTexture->DataBuffer()->Size(), 68u);
  const unsigned char aSignature[] = {0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a};
  EXPECT_EQ(std::memcmp(aTexture->DataBuffer()->Data(), aSignature, sizeof(aSignature)), 0);
  const occ::handle<XCAFDoc_VisMaterial> anOtherMaterial = aTool->GetMaterial(aMaterials.Last());
  ASSERT_FALSE(anOtherMaterial.IsNull());
  ASSERT_TRUE(anOtherMaterial->HasCommonMaterial());
  EXPECT_EQ(anOtherMaterial->CommonMaterial().DiffuseTexture, aTexture);
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, MissingCompanionPreservesExistingShape)
{
  const TopoDS_Shape anOriginal = BRepBuilderAPI_MakeVertex(gp_Pnt(1, 2, 3)).Vertex();
  for (const bool hasMaterial : {false, true})
  {
    SCOPED_TRACE(hasMaterial);
    std::istringstream          anObj("mtllib paint.mtl\nv 0 0 0\nv 1 0 0\nv 0 1 0\nf 1 2 3\n");
    std::istringstream          aMaterial("newmtl paint\nmap_Kd missing.png\n");
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("triangle.obj", anObj));
    aStreams.Append(
      DE_Provider::ReadStreamNode(hasMaterial ? "paint.mtl" : "other.mtl", aMaterial));
    DEOBJ_Provider aProvider(new DEOBJ_ConfigurationNode);
    TopoDS_Shape   aShape = anOriginal;
    EXPECT_FALSE(aProvider.Read(aStreams, aShape));
    EXPECT_TRUE(aShape.IsSame(anOriginal));
  }
}

//==================================================================================================

TEST(DEOBJ_ProviderTest, FileReadPreservesMaterialLookupAndMissingFileHandling)
{
  OSD_File aMaterialFile, anObjFile;
  aMaterialFile.BuildTemporary();
  ASSERT_FALSE(aMaterialFile.Failed());
  anObjFile.BuildTemporary();
  ASSERT_FALSE(anObjFile.Failed());
  const TCollection_AsciiString aMaterialText =
    "newmtl red\nKd 1 0 0\nmap_Kd missing.png\nnewmtl unused\nnewmtl blue\nKd 0 0 1";
  aMaterialFile.Write(aMaterialText, aMaterialText.Length());
  aMaterialFile.Close();
  EXPECT_FALSE(aMaterialFile.Failed());
  OSD_Path aMaterialPath, anObjPath;
  aMaterialFile.Path(aMaterialPath);
  anObjFile.Path(anObjPath);
  TCollection_AsciiString aMaterialName, anObjName, aFolder, aFileName;
  aMaterialPath.SystemName(aMaterialName);
  anObjPath.SystemName(anObjName);
  OSD_Path::FolderAndFileFromPath(aMaterialName, aFolder, aFileName);
  const TCollection_AsciiString anObjText =
    TCollection_AsciiString("mtllib ") + aFileName + "\nmtllib " + aFileName + "\nmtllib "
    + aFileName + ".missing\n"
    + "v 0 0 0\nv 1 0 0\nv 0 1 0\nusemtl red\nf 1 2 3\nusemtl blue\nf 3 2 1\n";
  anObjFile.Write(anObjText, anObjText.Length());
  anObjFile.Close();
  EXPECT_FALSE(anObjFile.Failed());

  DEOBJ_Provider                      aProvider(new DEOBJ_ConfigurationNode());
  const occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinXCAF");
  const bool                          isRead    = aProvider.Read(anObjName, aDocument);
  anObjFile.Remove();
  aMaterialFile.Remove();
  ASSERT_TRUE(isRead);

  const occ::handle<XCAFDoc_VisMaterialTool> aTool =
    XCAFDoc_DocumentTool::VisMaterialTool(aDocument->Main());
  NCollection_Sequence<TDF_Label> aMaterials;
  aTool->GetMaterials(aMaterials);
  ASSERT_EQ(aMaterials.Size(), 2u);
  bool hasRed = false, hasBlue = false;
  for (const TDF_Label& aLabel : aMaterials)
  {
    const occ::handle<XCAFDoc_VisMaterial> aMaterial = aTool->GetMaterial(aLabel);
    ASSERT_FALSE(aMaterial.IsNull());
    ASSERT_TRUE(aMaterial->HasCommonMaterial());
    const XCAFDoc_VisMaterialCommon& aCommon = aMaterial->CommonMaterial();
    if (aCommon.DiffuseColor.Red() == 1.0)
    {
      hasRed = true;
      ASSERT_FALSE(aCommon.DiffuseTexture.IsNull());
      EXPECT_EQ(aCommon.DiffuseTexture->FilePath(), aFolder + "missing.png");
      EXPECT_TRUE(aCommon.DiffuseTexture->DataBuffer().IsNull());
    }
    else
    {
      hasBlue = true;
      EXPECT_DOUBLE_EQ(aCommon.DiffuseColor.Blue(), 1.0);
      EXPECT_TRUE(aCommon.DiffuseTexture.IsNull());
    }
  }
  EXPECT_TRUE(hasRed);
  EXPECT_TRUE(hasBlue);
}
