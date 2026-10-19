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

#include <DEGLTF_ConfigurationNode.hxx>
#include <DEGLTF_Provider.hxx>
#include <DE_Wrapper.hxx>
#include <FSD_Base64.hxx>
#include <Image_Texture.hxx>
#include <NCollection_Buffer.hxx>
#include <NCollection_Sequence.hxx>
#include <Standard_ArrayStreamBuffer.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_VisMaterial.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>

#include <BRep_Tool.hxx>
#include <Poly_Triangulation.hxx>
#include <Poly_Triangle.hxx>
#include <TopLoc_Location.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <RWGltf_GltfAccessorCompType.hxx>
#include <RWGltf_GltfAccessor.hxx>
#include <RWGltf_GltfAccessorLayout.hxx>
#include <RWGltf_GltfArrayType.hxx>
#include <RWGltf_GltfPrimArrayData.hxx>
#include <RWGltf_GltfLatePrimitiveArray.hxx>
#include <RWGltf_GltfPrimitiveMode.hxx>
#include <RWGltf_TriangulationReader.hxx>
#include <gp_Pnt.hxx>

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <sstream>
#include <array>
#include <utility>

#include <gtest/gtest.h>

namespace
{
static int CountShapeElements(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, theType); anExplorer.More(); anExplorer.Next())
  {
    ++aCount;
  }
  return aCount;
}

static int CountTriangulationTriangles(const TopoDS_Shape& theShape)
{
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    TopLoc_Location                       aLocation;
    const occ::handle<Poly_Triangulation> aTriangulation =
      BRep_Tool::Triangulation(TopoDS::Face(anExplorer.Current()), aLocation);
    if (!aTriangulation.IsNull())
    {
      aCount += aTriangulation->NbTriangles();
    }
  }
  return aCount;
}

static int CountTriangulationNodes(const TopoDS_Shape& theShape)
{
  int aCount = 0;
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    TopLoc_Location                       aLocation;
    const occ::handle<Poly_Triangulation> aTriangulation =
      BRep_Tool::Triangulation(TopoDS::Face(anExplorer.Current()), aLocation);
    if (!aTriangulation.IsNull())
    {
      aCount += aTriangulation->NbNodes();
    }
  }
  return aCount;
}

static void WriteFloat(NCollection_Buffer& theBuffer, const size_t theOffset, const float theValue)
{
  std::memcpy(theBuffer.ChangeData() + theOffset, &theValue, sizeof(theValue));
}

static void WriteUint32(NCollection_Buffer& theBuffer,
                        const size_t        theOffset,
                        const uint32_t      theValue)
{
  std::memcpy(theBuffer.ChangeData() + theOffset, &theValue, sizeof(theValue));
}

static void WriteMeshData(NCollection_Buffer& theBuffer,
                          const size_t        thePositionOffset,
                          const size_t        theNormalOffset,
                          const size_t        theTexCoordOffset,
                          const size_t        theIndexOffset,
                          const bool          theIsInterleaved)
{
  for (int anIndex = 0; anIndex < 24; ++anIndex)
  {
    const float  aX = (anIndex & 1) != 0 ? 1.0f : -1.0f;
    const float  aY = (anIndex & 2) != 0 ? 1.0f : -1.0f;
    const float  aZ = (anIndex & 4) != 0 ? 1.0f : -1.0f;
    const size_t aPosition =
      thePositionOffset + (theIsInterleaved ? size_t(anIndex) * 24 : size_t(anIndex) * 12);
    const size_t aNormal =
      theIsInterleaved ? aPosition + 12 : theNormalOffset + size_t(anIndex) * 12;
    WriteFloat(theBuffer, aPosition, aX);
    WriteFloat(theBuffer, aPosition + 4, aY);
    WriteFloat(theBuffer, aPosition + 8, aZ);
    WriteFloat(theBuffer, aNormal, 0.0f);
    WriteFloat(theBuffer, aNormal + 4, 0.0f);
    WriteFloat(theBuffer, aNormal + 8, 1.0f);

    if (!theIsInterleaved)
    {
      const size_t aTexCoord = theTexCoordOffset + size_t(anIndex) * 8;
      WriteFloat(theBuffer, aTexCoord, anIndex % 2 == 0 ? 0.0f : 1.0f);
      WriteFloat(theBuffer, aTexCoord + 4, anIndex % 4 < 2 ? 0.0f : 1.0f);
    }
  }

  for (int anIndex = 0; anIndex < 36; ++anIndex)
  {
    const uint16_t aVertex = static_cast<uint16_t>(anIndex % 24);
    std::memcpy(theBuffer.ChangeData() + theIndexOffset + size_t(anIndex) * sizeof(aVertex),
                &aVertex,
                sizeof(aVertex));
  }
}

static occ::handle<NCollection_Buffer> MakeEmbeddedTextureBuffer()
{
  // A valid 1x1 PNG kept in the same buffer as the mesh data.
  static const uint8_t anImage[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48,
    0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x04, 0x00, 0x00,
    0x00, 0xb5, 0x1c, 0x0c, 0x02, 0x00, 0x00, 0x00, 0x0b, 0x49, 0x44, 0x41, 0x54, 0x78,
    0xda, 0x63, 0x64, 0x00, 0x01, 0x00, 0x00, 0x05, 0x00, 0x01, 0x0d, 0x0a, 0x2d, 0xb4,
    0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};
  const size_t                    anImageOffset = 840;
  occ::handle<NCollection_Buffer> aBuffer =
    new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator(),
                           anImageOffset + sizeof(anImage));
  WriteMeshData(*aBuffer, 0, 288, 576, 768, false);
  std::memcpy(aBuffer->ChangeData() + anImageOffset, anImage, sizeof(anImage));
  return aBuffer;
}

static TCollection_AsciiString MakeEmbeddedTextureJson(const NCollection_Buffer& theBuffer,
                                                       const bool                theWithDataUri)
{
  const size_t anImageOffset = 840;

  TCollection_AsciiString aJson =
    R"({"asset":{"version":"2.0"},"scene":0,"scenes":[{"nodes":[0]}],)"
    R"("nodes":[{"mesh":0,"name":"Cube"}],)"
    R"("materials":[{"name":"Material","pbrMetallicRoughness":{"baseColorTexture":{"index":0,"texCoord":0}}}],)"
    R"("meshes":[{"name":"Cube","primitives":[{"attributes":{"POSITION":0,"NORMAL":1,"TEXCOORD_0":2},"indices":3,"material":0}]}],)"
    R"("textures":[{"source":0}],"images":[{"bufferView":4,"mimeType":"image/png","name":"Embedded"}],)"
    R"("accessors":[{"bufferView":0,"componentType":5126,"count":24,"type":"VEC3"},)"
    R"({"bufferView":1,"componentType":5126,"count":24,"type":"VEC3"},)"
    R"({"bufferView":2,"componentType":5126,"count":24,"type":"VEC2"},)"
    R"({"bufferView":3,"componentType":5123,"count":36,"type":"SCALAR"}],)"
    R"("bufferViews":[{"buffer":0,"byteLength":288,"byteOffset":0},)"
    R"({"buffer":0,"byteLength":288,"byteOffset":288},)"
    R"({"buffer":0,"byteLength":192,"byteOffset":576},)"
    R"({"buffer":0,"byteLength":72,"byteOffset":768},)"
    R"({"buffer":0,"byteLength":)";
  aJson += static_cast<int>(theBuffer.Size() - anImageOffset);
  aJson += R"(,"byteOffset":)";
  aJson += static_cast<int>(anImageOffset);
  aJson += R"(}],"buffers":[{"byteLength":)";
  aJson += static_cast<int>(theBuffer.Size());
  if (theWithDataUri)
  {
    aJson += R"(,"uri":"data:application/octet-stream;base64,)";
    aJson += FSD_Base64::Encode(theBuffer.Data(), theBuffer.Size());
    aJson += "\"";
  }
  aJson += R"(}]})";
  return aJson;
}

static TCollection_AsciiString MakeEmbeddedTextureGltf()
{
  const occ::handle<NCollection_Buffer> aBuffer = MakeEmbeddedTextureBuffer();
  return MakeEmbeddedTextureJson(*aBuffer, true);
}

static TCollection_AsciiString MakeInterleavedJson(const bool theWithDataUri)
{
  NCollection_Buffer aBuffer(NCollection_BaseAllocator::CommonBaseAllocator(), 648);
  WriteMeshData(aBuffer, 0, 0, 0, 576, true);

  TCollection_AsciiString aJson =
    R"({"asset":{"version":"2.0"},"scene":0,"scenes":[{"nodes":[0]}],)"
    R"("nodes":[{"children":[1]},{"mesh":0,"name":"Mesh"}],)"
    R"("meshes":[{"primitives":[{"attributes":{"NORMAL":1,"POSITION":2},"indices":0,"mode":4}]}],)"
    R"("accessors":[{"bufferView":0,"byteOffset":0,"componentType":5123,"count":36,"type":"SCALAR"},)"
    R"({"bufferView":1,"byteOffset":0,"componentType":5126,"count":24,"type":"VEC3"},)"
    R"({"bufferView":1,"byteOffset":12,"componentType":5126,"count":24,"type":"VEC3"}],)"
    R"("bufferViews":[{"buffer":0,"byteOffset":576,"byteLength":72,"target":34963},)"
    R"({"buffer":0,"byteOffset":0,"byteLength":576,"byteStride":24,"target":34962}],)"
    R"("buffers":[{"byteLength":648)";
  if (theWithDataUri)
  {
    aJson += R"(,"uri":"data:application/octet-stream;base64,)";
    aJson += FSD_Base64::Encode(aBuffer.Data(), aBuffer.Size());
    aJson += R"("}]})";
  }
  else
  {
    aJson += R"(}]})";
  }
  return aJson;
}

static TCollection_AsciiString MakeInterleavedGltf()
{
  return MakeInterleavedJson(true);
}

static occ::handle<NCollection_Buffer> MakeGlb(const TCollection_AsciiString& theJson,
                                               const NCollection_Buffer&      theBinary)
{
  const size_t                    aJsonLength        = static_cast<size_t>(theJson.Length());
  const size_t                    aJsonPaddedLength  = (aJsonLength + 3) & ~size_t(3);
  const size_t                    aJsonChunkOffset   = 12;
  const size_t                    aBinaryChunkOffset = aJsonChunkOffset + 8 + aJsonPaddedLength;
  const size_t                    aBinaryOffset      = aBinaryChunkOffset + 8;
  const size_t                    aTotalLength       = aBinaryOffset + theBinary.Size();
  occ::handle<NCollection_Buffer> aBuffer =
    new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator(), aTotalLength);

  WriteUint32(*aBuffer, 0, 0x46546c67); // glTF
  WriteUint32(*aBuffer, 4, 2);
  WriteUint32(*aBuffer, 8, static_cast<uint32_t>(aTotalLength));
  WriteUint32(*aBuffer, aJsonChunkOffset, static_cast<uint32_t>(aJsonPaddedLength));
  WriteUint32(*aBuffer, aJsonChunkOffset + 4, 0x4e4f534a); // JSON
  std::memcpy(aBuffer->ChangeData() + aJsonChunkOffset + 8, theJson.ToCString(), aJsonLength);
  std::memset(aBuffer->ChangeData() + aJsonChunkOffset + 8 + aJsonLength,
              ' ',
              aJsonPaddedLength - aJsonLength);
  WriteUint32(*aBuffer, aBinaryChunkOffset, static_cast<uint32_t>(theBinary.Size()));
  WriteUint32(*aBuffer, aBinaryChunkOffset + 4, 0x004e4942); // BIN\0
  std::memcpy(aBuffer->ChangeData() + aBinaryOffset, theBinary.Data(), theBinary.Size());
  return aBuffer;
}

static occ::handle<NCollection_Buffer> MakeInterleavedGlb()
{
  occ::handle<NCollection_Buffer> aBinary =
    new NCollection_Buffer(NCollection_BaseAllocator::CommonBaseAllocator(), 648);
  WriteMeshData(*aBinary, 0, 0, 0, 576, true);
  return MakeGlb(MakeInterleavedJson(false), *aBinary);
}

static occ::handle<NCollection_Buffer> MakeEmbeddedTextureGlb()
{
  const occ::handle<NCollection_Buffer> aBinary = MakeEmbeddedTextureBuffer();
  return MakeGlb(MakeEmbeddedTextureJson(*aBinary, false), *aBinary);
}

static TCollection_AsciiString MakePointGltf()
{
  NCollection_Buffer aBuffer(NCollection_BaseAllocator::CommonBaseAllocator(), 12);
  WriteFloat(aBuffer, 0, 1.0f);
  WriteFloat(aBuffer, 4, 2.0f);
  WriteFloat(aBuffer, 8, 3.0f);

  TCollection_AsciiString aJson =
    R"({"asset":{"version":"2.0"},"scene":0,"scenes":[{"nodes":[0]}],)"
    R"("nodes":[{"mesh":0}],"meshes":[{"primitives":[{"attributes":{"POSITION":0},"mode":0}]}],)"
    R"("accessors":[{"bufferView":0,"componentType":5126,"count":1,"type":"VEC3"}],)"
    R"("bufferViews":[{"buffer":0,"byteLength":12}],"buffers":[{"byteLength":12,"uri":"data:application/octet-stream;base64,)";
  aJson += FSD_Base64::Encode(aBuffer.Data(), aBuffer.Size());
  aJson += R"("}]})";
  return aJson;
}

//==================================================================================================

static TCollection_AsciiString MakeIndexedPrimitivesJson(const NCollection_Buffer& theBuffer,
                                                         const int                 theComponentType,
                                                         const size_t              theIndexSize,
                                                         const TCollection_AsciiString& theUri)
{
  const int aPositionBytes = 9 * sizeof(float);
  return TCollection_AsciiString(
           R"({"asset":{"version":"2.0"},"scene":0,"scenes":[{"nodes":[0]}],)"
           R"("nodes":[{"mesh":0}],"meshes":[{"primitives":[)"
           R"({"mode":0,"attributes":{"POSITION":0,"NORMAL":0},"indices":1},)"
           R"({"mode":1,"attributes":{"POSITION":0,"NORMAL":0},"indices":2},)"
           R"({"mode":4,"attributes":{"POSITION":0,"NORMAL":0},"indices":1}]}],)"
           R"("accessors":[{"bufferView":0,"componentType":5126,"count":3,"type":"VEC3"},)"
           R"({"bufferView":1,"componentType":)")
         + theComponentType + R"(,"count":3,"type":"SCALAR"},{"bufferView":1,"componentType":)"
         + theComponentType
         + R"(,"count":2,"type":"SCALAR"}],"bufferViews":[{"buffer":0,"byteLength":)"
         + aPositionBytes + R"(},{"buffer":0,"byteLength":)" + static_cast<int>(3 * theIndexSize)
         + R"(,"byteOffset":)" + aPositionBytes + R"(}],"buffers":[{"byteLength":)"
         + static_cast<int>(theBuffer.Size()) + R"(,"uri":")" + theUri + R"("}]})";
}

//==================================================================================================

static occ::handle<TDocStd_Document> NewDocument()
{
  occ::handle<TDocStd_Application> anApplication = new TDocStd_Application();
  occ::handle<TDocStd_Document>    aDocument;
  anApplication->NewDocument("BinXCAF", aDocument);
  return aDocument;
}
} // namespace

// bugs/de_mesh/gltf_read/cubeemb: embedded texture data in a base64 bufferView is accepted from a
// caller-owned stream and the imported mesh keeps its material assignment.
TEST(DEGLTF_Bug_Test, MeshBug_31312_StreamEmbeddedTexture)
{
  occ::handle<DEGLTF_Provider> aProvider = new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  std::istringstream           anInput(MakeEmbeddedTextureGltf().ToCString());
  DE_Provider::ReadStreamList  aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("cube.gltf", anInput));
  occ::handle<TDocStd_Document> aDocument = NewDocument();

  ASSERT_TRUE(aProvider->Read(aStreams, aDocument));
  occ::handle<XCAFDoc_ShapeTool>  aShapeTool = XCAFDoc_DocumentTool::ShapeTool(aDocument->Main());
  NCollection_Sequence<TDF_Label> aLabels;
  aShapeTool->GetFreeShapes(aLabels);
  ASSERT_EQ(aLabels.Length(), 1);

  const TopoDS_Shape aShape = aShapeTool->GetShape(aLabels.First());
  ASSERT_FALSE(aShape.IsNull());
  EXPECT_EQ(CountShapeElements(aShape, TopAbs_FACE), 1);
  EXPECT_EQ(CountShapeElements(aShape, TopAbs_COMPOUND), 0);
  EXPECT_EQ(CountTriangulationTriangles(aShape), 12);
  EXPECT_EQ(CountTriangulationNodes(aShape), 24);

  occ::handle<XCAFDoc_VisMaterialTool> aMaterialTool =
    XCAFDoc_DocumentTool::VisMaterialTool(aDocument->Main());
  NCollection_Sequence<TDF_Label> aMaterialLabels;
  aMaterialTool->GetMaterials(aMaterialLabels);
  ASSERT_EQ(aMaterialLabels.Length(), 1);
  occ::handle<XCAFDoc_VisMaterial> aMaterial = aMaterialTool->GetMaterial(aMaterialLabels.First());
  ASSERT_FALSE(aMaterial.IsNull());
  EXPECT_FALSE(aMaterial->PbrMaterial().BaseColorTexture.IsNull());
}

TEST(DEGLTF_Bug_Test, DEWrapper_StreamRead)
{
  DE_Wrapper aWrapper;
  ASSERT_TRUE(aWrapper.Bind(new DEGLTF_ConfigurationNode()));

  std::istringstream          anInput(MakeInterleavedGltf().ToCString());
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("interleaved.gltf", anInput));
  TopoDS_Shape aShape;

  ASSERT_TRUE(aWrapper.Read(aStreams, aShape));
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(CountShapeElements(aShape, TopAbs_FACE), 1);
}

TEST(DEGLTF_Bug_Test, MeshStreamPoints)
{
  occ::handle<DEGLTF_Provider> aProvider = new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  std::istringstream           anInput(MakePointGltf().ToCString());
  DE_Provider::ReadStreamList  aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("points.gltf", anInput));
  TopoDS_Shape aShape;

  ASSERT_TRUE(aProvider->Read(aStreams, aShape));
  EXPECT_EQ(CountShapeElements(aShape, TopAbs_VERTEX), 1);
}

//==================================================================================================

TEST(DEGLTF_Bug_Test, FileReadResolvesPointAndLineAttributes)
{
  const float        aPositions[] = {0, 0, 0, 1, 0, 0, 0, 1, 0};
  NCollection_Buffer aBytes(NCollection_BaseAllocator::CommonBaseAllocator(),
                            sizeof(aPositions) + 3 * sizeof(uint32_t));
  std::memset(aBytes.ChangeData(), 0, aBytes.Size());
  std::memcpy(aBytes.ChangeData(), aPositions, sizeof(aPositions));
  const std::array<std::pair<int, size_t>, 3> aComponents = {
    {{RWGltf_GltfAccessorCompType_UInt8, sizeof(uint8_t)},
     {RWGltf_GltfAccessorCompType_UInt16, sizeof(uint16_t)},
     {RWGltf_GltfAccessorCompType_UInt32, sizeof(uint32_t)}}};
  DEGLTF_Provider                               aProvider(new DEGLTF_ConfigurationNode());
  const uint32_t                                aTriangleIndices[] = {0, 1, 2, 0, 1, 3};
  std::array<uint8_t, sizeof(aTriangleIndices)> anIndexBytes{};
  for (const auto& [aType, aSize] : aComponents)
  {
    SCOPED_TRACE(aType);
    for (uint32_t anIndex = 0; anIndex < 3; ++anIndex)
    {
      std::memcpy(aBytes.ChangeData() + sizeof(aPositions) + anIndex * aSize, &anIndex, aSize);
    }
    const TCollection_AsciiString aUri =
      TCollection_AsciiString("data:application/octet-stream;base64,")
      + FSD_Base64::Encode(aBytes.Data(), aBytes.Size());
    std::istringstream anInput(MakeIndexedPrimitivesJson(aBytes, aType, aSize, aUri).ToCString());
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("indexed.gltf", anInput));
    TopoDS_Shape aShape;
    ASSERT_TRUE(aProvider.Read(aStreams, aShape));
    EXPECT_EQ(CountShapeElements(aShape, TopAbs_VERTEX), 3);
    EXPECT_EQ(CountShapeElements(aShape, TopAbs_EDGE), 1);
    EXPECT_EQ(CountTriangulationTriangles(aShape), 1);

    for (size_t anIndex = 0; anIndex < 6; ++anIndex)
    {
      std::memcpy(anIndexBytes.data() + anIndex * aSize, aTriangleIndices + anIndex, aSize);
    }
    Standard_ArrayStreamBuffer anIndexBuffer(reinterpret_cast<const char*>(anIndexBytes.data()),
                                             6 * aSize);
    std::istream               anIndexStream(&anIndexBuffer);
    const occ::handle<RWGltf_GltfLatePrimitiveArray> aMesh =
      new RWGltf_GltfLatePrimitiveArray("indices", "indices");
    aMesh->SetPrimitiveMode(RWGltf_GltfPrimitiveMode_Triangles);
    aMesh->ResizeNodes(3, false);
    aMesh->SetNode(1, gp_Pnt(0, 0, 0));
    aMesh->SetNode(2, gp_Pnt(1, 0, 0));
    aMesh->SetNode(3, gp_Pnt(0, 1, 0));
    RWGltf_GltfAccessor anAccessor;
    anAccessor.ComponentType = static_cast<RWGltf_GltfAccessorCompType>(aType);
    anAccessor.Type          = RWGltf_GltfAccessorLayout_Scalar;
    anAccessor.Count         = 6;
    RWGltf_TriangulationReader aReader;
    aReader.SetToSkipDegenerates(false);
    ASSERT_TRUE(
      aReader.ReadStream(aMesh, aMesh, anIndexStream, anAccessor, RWGltf_GltfArrayType_Indices));
    EXPECT_EQ(aMesh->NbTriangles(), 1);
    EXPECT_EQ(aMesh->DegeneratedTriNb(), 1);
    EXPECT_EQ(aMesh->Triangle(1).Value(1), 1);
    EXPECT_EQ(aMesh->Triangle(1).Value(2), 2);
    EXPECT_EQ(aMesh->Triangle(1).Value(3), 3);
  }

  OSD_File aBinaryFile, aJsonFile;
  aBinaryFile.BuildTemporary();
  ASSERT_FALSE(aBinaryFile.Failed());
  aJsonFile.BuildTemporary();
  ASSERT_FALSE(aJsonFile.Failed());
  aBinaryFile.Write(aBytes.ChangeData(), static_cast<int>(aBytes.Size()));
  aBinaryFile.Close();
  EXPECT_FALSE(aBinaryFile.Failed());
  OSD_Path aBinaryPath, aJsonPath;
  aBinaryFile.Path(aBinaryPath);
  aJsonFile.Path(aJsonPath);
  TCollection_AsciiString aBinaryName, aJsonName, aFolder, aFileName;
  aBinaryPath.SystemName(aBinaryName);
  aJsonPath.SystemName(aJsonName);
  OSD_Path::FolderAndFileFromPath(aBinaryName, aFolder, aFileName);
  const TCollection_AsciiString aJson =
    MakeIndexedPrimitivesJson(aBytes,
                              RWGltf_GltfAccessorCompType_UInt32,
                              sizeof(uint32_t),
                              aFileName);
  aJsonFile.Write(aJson, aJson.Length());
  aJsonFile.Close();
  EXPECT_FALSE(aJsonFile.Failed());

  TopoDS_Shape aShape;
  const bool   isRead = aProvider.Read(aJsonName, aShape);
  aJsonFile.Remove();
  aBinaryFile.Remove();
  ASSERT_TRUE(isRead);
  EXPECT_EQ(CountShapeElements(aShape, TopAbs_VERTEX), 3);
  EXPECT_EQ(CountShapeElements(aShape, TopAbs_EDGE), 1);
  EXPECT_EQ(CountTriangulationTriangles(aShape), 1);
}

TEST(DEGLTF_Bug_Test, MeshBug_31312_GlbStreamEmbeddedTexture)
{
  occ::handle<DEGLTF_Provider>    aProvider = new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  occ::handle<NCollection_Buffer> aGlb      = MakeEmbeddedTextureGlb();
  Standard_ArrayStreamBuffer      aStreamBuffer(reinterpret_cast<const char*>(aGlb->Data()),
                                           aGlb->Size());
  std::istream                    anInput(&aStreamBuffer);
  DE_Provider::ReadStreamList     aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("cube.glb", anInput));
  occ::handle<TDocStd_Document> aDocument = NewDocument();

  ASSERT_TRUE(aProvider->Read(aStreams, aDocument));
  occ::handle<XCAFDoc_VisMaterialTool> aMaterialTool =
    XCAFDoc_DocumentTool::VisMaterialTool(aDocument->Main());
  NCollection_Sequence<TDF_Label> aMaterialLabels;
  aMaterialTool->GetMaterials(aMaterialLabels);
  ASSERT_EQ(aMaterialLabels.Length(), 1);
  occ::handle<XCAFDoc_VisMaterial> aMaterial = aMaterialTool->GetMaterial(aMaterialLabels.First());
  ASSERT_FALSE(aMaterial.IsNull());
  ASSERT_FALSE(aMaterial->PbrMaterial().BaseColorTexture.IsNull());
  EXPECT_TRUE(aMaterial->PbrMaterial().BaseColorTexture->FilePath().IsEmpty());
  EXPECT_EQ(aMaterial->PbrMaterial().BaseColorTexture->FileOffset(), -1);
  EXPECT_NE(aMaterial->PbrMaterial().BaseColorTexture->TextureId().Search("texturebuf://"), -1);
}

// bugs/de_mesh/gltf_read/boxinterleaved: byteStride is applied to interleaved vertex attributes
// when the glTF document is read from a stream.
TEST(DEGLTF_Bug_Test, MeshBug_31332_StreamInterleavedBuffer)
{
  occ::handle<DEGLTF_Provider> aProvider = new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  std::istringstream           anInput(MakeInterleavedGltf().ToCString());
  DE_Provider::ReadStreamList  aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("interleaved.gltf", anInput));
  TopoDS_Shape aShape;

  ASSERT_TRUE(aProvider->Read(aStreams, aShape));
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(CountShapeElements(aShape, TopAbs_FACE), 1);
  EXPECT_EQ(CountShapeElements(aShape, TopAbs_COMPOUND), 1);
  EXPECT_EQ(CountTriangulationTriangles(aShape), 12);
  EXPECT_EQ(CountTriangulationNodes(aShape), 24);
}

// The same interleaved regression is also checked in binary glTF to validate seeking within a
// caller-owned stream for the BIN chunk.
TEST(DEGLTF_Bug_Test, MeshBug_31332_GlbStreamInterleavedBuffer)
{
  occ::handle<DEGLTF_Provider>    aProvider = new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  occ::handle<NCollection_Buffer> aGlb      = MakeInterleavedGlb();
  ASSERT_EQ(aGlb->Data()[0], uint8_t('g'));
  ASSERT_EQ(aGlb->Data()[1], uint8_t('l'));
  ASSERT_EQ(aGlb->Data()[2], uint8_t('T'));
  ASSERT_EQ(aGlb->Data()[3], uint8_t('F'));
  Standard_ArrayStreamBuffer  aStreamBuffer(reinterpret_cast<const char*>(aGlb->Data()),
                                           aGlb->Size());
  std::istream                anInput(&aStreamBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("interleaved.glb", anInput));
  TopoDS_Shape aShape;

  ASSERT_TRUE(aProvider->Read(aStreams, aShape));
  EXPECT_FALSE(aShape.IsNull());
  EXPECT_EQ(CountShapeElements(aShape, TopAbs_FACE), 1);
  EXPECT_EQ(CountShapeElements(aShape, TopAbs_COMPOUND), 1);
  EXPECT_EQ(CountTriangulationTriangles(aShape), 12);
  EXPECT_EQ(CountTriangulationNodes(aShape), 24);
}

TEST(DEGLTF_Bug_Test, NamedStreamExternalBufferAndImageView)
{
  const occ::handle<NCollection_Buffer> aBytes = MakeEmbeddedTextureBuffer();
  TCollection_AsciiString               aJson  = MakeEmbeddedTextureJson(*aBytes, false);
  aJson.InsertBefore(aJson.Length() - 2, R"(,"uri":"../buffers%2Fmesh%2ebin")");
  Standard_ArrayStreamBuffer  aJsonBuffer(aJson.ToCString(), aJson.Length());
  Standard_ArrayStreamBuffer  aMeshBuffer(reinterpret_cast<const char*>(aBytes->Data()),
                                         aBytes->Size());
  std::istream                aJsonStream(&aJsonBuffer);
  std::istream                aMeshStream(&aMeshBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/cube.gltf", aJsonStream));
  aStreams.Append(DE_Provider::ReadStreamNode("buffers/mesh.bin", aMeshStream));
  occ::handle<DEGLTF_Provider> aProvider = new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  const occ::handle<TDocStd_Document> aDocument = NewDocument();
  ASSERT_TRUE(aProvider->Read(aStreams, aDocument));
  NCollection_Sequence<TDF_Label> aRoots;
  XCAFDoc_DocumentTool::ShapeTool(aDocument->Main())->GetFreeShapes(aRoots);
  ASSERT_EQ(aRoots.Length(), 1);
  EXPECT_EQ(CountTriangulationTriangles(XCAFDoc_ShapeTool::GetShape(aRoots.First())), 12);
  NCollection_Sequence<TDF_Label>            aMaterials;
  const occ::handle<XCAFDoc_VisMaterialTool> aTool =
    XCAFDoc_DocumentTool::VisMaterialTool(aDocument->Main());
  aTool->GetMaterials(aMaterials);
  ASSERT_EQ(aMaterials.Length(), 1);
  const occ::handle<Image_Texture> aTexture =
    aTool->GetMaterial(aMaterials.First())->PbrMaterial().BaseColorTexture;
  ASSERT_FALSE(aTexture.IsNull());
  EXPECT_TRUE(aTexture->FilePath().IsEmpty());
  ASSERT_FALSE(aTexture->DataBuffer().IsNull());
  EXPECT_EQ(aTexture->DataBuffer()->Size(), aBytes->Size() - 840);
  EXPECT_EQ(std::memcmp(aTexture->DataBuffer()->Data(), aBytes->Data() + 840, aBytes->Size() - 840),
            0);
}

TEST(DEGLTF_Bug_Test, NamedStreamExternalTextureRetainsBytes)
{
  const occ::handle<NCollection_Buffer> aBytes = MakeEmbeddedTextureBuffer();
  TCollection_AsciiString               aJson  = MakeEmbeddedTextureJson(*aBytes, false);
  aJson.InsertBefore(aJson.Length() - 2, R"(,"uri":"../buffers/mesh.bin")");
  const TCollection_AsciiString anOldImage =
    R"({"bufferView":4,"mimeType":"image/png","name":"Embedded"})";
  const int anImagePos = aJson.Search(anOldImage);
  ASSERT_GT(anImagePos, 0);
  aJson.Remove(anImagePos, anOldImage.Length());
  aJson.InsertBefore(anImagePos, R"({"uri":"../images/paint%20red.png"})");
  const occ::handle<TDocStd_Document> aDocument = NewDocument();
  {
    Standard_ArrayStreamBuffer  aJsonBuffer(aJson.ToCString(), aJson.Length());
    Standard_ArrayStreamBuffer  aMeshBuffer(reinterpret_cast<const char*>(aBytes->Data()),
                                           aBytes->Size());
    Standard_ArrayStreamBuffer  anImageBuffer(reinterpret_cast<const char*>(aBytes->Data() + 840),
                                             aBytes->Size() - 840);
    std::istream                aJsonStream(&aJsonBuffer);
    std::istream                aMeshStream(&aMeshBuffer);
    std::istream                anImageStream(&anImageBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("models/cube.gltf", aJsonStream));
    aStreams.Append(DE_Provider::ReadStreamNode("images/paint red.png", anImageStream));
    aStreams.Append(DE_Provider::ReadStreamNode("buffers/mesh.bin", aMeshStream));
    const occ::handle<DEGLTF_Provider> aProvider =
      new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
    ASSERT_TRUE(aProvider->Read(aStreams, aDocument));
  }
  NCollection_Sequence<TDF_Label>            aMaterials;
  const occ::handle<XCAFDoc_VisMaterialTool> aTool =
    XCAFDoc_DocumentTool::VisMaterialTool(aDocument->Main());
  aTool->GetMaterials(aMaterials);
  ASSERT_EQ(aMaterials.Length(), 1);
  const occ::handle<Image_Texture> aTexture =
    aTool->GetMaterial(aMaterials.First())->PbrMaterial().BaseColorTexture;
  ASSERT_FALSE(aTexture.IsNull());
  EXPECT_TRUE(aTexture->FilePath().IsEmpty());
  ASSERT_FALSE(aTexture->DataBuffer().IsNull());
  EXPECT_EQ(aTexture->DataBuffer()->Size(), aBytes->Size() - 840);
  EXPECT_EQ(std::memcmp(aTexture->DataBuffer()->Data(), aBytes->Data() + 840, aBytes->Size() - 840),
            0);
}

TEST(DEGLTF_Bug_Test, NamedStreamRejectsMissingTexture)
{
  const occ::handle<NCollection_Buffer> aBytes = MakeEmbeddedTextureBuffer();
  TCollection_AsciiString               aJson  = MakeEmbeddedTextureJson(*aBytes, false);
  aJson.InsertBefore(aJson.Length() - 2, R"(,"uri":"mesh.bin")");
  const TCollection_AsciiString anOldImage =
    R"({"bufferView":4,"mimeType":"image/png","name":"Embedded"})";
  const int anImagePos = aJson.Search(anOldImage);
  ASSERT_GT(anImagePos, 0);
  aJson.Remove(anImagePos, anOldImage.Length());
  aJson.InsertBefore(anImagePos, R"({"uri":"unadmitted.png"})");
  Standard_ArrayStreamBuffer  aJsonBuffer(aJson.ToCString(), aJson.Length());
  std::istream                aJsonStream(&aJsonBuffer);
  Standard_ArrayStreamBuffer  aMeshBuffer(reinterpret_cast<const char*>(aBytes->Data()),
                                         aBytes->Size());
  std::istream                aMeshStream(&aMeshBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("cube.gltf", aJsonStream));
  aStreams.Append(DE_Provider::ReadStreamNode("mesh.bin", aMeshStream));
  const occ::handle<DEGLTF_Provider> aProvider =
    new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  EXPECT_FALSE(aProvider->Read(aStreams, NewDocument()));
}

TEST(DEGLTF_Bug_Test, NamedStreamRejectsFailedMeshLoading)
{
  const float aPositions[] = {0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
  for (const int aMode : {0, 1})
  {
    SCOPED_TRACE(aMode);
    const TCollection_AsciiString aJson =
      TCollection_AsciiString(R"({"asset":{"version":"2.0"},"scene":0,"scenes":[{"nodes":[0]}],)"
                              R"("nodes":[{"mesh":0}],"meshes":[{"primitives":[{"mode":)")
      + aMode
      + R"(,"attributes":{"POSITION":0,"NORMAL":1}}]}],)"
        R"("accessors":[{"bufferView":0,"componentType":5126,"count":2,"type":"VEC3"},)"
        R"({"bufferView":0,"byteOffset":24,"componentType":5126,"count":2,"type":"VEC3"}],)"
        R"("bufferViews":[{"buffer":0,"byteLength":24}],"buffers":[{"byteLength":24,"uri":"mesh.bin"}]})";
    std::istringstream          aJsonStream(aJson.ToCString());
    Standard_ArrayStreamBuffer  aMeshBuffer(reinterpret_cast<const char*>(aPositions),
                                           sizeof(aPositions));
    std::istream                aMeshStream(&aMeshBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("points.gltf", aJsonStream));
    aStreams.Append(DE_Provider::ReadStreamNode("mesh.bin", aMeshStream));
    DEGLTF_Provider aProvider(new DEGLTF_ConfigurationNode());
    TopoDS_Shape    aShape;
    EXPECT_FALSE(aProvider.Read(aStreams, aShape));
    EXPECT_TRUE(aShape.IsNull());
  }
  const occ::handle<NCollection_Buffer> aBytes = MakeEmbeddedTextureBuffer();
  TCollection_AsciiString               aJson  = MakeEmbeddedTextureJson(*aBytes, false);
  aJson.InsertBefore(aJson.Length() - 2, R"(,"uri":"mesh.bin")");
  const TCollection_AsciiString aCount    = R"("count":36)";
  const int                     aCountPos = aJson.Search(aCount);
  ASSERT_GT(aCountPos, 0);
  aJson.Remove(aCountPos, aCount.Length());
  aJson.InsertBefore(aCountPos, R"("count":200)");
  const TCollection_AsciiString aPrimitive =
    R"({"attributes":{"POSITION":0,"NORMAL":1,"TEXCOORD_0":2},"indices":3,"material":0})";
  const int aPrimitivePos = aJson.Search(aPrimitive);
  ASSERT_GT(aPrimitivePos, 0);
  aJson.InsertBefore(aPrimitivePos + aPrimitive.Length(),
                     TCollection_AsciiString(",") + aPrimitive);
  Standard_ArrayStreamBuffer  aJsonBuffer(aJson.ToCString(), aJson.Length());
  Standard_ArrayStreamBuffer  aMeshBuffer(reinterpret_cast<const char*>(aBytes->Data()),
                                         aBytes->Size());
  std::istream                aJsonStream(&aJsonBuffer);
  std::istream                aMeshStream(&aMeshBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("cube.gltf", aJsonStream));
  aStreams.Append(DE_Provider::ReadStreamNode("mesh.bin", aMeshStream));
  const occ::handle<DEGLTF_ConfigurationNode> aNode     = new DEGLTF_ConfigurationNode();
  const occ::handle<DEGLTF_Provider>          aProvider = new DEGLTF_Provider(aNode);
  TopoDS_Shape                                aShape;
  for (const bool isParallel : {false, true})
  {
    for (const bool isSkipLoading : {false, true})
    {
      aNode->InternalParameters.ReadParallel            = isParallel;
      aNode->InternalParameters.ReadSkipLateDataLoading = isSkipLoading;
      aJsonStream.clear();
      aJsonStream.seekg(0);
      aMeshStream.clear();
      aMeshStream.seekg(0);
      EXPECT_FALSE(aProvider->Read(aStreams, aShape));
    }
  }
}

TEST(DEGLTF_Bug_Test, NamedStreamGlbTextureViewUsesItsExternalBuffer)
{
  const occ::handle<NCollection_Buffer> aBytes      = MakeEmbeddedTextureBuffer();
  TCollection_AsciiString               aJson       = MakeEmbeddedTextureJson(*aBytes, false);
  const int                             anImageSize = static_cast<int>(aBytes->Size() - 840);
  const TCollection_AsciiString         anOldView =
    TCollection_AsciiString(R"({"buffer":0,"byteLength":)") + anImageSize + R"(,"byteOffset":840})";
  const int aViewPos = aJson.Search(anOldView);
  ASSERT_GT(aViewPos, 0);
  aJson.Remove(aViewPos, anOldView.Length());
  aJson.InsertBefore(aViewPos,
                     TCollection_AsciiString(R"({"buffer":1,"byteLength":)") + anImageSize
                       + R"(,"byteOffset":0})");
  aJson.InsertBefore(aJson.Length() - 1,
                     TCollection_AsciiString(R"(,{"byteLength":)") + anImageSize
                       + R"(,"uri":"../images/paint.png"})");
  const occ::handle<NCollection_Buffer> aGlb = MakeGlb(aJson, *aBytes);
  Standard_ArrayStreamBuffer aMainBuffer(reinterpret_cast<const char*>(aGlb->Data()), aGlb->Size());
  Standard_ArrayStreamBuffer anImageBuffer(reinterpret_cast<const char*>(aBytes->Data() + 840),
                                           aBytes->Size() - 840);
  std::istream               aMainStream(&aMainBuffer);
  std::istream               anImageStream(&anImageBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("models/cube.glb", aMainStream));
  aStreams.Append(DE_Provider::ReadStreamNode("images/paint.png", anImageStream));
  const occ::handle<DEGLTF_Provider> aProvider =
    new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  const occ::handle<TDocStd_Document> aDocument = NewDocument();
  ASSERT_TRUE(aProvider->Read(aStreams, aDocument));
  NCollection_Sequence<TDF_Label>            aMaterials;
  const occ::handle<XCAFDoc_VisMaterialTool> aTool =
    XCAFDoc_DocumentTool::VisMaterialTool(aDocument->Main());
  aTool->GetMaterials(aMaterials);
  ASSERT_EQ(aMaterials.Length(), 1);
  const occ::handle<Image_Texture> aTexture =
    aTool->GetMaterial(aMaterials.First())->PbrMaterial().BaseColorTexture;
  ASSERT_FALSE(aTexture.IsNull());
  ASSERT_FALSE(aTexture->DataBuffer().IsNull());
  ASSERT_EQ(aTexture->DataBuffer()->Size(), aBytes->Size() - 840);
  EXPECT_EQ(std::memcmp(aTexture->DataBuffer()->Data(), aBytes->Data() + 840, aBytes->Size() - 840),
            0);
}

TEST(DEGLTF_Bug_Test, NamedStreamRejectsUnadmittedBufferReferences)
{
  const occ::handle<NCollection_Buffer> aBytes = MakeEmbeddedTextureBuffer();
  const occ::handle<DEGLTF_Provider>    aProvider =
    new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  for (const char* aUri : {"mesh.bin",
                           "../../mesh.bin",
                           "/mesh.bin",
                           "file:mesh.bin",
                           "https://example.test/mesh.bin",
                           "mesh%00.bin",
                           "mesh%ZZ.bin",
                           "mesh%0x.bin",
                           "mesh%+1.bin",
                           "mesh%2G.bin",
                           "mesh%",
                           "mesh%2"})
  {
    TCollection_AsciiString aJson = MakeEmbeddedTextureJson(*aBytes, false);
    aJson.InsertBefore(aJson.Length() - 2, TCollection_AsciiString(",\"uri\":\"") + aUri + "\"");
    Standard_ArrayStreamBuffer  aJsonBuffer(aJson.ToCString(), aJson.Length());
    std::istream                aJsonStream(&aJsonBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("models/cube.gltf", aJsonStream));
    aStreams.Append(DE_Provider::ReadStreamNode("unused.gltf", aJsonStream));
    TopoDS_Shape aShape;
    EXPECT_FALSE(aProvider->Read(aStreams, aShape)) << aUri;
  }
}

TEST(DEGLTF_Bug_Test, NamedStreamRejectsTruncatedResourceAndDuplicateNames)
{
  const occ::handle<NCollection_Buffer> aBytes = MakeEmbeddedTextureBuffer();
  TCollection_AsciiString               aJson  = MakeEmbeddedTextureJson(*aBytes, false);
  aJson.InsertBefore(aJson.Length() - 2, R"(,"uri":"mesh.bin")");
  const occ::handle<DEGLTF_Provider> aProvider =
    new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  for (const bool isDuplicate : {false, true})
  {
    Standard_ArrayStreamBuffer  aJsonBuffer(aJson.ToCString(), aJson.Length());
    Standard_ArrayStreamBuffer  aMeshBuffer(reinterpret_cast<const char*>(aBytes->Data()), 500);
    std::istream                aJsonStream(&aJsonBuffer);
    std::istream                aMeshStream(&aMeshBuffer);
    DE_Provider::ReadStreamList aStreams;
    aStreams.Append(DE_Provider::ReadStreamNode("cube.gltf", aJsonStream));
    aStreams.Append(DE_Provider::ReadStreamNode("mesh.bin", aMeshStream));
    if (isDuplicate)
    {
      aStreams.Append(DE_Provider::ReadStreamNode("./mesh.bin", aMeshStream));
    }
    TopoDS_Shape aShape;
    EXPECT_FALSE(aProvider->Read(aStreams, aShape));
  }
}

TEST(DEGLTF_Bug_Test, NamedStreamRejectsUriWithEmbeddedNull)
{
  const occ::handle<NCollection_Buffer> aBytes = MakeEmbeddedTextureBuffer();
  TCollection_AsciiString               aJson  = MakeEmbeddedTextureJson(*aBytes, false);
  aJson.InsertBefore(aJson.Length() - 2, R"(,"uri":"mesh.bin\u0000suffix")");
  Standard_ArrayStreamBuffer  aJsonBuffer(aJson.ToCString(), aJson.Length());
  Standard_ArrayStreamBuffer  aMeshBuffer(reinterpret_cast<const char*>(aBytes->Data()),
                                         aBytes->Size());
  std::istream                aJsonStream(&aJsonBuffer);
  std::istream                aMeshStream(&aMeshBuffer);
  DE_Provider::ReadStreamList aStreams;
  aStreams.Append(DE_Provider::ReadStreamNode("cube.gltf", aJsonStream));
  aStreams.Append(DE_Provider::ReadStreamNode("mesh.bin", aMeshStream));
  const occ::handle<DEGLTF_Provider> aProvider =
    new DEGLTF_Provider(new DEGLTF_ConfigurationNode());
  EXPECT_FALSE(aProvider->Read(aStreams, NewDocument()));
}
