// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <RWGltf_CafWriter.hxx>

#include <BRep_Builder.hxx>
#include <gp_Quaternion.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <OSD_FileSystem.hxx>
#include <OSD_File.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_Path.hxx>
#include <OSD_Timer.hxx>
#include <RWGltf_GltfAccessorLayout.hxx>
#include <RWGltf_GltfMaterialMap.hxx>
#include <RWGltf_GltfPrimitiveMode.hxx>
#include <RWGltf_GltfRootElement.hxx>
#include <RWGltf_GltfSceneNodeMap.hxx>
#include <RWMesh.hxx>
#include <RWMesh_EdgeIterator.hxx>
#include <RWMesh_ShapeIterator.hxx>
#include <RWMesh_FaceIterator.hxx>
#include <RWMesh_VertexIterator.hxx>
#include <Standard_Version.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS_Compound.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>

#ifdef HAVE_RAPIDJSON
  #include <RWGltf_GltfOStreamWriter.hxx>
#endif

#ifdef HAVE_DRACO
  #include <Standard_WarningsDisable.hxx>
  #include <draco/compression/encode.h>
  #include <Standard_WarningsRestore.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(RWGltf_CafWriter, Standard_Transient)

namespace
{
//! Write three float values.
static void writeVec3(std::ostream& theStream, const gp_XYZ& theVec3)
{
  Graphic3d_Vec3 aVec3(float(theVec3.X()), float(theVec3.Y()), float(theVec3.Z()));
  theStream.write((const char*)aVec3.GetData(), sizeof(aVec3));
}

//! Write three float values.
static void writeVec3(std::ostream& theStream, const Graphic3d_Vec3& theVec3)
{
  theStream.write((const char*)theVec3.GetData(), sizeof(theVec3));
}

//! Write two float values.
static void writeVec2(std::ostream& theStream, const gp_XY& theVec2)
{
  Graphic3d_Vec2 aVec2(float(theVec2.X()), float(theVec2.Y()));
  theStream.write((const char*)aVec2.GetData(), sizeof(aVec2));
}

//! General function to write triangle indices.
template <typename VecType>
static void writeTriangle(std::ostream& theStream, const VecType& theTri)
{
  theStream.write(reinterpret_cast<const char*>(theTri.GetData()), sizeof(theTri));
}

//! General function to write vertex index.
template <typename T>
static void writeVertex(std::ostream& theStream, const T& theVertex)
{
  theStream.write(reinterpret_cast<const char*>(&theVertex), sizeof(T));
}

#ifdef HAVE_DRACO
//! Write nodes to Draco mesh
static void writeNodesToDracoMesh(draco::Mesh& theMesh, const std::vector<Graphic3d_Vec3>& theNodes)
{
  if (theNodes.empty())
  {
    return;
  }

  draco::PointAttribute anAttr;
  anAttr.Init(draco::GeometryAttribute::POSITION, 3, draco::DT_FLOAT32, false, sizeof(float) * 3);
  const int              anId = theMesh.AddAttribute(anAttr, true, uint32_t(theNodes.size()));
  draco::PointAttribute* aPtr = theMesh.attribute(anId);
  draco::PointIndex      anIndex(0);
  for (size_t aNodeInd = 0; aNodeInd < theNodes.size(); ++aNodeInd, ++anIndex)
  {
    aPtr->SetAttributeValue(aPtr->mapped_index(anIndex), theNodes[aNodeInd].GetData());
  }
}

//! Write normals to Draco mesh
static void writeNormalsToDracoMesh(draco::Mesh&                       theMesh,
                                    const std::vector<Graphic3d_Vec3>& theNormals)
{
  if (theNormals.empty())
  {
    return;
  }

  draco::PointAttribute anAttr;
  anAttr.Init(draco::GeometryAttribute::NORMAL, 3, draco::DT_FLOAT32, false, sizeof(float) * 3);
  const int              anId = theMesh.AddAttribute(anAttr, true, uint32_t(theNormals.size()));
  draco::PointAttribute* aPtr = theMesh.attribute(anId);
  draco::PointIndex      anIndex(0);
  for (size_t aNormInd = 0; aNormInd < theNormals.size(); ++aNormInd, ++anIndex)
  {
    aPtr->SetAttributeValue(aPtr->mapped_index(anIndex), theNormals[aNormInd].GetData());
  }
}

//! Write texture UV coordinates to Draco mesh
static void writeTexCoordsToDracoMesh(draco::Mesh&                       theMesh,
                                      const std::vector<Graphic3d_Vec2>& theTexCoord)
{
  if (theTexCoord.empty())
  {
    return;
  }

  draco::PointAttribute anAttr;
  anAttr.Init(draco::GeometryAttribute::TEX_COORD, 2, draco::DT_FLOAT32, false, sizeof(float) * 2);
  const int              anId = theMesh.AddAttribute(anAttr, true, uint32_t(theTexCoord.size()));
  draco::PointAttribute* aPtr = theMesh.attribute(anId);
  draco::PointIndex      anIndex(0);
  for (size_t aTexInd = 0; aTexInd < theTexCoord.size(); ++aTexInd, ++anIndex)
  {
    aPtr->SetAttributeValue(aPtr->mapped_index(anIndex), theTexCoord[aTexInd].GetData());
  }
}

//! Write indices to Draco mesh
static void writeIndicesToDracoMesh(draco::Mesh&                      theMesh,
                                    const std::vector<Poly_Triangle>& theIndices)
{
  draco::Mesh::Face aFace;
  int               anIndex = 0;
  for (size_t anInd = 0; anInd < theIndices.size(); ++anInd, ++anIndex)
  {
    const Poly_Triangle& anElem = theIndices[anInd];
    aFace[0]                    = anElem.Value(1);
    aFace[1]                    = anElem.Value(2);
    aFace[2]                    = anElem.Value(3);
    theMesh.SetFace(draco::FaceIndex(anIndex), aFace);
  }
}
#endif
} // namespace

#ifdef HAVE_DRACO
//! Functor for parallel execution of encoding meshes to Draco buffers.
class DracoEncodingFunctor
{
public:
  DracoEncodingFunctor(const Message_ProgressRange&                                theProgress,
                       draco::Encoder&                                             theDracoEncoder,
                       const std::vector<std::shared_ptr<RWGltf_CafWriter::Mesh>>& theMeshes,
                       std::vector<std::shared_ptr<draco::EncoderBuffer>>& theEncoderBuffers)
      : myProgress(theProgress, "Draco compression", std::max(1, int(theMeshes.size()))),
        myDracoEncoder(&theDracoEncoder),
        myRanges(0, int(theMeshes.size()) - 1),
        myMeshes(&theMeshes),
        myEncoderBuffers(&theEncoderBuffers)
  {
    for (int anIndex = 0; anIndex != int(theMeshes.size()); ++anIndex)
    {
      myRanges.SetValue(anIndex, myProgress.Next());
    }
  }

  void operator()(int theMeshIndex) const
  {
    const std::shared_ptr<RWGltf_CafWriter::Mesh>& aCurrentMesh = myMeshes->at(theMeshIndex);
    if (aCurrentMesh->NodesVec.empty())
    {
      return;
    }

    Message_ProgressScope aScope(myRanges[theMeshIndex], NULL, 1);

    draco::Mesh aMesh;
    writeNodesToDracoMesh(aMesh, aCurrentMesh->NodesVec);

    if (!aCurrentMesh->NormalsVec.empty())
    {
      writeNormalsToDracoMesh(aMesh, aCurrentMesh->NormalsVec);
    }

    if (!aCurrentMesh->TexCoordsVec.empty())
    {
      writeTexCoordsToDracoMesh(aMesh, aCurrentMesh->TexCoordsVec);
    }

    writeIndicesToDracoMesh(aMesh, aCurrentMesh->IndicesVec);

    std::shared_ptr<draco::EncoderBuffer> anEncoderBuffer =
      std::make_shared<draco::EncoderBuffer>();
    draco::Status aStatus = myDracoEncoder->EncodeMeshToBuffer(aMesh, anEncoderBuffer.get());
    if (aStatus.ok())
    {
      myEncoderBuffers->at(theMeshIndex) = anEncoderBuffer;
    }

    aScope.Next();
  }

private:
  Message_ProgressScope                                       myProgress;
  draco::Encoder*                                             myDracoEncoder;
  NCollection_Array1<Message_ProgressRange>                   myRanges;
  const std::vector<std::shared_ptr<RWGltf_CafWriter::Mesh>>* myMeshes;
  std::vector<std::shared_ptr<draco::EncoderBuffer>>*         myEncoderBuffers;
};
#endif

//=================================================================================================

RWGltf_CafWriter::RWGltf_CafWriter(const TCollection_AsciiString& theFile,
                                   Standard_Boolean               theIsBinary)
    : myFile(theFile),
      myTrsfFormat(RWGltf_WriterTrsfFormat_Compact),
      myNodeNameFormat(RWMesh_NameFormat_InstanceOrProduct),
      myMeshNameFormat(RWMesh_NameFormat_Product),
      myIsBinary(theIsBinary),
      myIsForcedUVExport(false),
      myToEmbedTexturesInGlb(true),
      myToMergeFaces(false),
      myToSplitIndices16(false),
      myBinDataLen64(0),
      myToParallel(false)
{
  myCSTrsf.SetOutputLengthUnit(1.0); // meters
  myCSTrsf.SetOutputCoordinateSystem(RWMesh_CoordinateSystem_glTF);

  TCollection_AsciiString aFolder, aFileName, aShortFileNameBase, aFileExt;
  OSD_Path::FolderAndFileFromPath(theFile, aFolder, aFileName);
  OSD_Path::FileNameAndExtension(aFileName, aShortFileNameBase, aFileExt);

  myBinFileNameShort = aShortFileNameBase + ".bin" + (myIsBinary ? ".tmp" : "");
  myBinFileNameFull  = !aFolder.IsEmpty() ? aFolder + myBinFileNameShort : myBinFileNameShort;
}

//=================================================================================================

RWGltf_CafWriter::~RWGltf_CafWriter()
{
  myWriter.reset();
}

//=================================================================================================

TCollection_AsciiString RWGltf_CafWriter::formatName(RWMesh_NameFormat theFormat,
                                                     const TDF_Label&  theLabel,
                                                     const TDF_Label&  theRefLabel) const
{
  return RWMesh::FormatName(theFormat, theLabel, theRefLabel);
}

//=================================================================================================

TopAbs_ShapeEnum RWGltf_CafWriter::getShapeType(const TopoDS_Shape& theShape) const
{
  TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
  if (aShapeType == TopAbs_COMPOUND)
  {
    // Compounds are created in the case of merged faces.
    // Assuming that all shapes in the compound are of the same type
    TopoDS_Iterator it(theShape);
    Standard_ProgramError_Raise_if(!it.More(), "Empty compound");
    aShapeType = it.Value().ShapeType();
  }

  return aShapeType;
}

//=================================================================================================

Standard_Boolean RWGltf_CafWriter::toSkipShape(const RWMesh_ShapeIterator& theShapeIter) const
{
  return theShapeIter.IsEmpty();
}

//=================================================================================================

Standard_Boolean RWGltf_CafWriter::hasTriangulation(const RWGltf_GltfFace& theGltfFace) const
{
  TopAbs_ShapeEnum shapeType = getShapeType(theGltfFace.Shape);

  switch (shapeType)
  {
    case TopAbs_FACE:
      return true;
    case TopAbs_EDGE:
    case TopAbs_VERTEX:
      return false;
    default:
      throw Standard_ProgramError("Unsupported shape type");
  }
}

//=================================================================================================

void RWGltf_CafWriter::saveNodes(RWGltf_GltfFace&                               theGltfFace,
                                 std::ostream&                                  theBinFile,
                                 const RWMesh_ShapeIterator&                    theShapeIter,
                                 Standard_Integer&                              theAccessorNb,
                                 const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh) const
{
  if (theGltfFace.NodePos.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    theGltfFace.NodePos.Id            = theAccessorNb++;
    theGltfFace.NodePos.ByteOffset    = (int64_t)theBinFile.tellp() - myBuffViewPos.ByteOffset;
    theGltfFace.NodePos.Type          = RWGltf_GltfAccessorLayout_Vec3;
    theGltfFace.NodePos.ComponentType = RWGltf_GltfAccessorCompType_Float32;
  }
  else
  {
    if (theMesh.get() == nullptr)
    {
      const int64_t aPos = theGltfFace.NodePos.ByteOffset + myBuffViewPos.ByteOffset
                           + theGltfFace.NodePos.Count * sizeof(Graphic3d_Vec3);
      Standard_ASSERT_RAISE(aPos == (int64_t)theBinFile.tellp(), "wrong offset");
    }
  }
  theGltfFace.NodePos.Count += theShapeIter.NbNodes();

  const Standard_Integer aNodeUpper = theShapeIter.NodeUpper();
  for (Standard_Integer aNodeIter = theShapeIter.NodeLower(); aNodeIter <= aNodeUpper; ++aNodeIter)
  {
    gp_XYZ aNode = theShapeIter.NodeTransformed(aNodeIter).XYZ();
    myCSTrsf.TransformPosition(aNode);
    theGltfFace.NodePos.BndBox.Add(Graphic3d_Vec3d(aNode.X(), aNode.Y(), aNode.Z()));
    if (theMesh.get() != nullptr && hasTriangulation(theGltfFace))
    {
      theMesh->NodesVec.push_back(
        Graphic3d_Vec3(float(aNode.X()), float(aNode.Y()), float(aNode.Z())));
    }
    else
    {
      writeVec3(theBinFile, aNode);
    }
  }
}

//=================================================================================================

void RWGltf_CafWriter::saveNormals(RWGltf_GltfFace&                               theGltfFace,
                                   std::ostream&                                  theBinFile,
                                   const RWMesh_FaceIterator&                     theFaceIter,
                                   Standard_Integer&                              theAccessorNb,
                                   const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh) const
{
  if (!theFaceIter.HasNormals())
  {
    return;
  }

  if (theGltfFace.NodeNorm.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    theGltfFace.NodeNorm.Id            = theAccessorNb++;
    theGltfFace.NodeNorm.ByteOffset    = (int64_t)theBinFile.tellp() - myBuffViewNorm.ByteOffset;
    theGltfFace.NodeNorm.Type          = RWGltf_GltfAccessorLayout_Vec3;
    theGltfFace.NodeNorm.ComponentType = RWGltf_GltfAccessorCompType_Float32;
  }
  else
  {
    if (theMesh.get() == nullptr)
    {
      const int64_t aPos = theGltfFace.NodeNorm.ByteOffset + myBuffViewNorm.ByteOffset
                           + theGltfFace.NodeNorm.Count * sizeof(Graphic3d_Vec3);
      Standard_ASSERT_RAISE(aPos == (int64_t)theBinFile.tellp(), "wrong offset");
    }
  }
  theGltfFace.NodeNorm.Count += theFaceIter.NbNodes();

  const Standard_Integer aNodeUpper = theFaceIter.NodeUpper();
  for (Standard_Integer aNodeIter = theFaceIter.NodeLower(); aNodeIter <= aNodeUpper; ++aNodeIter)
  {
    const gp_Dir   aNormal = theFaceIter.NormalTransformed(aNodeIter);
    Graphic3d_Vec3 aVecNormal((float)aNormal.X(), (float)aNormal.Y(), (float)aNormal.Z());
    myCSTrsf.TransformNormal(aVecNormal);
    if (theMesh.get() != nullptr)
    {
      theMesh->NormalsVec.push_back(aVecNormal);
    }
    else
    {
      writeVec3(theBinFile, aVecNormal);
    }
  }
}

//=================================================================================================

void RWGltf_CafWriter::saveTextCoords(RWGltf_GltfFace&                               theGltfFace,
                                      std::ostream&                                  theBinFile,
                                      const RWMesh_FaceIterator&                     theFaceIter,
                                      Standard_Integer&                              theAccessorNb,
                                      const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh) const
{
  if (!theFaceIter.HasTexCoords())
  {
    return;
  }
  if (!myIsForcedUVExport)
  {
    if (theFaceIter.Style().Material().IsNull())
    {
      return;
    }

    if (RWGltf_GltfMaterialMap::baseColorTexture(theFaceIter.Style().Material()).IsNull()
        && theFaceIter.Style().Material()->PbrMaterial().MetallicRoughnessTexture.IsNull()
        && theFaceIter.Style().Material()->PbrMaterial().EmissiveTexture.IsNull()
        && theFaceIter.Style().Material()->PbrMaterial().OcclusionTexture.IsNull()
        && theFaceIter.Style().Material()->PbrMaterial().NormalTexture.IsNull())
    {
      return;
    }
  }

  if (theGltfFace.NodeUV.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    theGltfFace.NodeUV.Id            = theAccessorNb++;
    theGltfFace.NodeUV.ByteOffset    = (int64_t)theBinFile.tellp() - myBuffViewTextCoord.ByteOffset;
    theGltfFace.NodeUV.Type          = RWGltf_GltfAccessorLayout_Vec2;
    theGltfFace.NodeUV.ComponentType = RWGltf_GltfAccessorCompType_Float32;
  }
  else
  {
    if (theMesh.get() == nullptr)
    {
      const int64_t aPos = theGltfFace.NodeUV.ByteOffset + myBuffViewTextCoord.ByteOffset
                           + theGltfFace.NodeUV.Count * sizeof(Graphic3d_Vec2);
      Standard_ASSERT_RAISE(aPos == (int64_t)theBinFile.tellp(), "wrong offset");
    }
  }
  theGltfFace.NodeUV.Count += theFaceIter.NbNodes();

  const Standard_Integer aNodeUpper = theFaceIter.NodeUpper();
  for (Standard_Integer aNodeIter = theFaceIter.NodeLower(); aNodeIter <= aNodeUpper; ++aNodeIter)
  {
    gp_Pnt2d aTexCoord = theFaceIter.NodeTexCoord(aNodeIter);
    aTexCoord.SetY(1.0 - aTexCoord.Y());
    if (theMesh.get() != nullptr)
    {
      theMesh->TexCoordsVec.push_back(Graphic3d_Vec2((float)aTexCoord.X(), (float)aTexCoord.Y()));
    }
    else
    {
      writeVec2(theBinFile, aTexCoord.XY());
    }
  }
}

//=================================================================================================

void RWGltf_CafWriter::saveTriangleIndices(RWGltf_GltfFace&           theGltfFace,
                                           std::ostream&              theBinFile,
                                           const RWMesh_FaceIterator& theFaceIter,
                                           const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh)
{
  const Standard_Integer aNodeFirst = theGltfFace.NbIndexedNodes - theFaceIter.ElemLower();
  theGltfFace.NbIndexedNodes += theFaceIter.NbNodes();
  theGltfFace.Indices.Count += theFaceIter.NbTriangles() * 3;
  for (Standard_Integer anElemIter = theFaceIter.ElemLower(); anElemIter <= theFaceIter.ElemUpper();
       ++anElemIter)
  {
    Poly_Triangle aTri = theFaceIter.TriangleOriented(anElemIter);
    aTri(1) += aNodeFirst;
    aTri(2) += aNodeFirst;
    aTri(3) += aNodeFirst;
    if (theMesh.get() != nullptr)
    {
      theMesh->IndicesVec.push_back(aTri);
    }
    else
    {
      if (theGltfFace.Indices.ComponentType == RWGltf_GltfAccessorCompType_UInt16)
      {
        writeTriangle(
          theBinFile,
          NCollection_Vec3<uint16_t>((uint16_t)aTri(1), (uint16_t)aTri(2), (uint16_t)aTri(3)));
      }
      else
      {
        writeTriangle(theBinFile, Graphic3d_Vec3i(aTri(1), aTri(2), aTri(3)));
      }
    }
  }
}

//=================================================================================================

void RWGltf_CafWriter::saveEdgeIndices(RWGltf_GltfFace&           theGltfFace,
                                       std::ostream&              theBinFile,
                                       const RWMesh_EdgeIterator& theEdgeIter)
{
  const Standard_Integer aNodeFirst = theGltfFace.NbIndexedNodes;
  theGltfFace.NbIndexedNodes += theEdgeIter.NbNodes();

  const Standard_Integer numSegments = std::max(0, theEdgeIter.NbNodes() - 1);
  // each segment writes two indices
  theGltfFace.Indices.Count += numSegments * 2;

  for (Standard_Integer i = 0; i < numSegments; ++i)
  {
    Standard_Integer i0 = aNodeFirst + i;
    Standard_Integer i1 = aNodeFirst + i + 1;

    if (theGltfFace.Indices.ComponentType == RWGltf_GltfAccessorCompType_UInt16)
    {
      writeVertex(theBinFile, (uint16_t)i0);
      writeVertex(theBinFile, (uint16_t)i1);
    }
    else
    {
      writeVertex(theBinFile, i0);
      writeVertex(theBinFile, i1);
    }
  }
}

//=================================================================================================

void RWGltf_CafWriter::saveVertexIndices(RWGltf_GltfFace&             theGltfFace,
                                         std::ostream&                theBinFile,
                                         const RWMesh_VertexIterator& theVertexIter)
{
  const Standard_Integer aNodeFirst = theGltfFace.NbIndexedNodes - theVertexIter.ElemLower();
  theGltfFace.NbIndexedNodes += theVertexIter.NbNodes();
  theGltfFace.Indices.Count += theVertexIter.NbNodes();
  for (Standard_Integer anElemIter = theVertexIter.ElemLower();
       anElemIter <= theVertexIter.ElemUpper();
       ++anElemIter)
  {
    if (theGltfFace.Indices.ComponentType == RWGltf_GltfAccessorCompType_UInt16)
    {
      writeVertex(theBinFile, (uint16_t)(anElemIter + aNodeFirst));
    }
    else
    {
      writeVertex(theBinFile, anElemIter + aNodeFirst);
    }
  }
}

//=================================================================================================

void RWGltf_CafWriter::saveIndices(RWGltf_GltfFace&                               theGltfFace,
                                   std::ostream&                                  theBinFile,
                                   const RWMesh_ShapeIterator&                    theShapeIter,
                                   Standard_Integer&                              theAccessorNb,
                                   const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh)
{
  if (theGltfFace.Indices.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    theGltfFace.Indices.Id         = theAccessorNb++;
    theGltfFace.Indices.ByteOffset = (int64_t)theBinFile.tellp() - myBuffViewInd.ByteOffset;
    theGltfFace.Indices.Type       = RWGltf_GltfAccessorLayout_Scalar;
    theGltfFace.Indices.ComponentType =
      theGltfFace.NodePos.Count > std::numeric_limits<uint16_t>::max()
        ? RWGltf_GltfAccessorCompType_UInt32
        : RWGltf_GltfAccessorCompType_UInt16;
  }
  else
  {
    if (theMesh.get() == nullptr)
    {
      const int64_t aRefPos = (int64_t)theBinFile.tellp();
      const int64_t aPos =
        theGltfFace.Indices.ByteOffset + myBuffViewInd.ByteOffset
        + theGltfFace.Indices.Count
            * (theGltfFace.Indices.ComponentType == RWGltf_GltfAccessorCompType_UInt32
                 ? sizeof(uint32_t)
                 : sizeof(uint16_t));
      Standard_ASSERT_RAISE(aPos == aRefPos, "wrong offset");
    }
  }

  if (const RWMesh_FaceIterator* aFaceIter =
        dynamic_cast<const RWMesh_FaceIterator*>(&theShapeIter))
  {
    saveTriangleIndices(theGltfFace, theBinFile, *aFaceIter, theMesh);
  }
  else if (const RWMesh_EdgeIterator* anEdgeIter =
             dynamic_cast<const RWMesh_EdgeIterator*>(&theShapeIter))
  {
    saveEdgeIndices(theGltfFace, theBinFile, *anEdgeIter);
  }
  else if (const RWMesh_VertexIterator* aVertexIter =
             dynamic_cast<const RWMesh_VertexIterator*>(&theShapeIter))
  {
    saveVertexIndices(theGltfFace, theBinFile, *aVertexIter);
  }
}

//=================================================================================================

bool RWGltf_CafWriter::Perform(const Handle(TDocStd_Document)&             theDocument,
                               const TColStd_IndexedDataMapOfStringString& theFileInfo,
                               const Message_ProgressRange&                theProgress)
{
  TDF_LabelSequence         aRoots;
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());
  aShapeTool->GetFreeShapes(aRoots);
  return Perform(theDocument, aRoots, NULL, theFileInfo, theProgress);
}

//=================================================================================================

bool RWGltf_CafWriter::Perform(const Handle(TDocStd_Document)&             theDocument,
                               const TDF_LabelSequence&                    theRootLabels,
                               const TColStd_MapOfAsciiString*             theLabelFilter,
                               const TColStd_IndexedDataMapOfStringString& theFileInfo,
                               const Message_ProgressRange&                theProgress)
{
  Standard_Real aLengthUnit = 1.;
  if (XCAFDoc_DocumentTool::GetLengthUnit(theDocument, aLengthUnit))
  {
    myCSTrsf.SetInputLengthUnit(aLengthUnit);
  }
  const Standard_Integer aDefSamplerId = 0;
  myMaterialMap                        = new RWGltf_GltfMaterialMap(myFile, aDefSamplerId);
  myMaterialMap->SetDefaultStyle(myDefaultStyle);

  Message_ProgressScope aPSentry(theProgress, "Writing glTF file", 2);
  if (!writeBinData(theDocument, theRootLabels, theLabelFilter, aPSentry.Next()))
  {
    return false;
  }

  if (!aPSentry.More())
  {
    return false;
  }

  return writeJson(theDocument, theRootLabels, theLabelFilter, theFileInfo, aPSentry.Next());
}

//=================================================================================================

void RWGltf_CafWriter::dispatchShapes(
  const XCAFPrs_DocumentNode&                                  theDocNode,
  const Message_ProgressScope&                                 thePSentryBin,
  NCollection_DataMap<XCAFPrs_Style, Handle(RWGltf_GltfFace)>& theMergedFaces,
  RWMesh_ShapeIterator&                                        theShapeIter)
{
  if (myToMergeFaces && theShapeIter.More())
  {
    RWGltf_StyledShape aStyledShape(theShapeIter.ExploredShape(), theDocNode.Style);
    if (myBinDataMap.Contains(aStyledShape))
    {
      return;
    }

    Handle(RWGltf_GltfFaceList) aGltfFaceList = new RWGltf_GltfFaceList();
    myBinDataMap.Add(aStyledShape, aGltfFaceList);
    for (; theShapeIter.More() && thePSentryBin.More(); theShapeIter.Next())
    {
      if (toSkipShape(theShapeIter))
      {
        continue;
      }

      Handle(RWGltf_GltfFace) aGltfFace;
      if (!theMergedFaces.Find(theShapeIter.Style(), aGltfFace))
      {
        aGltfFace = new RWGltf_GltfFace();
        aGltfFaceList->Append(aGltfFace);
        aGltfFace->Shape          = theShapeIter.Shape();
        aGltfFace->Style          = theShapeIter.Style();
        aGltfFace->NbIndexedNodes = theShapeIter.NbNodes();
        theMergedFaces.Bind(theShapeIter.Style(), aGltfFace);
      }
      else if (myToSplitIndices16
               && aGltfFace->NbIndexedNodes < std::numeric_limits<uint16_t>::max()
               && (aGltfFace->NbIndexedNodes + theShapeIter.NbNodes())
                    >= std::numeric_limits<uint16_t>::max())
      {
        theMergedFaces.UnBind(theShapeIter.Style());
        aGltfFace = new RWGltf_GltfFace();
        aGltfFaceList->Append(aGltfFace);
        aGltfFace->Shape          = theShapeIter.Shape();
        aGltfFace->Style          = theShapeIter.Style();
        aGltfFace->NbIndexedNodes = theShapeIter.NbNodes();
        theMergedFaces.Bind(theShapeIter.Style(), aGltfFace);
      }
      else
      {
        if (aGltfFace->Shape.ShapeType() != TopAbs_COMPOUND)
        {
          TopoDS_Shape    anOldShape = aGltfFace->Shape;
          TopoDS_Compound aComp;
          BRep_Builder().MakeCompound(aComp);
          BRep_Builder().Add(aComp, anOldShape);
          aGltfFace->Shape = aComp;
        }
        BRep_Builder().Add(aGltfFace->Shape, theShapeIter.Shape());
        aGltfFace->NbIndexedNodes += theShapeIter.NbNodes();
      }
    }
  }
  else
  {
    for (; theShapeIter.More() && thePSentryBin.More(); theShapeIter.Next())
    {
      RWGltf_StyledShape aStyledShape(theShapeIter.Shape(), theShapeIter.Style());
      if (toSkipShape(theShapeIter) || myBinDataMap.Contains(aStyledShape))
      {
        continue;
      }

      Handle(RWGltf_GltfFaceList) aGltfFaceList = new RWGltf_GltfFaceList();
      Handle(RWGltf_GltfFace)     aGltfFace     = new RWGltf_GltfFace();
      aGltfFace->Shape                          = theShapeIter.Shape();
      aGltfFace->Style                          = theShapeIter.Style();
      aGltfFaceList->Append(aGltfFace);
      myBinDataMap.Add(aStyledShape, aGltfFaceList);
    }
  }
}

//=================================================================================================

Standard_Boolean RWGltf_CafWriter::writeShapesToBin(
  RWGltf_GltfFace&                               theGltfFace,
  std::ostream&                                  theBinFile,
  RWMesh_ShapeIterator&                          theShapeIter,
  Standard_Integer&                              theAccessorNb,
  const std::shared_ptr<RWGltf_CafWriter::Mesh>& theMesh,
  const RWGltf_GltfArrayType                     theArrType,
  const Message_ProgressScope&                   thePSentryBin)
{
  for (; theShapeIter.More() && thePSentryBin.More(); theShapeIter.Next())
  {
    switch (theArrType)
    {
      case RWGltf_GltfArrayType_Position: {
        // clang-format off
        theGltfFace.NbIndexedNodes = 0; // reset to zero before RWGltf_GltfArrayType_Indices step
        // clang-format on
        saveNodes(theGltfFace, theBinFile, theShapeIter, theAccessorNb, theMesh);
        break;
      }
      case RWGltf_GltfArrayType_Normal: {
        if (const RWMesh_FaceIterator* aFaceIter =
              dynamic_cast<const RWMesh_FaceIterator*>(&theShapeIter))
        {
          saveNormals(theGltfFace, theBinFile, *aFaceIter, theAccessorNb, theMesh);
        }
        break;
      }
      case RWGltf_GltfArrayType_TCoord0: {
        if (const RWMesh_FaceIterator* aFaceIter =
              dynamic_cast<const RWMesh_FaceIterator*>(&theShapeIter))
        {
          saveTextCoords(theGltfFace, theBinFile, *aFaceIter, theAccessorNb, theMesh);
        }
        break;
      }
      case RWGltf_GltfArrayType_Indices: {
        saveIndices(theGltfFace, theBinFile, theShapeIter, theAccessorNb, theMesh);
        break;
      }
      default: {
        break;
      }
    }

    if (!theBinFile.good())
    {
      Message::SendFail(TCollection_AsciiString("File '") + myBinFileNameFull
                        + "' cannot be written");
      return false;
    }
  }
  return true;
}

//=================================================================================================

bool RWGltf_CafWriter::writeBinData(const Handle(TDocStd_Document)& theDocument,
                                    const TDF_LabelSequence&        theRootLabels,
                                    const TColStd_MapOfAsciiString* theLabelFilter,
                                    const Message_ProgressRange&    theProgress)
{
#ifndef HAVE_DRACO
  if (myDracoParameters.DracoCompression)
  {
    Message::SendFail("Error: cannot use Draco compression, Draco library missing.");
    return false;
  }
#endif

  myBuffViewPos.Id         = RWGltf_GltfAccessor::INVALID_ID;
  myBuffViewPos.ByteOffset = 0;
  myBuffViewPos.ByteLength = 0;
  myBuffViewPos.ByteStride = 12;
  myBuffViewPos.Target     = RWGltf_GltfBufferViewTarget_ARRAY_BUFFER;

  myBuffViewNorm.Id         = RWGltf_GltfAccessor::INVALID_ID;
  myBuffViewNorm.ByteOffset = 0;
  myBuffViewNorm.ByteLength = 0;
  myBuffViewNorm.ByteStride = 12;
  myBuffViewNorm.Target     = RWGltf_GltfBufferViewTarget_ARRAY_BUFFER;

  myBuffViewTextCoord.Id         = RWGltf_GltfAccessor::INVALID_ID;
  myBuffViewTextCoord.ByteOffset = 0;
  myBuffViewTextCoord.ByteLength = 0;
  myBuffViewTextCoord.ByteStride = 8;
  myBuffViewTextCoord.Target     = RWGltf_GltfBufferViewTarget_ARRAY_BUFFER;

  myBuffViewInd.Id         = RWGltf_GltfAccessor::INVALID_ID;
  myBuffViewInd.ByteOffset = 0;
  myBuffViewInd.ByteLength = 0;
  myBuffViewInd.Target     = RWGltf_GltfBufferViewTarget_ELEMENT_ARRAY_BUFFER;

  myBuffViewsDraco.clear();

  myBinDataMap.Clear();
  myBinDataLen64 = 0;

  Message_ProgressScope aScope(theProgress,
                               "Write binary data",
                               myDracoParameters.DracoCompression ? 2 : 1);

  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> aBinFile =
    aFileSystem->OpenOStream(myBinFileNameFull, std::ios::out | std::ios::binary);
  if (aBinFile.get() == NULL || !aBinFile->good())
  {
    Message::SendFail(TCollection_AsciiString("File '") + myBinFileNameFull
                      + "' can not be created");
    return false;
  }

  Message_ProgressScope      aPSentryBin(aScope.Next(), "Binary data", 4);
  const RWGltf_GltfArrayType anArrTypes[4] = {RWGltf_GltfArrayType_Position,
                                              RWGltf_GltfArrayType_Normal,
                                              RWGltf_GltfArrayType_TCoord0,
                                              RWGltf_GltfArrayType_Indices};

  // dispatch shapes
  NCollection_DataMap<XCAFPrs_Style, Handle(RWGltf_GltfFace)> aMergedFaces;
  for (XCAFPrs_DocumentExplorer aDocExplorer(theDocument,
                                             theRootLabels,
                                             XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
       aDocExplorer.More() && aPSentryBin.More();
       aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL && !theLabelFilter->Contains(aDocNode.Id))
    {
      continue;
    }

    // transformation will be stored at scene nodes
    aMergedFaces.Clear(false);
    {
      RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
      dispatchShapes(aDocNode, aPSentryBin, aMergedFaces, aFaceIter);
    }
    {
      RWMesh_EdgeIterator anEdgeIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
      dispatchShapes(aDocNode, aPSentryBin, aMergedFaces, anEdgeIter);
    }
    {
      RWMesh_VertexIterator aVertexIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
      dispatchShapes(aDocNode, aPSentryBin, aMergedFaces, aVertexIter);
    }
  }

  std::vector<std::shared_ptr<RWGltf_CafWriter::Mesh>> aMeshes;
  Standard_Integer                                     aNbAccessors = 0;
  NCollection_Map<Handle(RWGltf_GltfFaceList)>         aWrittenFaces;
  NCollection_DataMap<TopoDS_Shape, Handle(RWGltf_GltfFace), TopTools_ShapeMapHasher>
    aWrittenPrimData;
  for (Standard_Integer aTypeIter = 0; aTypeIter < 4; ++aTypeIter)
  {
    const RWGltf_GltfArrayType anArrType = (RWGltf_GltfArrayType)anArrTypes[aTypeIter];
    RWGltf_GltfBufferView*     aBuffView = NULL;
    switch (anArrType)
    {
      case RWGltf_GltfArrayType_Position:
        aBuffView = &myBuffViewPos;
        break;
      case RWGltf_GltfArrayType_Normal:
        aBuffView = &myBuffViewNorm;
        break;
      case RWGltf_GltfArrayType_TCoord0:
        aBuffView = &myBuffViewTextCoord;
        break;
      case RWGltf_GltfArrayType_Indices:
        aBuffView = &myBuffViewInd;
        break;
      default:
        break;
    }
    aBuffView->ByteOffset = aBinFile->tellp();
    aWrittenFaces.Clear(false);
    aWrittenPrimData.Clear(false);
#ifdef HAVE_DRACO
    size_t aMeshIndex = 0;
#endif

    Standard_Boolean isFacesOnly = Standard_True;
    for (ShapeToGltfFaceMap::Iterator aBinDataIter(myBinDataMap);
         aBinDataIter.More() && aPSentryBin.More();
         aBinDataIter.Next())
    {
      const Handle(RWGltf_GltfFaceList)& aGltfFaceList = aBinDataIter.Value();
      if (!aWrittenFaces.Add(aGltfFaceList)) // skip repeating faces
      {
        continue;
      }

      for (RWGltf_GltfFaceList::Iterator aGltfFaceIter(*aGltfFaceList);
           aGltfFaceIter.More() && aPSentryBin.More();
           aGltfFaceIter.Next())
      {
        const Handle(RWGltf_GltfFace)& aGltfFace = aGltfFaceIter.Value();

        std::shared_ptr<RWGltf_CafWriter::Mesh> aMeshPtr;
#ifdef HAVE_DRACO
        ++aMeshIndex;
        if (myDracoParameters.DracoCompression)
        {
          if (aMeshIndex <= aMeshes.size())
          {
            aMeshPtr = aMeshes.at(aMeshIndex - 1);
          }
          else
          {
            aMeshes.push_back(std::make_shared<RWGltf_CafWriter::Mesh>(RWGltf_CafWriter::Mesh()));
            aMeshPtr = aMeshes.back();
          }
        }
#endif

        Handle(RWGltf_GltfFace) anOldGltfFace;
        if (aWrittenPrimData.Find(aGltfFace->Shape, anOldGltfFace))
        {
          switch (anArrType)
          {
            case RWGltf_GltfArrayType_Position: {
              aGltfFace->NodePos = anOldGltfFace->NodePos;
              break;
            }
            case RWGltf_GltfArrayType_Normal: {
              aGltfFace->NodeNorm = anOldGltfFace->NodeNorm;
              break;
            }
            case RWGltf_GltfArrayType_TCoord0: {
              aGltfFace->NodeUV = anOldGltfFace->NodeUV;
              break;
            }
            case RWGltf_GltfArrayType_Indices: {
              aGltfFace->Indices = anOldGltfFace->Indices;
              break;
            }
            default: {
              break;
            }
          }
          continue;
        }
        aWrittenPrimData.Bind(aGltfFace->Shape, aGltfFace);

        Standard_Boolean wasWrittenNonFace = Standard_False;
        TopAbs_ShapeEnum shapeType         = getShapeType(aGltfFace->Shape);

        switch (shapeType)
        {
          case TopAbs_EDGE: {
            RWMesh_EdgeIterator anIter(aGltfFace->Shape, aGltfFace->Style);
            if (!writeShapesToBin(*aGltfFace,
                                  *aBinFile,
                                  anIter,
                                  aNbAccessors,
                                  aMeshPtr,
                                  anArrType,
                                  aPSentryBin))
            {
              return false;
            }
            wasWrittenNonFace = Standard_True;
            break;
          }
          case TopAbs_VERTEX: {
            RWMesh_VertexIterator anIter(aGltfFace->Shape, aGltfFace->Style);
            if (!writeShapesToBin(*aGltfFace,
                                  *aBinFile,
                                  anIter,
                                  aNbAccessors,
                                  aMeshPtr,
                                  anArrType,
                                  aPSentryBin))
            {
              return false;
            }
            wasWrittenNonFace = Standard_True;
            break;
          }
          default: {
            RWMesh_FaceIterator anIter(aGltfFace->Shape, aGltfFace->Style);
            if (!writeShapesToBin(*aGltfFace,
                                  *aBinFile,
                                  anIter,
                                  aNbAccessors,
                                  aMeshPtr,
                                  anArrType,
                                  aPSentryBin))
            {
              return false;
            }
            break;
          }
        }

        // add alignment by 4 bytes (might happen on RWGltf_GltfAccessorCompType_UInt16 indices)
        if (!myDracoParameters.DracoCompression || wasWrittenNonFace)
        {
          isFacesOnly           = Standard_False;
          int64_t aContentLen64 = (int64_t)aBinFile->tellp();
          while (aContentLen64 % 4 != 0)
          {
            aBinFile->write(" ", 1);
            ++aContentLen64;
          }
        }
      }
    }

    if (!myDracoParameters.DracoCompression || !isFacesOnly)
    {
      aBuffView->ByteLength = (int64_t)aBinFile->tellp() - aBuffView->ByteOffset;
    }
    if (!aPSentryBin.More())
    {
      return false;
    }

    aPSentryBin.Next();
  }

  int aBuffViewId = 0;
  if (myBuffViewPos.ByteLength > 0)
  {
    myBuffViewPos.Id = aBuffViewId++;
  }
  if (myBuffViewNorm.ByteLength > 0)
  {
    myBuffViewNorm.Id = aBuffViewId++;
  }
  if (myBuffViewTextCoord.ByteLength > 0)
  {
    myBuffViewTextCoord.Id = aBuffViewId++;
  }
  if (myBuffViewInd.ByteLength > 0)
  {
    myBuffViewInd.Id = aBuffViewId++;
  }

  if (myDracoParameters.DracoCompression)
  {
#ifdef HAVE_DRACO
    OSD_Timer aDracoTimer;
    aDracoTimer.Start();
    draco::Encoder aDracoEncoder;
    aDracoEncoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION,
                                           myDracoParameters.QuantizePositionBits);
    aDracoEncoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL,
                                           myDracoParameters.QuantizeNormalBits);
    aDracoEncoder.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD,
                                           myDracoParameters.QuantizeTexcoordBits);
    aDracoEncoder.SetAttributeQuantization(draco::GeometryAttribute::COLOR,
                                           myDracoParameters.QuantizeColorBits);
    aDracoEncoder.SetAttributeQuantization(draco::GeometryAttribute::GENERIC,
                                           myDracoParameters.QuantizeGenericBits);
    aDracoEncoder.SetSpeedOptions(myDracoParameters.CompressionLevel,
                                  myDracoParameters.CompressionLevel);

    std::vector<std::shared_ptr<draco::EncoderBuffer>> anEncoderBuffers(aMeshes.size());
    DracoEncodingFunctor aFunctor(aScope.Next(), aDracoEncoder, aMeshes, anEncoderBuffers);
    OSD_Parallel::For(0, int(aMeshes.size()), aFunctor, !myToParallel);

    int aNbSkippedBuffers = 0;
    for (size_t aBuffInd = 0; aBuffInd != anEncoderBuffers.size(); ++aBuffInd)
    {
      if (anEncoderBuffers.at(aBuffInd).get() == nullptr)
      {
        if (aBuffViewId == 0)
        {
          Message::SendFail() << "Error: mesh not encoded in draco buffer.";
          return false;
        }
        Message::SendWarning() << "Warning: mesh is not encoded as a Draco buffer and has been "
                                  "loaded into a regular buffer.";
        aNbSkippedBuffers++;
        continue;
      }
      RWGltf_GltfBufferView aBuffViewDraco;
      aBuffViewDraco.Id                         = (int)aBuffInd + aBuffViewId - aNbSkippedBuffers;
      aBuffViewDraco.ByteOffset                 = aBinFile->tellp();
      const draco::EncoderBuffer& anEncoderBuff = *anEncoderBuffers.at(aBuffInd);
      aBinFile->write(anEncoderBuff.data(), std::streamsize(anEncoderBuff.size()));
      if (!aBinFile->good())
      {
        Message::SendFail(TCollection_AsciiString("File '") + myBinFileNameFull
                          + "' cannot be written");
        return false;
      }

      int64_t aLength = (int64_t)aBinFile->tellp();
      while (aLength % 4 != 0)
      {
        aBinFile->write(" ", 1);
        ++aLength;
      }

      aBuffViewDraco.ByteLength = aLength - aBuffViewDraco.ByteOffset;
      myBuffViewsDraco.push_back(aBuffViewDraco);
    }
    aDracoTimer.Stop();
    Message::SendInfo(TCollection_AsciiString("Draco compression time: ")
                      + aDracoTimer.ElapsedTime() + " s");
#endif
  }

  if (myIsBinary && myToEmbedTexturesInGlb)
  {
    // save unique image textures
    for (XCAFPrs_DocumentExplorer aDocExplorer(theDocument,
                                               theRootLabels,
                                               XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
         aDocExplorer.More() && aPSentryBin.More();
         aDocExplorer.Next())
    {
      const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
      if (theLabelFilter != NULL && !theLabelFilter->Contains(aDocNode.Id))
      {
        continue;
      }

      for (RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel,
                                         TopLoc_Location(),
                                         true,
                                         aDocNode.Style);
           aFaceIter.More();
           aFaceIter.Next())
      {
        if (toSkipShape(aFaceIter))
        {
          continue;
        }

        myMaterialMap->AddGlbImages(*aBinFile, aFaceIter.Style());
      }
    }
  }
  // myMaterialMap->FlushGlbBufferViews() will put image bufferView's IDs at the end of list

  myBinDataLen64 = aBinFile->tellp();
  aBinFile->flush();
  if (!aBinFile->good())
  {
    Message::SendFail(TCollection_AsciiString("File '") + myBinFileNameFull
                      + "' cannot be written");
    return false;
  }
  aBinFile.reset();
  return true;
}

//=================================================================================================

bool RWGltf_CafWriter::writeJson(const Handle(TDocStd_Document)&             theDocument,
                                 const TDF_LabelSequence&                    theRootLabels,
                                 const TColStd_MapOfAsciiString*             theLabelFilter,
                                 const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                 const Message_ProgressRange&                theProgress)
{
#ifdef HAVE_RAPIDJSON
  myWriter.reset();

  // write vertex arrays
  Message_ProgressScope aPSentryBin(theProgress, "Header data", 2);

  const Standard_Integer aBinDataBufferId = 0;
  const Standard_Integer aDefSceneId      = 0;

  const TCollection_AsciiString aFileNameGltf = myFile;
  const Handle(OSD_FileSystem)& aFileSystem   = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> aGltfContentFile =
    aFileSystem->OpenOStream(aFileNameGltf, std::ios::out | std::ios::binary);
  if (aGltfContentFile.get() == NULL || !aGltfContentFile->good())
  {
    Message::SendFail(TCollection_AsciiString("File '") + aFileNameGltf + "' can not be created");
    return false;
  }
  if (myIsBinary)
  {
    const char* aMagic         = "glTF";
    uint32_t    aVersion       = 2;
    uint32_t    aLength        = 0;
    uint32_t    aContentLength = 0;
    uint32_t    aContentType   = 0x4E4F534A;

    aGltfContentFile->write(aMagic, 4);
    aGltfContentFile->write((const char*)&aVersion, sizeof(aVersion));
    aGltfContentFile->write((const char*)&aLength, sizeof(aLength));
    aGltfContentFile->write((const char*)&aContentLength, sizeof(aContentLength));
    aGltfContentFile->write((const char*)&aContentType, sizeof(aContentType));
  }

  // Prepare an indexed map of scene nodes (without assemblies) in correct order.
  // Note: this is also order of meshes in glTF document array.
  RWGltf_GltfSceneNodeMap aSceneNodeMap;
  for (XCAFPrs_DocumentExplorer aDocExplorer(theDocument,
                                             theRootLabels,
                                             XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
       aDocExplorer.More();
       aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL && !theLabelFilter->Contains(aDocNode.Id))
    {
      continue;
    }

    bool hasShapeData = false;
    if (!aDocNode.IsAssembly)
    {
      auto checkShapeData = [&](RWMesh_ShapeIterator& anIter) {
        for (; anIter.More(); anIter.Next())
        {
          if (!toSkipShape(anIter))
          {
            return true;
          }
        }
        return false;
      };
      {
        RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
        hasShapeData = checkShapeData(aFaceIter);
      }
      if (!hasShapeData)
      {
        RWMesh_EdgeIterator anEdgeIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
        hasShapeData = checkShapeData(anEdgeIter);
      }
      if (!hasShapeData)
      {
        RWMesh_VertexIterator aVertIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
        hasShapeData = checkShapeData(aVertIter);
      }
    }

    if (hasShapeData)
    {
      aSceneNodeMap.Add(aDocNode);
    }
    else
    {
      // glTF disallows empty shapes / primitive arrays
      const TCollection_AsciiString aNodeName =
        formatName(RWMesh_NameFormat_ProductOrInstance, aDocNode.Label, aDocNode.RefLabel);
      Message::SendWarning(TCollection_AsciiString("RWGltf_CafWriter skipped node '") + aNodeName
                           + "' without geometry data");
    }
  }

  rapidjson::OStreamWrapper aFileStream(*aGltfContentFile);
  myWriter.reset(new RWGltf_GltfOStreamWriter(aFileStream));

  myWriter->StartObject();

  writeAccessors(aSceneNodeMap);
  writeAnimations();
  writeAsset(theFileInfo);
  writeBufferViews(aBinDataBufferId);
  writeBuffers();
  writeExtensions();

  writeImages(aSceneNodeMap);
  writeMaterials(aSceneNodeMap);
  writeMeshes(aSceneNodeMap);

  aPSentryBin.Next();
  if (!aPSentryBin.More())
  {
    return false;
  }

  // root nodes indices starting from 0
  NCollection_Sequence<Standard_Integer> aSceneRootNodeInds;
  writeNodes(theDocument, theRootLabels, theLabelFilter, aSceneNodeMap, aSceneRootNodeInds);
  writeSamplers();
  writeScene(aDefSceneId);
  writeScenes(aSceneRootNodeInds);
  writeSkins();
  writeTextures(aSceneNodeMap);

  myWriter->EndObject();

  if (!myIsBinary)
  {
    aGltfContentFile->flush();
    if (!aGltfContentFile->good())
    {
      Message::SendFail(TCollection_AsciiString("File '") + aFileNameGltf + "' can not be written");
      return false;
    }
    aGltfContentFile.reset();
    return true;
  }

  int64_t aContentLen64 = (int64_t)aGltfContentFile->tellp() - 20;
  while (aContentLen64 % 4 != 0)
  {
    aGltfContentFile->write(" ", 1);
    ++aContentLen64;
  }

  const uint32_t aBinLength = (uint32_t)myBinDataLen64;
  const uint32_t aBinType   = 0x004E4942;
  aGltfContentFile->write((const char*)&aBinLength, 4);
  aGltfContentFile->write((const char*)&aBinType, 4);

  const int64_t aFullLen64 = aContentLen64 + 20 + myBinDataLen64 + 8;
  if (aFullLen64 < std::numeric_limits<uint32_t>::max())
  {
    {
      std::shared_ptr<std::istream> aBinFile =
        aFileSystem->OpenIStream(myBinFileNameFull, std::ios::in | std::ios::binary);
      if (aBinFile.get() == NULL || !aBinFile->good())
      {
        Message::SendFail(TCollection_AsciiString("File '") + myBinFileNameFull
                          + "' cannot be opened");
        return false;
      }
      char aBuffer[4096];
      for (; aBinFile->good();)
      {
        aBinFile->read(aBuffer, 4096);
        const Standard_Integer aReadLen = (Standard_Integer)aBinFile->gcount();
        if (aReadLen == 0)
        {
          break;
        }
        aGltfContentFile->write(aBuffer, aReadLen);
      }
    }
    OSD_Path aBinFilePath(myBinFileNameFull);
    OSD_File(aBinFilePath).Remove();
    if (OSD_File(aBinFilePath).Exists())
    {
      Message::SendFail(TCollection_AsciiString("Unable to remove temporary glTF content file '")
                        + myBinFileNameFull + "'");
    }
  }
  else
  {
    Message::SendFail("glTF file content is too big for binary format");
    return false;
  }

  const uint32_t aLength        = (uint32_t)aFullLen64;
  const uint32_t aContentLength = (uint32_t)aContentLen64;
  aGltfContentFile->seekp(8);
  aGltfContentFile->write((const char*)&aLength, 4);
  aGltfContentFile->write((const char*)&aContentLength, 4);

  aGltfContentFile->flush();
  if (!aGltfContentFile->good())
  {
    Message::SendFail(TCollection_AsciiString("File '") + aFileNameGltf + "' can not be written");
    return false;
  }
  aGltfContentFile.reset();
  myWriter.reset();
  return true;
#else
  (void)theDocument;
  (void)theRootLabels;
  (void)theLabelFilter;
  (void)theFileInfo;
  (void)theProgress;
  Message::SendFail("Error: glTF writer is unavailable - OCCT has been built without RapidJSON "
                    "support [HAVE_RAPIDJSON undefined]");
  return false;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeAccessors(const RWGltf_GltfSceneNodeMap&)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeAccessors()");

  myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Accessors));
  myWriter->StartArray();

  const RWGltf_GltfArrayType                   anArrTypes[4] = {RWGltf_GltfArrayType_Position,
                                                                RWGltf_GltfArrayType_Normal,
                                                                RWGltf_GltfArrayType_TCoord0,
                                                                RWGltf_GltfArrayType_Indices};
  NCollection_Map<Handle(RWGltf_GltfFaceList)> aWrittenFaces;
  NCollection_Map<int>                         aWrittenIds;
  int                                          aNbAccessors = 0;
  for (Standard_Integer aTypeIter = 0; aTypeIter < 4; ++aTypeIter)
  {
    const RWGltf_GltfArrayType anArrType = (RWGltf_GltfArrayType)anArrTypes[aTypeIter];
    aWrittenFaces.Clear(false);
    for (ShapeToGltfFaceMap::Iterator aBinDataIter(myBinDataMap); aBinDataIter.More();
         aBinDataIter.Next())
    {
      const Handle(RWGltf_GltfFaceList)& aGltfFaceList = aBinDataIter.Value();
      if (!aWrittenFaces.Add(aGltfFaceList)) // skip repeating faces
      {
        continue;
      }

      for (RWGltf_GltfFaceList::Iterator aFaceIter(*aGltfFaceList); aFaceIter.More();
           aFaceIter.Next())
      {
        const Handle(RWGltf_GltfFace)& aGltfFace = aFaceIter.Value();
        switch (anArrType)
        {
          case RWGltf_GltfArrayType_Position: {
            const int anAccessorId = aGltfFace->NodePos.Id;
            if (anAccessorId == RWGltf_GltfAccessor::INVALID_ID || !aWrittenIds.Add(anAccessorId))
            {
              break;
            }

            if (anAccessorId != aNbAccessors)
            {
              throw Standard_ProgramError("Internal error: RWGltf_CafWriter::writeAccessors()");
            }
            ++aNbAccessors;
            writePositions(*aGltfFace);
            break;
          }
          case RWGltf_GltfArrayType_Normal: {
            const int anAccessorId = aGltfFace->NodeNorm.Id;
            if (anAccessorId == RWGltf_GltfAccessor::INVALID_ID || !aWrittenIds.Add(anAccessorId))
            {
              break;
            }

            if (anAccessorId != aNbAccessors)
            {
              throw Standard_ProgramError("Internal error: RWGltf_CafWriter::writeAccessors()");
            }
            ++aNbAccessors;
            writeNormals(*aGltfFace);
            break;
          }
          case RWGltf_GltfArrayType_TCoord0: {
            const int anAccessorId = aGltfFace->NodeUV.Id;
            if (anAccessorId == RWGltf_GltfAccessor::INVALID_ID || !aWrittenIds.Add(anAccessorId))
            {
              break;
            }

            if (anAccessorId != aNbAccessors)
            {
              throw Standard_ProgramError("Internal error: RWGltf_CafWriter::writeAccessors()");
            }
            ++aNbAccessors;
            writeTextCoords(*aGltfFace);
            break;
          }
          case RWGltf_GltfArrayType_Indices: {
            const int anAccessorId = aGltfFace->Indices.Id;
            if (anAccessorId == RWGltf_GltfAccessor::INVALID_ID || !aWrittenIds.Add(anAccessorId))
            {
              break;
            }

            if (anAccessorId != aNbAccessors)
            {
              throw Standard_ProgramError("Internal error: RWGltf_CafWriter::writeAccessors()");
            }
            ++aNbAccessors;
            writeIndices(*aGltfFace);
            break;
          }
          default: {
            break;
          }
        }
      }
    }
  }

  myWriter->EndArray();
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writePositions(const RWGltf_GltfFace& theGltfFace)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writePositions()");
  if (theGltfFace.NodePos.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    return;
  }

  myWriter->StartObject();
  if (!myDracoParameters.DracoCompression || !hasTriangulation(theGltfFace))
  {
    myWriter->Key("bufferView");
    myWriter->Int(myBuffViewPos.Id);
    myWriter->Key("byteOffset");
    myWriter->Int64(theGltfFace.NodePos.ByteOffset);
  }
  myWriter->Key("componentType");
  myWriter->Int(theGltfFace.NodePos.ComponentType);
  myWriter->Key("count");
  myWriter->Int64(theGltfFace.NodePos.Count);

  if (theGltfFace.NodePos.BndBox.IsValid())
  {
    myWriter->Key("max");
    myWriter->StartArray();
    myWriter->Double(theGltfFace.NodePos.BndBox.CornerMax().x());
    myWriter->Double(theGltfFace.NodePos.BndBox.CornerMax().y());
    myWriter->Double(theGltfFace.NodePos.BndBox.CornerMax().z());
    myWriter->EndArray();

    myWriter->Key("min");
    myWriter->StartArray();
    myWriter->Double(theGltfFace.NodePos.BndBox.CornerMin().x());
    myWriter->Double(theGltfFace.NodePos.BndBox.CornerMin().y());
    myWriter->Double(theGltfFace.NodePos.BndBox.CornerMin().z());
    myWriter->EndArray();
  }
  myWriter->Key("type");
  myWriter->String("VEC3");

  myWriter->EndObject();
#else
  (void)theGltfFace;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeNormals(const RWGltf_GltfFace& theGltfFace)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeNormals()");
  if (theGltfFace.NodeNorm.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    return;
  }

  myWriter->StartObject();
  if (!myDracoParameters.DracoCompression)
  {
    myWriter->Key("bufferView");
    myWriter->Int(myBuffViewNorm.Id);
    myWriter->Key("byteOffset");
    myWriter->Int64(theGltfFace.NodeNorm.ByteOffset);
  }
  myWriter->Key("componentType");
  myWriter->Int(theGltfFace.NodeNorm.ComponentType);
  myWriter->Key("count");
  myWriter->Int64(theGltfFace.NodeNorm.Count);
  // min/max values are optional, and not very useful for normals - skip them
  /*{
    myWriter->Key ("max");
    myWriter->StartArray();
    myWriter->Double (1.0);
    myWriter->Double (1.0);
    myWriter->Double (1.0);
    myWriter->EndArray();
  }
  {
    myWriter->Key ("min");
    myWriter->StartArray();
    myWriter->Double (0.0);
    myWriter->Double (0.0);
    myWriter->Double (0.0);
    myWriter->EndArray();
  }*/
  myWriter->Key("type");
  myWriter->String("VEC3");

  myWriter->EndObject();
#else
  (void)theGltfFace;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeTextCoords(const RWGltf_GltfFace& theGltfFace)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeTextCoords()");
  if (theGltfFace.NodeUV.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    return;
  }

  myWriter->StartObject();
  if (!myDracoParameters.DracoCompression)
  {
    myWriter->Key("bufferView");
    myWriter->Int(myBuffViewTextCoord.Id);
    myWriter->Key("byteOffset");
    myWriter->Int64(theGltfFace.NodeUV.ByteOffset);
  }
  myWriter->Key("componentType");
  myWriter->Int(theGltfFace.NodeUV.ComponentType);
  myWriter->Key("count");
  myWriter->Int64(theGltfFace.NodeUV.Count);
  // min/max values are optional, and not very useful for UV coordinates - skip them
  /*{
    myWriter->Key ("max");
    myWriter->StartArray();
    myWriter->Double (1.0);
    myWriter->Double (1.0);
    myWriter->Double (1.0);
    myWriter->EndArray();
  }
  {
    myWriter->Key ("min");
    myWriter->StartArray();
    myWriter->Double (0.0);
    myWriter->Double (0.0);
    myWriter->Double (0.0);
    myWriter->EndArray();
  }*/
  myWriter->Key("type");
  myWriter->String("VEC2");

  myWriter->EndObject();
#else
  (void)theGltfFace;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeIndices(const RWGltf_GltfFace& theGltfFace)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeIndices()");
  if (theGltfFace.Indices.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    return;
  }

  myWriter->StartObject();
  if (!myDracoParameters.DracoCompression || !hasTriangulation(theGltfFace))
  {
    myWriter->Key("bufferView");
    myWriter->Int(myBuffViewInd.Id);
    myWriter->Key("byteOffset");
    myWriter->Int64(theGltfFace.Indices.ByteOffset);
  }
  myWriter->Key("componentType");
  myWriter->Int(theGltfFace.Indices.ComponentType);
  myWriter->Key("count");
  myWriter->Int64(theGltfFace.Indices.Count);

  myWriter->Key("type");
  myWriter->String("SCALAR");

  myWriter->EndObject();
#else
  (void)theGltfFace;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeAnimations()
{
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeAnimations()");

  // This section should be skipped if it doesn't contain any information but not be empty
  // myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Animations));
  // myWriter->StartArray();
  // myWriter->EndArray();
}

//=================================================================================================

void RWGltf_CafWriter::writeAsset(const TColStd_IndexedDataMapOfStringString& theFileInfo)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeAsset()");

  myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Asset));
  myWriter->StartObject();
  myWriter->Key("generator");
  myWriter->String("Open CASCADE Technology " OCC_VERSION_STRING " [dev.opencascade.org]");
  myWriter->Key("version");
  myWriter->String("2.0"); // glTF format version

  bool anIsStarted = false;
  for (TColStd_IndexedDataMapOfStringString::Iterator aKeyValueIter(theFileInfo);
       aKeyValueIter.More();
       aKeyValueIter.Next())
  {
    if (!anIsStarted)
    {
      myWriter->Key("extras");
      myWriter->StartObject();
      anIsStarted = true;
    }
    myWriter->Key(aKeyValueIter.Key().ToCString());
    myWriter->String(aKeyValueIter.Value().ToCString());
  }
  if (anIsStarted)
  {
    myWriter->EndObject();
  }

  myWriter->EndObject();
#else
  (void)theFileInfo;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeBufferViews(const Standard_Integer theBinDataBufferId)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeBufferViews()");

  int aBuffViewId = 0;
  myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_BufferViews));
  myWriter->StartArray();
  if (myBuffViewPos.Id != RWGltf_GltfAccessor::INVALID_ID)
  {
    aBuffViewId++;
    myWriter->StartObject();
    myWriter->Key("buffer");
    myWriter->Int(theBinDataBufferId);
    myWriter->Key("byteLength");
    myWriter->Int64(myBuffViewPos.ByteLength);
    myWriter->Key("byteOffset");
    myWriter->Int64(myBuffViewPos.ByteOffset);
    myWriter->Key("byteStride");
    myWriter->Int64(myBuffViewPos.ByteStride);
    myWriter->Key("target");
    myWriter->Int(myBuffViewPos.Target);
    myWriter->EndObject();
  }
  if (myBuffViewNorm.Id != RWGltf_GltfAccessor::INVALID_ID)
  {
    aBuffViewId++;
    myWriter->StartObject();
    myWriter->Key("buffer");
    myWriter->Int(theBinDataBufferId);
    myWriter->Key("byteLength");
    myWriter->Int64(myBuffViewNorm.ByteLength);
    myWriter->Key("byteOffset");
    myWriter->Int64(myBuffViewNorm.ByteOffset);
    myWriter->Key("byteStride");
    myWriter->Int64(myBuffViewNorm.ByteStride);
    myWriter->Key("target");
    myWriter->Int(myBuffViewNorm.Target);
    myWriter->EndObject();
  }
  if (myBuffViewTextCoord.Id != RWGltf_GltfAccessor::INVALID_ID)
  {
    aBuffViewId++;
    myWriter->StartObject();
    myWriter->Key("buffer");
    myWriter->Int(theBinDataBufferId);
    myWriter->Key("byteLength");
    myWriter->Int64(myBuffViewTextCoord.ByteLength);
    myWriter->Key("byteOffset");
    myWriter->Int64(myBuffViewTextCoord.ByteOffset);
    myWriter->Key("byteStride");
    myWriter->Int64(myBuffViewTextCoord.ByteStride);
    myWriter->Key("target");
    myWriter->Int(myBuffViewTextCoord.Target);
    myWriter->EndObject();
  }
  if (myBuffViewInd.Id != RWGltf_GltfAccessor::INVALID_ID)
  {
    aBuffViewId++;
    myWriter->StartObject();
    myWriter->Key("buffer");
    myWriter->Int(theBinDataBufferId);
    myWriter->Key("byteLength");
    myWriter->Int64(myBuffViewInd.ByteLength);
    myWriter->Key("byteOffset");
    myWriter->Int64(myBuffViewInd.ByteOffset);
    myWriter->Key("target");
    myWriter->Int(myBuffViewInd.Target);
    myWriter->EndObject();
  }
  if (myDracoParameters.DracoCompression)
  {
    for (size_t aBufInd = 0; aBufInd != myBuffViewsDraco.size(); ++aBufInd)
    {
      if (myBuffViewsDraco[aBufInd].Id != RWGltf_GltfAccessor::INVALID_ID)
      {
        aBuffViewId++;
        myWriter->StartObject();
        myWriter->Key("buffer");
        myWriter->Int(theBinDataBufferId);
        myWriter->Key("byteLength");
        myWriter->Int64(myBuffViewsDraco[aBufInd].ByteLength);
        myWriter->Key("byteOffset");
        myWriter->Int64(myBuffViewsDraco[aBufInd].ByteOffset);
        myWriter->EndObject();
      }
    }
  }

  myMaterialMap->FlushGlbBufferViews(myWriter.get(), theBinDataBufferId, aBuffViewId);

  myWriter->EndArray();
#else
  (void)theBinDataBufferId;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeBuffers()
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeBuffers()");

  myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Buffers));
  myWriter->StartArray();
  {
    myWriter->StartObject();
    {
      myWriter->Key("byteLength");
      myWriter->Int64(myBinDataLen64);
      if (!myIsBinary)
      {
        myWriter->Key("uri");
        myWriter->String(myBinFileNameShort.ToCString());
      }
    }
    myWriter->EndObject();
  }
  myWriter->EndArray();
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeExtensions()
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeExtensions()");

  if (myDracoParameters.DracoCompression)
  {
    myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_ExtensionsUsed));

    myWriter->StartArray();
    {
      myWriter->Key("KHR_draco_mesh_compression");
    }
    myWriter->EndArray();

    myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_ExtensionsRequired));

    myWriter->StartArray();
    {
      myWriter->Key("KHR_draco_mesh_compression");
    }
    myWriter->EndArray();
  }
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeImages(const RWGltf_GltfSceneNodeMap& theSceneNodeMap)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeImages()");

  // empty RWGltf_GltfRootElement_Images section should NOT be written to avoid validator errors
  if (myIsBinary && myToEmbedTexturesInGlb)
  {
    myMaterialMap->FlushGlbImages(myWriter.get());
  }
  else
  {
    bool anIsStarted = false;
    for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter(theSceneNodeMap); aSceneNodeIter.More();
         aSceneNodeIter.Next())
    {
      const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
      for (RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel,
                                         TopLoc_Location(),
                                         true,
                                         aDocNode.Style);
           aFaceIter.More();
           aFaceIter.Next())
      {
        myMaterialMap->AddImages(myWriter.get(), aFaceIter.Style(), anIsStarted);
      }
    }
    if (anIsStarted)
    {
      myWriter->EndArray();
    }
  }
#else
  (void)theSceneNodeMap;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeMaterial(RWMesh_ShapeIterator& theShapeIter,
                                     Standard_Boolean&     theIsStarted)
{
  for (; theShapeIter.More(); theShapeIter.Next())
  {
    myMaterialMap->AddMaterial(myWriter.get(), theShapeIter.Style(), theIsStarted);
  }
}

//=================================================================================================

void RWGltf_CafWriter::writeMaterials(const RWGltf_GltfSceneNodeMap& theSceneNodeMap)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeMaterials()");

  // empty RWGltf_GltfRootElement_Materials section should NOT be written to avoid validator errors
  bool anIsStarted = false;
  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter(theSceneNodeMap); aSceneNodeIter.More();
       aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
    {
      RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
      writeMaterial(aFaceIter, anIsStarted);
    }
    {
      RWMesh_EdgeIterator anEdgeIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
      writeMaterial(anEdgeIter, anIsStarted);
    }
    {
      RWMesh_VertexIterator VertexIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
      writeMaterial(VertexIter, anIsStarted);
    }
  }
  if (anIsStarted)
  {
    myWriter->EndArray();
  }
#else
  (void)theSceneNodeMap;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writePrimArray(const RWGltf_GltfFace&         theGltfFace,
                                      const TCollection_AsciiString& theName,
                                      const int                      theDracoBufInd,
                                      bool&                          theToStartPrims)
{
#ifdef HAVE_RAPIDJSON
  if (theToStartPrims)
  {
    theToStartPrims = false;
    myWriter->StartObject();
    if (!theName.IsEmpty())
    {
      myWriter->Key("name");
      myWriter->String(theName.ToCString());
    }
    myWriter->Key("primitives");
    myWriter->StartArray();
  }

  const TCollection_AsciiString aMatId = myMaterialMap->FindMaterial(theGltfFace.Style);
  myWriter->StartObject();
  {
    myWriter->Key("attributes");
    myWriter->StartObject();
    {
      if (theGltfFace.NodeNorm.Id != RWGltf_GltfAccessor::INVALID_ID)
      {
        myWriter->Key("NORMAL");
        myWriter->Int(theGltfFace.NodeNorm.Id);
      }
      myWriter->Key("POSITION");
      myWriter->Int(theGltfFace.NodePos.Id);
      if (theGltfFace.NodeUV.Id != RWGltf_GltfAccessor::INVALID_ID)
      {
        myWriter->Key("TEXCOORD_0");
        myWriter->Int(theGltfFace.NodeUV.Id);
      }
    }
    myWriter->EndObject();

    myWriter->Key("indices");
    myWriter->Int(theGltfFace.Indices.Id);
    if (!aMatId.IsEmpty())
    {
      myWriter->Key("material");
      myWriter->Int(aMatId.IntegerValue());
    }

    myWriter->Key("mode");

    TopAbs_ShapeEnum shapeType = getShapeType(theGltfFace.Shape);

    switch (shapeType)
    {
      case TopAbs_EDGE:
        myWriter->Int(RWGltf_GltfPrimitiveMode_Lines);
        break;
      case TopAbs_VERTEX:
        myWriter->Int(RWGltf_GltfPrimitiveMode_Points);
        break;
      default:
        myWriter->Int(RWGltf_GltfPrimitiveMode_Triangles);
        break;
    }

    if (myDracoParameters.DracoCompression && hasTriangulation(theGltfFace))
    {
      myWriter->Key("extensions");
      myWriter->StartObject();
      {
        myWriter->Key("KHR_draco_mesh_compression");
        myWriter->StartObject();
        myWriter->Key("bufferView");
        myWriter->Int(myBuffViewsDraco[theDracoBufInd].Id);
        myWriter->Key("attributes");
        myWriter->StartObject();
        {
          int anAttrInd = 0;
          if (theGltfFace.NodePos.Id != RWGltf_GltfAccessor::INVALID_ID)
          {
            myWriter->Key("POSITION");
            myWriter->Int(anAttrInd++);
          }
          if (theGltfFace.NodeNorm.Id != RWGltf_GltfAccessor::INVALID_ID)
          {
            myWriter->Key("NORMAL");
            myWriter->Int(anAttrInd++);
          }
          if (theGltfFace.NodeUV.Id != RWGltf_GltfAccessor::INVALID_ID)
          {
            myWriter->Key("TEXCOORD_0");
            myWriter->Int(anAttrInd++);
          }
        }
        myWriter->EndObject();
        myWriter->EndObject();
      }
      myWriter->EndObject();
    }
  }
  myWriter->EndObject();
#else
  (void)theGltfFace;
  (void)theName;
  (void)theToStartPrims;
  (void)theDracoBufInd;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeShapes(RWMesh_ShapeIterator&                         theShapeIter,
                                   Standard_Integer&                             theDracoBufInd,
                                   Standard_Boolean&                             theToStartPrims,
                                   const TCollection_AsciiString&                theNodeName,
                                   NCollection_Map<Handle(RWGltf_GltfFaceList)>& theWrittenShapes,
                                   NCollection_IndexedDataMap<int, int>&         theDracoBufIndMap)
{
  for (; theShapeIter.More(); theShapeIter.Next())
  {
    if (toSkipShape(theShapeIter))
    {
      continue;
    }

    RWGltf_StyledShape                 aStyledShape(theShapeIter.Shape(), theShapeIter.Style());
    const Handle(RWGltf_GltfFaceList)& aGltfShapeList = myBinDataMap.FindFromKey(aStyledShape);
    if (!theWrittenShapes.Add(aGltfShapeList))
    {
      continue;
    }

    const Handle(RWGltf_GltfFace)& aGltfShape          = aGltfShapeList->First();
    int                            aCurrentDracoBufInd = 0;

    if (myDracoParameters.DracoCompression && hasTriangulation(*aGltfShape))
    {
      // Check if we've seen this NodePos.Id before
      if (!theDracoBufIndMap.FindFromKey(aGltfShape->NodePos.Id, aCurrentDracoBufInd))
      {
        // New Draco buffer entry needed
        aCurrentDracoBufInd = theDracoBufInd;
        theDracoBufIndMap.Add(aGltfShape->NodePos.Id, aCurrentDracoBufInd);
        ++theDracoBufInd;
      }
    }

    writePrimArray(*aGltfShape, theNodeName, aCurrentDracoBufInd, theToStartPrims);
  }
}

//=================================================================================================

void RWGltf_CafWriter::writeMeshes(const RWGltf_GltfSceneNodeMap& theSceneNodeMap)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeMeshes()");

  myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Meshes));
  myWriter->StartArray();

  int                                          aDracoBufInd = 0;
  NCollection_IndexedDataMap<int, int>         aDracoBufMap;
  NCollection_Map<Handle(RWGltf_GltfFaceList)> aWrittenShapes;
  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter(theSceneNodeMap); aSceneNodeIter.More();
       aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode&   aDocNode = aSceneNodeIter.Value();
    const TCollection_AsciiString aNodeName =
      formatName(myMeshNameFormat, aDocNode.Label, aDocNode.RefLabel);

    bool toStartPrims = true;
    aWrittenShapes.Clear(false);
    if (myToMergeFaces)
    {
      TopoDS_Shape aShape;
      if (!XCAFDoc_ShapeTool::GetShape(aDocNode.RefLabel, aShape) || aShape.IsNull())
      {
        continue;
      }

      Handle(RWGltf_GltfFaceList) aGltfFaceList;
      aShape.Location(TopLoc_Location());
      RWGltf_StyledShape aStyledShape(aShape, aDocNode.Style);
      myBinDataMap.FindFromKey(aStyledShape, aGltfFaceList);
      if (!aWrittenShapes.Add(aGltfFaceList))
      {
        continue;
      }

      for (RWGltf_GltfFaceList::Iterator aFaceGroupIter(*aGltfFaceList); aFaceGroupIter.More();
           aFaceGroupIter.Next())
      {
        const Handle(RWGltf_GltfFace)& aGltfFace           = aFaceGroupIter.Value();
        int                            aCurrentDracoBufInd = 0;

        if (myDracoParameters.DracoCompression && hasTriangulation(*aGltfFace))
        {
          // Check if we've seen this NodePos.Id before
          if (!aDracoBufMap.FindFromKey(aGltfFace->NodePos.Id, aCurrentDracoBufInd))
          {
            // New Draco buffer entry needed
            aCurrentDracoBufInd = aDracoBufInd;
            aDracoBufMap.Add(aGltfFace->NodePos.Id, aCurrentDracoBufInd);
            ++aDracoBufInd;
          }
        }
        writePrimArray(*aGltfFace, aNodeName, aCurrentDracoBufInd, toStartPrims);
      }
    }
    else
    {
      {
        RWMesh_FaceIterator anIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
        writeShapes(anIter, aDracoBufInd, toStartPrims, aNodeName, aWrittenShapes, aDracoBufMap);
      }
      {
        RWMesh_EdgeIterator anIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
        writeShapes(anIter, aDracoBufInd, toStartPrims, aNodeName, aWrittenShapes, aDracoBufMap);
      }
      {
        RWMesh_VertexIterator anIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
        writeShapes(anIter, aDracoBufInd, toStartPrims, aNodeName, aWrittenShapes, aDracoBufMap);
      }
    }

    if (!toStartPrims)
    {
      myWriter->EndArray();
      myWriter->EndObject();
    }
  }
  myWriter->EndArray();
#else
  (void)theSceneNodeMap;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeNodes(const Handle(TDocStd_Document)&         theDocument,
                                  const TDF_LabelSequence&                theRootLabels,
                                  const TColStd_MapOfAsciiString*         theLabelFilter,
                                  const RWGltf_GltfSceneNodeMap&          theSceneNodeMap,
                                  NCollection_Sequence<Standard_Integer>& theSceneRootNodeInds)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeNodes()");

  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDocument->Main());

  // Prepare full indexed map of scene nodes in correct order.
  RWGltf_GltfSceneNodeMap aSceneNodeMapWithChildren; // indexes starting from 1
  for (XCAFPrs_DocumentExplorer aDocExplorer(theDocument,
                                             theRootLabels,
                                             XCAFPrs_DocumentExplorerFlags_None);
       aDocExplorer.More();
       aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL && !theLabelFilter->Contains(aDocNode.Id))
    {
      continue;
    }

    // keep empty nodes
    // RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), false);
    // if (!aFaceIter.More()) { continue; }

    Standard_Integer aNodeIndex = aSceneNodeMapWithChildren.Add(aDocNode);
    if (aDocExplorer.CurrentDepth() == 0)
    {
      // save root node index (starting from 0 not 1)
      theSceneRootNodeInds.Append(aNodeIndex - 1);
    }
  }

  // Write scene nodes using prepared map for correct order of array members
  myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Nodes));
  myWriter->StartArray();

  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter(aSceneNodeMapWithChildren);
       aSceneNodeIter.More();
       aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();

    myWriter->StartObject();
    {
      if (aDocNode.IsAssembly)
      {
        myWriter->Key("children");
        myWriter->StartArray();
        {
          for (TDF_ChildIterator aChildIter(aDocNode.RefLabel); aChildIter.More();
               aChildIter.Next())
          {
            const TDF_Label& aChildLabel = aChildIter.Value();
            if (aChildLabel.IsNull())
            {
              continue;
            }

            const TCollection_AsciiString aChildId =
              XCAFPrs_DocumentExplorer::DefineChildId(aChildLabel, aDocNode.Id);
            Standard_Integer aChildIdx = aSceneNodeMapWithChildren.FindIndex(aChildId);
            if (aChildIdx > 0)
            {
              myWriter->Int(aChildIdx - 1);
            }
          }
        }
        myWriter->EndArray();
      }
    }
    if (!aDocNode.LocalTrsf.IsIdentity())
    {
      gp_Trsf aTrsf = aDocNode.LocalTrsf.Transformation();
      if (aTrsf.Form() != gp_Identity)
      {
        myCSTrsf.TransformTransformation(aTrsf);
        const gp_Quaternion aQuaternion = aTrsf.GetRotation();
        const bool          hasRotation = std::abs(aQuaternion.X()) > gp::Resolution()
                                 || std::abs(aQuaternion.Y()) > gp::Resolution()
                                 || std::abs(aQuaternion.Z()) > gp::Resolution()
                                 || std::abs(aQuaternion.W() - 1.0) > gp::Resolution();
        const Standard_Real aScaleFactor   = aTrsf.ScaleFactor();
        const bool          hasScale       = std::abs(aScaleFactor - 1.0) > Precision::Confusion();
        const gp_XYZ&       aTranslPart    = aTrsf.TranslationPart();
        const bool          hasTranslation = aTranslPart.SquareModulus() > gp::Resolution();

        RWGltf_WriterTrsfFormat aTrsfFormat = myTrsfFormat;
        if (myTrsfFormat == RWGltf_WriterTrsfFormat_Compact)
        {
          aTrsfFormat = hasRotation && hasScale && hasTranslation ? RWGltf_WriterTrsfFormat_Mat4
                                                                  : RWGltf_WriterTrsfFormat_TRS;
        }

        if (aTrsfFormat == RWGltf_WriterTrsfFormat_Mat4)
        {
          // write full matrix
          Graphic3d_Mat4 aMat4;
          aTrsf.GetMat4(aMat4);
          if (!aMat4.IsIdentity())
          {
            myWriter->Key("matrix");
            myWriter->StartArray();
            for (Standard_Integer aColIter = 0; aColIter < 4; ++aColIter)
            {
              for (Standard_Integer aRowIter = 0; aRowIter < 4; ++aRowIter)
              {
                myWriter->Double(aMat4.GetValue(aRowIter, aColIter));
              }
            }
            myWriter->EndArray();
          }
        }
        else // if (aTrsfFormat == RWGltf_WriterTrsfFormat_TRS)
        {
          if (hasRotation)
          {
            myWriter->Key("rotation");
            myWriter->StartArray();
            myWriter->Double(aQuaternion.X());
            myWriter->Double(aQuaternion.Y());
            myWriter->Double(aQuaternion.Z());
            myWriter->Double(aQuaternion.W());
            myWriter->EndArray();
          }
          if (hasScale)
          {
            myWriter->Key("scale");
            myWriter->StartArray();
            myWriter->Double(aScaleFactor);
            myWriter->Double(aScaleFactor);
            myWriter->Double(aScaleFactor);
            myWriter->EndArray();
          }
          if (hasTranslation)
          {
            myWriter->Key("translation");
            myWriter->StartArray();
            myWriter->Double(aTranslPart.X());
            myWriter->Double(aTranslPart.Y());
            myWriter->Double(aTranslPart.Z());
            myWriter->EndArray();
          }
        }
      }
    }
    if (!aDocNode.IsAssembly)
    {
      // Mesh order of current node is equal to order of this node in scene nodes map
      Standard_Integer aMeshIdx = theSceneNodeMap.FindIndex(aDocNode.Id);
      if (aMeshIdx > 0)
      {
        myWriter->Key("mesh");
        myWriter->Int(aMeshIdx - 1);
      }
    }
    {
      const TCollection_AsciiString aNodeName =
        formatName(myNodeNameFormat, aDocNode.Label, aDocNode.RefLabel);
      if (!aNodeName.IsEmpty())
      {
        myWriter->Key("name");
        myWriter->String(aNodeName.ToCString());
      }
    }
    {
      Handle(TDataStd_NamedData) aNamedData    = aShapeTool->GetNamedProperties(aDocNode.Label);
      Handle(TDataStd_NamedData) aRefNamedData = aShapeTool->GetNamedProperties(aDocNode.RefLabel);
      if (!aNamedData.IsNull() || !aRefNamedData.IsNull())
      {
        myWriter->Key("extras");
        myWriter->StartObject();
        writeExtrasAttributes(aNamedData);
        writeExtrasAttributes(aRefNamedData);
        myWriter->EndObject();
      }
    }
    myWriter->EndObject();
  }
  myWriter->EndArray();
#else
  (void)theDocument;
  (void)theRootLabels;
  (void)theLabelFilter;
  (void)theSceneNodeMap;
  (void)theSceneRootNodeInds;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeExtrasAttributes(const Handle(TDataStd_NamedData)& theNamedData)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeExtrasAttributes()");

  if (theNamedData.IsNull())
    return;
  theNamedData->LoadDeferredData();
  if (theNamedData->HasIntegers())
  {
    const TColStd_DataMapOfStringInteger& anIntProperties = theNamedData->GetIntegersContainer();
    for (TColStd_DataMapIteratorOfDataMapOfStringInteger anIter(anIntProperties); anIter.More();
         anIter.Next())
    {
      TCollection_AsciiString aKey(anIter.Key());
      myWriter->Key(aKey.ToCString());
      myWriter->Int(anIter.Value());
    }
  }
  if (theNamedData->HasReals())
  {
    const TDataStd_DataMapOfStringReal& aRealProperties = theNamedData->GetRealsContainer();
    for (TDataStd_DataMapIteratorOfDataMapOfStringReal anIter(aRealProperties); anIter.More();
         anIter.Next())
    {
      TCollection_AsciiString aKey(anIter.Key());
      myWriter->Key(aKey.ToCString());
      myWriter->Double(anIter.Value());
    }
  }
  if (theNamedData->HasStrings())
  {
    const TDataStd_DataMapOfStringString& aStringProperties = theNamedData->GetStringsContainer();
    for (TDataStd_DataMapIteratorOfDataMapOfStringString anIter(aStringProperties); anIter.More();
         anIter.Next())
    {
      TCollection_AsciiString aKey(anIter.Key());
      TCollection_AsciiString aValue(anIter.Value());
      myWriter->Key(aKey.ToCString());
      myWriter->String(aValue.ToCString());
    }
  }
  if (theNamedData->HasBytes())
  {
    const TDataStd_DataMapOfStringByte& aByteProperties = theNamedData->GetBytesContainer();
    for (TDataStd_DataMapOfStringByte::Iterator anIter(aByteProperties); anIter.More();
         anIter.Next())
    {
      TCollection_AsciiString aKey(anIter.Key());
      myWriter->Key(aKey.ToCString());
      myWriter->Int(anIter.Value());
    }
  }
  if (theNamedData->HasArraysOfIntegers())
  {
    const TDataStd_DataMapOfStringHArray1OfInteger& anArrayIntegerProperties =
      theNamedData->GetArraysOfIntegersContainer();
    for (TDataStd_DataMapOfStringHArray1OfInteger::Iterator anIter(anArrayIntegerProperties);
         anIter.More();
         anIter.Next())
    {
      TCollection_AsciiString aKey(anIter.Key());
      myWriter->Key(aKey.ToCString());
      myWriter->StartArray();
      for (TColStd_HArray1OfInteger::Iterator anSubIter(anIter.Value()->Array1()); anSubIter.More();
           anSubIter.Next())
      {
        myWriter->Int(anSubIter.Value());
      }
      myWriter->EndArray();
    }
  }
  if (theNamedData->HasArraysOfReals())
  {
    const TDataStd_DataMapOfStringHArray1OfReal& anArrayRealsProperties =
      theNamedData->GetArraysOfRealsContainer();
    for (TDataStd_DataMapOfStringHArray1OfReal::Iterator anIter(anArrayRealsProperties);
         anIter.More();
         anIter.Next())
    {
      TCollection_AsciiString aKey(anIter.Key());
      myWriter->Key(aKey.ToCString());
      myWriter->StartArray();
      for (TColStd_HArray1OfReal::Iterator anSubIter(anIter.Value()->Array1()); anSubIter.More();
           anSubIter.Next())
      {
        myWriter->Double(anSubIter.Value());
      }
      myWriter->EndArray();
    }
  }
#else
  (void)theNamedData;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeSamplers()
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeSamplers()");
  if (myMaterialMap->NbImages() == 0)
  {
    return;
  }

  myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Samplers));
  myWriter->StartArray();
  {
    myWriter->StartObject();
    {
      // myWriter->Key ("magFilter");
      // myWriter->Int (9729);
      // myWriter->Key ("minFilter");
      // myWriter->Int (9729);
    }
    myWriter->EndObject();
  }
  myWriter->EndArray();
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeScene(const Standard_Integer theDefSceneId)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeScene()");

  myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Scene));
  myWriter->Int(theDefSceneId);
#else
  (void)theDefSceneId;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeScenes(
  const NCollection_Sequence<Standard_Integer>& theSceneRootNodeInds)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeScenes()");

  myWriter->Key(RWGltf_GltfRootElementName(RWGltf_GltfRootElement_Scenes));
  myWriter->StartArray();
  {
    myWriter->StartObject();
    myWriter->Key("nodes");
    myWriter->StartArray();
    for (NCollection_Sequence<Standard_Integer>::Iterator aRootIter(theSceneRootNodeInds);
         aRootIter.More();
         aRootIter.Next())
    {
      myWriter->Int(aRootIter.Value());
    }
    myWriter->EndArray();
    myWriter->EndObject();
  }
  myWriter->EndArray();
#else
  (void)theSceneRootNodeInds;
#endif
}

//=================================================================================================

void RWGltf_CafWriter::writeSkins()
{
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeSkins()");

  // This section should be skipped if it doesn't contain any information but not be empty
  /*myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Skins));
  myWriter->StartArray();
  myWriter->EndArray();*/
}

//=================================================================================================

void RWGltf_CafWriter::writeTextures(const RWGltf_GltfSceneNodeMap& theSceneNodeMap)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if(myWriter.get() == NULL,
                                 "Internal error: RWGltf_CafWriter::writeTextures()");

  // empty RWGltf_GltfRootElement_Textures section should not be written to avoid validator errors
  bool anIsStarted = false;
  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter(theSceneNodeMap); aSceneNodeIter.More();
       aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
    for (RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style);
         aFaceIter.More();
         aFaceIter.Next())
    {
      myMaterialMap->AddTextures(myWriter.get(), aFaceIter.Style(), anIsStarted);
    }
  }
  if (anIsStarted)
  {
    myWriter->EndArray();
  }
#else
  (void)theSceneNodeMap;
#endif
}
