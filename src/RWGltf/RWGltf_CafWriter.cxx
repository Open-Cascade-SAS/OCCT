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

#include <gp_Quaternion.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_OpenFile.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <Poly_Triangulation.hxx>
#include <RWGltf_GltfAccessorLayout.hxx>
#include <RWGltf_GltfMaterialMap.hxx>
#include <RWGltf_GltfPrimitiveMode.hxx>
#include <RWGltf_GltfRootElement.hxx>
#include <RWGltf_GltfSceneNodeMap.hxx>
#include <RWMesh_FaceIterator.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs_DocumentExplorer.hxx>

#ifdef HAVE_RAPIDJSON
  #include <RWGltf_GltfOStreamWriter.hxx>
#endif

IMPLEMENT_STANDARD_RTTIEXT(RWGltf_CafWriter, Standard_Transient)

namespace
{
  //! Write three float values.
  static void writeVec3 (std::ostream& theStream,
                         const gp_XYZ& theVec3)
  {
    Graphic3d_Vec3 aVec3 (float(theVec3.X()), float(theVec3.Y()), float(theVec3.Z()));
    theStream.write ((const char* )aVec3.GetData(), sizeof(aVec3));
  }

  //! Write three float values.
  static void writeVec3 (std::ostream& theStream,
                         const Graphic3d_Vec3& theVec3)
  {
    theStream.write ((const char* )theVec3.GetData(), sizeof(theVec3));
  }

  //! Write two float values.
  static void writeVec2 (std::ostream& theStream,
                         const gp_XY&  theVec2)
  {
    Graphic3d_Vec2 aVec2 (float(theVec2.X()), float(theVec2.Y()));
    theStream.write ((const char* )aVec2.GetData(), sizeof(aVec2));
  }

  //! Write triangle indices.
  static void writeTriangle32 (std::ostream& theStream,
                               const Graphic3d_Vec3i& theTri)
  {
    theStream.write ((const char* )theTri.GetData(), sizeof(theTri));
  }

  //! Write triangle indices.
  static void writeTriangle16 (std::ostream& theStream,
                               const NCollection_Vec3<uint16_t>& theTri)
  {
    theStream.write ((const char* )theTri.GetData(), sizeof(theTri));
  }

#ifdef HAVE_RAPIDJSON
  //! Read name attribute.
  static TCollection_AsciiString readNameAttribute (const TDF_Label& theRefLabel)
  {
    Handle(TDataStd_Name) aNodeName;
    if (!theRefLabel.FindAttribute (TDataStd_Name::GetID(), aNodeName))
    {
      return TCollection_AsciiString();
    }
    return TCollection_AsciiString (aNodeName->Get());
  }
#endif
}

//================================================================
// Function : Constructor
// Purpose  :
//================================================================
RWGltf_CafWriter::RWGltf_CafWriter (const TCollection_AsciiString& theFile,
                                    Standard_Boolean theIsBinary)
: myFile          (theFile),
  myTrsfFormat    (RWGltf_WriterTrsfFormat_Compact),
  myIsBinary      (theIsBinary),
  myBinDataLen64  (0)
{
  myCSTrsf.SetOutputLengthUnit (1.0); // meters
  myCSTrsf.SetOutputCoordinateSystem (RWMesh_CoordinateSystem_glTF);

  TCollection_AsciiString aFolder, aFileName, aShortFileNameBase, aFileExt;
  OSD_Path::FolderAndFileFromPath (theFile, aFolder, aFileName);
  OSD_Path::FileNameAndExtension (aFileName, aShortFileNameBase, aFileExt);

  myBinFileNameShort = aShortFileNameBase + ".bin" + (myIsBinary ? ".tmp" : "");
  myBinFileNameFull = !aFolder.IsEmpty() ? aFolder + myBinFileNameShort : myBinFileNameShort;
}

//================================================================
// Function : Destructor
// Purpose  :
//================================================================
RWGltf_CafWriter::~RWGltf_CafWriter()
{
  myWriter.reset();
}

//================================================================
// Function : toSkipFaceMesh
// Purpose  :
//================================================================
Standard_Boolean RWGltf_CafWriter::toSkipFaceMesh (const RWMesh_FaceIterator& theFaceIter)
{
  return theFaceIter.IsEmptyMesh();
}

// =======================================================================
// function : saveNodes
// purpose  :
// =======================================================================
void RWGltf_CafWriter::saveNodes (RWGltf_GltfFace& theGltfFace,
                                  std::ostream& theBinFile,
                                  const RWMesh_FaceIterator& theFaceIter,
                                  Standard_Integer& theAccessorNb) const
{
  theGltfFace.NodePos.Id            = theAccessorNb++;
  theGltfFace.NodePos.Count         = theFaceIter.NbNodes();
  theGltfFace.NodePos.ByteOffset    = (int64_t )theBinFile.tellp() - myBuffViewPos.ByteOffset;
  theGltfFace.NodePos.Type          = RWGltf_GltfAccessorLayout_Vec3;
  theGltfFace.NodePos.ComponentType = RWGltf_GltfAccessorCompType_Float32;

  const Standard_Integer aNodeUpper = theFaceIter.NodeUpper();
  for (Standard_Integer aNodeIter = theFaceIter.NodeLower(); aNodeIter <= aNodeUpper; ++aNodeIter)
  {
    gp_XYZ aNode = theFaceIter.NodeTransformed (aNodeIter).XYZ();
    myCSTrsf.TransformPosition (aNode);
    theGltfFace.NodePos.BndBox.Add (Graphic3d_Vec3d(aNode.X(), aNode.Y(), aNode.Z()));
    writeVec3 (theBinFile, aNode);
  }
}

// =======================================================================
// function : saveNormals
// purpose  :
// =======================================================================
void RWGltf_CafWriter::saveNormals (RWGltf_GltfFace& theGltfFace,
                                    std::ostream& theBinFile,
                                    RWMesh_FaceIterator& theFaceIter,
                                    Standard_Integer& theAccessorNb) const
{
  if (!theFaceIter.HasNormals())
  {
    return;
  }

  theGltfFace.NodeNorm.Id            = theAccessorNb++;
  theGltfFace.NodeNorm.Count         = theFaceIter.NbNodes();
  theGltfFace.NodeNorm.ByteOffset    = (int64_t )theBinFile.tellp() - myBuffViewNorm.ByteOffset;
  theGltfFace.NodeNorm.Type          = RWGltf_GltfAccessorLayout_Vec3;
  theGltfFace.NodeNorm.ComponentType = RWGltf_GltfAccessorCompType_Float32;

  const Standard_Integer aNodeUpper = theFaceIter.NodeUpper();
  for (Standard_Integer aNodeIter = theFaceIter.NodeLower(); aNodeIter <= aNodeUpper; ++aNodeIter)
  {
    const gp_Dir aNormal = theFaceIter.NormalTransformed (aNodeIter);
    Graphic3d_Vec3 aVecNormal ((float )aNormal.X(), (float )aNormal.Y(), (float )aNormal.Z());
    myCSTrsf.TransformNormal (aVecNormal);
    writeVec3 (theBinFile, aVecNormal);
  }
}

// =======================================================================
// function : saveTextCoords
// purpose  :
// =======================================================================
void RWGltf_CafWriter::saveTextCoords (RWGltf_GltfFace& theGltfFace,
                                       std::ostream& theBinFile,
                                       const RWMesh_FaceIterator& theFaceIter,
                                       Standard_Integer& theAccessorNb) const
{
  if (!theFaceIter.HasTexCoords())
  {
    return;
  }
  if (!myIsForcedUVExport)
  {
    if (theFaceIter.FaceStyle().Material().IsNull())
    {
      return;
    }

    if (RWGltf_GltfMaterialMap::baseColorTexture (theFaceIter.FaceStyle().Material()).IsNull()
     && theFaceIter.FaceStyle().Material()->PbrMaterial().MetallicRoughnessTexture.IsNull()
     && theFaceIter.FaceStyle().Material()->PbrMaterial().EmissiveTexture.IsNull()
     && theFaceIter.FaceStyle().Material()->PbrMaterial().OcclusionTexture.IsNull()
     && theFaceIter.FaceStyle().Material()->PbrMaterial().NormalTexture.IsNull())
    {
      return;
    }
  }

  theGltfFace.NodeUV.Id            = theAccessorNb++;
  theGltfFace.NodeUV.Count         = theFaceIter.NbNodes();
  theGltfFace.NodeUV.ByteOffset    = (int64_t )theBinFile.tellp() - myBuffViewTextCoord.ByteOffset;
  theGltfFace.NodeUV.Type          = RWGltf_GltfAccessorLayout_Vec2;
  theGltfFace.NodeUV.ComponentType = RWGltf_GltfAccessorCompType_Float32;
  const Standard_Integer aNodeUpper = theFaceIter.NodeUpper();
  for (Standard_Integer aNodeIter = theFaceIter.NodeLower(); aNodeIter <= aNodeUpper; ++aNodeIter)
  {
    gp_Pnt2d aTexCoord = theFaceIter.NodeTexCoord (aNodeIter);
    aTexCoord.SetY (1.0 - aTexCoord.Y());
    writeVec2 (theBinFile, aTexCoord.XY());
  }
}

// =======================================================================
// function : saveIndices
// purpose  :
// =======================================================================
void RWGltf_CafWriter::saveIndices (RWGltf_GltfFace& theGltfFace,
                                    std::ostream& theBinFile,
                                    const RWMesh_FaceIterator& theFaceIter,
                                    Standard_Integer& theAccessorNb)
{
  theGltfFace.Indices.Id            = theAccessorNb++;
  theGltfFace.Indices.Count         = theFaceIter.NbTriangles() * 3;
  theGltfFace.Indices.ByteOffset    = (int64_t )theBinFile.tellp() - myBuffViewInd.ByteOffset;
  theGltfFace.Indices.Type          = RWGltf_GltfAccessorLayout_Scalar;
  theGltfFace.Indices.ComponentType = theGltfFace.NodePos.Count > std::numeric_limits<uint16_t>::max()
                                    ? RWGltf_GltfAccessorCompType_UInt32
                                    : RWGltf_GltfAccessorCompType_UInt16;

  const Standard_Integer anElemLower = theFaceIter.ElemLower();
  const Standard_Integer anElemUpper = theFaceIter.ElemUpper();
  for (Standard_Integer anElemIter = anElemLower; anElemIter <= anElemUpper; ++anElemIter)
  {
    Poly_Triangle aTri = theFaceIter.TriangleOriented (anElemIter);
    aTri(1) -= anElemLower;
    aTri(2) -= anElemLower;
    aTri(3) -= anElemLower;
    if (theGltfFace.Indices.ComponentType == RWGltf_GltfAccessorCompType_UInt16)
    {
      writeTriangle16 (theBinFile, NCollection_Vec3<uint16_t>((uint16_t)aTri(1), (uint16_t)aTri(2), (uint16_t)aTri(3)));
    }
    else
    {
      writeTriangle32 (theBinFile, Graphic3d_Vec3i (aTri(1), aTri(2), aTri(3)));
    }
  }
  if (theGltfFace.Indices.ComponentType == RWGltf_GltfAccessorCompType_UInt16)
  {
    // alignment by 4 bytes
    int64_t aContentLen64 = (int64_t)theBinFile.tellp();
    while (aContentLen64 % 4 != 0)
    {
      theBinFile.write (" ", 1);
      ++aContentLen64;
    }
  }
}

// =======================================================================
// function : Perform
// purpose  :
// =======================================================================
bool RWGltf_CafWriter::Perform (const Handle(TDocStd_Document)& theDocument,
                                const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                const Message_ProgressRange& theProgress)
{
  TDF_LabelSequence aRoots;
  Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool (theDocument->Main());
  aShapeTool->GetFreeShapes (aRoots);
  return Perform (theDocument, aRoots, NULL, theFileInfo, theProgress);
}

// =======================================================================
// function : Perform
// purpose  :
// =======================================================================
bool RWGltf_CafWriter::Perform (const Handle(TDocStd_Document)& theDocument,
                                const TDF_LabelSequence& theRootLabels,
                                const TColStd_MapOfAsciiString* theLabelFilter,
                                const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                const Message_ProgressRange& theProgress)
{
  Message_ProgressScope aPSentry (theProgress, "Writing glTF file", 2);
  if (!writeBinData (theDocument, theRootLabels, theLabelFilter, aPSentry.Next()))
  {
    return false;
  }

  if (!aPSentry.More())
  {
    return false;
  }

  return writeJson (theDocument, theRootLabels, theLabelFilter, theFileInfo, aPSentry.Next());
}

// =======================================================================
// function : writeBinData
// purpose  :
// =======================================================================
bool RWGltf_CafWriter::writeBinData (const Handle(TDocStd_Document)& theDocument,
                                     const TDF_LabelSequence& theRootLabels,
                                     const TColStd_MapOfAsciiString* theLabelFilter,
                                     const Message_ProgressRange& theProgress)
{
  myBuffViewPos.ByteOffset       = 0;
  myBuffViewPos.ByteLength       = 0;
  myBuffViewPos.ByteStride       = 12;
  myBuffViewPos.Target           = RWGltf_GltfBufferViewTarget_ARRAY_BUFFER;
  myBuffViewNorm.ByteOffset      = 0;
  myBuffViewNorm.ByteLength      = 0;
  myBuffViewNorm.ByteStride      = 12;
  myBuffViewNorm.Target          = RWGltf_GltfBufferViewTarget_ARRAY_BUFFER;
  myBuffViewTextCoord.ByteOffset = 0;
  myBuffViewTextCoord.ByteLength = 0;
  myBuffViewTextCoord.ByteStride = 8;
  myBuffViewTextCoord.Target     = RWGltf_GltfBufferViewTarget_ARRAY_BUFFER;
  myBuffViewInd.ByteOffset       = 0;
  myBuffViewInd.ByteLength       = 0;
  myBuffViewInd.Target           = RWGltf_GltfBufferViewTarget_ELEMENT_ARRAY_BUFFER;

  myBinDataMap.Clear();
  myBinDataLen64 = 0;

  std::ofstream aBinFile;
  OSD_OpenStream (aBinFile, myBinFileNameFull.ToCString(), std::ios::out | std::ios::binary);
  if (!aBinFile.is_open()
   || !aBinFile.good())
  {
    Message::SendFail (TCollection_AsciiString ("File '") + myBinFileNameFull + "' can not be created");
    return false;
  }

  Message_ProgressScope aPSentryBin (theProgress, "Binary data", 4);

  Standard_Integer aNbAccessors = 0;

  // write positions
  myBuffViewPos.ByteOffset = aBinFile.tellp();
  for (XCAFPrs_DocumentExplorer aDocExplorer (theDocument, theRootLabels, XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
       aDocExplorer.More() && aPSentryBin.More(); aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL
    && !theLabelFilter->Contains (aDocNode.Id))
    {
      continue;
    }

    // transformation will be stored at scene nodes
    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More() && aPSentryBin.More(); aFaceIter.Next())
    {
      if (myBinDataMap.IsBound (aFaceIter.Face())
       || toSkipFaceMesh (aFaceIter))
      {
        continue;
      }

      RWGltf_GltfFace aGltfFace;
      saveNodes (aGltfFace, aBinFile, aFaceIter, aNbAccessors);

      if (!aBinFile.good())
      {
        Message::SendFail (TCollection_AsciiString ("File '") + myBinFileNameFull + "' can not be written");
        return false;
      }

      myBinDataMap.Bind (aFaceIter.Face(), aGltfFace);
    }
  }
  myBuffViewPos.ByteLength = (int64_t )aBinFile.tellp() - myBuffViewPos.ByteOffset;
  if (!aPSentryBin.More())
  {
    return false;
  }
  aPSentryBin.Next();

  // write normals
  myBuffViewNorm.ByteOffset = aBinFile.tellp();
  for (XCAFPrs_DocumentExplorer aDocExplorer (theDocument, theRootLabels, XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
       aDocExplorer.More() && aPSentryBin.More(); aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL
    && !theLabelFilter->Contains (aDocNode.Id))
    {
      continue;
    }
    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More() && aPSentryBin.More(); aFaceIter.Next())
    {
      if (toSkipFaceMesh (aFaceIter))
      {
        continue;
      }

      RWGltf_GltfFace& aGltfFace = myBinDataMap.ChangeFind (aFaceIter.Face());
      if (aGltfFace.NodeNorm.Id != RWGltf_GltfAccessor::INVALID_ID)
      {
        continue;
      }

      saveNormals (aGltfFace, aBinFile, aFaceIter, aNbAccessors);

      if (!aBinFile.good())
      {
        Message::SendFail (TCollection_AsciiString ("File '") + myBinFileNameFull + "' can not be written");
        return false;
      }
    }
  }
  myBuffViewNorm.ByteLength = (int64_t )aBinFile.tellp() - myBuffViewNorm.ByteOffset;
  if (!aPSentryBin.More())
  {
    return false;
  }
  aPSentryBin.Next();

  // write texture coordinates
  myBuffViewTextCoord.ByteOffset = aBinFile.tellp();
  for (XCAFPrs_DocumentExplorer aDocExplorer (theDocument, theRootLabels, XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
       aDocExplorer.More() && aPSentryBin.More(); aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL
    && !theLabelFilter->Contains (aDocNode.Id))
    {
      continue;
    }

    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More() && aPSentryBin.More(); aFaceIter.Next())
    {
      if (toSkipFaceMesh (aFaceIter))
      {
        continue;
      }

      RWGltf_GltfFace& aGltfFace = myBinDataMap.ChangeFind (aFaceIter.Face());
      if (aGltfFace.NodeUV.Id != RWGltf_GltfAccessor::INVALID_ID)
      {
        continue;
      }

      saveTextCoords (aGltfFace, aBinFile, aFaceIter, aNbAccessors);

      if (!aBinFile.good())
      {
        Message::SendFail (TCollection_AsciiString ("File '") + myBinFileNameFull + "' can not be written");
        return false;
      }
    }
  }
  myBuffViewTextCoord.ByteLength = (int64_t )aBinFile.tellp() - myBuffViewTextCoord.ByteOffset;
  if (!aPSentryBin.More())
  {
    return false;
  }
  aPSentryBin.Next();

  // write indices
  myBuffViewInd.ByteOffset = aBinFile.tellp();
  for (XCAFPrs_DocumentExplorer aDocExplorer (theDocument, theRootLabels, XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
       aDocExplorer.More() && aPSentryBin.More(); aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL
    && !theLabelFilter->Contains (aDocNode.Id))
    {
      continue;
    }

    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More() && aPSentryBin.More(); aFaceIter.Next())
    {
      if (toSkipFaceMesh (aFaceIter))
      {
        continue;
      }

      RWGltf_GltfFace& aGltfFace = myBinDataMap.ChangeFind (aFaceIter.Face());
      if (aGltfFace.Indices.Id != RWGltf_GltfAccessor::INVALID_ID)
      {
        continue;
      }

      saveIndices (aGltfFace, aBinFile, aFaceIter, aNbAccessors);

      if (!aBinFile.good())
      {
        Message::SendFail (TCollection_AsciiString ("File '") + myBinFileNameFull + "' can not be written");
        return false;
      }
    }
  }
  myBuffViewInd.ByteLength = (int64_t )aBinFile.tellp() - myBuffViewInd.ByteOffset;

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

  myBinDataLen64 = aBinFile.tellp();
  aBinFile.close();
  if (!aBinFile.good())
  {
    Message::SendFail (TCollection_AsciiString ("File '") + myBinFileNameFull + "' can not be written");
    return false;
  }
  return true;
}

//================================================================
// Function : writeJson
// Purpose  :
//================================================================
bool RWGltf_CafWriter::writeJson (const Handle(TDocStd_Document)&  theDocument,
                                  const TDF_LabelSequence&         theRootLabels,
                                  const TColStd_MapOfAsciiString*  theLabelFilter,
                                  const TColStd_IndexedDataMapOfStringString& theFileInfo,
                                  const Message_ProgressRange& theProgress)
{
#ifdef HAVE_RAPIDJSON
  myWriter.reset();

  // write vertex arrays
  Message_ProgressScope aPSentryBin (theProgress, "Header data", 2);

  const Standard_Integer aBinDataBufferId = 0;
  const Standard_Integer aDefSamplerId    = 0;
  const Standard_Integer aDefSceneId      = 0;

  const TCollection_AsciiString aFileNameGltf = myFile;
  std::ofstream aGltfContentFile;
  OSD_OpenStream (aGltfContentFile, aFileNameGltf.ToCString(), std::ios::out | std::ios::binary);
  if (!aGltfContentFile.is_open()
   || !aGltfContentFile.good())
  {
    Message::SendFail (TCollection_AsciiString ("File '") + aFileNameGltf + "' can not be created");
    return false;
  }
  if (myIsBinary)
  {
    const char* aMagic = "glTF";
    uint32_t aVersion       = 2;
    uint32_t aLength        = 0;
    uint32_t aContentLength = 0;
    uint32_t aContentType   = 0x4E4F534A;

    aGltfContentFile.write (aMagic, 4);
    aGltfContentFile.write ((const char* )&aVersion,       sizeof(aVersion));
    aGltfContentFile.write ((const char* )&aLength,        sizeof(aLength));
    aGltfContentFile.write ((const char* )&aContentLength, sizeof(aContentLength));
    aGltfContentFile.write ((const char* )&aContentType,   sizeof(aContentType));
  }

  // Prepare an indexed map of scene nodes (without assemblies) in correct order.
  // Note: this is also order of meshes in glTF document array.
  RWGltf_GltfSceneNodeMap aSceneNodeMap;
  for (XCAFPrs_DocumentExplorer aDocExplorer (theDocument, theRootLabels, XCAFPrs_DocumentExplorerFlags_OnlyLeafNodes);
       aDocExplorer.More(); aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    if (theLabelFilter != NULL
    && !theLabelFilter->Contains (aDocNode.Id))
    {
      continue;
    }
    aSceneNodeMap.Add (aDocNode);
  }

  rapidjson::OStreamWrapper aFileStream (aGltfContentFile);
  myWriter.reset (new RWGltf_GltfOStreamWriter (aFileStream));

  myWriter->StartObject();

  writeAccessors (aSceneNodeMap);
  writeAnimations();
  writeAsset (theFileInfo);
  writeBufferViews (aBinDataBufferId);
  writeBuffers();
  writeExtensions ();

  RWGltf_GltfMaterialMap aMaterialMap (myFile, aDefSamplerId);
  aMaterialMap.SetDefaultStyle (myDefaultStyle);
  writeImages    (aSceneNodeMap, aMaterialMap);
  writeMaterials (aSceneNodeMap, aMaterialMap);
  writeMeshes    (aSceneNodeMap, aMaterialMap);

  aPSentryBin.Next();
  if (!aPSentryBin.More())
  {
    return false;
  }

  // root nodes indices starting from 0
  NCollection_Sequence<Standard_Integer> aSceneRootNodeInds;
  writeNodes (theDocument, theRootLabels, theLabelFilter, aSceneNodeMap, aSceneRootNodeInds);
  writeSamplers (aMaterialMap);
  writeScene (aDefSceneId);
  writeScenes (aSceneRootNodeInds);
  writeSkins();
  writeTextures (aSceneNodeMap, aMaterialMap);

  myWriter->EndObject();

  if (!myIsBinary)
  {
    aGltfContentFile.close();
    if (!aGltfContentFile.good())
    {
      Message::SendFail (TCollection_AsciiString ("File '") + aFileNameGltf + "' can not be written");
      return false;
    }
    return true;
  }

  int64_t aContentLen64 = (int64_t )aGltfContentFile.tellp() - 20;
  while (aContentLen64 % 4 != 0)
  {
    aGltfContentFile.write (" ", 1);
    ++aContentLen64;
  }

  const uint32_t aBinLength = (uint32_t )myBinDataLen64;
  const uint32_t aBinType   = 0x004E4942;
  aGltfContentFile.write ((const char*)&aBinLength, 4);
  aGltfContentFile.write ((const char*)&aBinType,   4);

  const int64_t aFullLen64 = aContentLen64 + 20 + myBinDataLen64 + 8;
  if (aFullLen64 < std::numeric_limits<uint32_t>::max())
  {
    {
      std::ifstream aBinFile;
      OSD_OpenStream (aBinFile, myBinFileNameFull.ToCString(), std::ios::in | std::ios::binary);
      if (!aBinFile.is_open()
       || !aBinFile.good())
      {
        Message::SendFail (TCollection_AsciiString ("File '") + myBinFileNameFull + "' cannot be opened");
        return false;
      }
      char aBuffer[4096];
      for (; aBinFile.good();)
      {
        aBinFile.read (aBuffer, 4096);
        const Standard_Integer aReadLen = (Standard_Integer )aBinFile.gcount();
        if (aReadLen == 0)
        {
          break;
        }
        aGltfContentFile.write (aBuffer, aReadLen);
      }
    }
    OSD_Path aBinFilePath (myBinFileNameFull);
    OSD_File (aBinFilePath).Remove();
    if (OSD_File (aBinFilePath).Exists())
    {
      Message::SendFail (TCollection_AsciiString ("Unable to remove temporary glTF content file '") + myBinFileNameFull + "'");
    }
  }
  else
  {
    Message::SendFail ("glTF file content is too big for binary format");
    return false;
  }

  const uint32_t aLength        = (uint32_t )aFullLen64;
  const uint32_t aContentLength = (uint32_t )aContentLen64;
  aGltfContentFile.seekp (8);
  aGltfContentFile.write ((const char* )&aLength,        4);
  aGltfContentFile.write ((const char* )&aContentLength, 4);

  aGltfContentFile.close();
  if (!aGltfContentFile.good())
  {
    Message::SendFail (TCollection_AsciiString ("File '") + aFileNameGltf + "' can not be written");
    return false;
  }

  myWriter.reset();
  return true;
#else
  (void )theDocument;
  (void )theRootLabels;
  (void )theLabelFilter;
  (void )theFileInfo;
  (void )theProgress;
  Message::SendFail ("Error: glTF writer is unavailable - OCCT has been built without RapidJSON support [HAVE_RAPIDJSON undefined]");
  return false;
#endif
}

// =======================================================================
// function : writeAccessors
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeAccessors (const RWGltf_GltfSceneNodeMap& theSceneNodeMap)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeAccessors()");

  myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Accessors));
  myWriter->StartArray();

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aWrittenFaces;
  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter (theSceneNodeMap); aSceneNodeIter.More(); aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More(); aFaceIter.Next())
    {
      if (!aWrittenFaces.Add (aFaceIter.Face()) // skip repeating faces
        || toSkipFaceMesh (aFaceIter))
      {
        continue;
      }

      const RWGltf_GltfFace& aGltfFace = myBinDataMap.Find (aFaceIter.Face());
      writePositions (aGltfFace);
    }
  }
  aWrittenFaces.Clear();
  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter (theSceneNodeMap); aSceneNodeIter.More(); aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More(); aFaceIter.Next())
    {
      if (!aWrittenFaces.Add (aFaceIter.Face()) // skip repeating faces
        || toSkipFaceMesh (aFaceIter))
      {
        continue;
      }

      const RWGltf_GltfFace& aGltfFace = myBinDataMap.Find (aFaceIter.Face());
      writeNormals (aGltfFace);
    }
  }
  aWrittenFaces.Clear();
  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter (theSceneNodeMap); aSceneNodeIter.More(); aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More(); aFaceIter.Next())
    {
      if (!aWrittenFaces.Add (aFaceIter.Face()) // skip repeating faces
        || toSkipFaceMesh (aFaceIter))
      {
        continue;
      }

      const RWGltf_GltfFace& aGltfFace = myBinDataMap.Find (aFaceIter.Face());
      writeTextCoords (aGltfFace);
    }
  }
  aWrittenFaces.Clear();
  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter (theSceneNodeMap); aSceneNodeIter.More(); aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More(); aFaceIter.Next())
    {
      if (!aWrittenFaces.Add (aFaceIter.Face()) // skip repeating faces
        || toSkipFaceMesh (aFaceIter))
      {
        continue;
      }

      const RWGltf_GltfFace& aGltfFace = myBinDataMap.Find (aFaceIter.Face());
      writeIndices (aGltfFace);
    }
  }

  myWriter->EndArray();
#else
  (void )theSceneNodeMap;
#endif
}

// =======================================================================
// function : writePositions
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writePositions (const RWGltf_GltfFace& theGltfFace)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writePositions()");
  if (theGltfFace.NodePos.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    return;
  }

  myWriter->StartObject();
  myWriter->Key    ("bufferView");
  myWriter->Int    (myBuffViewPos.Id);
  myWriter->Key    ("byteOffset");
  myWriter->Int64  (theGltfFace.NodePos.ByteOffset);
  myWriter->Key    ("componentType");
  myWriter->Int    (theGltfFace.NodePos.ComponentType);
  myWriter->Key    ("count");
  myWriter->Int64  (theGltfFace.NodePos.Count);

  if (theGltfFace.NodePos.BndBox.IsValid())
  {
    myWriter->Key ("max");
    myWriter->StartArray();
    myWriter->Double (theGltfFace.NodePos.BndBox.CornerMax().x());
    myWriter->Double (theGltfFace.NodePos.BndBox.CornerMax().y());
    myWriter->Double (theGltfFace.NodePos.BndBox.CornerMax().z());
    myWriter->EndArray();

    myWriter->Key("min");
    myWriter->StartArray();
    myWriter->Double (theGltfFace.NodePos.BndBox.CornerMin().x());
    myWriter->Double (theGltfFace.NodePos.BndBox.CornerMin().y());
    myWriter->Double (theGltfFace.NodePos.BndBox.CornerMin().z());
    myWriter->EndArray();
  }
  myWriter->Key    ("type");
  myWriter->String ("VEC3");

  myWriter->EndObject();
#else
  (void )theGltfFace;
#endif
}

// =======================================================================
// function : writeNormals
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeNormals (const RWGltf_GltfFace& theGltfFace)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeNormals()");
  if (theGltfFace.NodeNorm.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    return;
  }

  myWriter->StartObject();
  myWriter->Key    ("bufferView");
  myWriter->Int    (myBuffViewNorm.Id);
  myWriter->Key    ("byteOffset");
  myWriter->Int64  (theGltfFace.NodeNorm.ByteOffset);
  myWriter->Key    ("componentType");
  myWriter->Int    (theGltfFace.NodeNorm.ComponentType);
  myWriter->Key    ("count");
  myWriter->Int64  (theGltfFace.NodeNorm.Count);
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
  myWriter->Key    ("type");
  myWriter->String ("VEC3");

  myWriter->EndObject();
#else
  (void )theGltfFace;
#endif
}

// =======================================================================
// function : writeTextCoords
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeTextCoords (const RWGltf_GltfFace& theGltfFace)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeTextCoords()");
  if (theGltfFace.NodeUV.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    return;
  }

  myWriter->StartObject();
  myWriter->Key    ("bufferView");
  myWriter->Int    (myBuffViewTextCoord.Id);
  myWriter->Key    ("byteOffset");
  myWriter->Int64  (theGltfFace.NodeUV.ByteOffset);
  myWriter->Key    ("componentType");
  myWriter->Int    (theGltfFace.NodeUV.ComponentType);
  myWriter->Key    ("count");
  myWriter->Int64  (theGltfFace.NodeUV.Count);
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
  myWriter->Key    ("type");
  myWriter->String ("VEC2");

  myWriter->EndObject();
#else
  (void )theGltfFace;
#endif
}

// =======================================================================
// function : writeIndices
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeIndices (const RWGltf_GltfFace& theGltfFace)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeIndices()");
  if (theGltfFace.Indices.Id == RWGltf_GltfAccessor::INVALID_ID)
  {
    return;
  }

  myWriter->StartObject();
  myWriter->Key    ("bufferView");
  myWriter->Int    (myBuffViewInd.Id);
  myWriter->Key    ("byteOffset");
  myWriter->Int64  (theGltfFace.Indices.ByteOffset);
  myWriter->Key    ("componentType");
  myWriter->Int    (theGltfFace.Indices.ComponentType);
  myWriter->Key    ("count");
  myWriter->Int64  (theGltfFace.Indices.Count);

  myWriter->Key    ("type");
  myWriter->String ("SCALAR");

  myWriter->EndObject();
#else
  (void )theGltfFace;
#endif
}

// =======================================================================
// function : writeAnimations
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeAnimations()
{
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeAnimations()");

  // This section should be skipped if it doesn't contain any information but not be empty
  //myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Animations));
  //myWriter->StartArray();
  //myWriter->EndArray();
}

// =======================================================================
// function : writeAsset
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeAsset (const TColStd_IndexedDataMapOfStringString& theFileInfo)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeAsset()");

  myWriter->Key    (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Asset));
  myWriter->StartObject();
  myWriter->Key    ("generator");
  myWriter->String ("Open CASCADE Technology [www.opencascade.com]");
  myWriter->Key    ("version");
  myWriter->String ("2.0"); // glTF format version

  bool anIsStarted = false;
  for (TColStd_IndexedDataMapOfStringString::Iterator aKeyValueIter (theFileInfo); aKeyValueIter.More(); aKeyValueIter.Next())
  {
    if (!anIsStarted)
    {
      myWriter->Key ("extras");
      myWriter->StartObject();
      anIsStarted = true;
    }
    myWriter->Key (aKeyValueIter.Key().ToCString());
    myWriter->String (aKeyValueIter.Value().ToCString());
  }
  if (anIsStarted)
  {
    myWriter->EndObject();
  }

  myWriter->EndObject();
#else
  (void )theFileInfo;
#endif
}

// =======================================================================
// function : writeBufferViews
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeBufferViews (const Standard_Integer theBinDataBufferId)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeBufferViews()");

  myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_BufferViews));
  myWriter->StartArray();
  if (myBuffViewPos.Id != RWGltf_GltfAccessor::INVALID_ID)
  {
    myWriter->StartObject();
    myWriter->Key    ("buffer");
    myWriter->Int    (theBinDataBufferId);
    myWriter->Key    ("byteLength");
    myWriter->Int64  (myBuffViewPos.ByteLength);
    myWriter->Key    ("byteOffset");
    myWriter->Int64  (myBuffViewPos.ByteOffset);
    myWriter->Key    ("byteStride");
    myWriter->Int64  (myBuffViewPos.ByteStride);
    myWriter->Key    ("target");
    myWriter->Int    (myBuffViewPos.Target);
    myWriter->EndObject();
  }
  if (myBuffViewNorm.Id != RWGltf_GltfAccessor::INVALID_ID)
  {
    myWriter->StartObject();
    myWriter->Key    ("buffer");
    myWriter->Int    (theBinDataBufferId);
    myWriter->Key    ("byteLength");
    myWriter->Int64  (myBuffViewNorm.ByteLength);
    myWriter->Key    ("byteOffset");
    myWriter->Int64  (myBuffViewNorm.ByteOffset);
    myWriter->Key    ("byteStride");
    myWriter->Int64  (myBuffViewNorm.ByteStride);
    myWriter->Key    ("target");
    myWriter->Int    (myBuffViewNorm.Target);
    myWriter->EndObject();
  }
  if (myBuffViewTextCoord.Id != RWGltf_GltfAccessor::INVALID_ID)
  {
    myWriter->StartObject();
    myWriter->Key    ("buffer");
    myWriter->Int    (theBinDataBufferId);
    myWriter->Key    ("byteLength");
    myWriter->Int64  (myBuffViewTextCoord.ByteLength);
    myWriter->Key    ("byteOffset");
    myWriter->Int64  (myBuffViewTextCoord.ByteOffset);
    myWriter->Key    ("byteStride");
    myWriter->Int64  (myBuffViewTextCoord.ByteStride);
    myWriter->Key    ("target");
    myWriter->Int    (myBuffViewTextCoord.Target);
    myWriter->EndObject();
  }
  if (myBuffViewInd.Id != RWGltf_GltfAccessor::INVALID_ID)
  {
    myWriter->StartObject();
    myWriter->Key    ("buffer");
    myWriter->Int    (theBinDataBufferId);
    myWriter->Key    ("byteLength");
    myWriter->Int64  (myBuffViewInd.ByteLength);
    myWriter->Key    ("byteOffset");
    myWriter->Int64  (myBuffViewInd.ByteOffset);
    myWriter->Key    ("target");
    myWriter->Int    (myBuffViewInd.Target);
    myWriter->EndObject();
  }
  myWriter->EndArray();
#else
  (void )theBinDataBufferId;
#endif
}

// =======================================================================
// function : writeBuffers
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeBuffers()
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeBuffers()");

  myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Buffers));
  myWriter->StartArray();
  {
    myWriter->StartObject();
    {
      myWriter->Key   ("byteLength");
      myWriter->Int64 (myBuffViewPos.ByteLength + myBuffViewNorm.ByteLength +
                       myBuffViewTextCoord.ByteLength + myBuffViewInd.ByteLength);
      if (!myIsBinary)
      {
        myWriter->Key   ("uri");
        myWriter->String (myBinFileNameShort.ToCString());
      }
    }
    myWriter->EndObject();
  }
  myWriter->EndArray();
#endif
}

// =======================================================================
// function : writeExtensions
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeExtensions()
{
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeExtensions()");
}

// =======================================================================
// function : writeImages
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeImages (const RWGltf_GltfSceneNodeMap& theSceneNodeMap,
                                    RWGltf_GltfMaterialMap& theMaterialMap)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeImages()");

  // empty RWGltf_GltfRootElement_Images section should NOT be written to avoid validator errors
  bool anIsStarted = false;
  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter (theSceneNodeMap); aSceneNodeIter.More(); aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More(); aFaceIter.Next())
    {
      theMaterialMap.AddImages (myWriter.get(), aFaceIter.FaceStyle(), anIsStarted);
    }
  }
  if (anIsStarted)
  {
    myWriter->EndArray();
  }
#else
  (void )theSceneNodeMap;
  (void )theMaterialMap;
#endif
}

// =======================================================================
// function : writeMaterials
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeMaterials (const RWGltf_GltfSceneNodeMap& theSceneNodeMap,
                                       RWGltf_GltfMaterialMap& theMaterialMap)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeMaterials()");

  // empty RWGltf_GltfRootElement_Materials section should NOT be written to avoid validator errors
  bool anIsStarted = false;
  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter (theSceneNodeMap); aSceneNodeIter.More(); aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More(); aFaceIter.Next())
    {
      theMaterialMap.AddMaterial (myWriter.get(), aFaceIter.FaceStyle(), anIsStarted);
    }
  }
  if (anIsStarted)
  {
    myWriter->EndArray();
  }
#else
  (void )theSceneNodeMap;
  (void )theMaterialMap;
#endif
}

// =======================================================================
// function : writeMeshes
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeMeshes (const RWGltf_GltfSceneNodeMap& theSceneNodeMap,
                                    const RWGltf_GltfMaterialMap&  theMaterialMap)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeMeshes()");

  myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Meshes));
  myWriter->StartArray();

  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter (theSceneNodeMap); aSceneNodeIter.More(); aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
    const TCollection_AsciiString aNodeName = readNameAttribute (aDocNode.RefLabel);
    {
      RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel, TopLoc_Location(), false);
      if (!aFaceIter.More())
      {
        Message::SendWarning (TCollection_AsciiString("RWGltf_CafWriter skipped node '") + aNodeName + "' without triangulation data");
        continue;
      }
    }
    myWriter->StartObject();
    myWriter->Key ("name");
    myWriter->String (aNodeName.ToCString());
    myWriter->Key ("primitives");
    myWriter->StartArray();

    Standard_Integer aNbFacesInNode = 0;
    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More(); aFaceIter.Next(), ++aNbFacesInNode)
    {
      if (toSkipFaceMesh (aFaceIter))
      {
        continue;
      }

      const RWGltf_GltfFace& aGltfFace = myBinDataMap.Find (aFaceIter.Face());
      const TCollection_AsciiString aMatId = theMaterialMap.FindMaterial (aFaceIter.FaceStyle());
      myWriter->StartObject();
      {
        myWriter->Key ("attributes");
        myWriter->StartObject();
        {
          if (aGltfFace.NodeNorm.Id != RWGltf_GltfAccessor::INVALID_ID)
          {
            myWriter->Key ("NORMAL");
            myWriter->Int (aGltfFace.NodeNorm.Id);
          }
          myWriter->Key ("POSITION");
          myWriter->Int (aGltfFace.NodePos.Id);
          if (aGltfFace.NodeUV.Id != RWGltf_GltfAccessor::INVALID_ID)
          {
            myWriter->Key ("TEXCOORD_0");
            myWriter->Int (aGltfFace.NodeUV.Id);
          }
        }
        myWriter->EndObject();

        myWriter->Key ("indices");
        myWriter->Int (aGltfFace.Indices.Id);
        if (!aMatId.IsEmpty())
        {
          myWriter->Key ("material");
          myWriter->Int (aMatId.IntegerValue());
        }
        myWriter->Key ("mode");
        myWriter->Int (RWGltf_GltfPrimitiveMode_Triangles);
      }
      myWriter->EndObject();
    }
    myWriter->EndArray();
    myWriter->EndObject();
  }
  myWriter->EndArray();
#else
  (void )theSceneNodeMap;
  (void )theMaterialMap;
#endif
}

// =======================================================================
// function : writeNodes
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeNodes (const Handle(TDocStd_Document)&  theDocument,
                                   const TDF_LabelSequence&         theRootLabels,
                                   const TColStd_MapOfAsciiString*  theLabelFilter,
                                   const RWGltf_GltfSceneNodeMap&   theSceneNodeMap,
                                   NCollection_Sequence<Standard_Integer>& theSceneRootNodeInds)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeNodes()");

  // Prepare full indexed map of scene nodes in correct order.
  RWGltf_GltfSceneNodeMap aSceneNodeMapWithChildren; // indexes starting from 1
  for (XCAFPrs_DocumentExplorer aDocExplorer (theDocument, theRootLabels, XCAFPrs_DocumentExplorerFlags_None);
       aDocExplorer.More(); aDocExplorer.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aDocExplorer.Current();
    {
      RWMesh_FaceIterator aFaceIter(aDocNode.RefLabel, TopLoc_Location(), false);
      if (!aFaceIter.More())
      {
        continue;
      }
    }
    if (theLabelFilter != NULL
    && !theLabelFilter->Contains (aDocNode.Id))
    {
      continue;
    }

    Standard_Integer aNodeIndex = aSceneNodeMapWithChildren.Add (aDocNode);
    if (aDocExplorer.CurrentDepth() == 0)
    {
      // save root node index (starting from 0 not 1)
      theSceneRootNodeInds.Append (aNodeIndex - 1);
    }
  }

  // Write scene nodes using prepared map for correct order of array members
  myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Nodes));
  myWriter->StartArray();

  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter (aSceneNodeMapWithChildren); aSceneNodeIter.More(); aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();

    myWriter->StartObject();
    {
      if (aDocNode.IsAssembly)
      {
        myWriter->Key ("children");
        myWriter->StartArray();
        {
          for (TDF_ChildIterator aChildIter (aDocNode.RefLabel); aChildIter.More(); aChildIter.Next())
          {
            const TDF_Label& aChildLabel = aChildIter.Value();
            if (aChildLabel.IsNull())
            {
              continue;
            }

            const TCollection_AsciiString aChildId = XCAFPrs_DocumentExplorer::DefineChildId (aChildLabel, aDocNode.Id);
            Standard_Integer aChildIdx = aSceneNodeMapWithChildren.FindIndex (aChildId);
            if (aChildIdx > 0)
            {
              myWriter->Int (aChildIdx - 1);
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
        myCSTrsf.TransformTransformation (aTrsf);
        const gp_Quaternion aQuaternion = aTrsf.GetRotation();
        const bool hasRotation = Abs (aQuaternion.X())       > gp::Resolution()
                              || Abs (aQuaternion.Y())       > gp::Resolution()
                              || Abs (aQuaternion.Z())       > gp::Resolution()
                              || Abs (aQuaternion.W() - 1.0) > gp::Resolution();
        const Standard_Real aScaleFactor = aTrsf.ScaleFactor();
        const bool hasScale = Abs (aScaleFactor - 1.0) > Precision::Confusion();
        const gp_XYZ& aTranslPart = aTrsf.TranslationPart();
        const bool hasTranslation = aTranslPart.SquareModulus() > gp::Resolution();

        RWGltf_WriterTrsfFormat aTrsfFormat = myTrsfFormat;
        if (myTrsfFormat == RWGltf_WriterTrsfFormat_Compact)
        {
          aTrsfFormat = hasRotation && hasScale && hasTranslation
                      ? RWGltf_WriterTrsfFormat_Mat4
                      : RWGltf_WriterTrsfFormat_TRS;
        }

        if (aTrsfFormat == RWGltf_WriterTrsfFormat_Mat4)
        {
          // write full matrix
          Graphic3d_Mat4 aMat4;
          aTrsf.GetMat4 (aMat4);
          if (!aMat4.IsIdentity())
          {
            myWriter->Key ("matrix");
            myWriter->StartArray();
            for (Standard_Integer aColIter = 0; aColIter < 4; ++aColIter)
            {
              for (Standard_Integer aRowIter = 0; aRowIter < 4; ++aRowIter)
              {
                myWriter->Double (aMat4.GetValue (aRowIter, aColIter));
              }
            }
            myWriter->EndArray();
          }
        }
        else //if (aTrsfFormat == RWGltf_WriterTrsfFormat_TRS)
        {
          if (hasRotation)
          {
            myWriter->Key ("rotation");
            myWriter->StartArray();
            myWriter->Double (aQuaternion.X());
            myWriter->Double (aQuaternion.Y());
            myWriter->Double (aQuaternion.Z());
            myWriter->Double (aQuaternion.W());
            myWriter->EndArray();
          }
          if (hasScale)
          {
            myWriter->Key ("scale");
            myWriter->StartArray();
            myWriter->Double (aScaleFactor);
            myWriter->Double (aScaleFactor);
            myWriter->Double (aScaleFactor);
            myWriter->EndArray();
          }
          if (hasTranslation)
          {
            myWriter->Key ("translation");
            myWriter->StartArray();
            myWriter->Double (aTranslPart.X());
            myWriter->Double (aTranslPart.Y());
            myWriter->Double (aTranslPart.Z());
            myWriter->EndArray();
          }
        }
      }
    }
    if (!aDocNode.IsAssembly)
    {
      myWriter->Key ("mesh");
      // Mesh order of current node is equal to order of this node in scene nodes map
      Standard_Integer aMeshIdx = theSceneNodeMap.FindIndex (aDocNode.Id);
      if (aMeshIdx > 0)
      {
        myWriter->Int (aMeshIdx - 1);
      }
    }
    {
      TCollection_AsciiString aNodeName = readNameAttribute (aDocNode.Label);
      if (aNodeName.IsEmpty())
      {
        aNodeName = readNameAttribute (aDocNode.RefLabel);
      }
      myWriter->Key ("name");
      myWriter->String (aNodeName.ToCString());
    }
    myWriter->EndObject();
  }
  myWriter->EndArray();
#else
  (void )theDocument;
  (void )theRootLabels;
  (void )theLabelFilter;
  (void )theSceneNodeMap;
  (void )theSceneRootNodeInds;
#endif
}

// =======================================================================
// function : writeSamplers
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeSamplers (const RWGltf_GltfMaterialMap& theMaterialMap)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeSamplers()");
  if (theMaterialMap.NbImages() == 0)
  {
    return;
  }

  myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Samplers));
  myWriter->StartArray();
  {
    myWriter->StartObject();
    {
      //myWriter->Key ("magFilter");
      //myWriter->Int (9729);
      //myWriter->Key ("minFilter");
      //myWriter->Int (9729);
    }
    myWriter->EndObject();
  }
  myWriter->EndArray();
#else
  (void )theMaterialMap;
#endif
}

// =======================================================================
// function : writeScene
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeScene (const Standard_Integer theDefSceneId)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeScene()");

  myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Scene));
  myWriter->Int (theDefSceneId);
#else
  (void )theDefSceneId;
#endif
}

// =======================================================================
// function : writeScenes
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeScenes (const NCollection_Sequence<Standard_Integer>& theSceneRootNodeInds)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeScenes()");

  myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Scenes));
  myWriter->StartArray();
  {
    myWriter->StartObject();
    myWriter->Key ("nodes");
    myWriter->StartArray();
    for (NCollection_Sequence<Standard_Integer>::Iterator aRootIter (theSceneRootNodeInds); aRootIter.More(); aRootIter.Next())
    {
      myWriter->Int (aRootIter.Value());
    }
    myWriter->EndArray();
    myWriter->EndObject();
  }
  myWriter->EndArray();
#else
  (void )theSceneRootNodeInds;
#endif
}

// =======================================================================
// function : writeSkins
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeSkins()
{
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeSkins()");

  // This section should be skipped if it doesn't contain any information but not be empty
  /*myWriter->Key (RWGltf_GltfRootElementName (RWGltf_GltfRootElement_Skins));
  myWriter->StartArray();
  myWriter->EndArray();*/
}

// =======================================================================
// function : writeTextures
// purpose  :
// =======================================================================
void RWGltf_CafWriter::writeTextures (const RWGltf_GltfSceneNodeMap& theSceneNodeMap,
                                      RWGltf_GltfMaterialMap& theMaterialMap)
{
#ifdef HAVE_RAPIDJSON
  Standard_ProgramError_Raise_if (myWriter.get() == NULL, "Internal error: RWGltf_CafWriter::writeTextures()");

  // empty RWGltf_GltfRootElement_Textures section should not be written to avoid validator errors
  bool anIsStarted = false;
  for (RWGltf_GltfSceneNodeMap::Iterator aSceneNodeIter (theSceneNodeMap); aSceneNodeIter.More(); aSceneNodeIter.Next())
  {
    const XCAFPrs_DocumentNode& aDocNode = aSceneNodeIter.Value();
    for (RWMesh_FaceIterator aFaceIter (aDocNode.RefLabel, TopLoc_Location(), true, aDocNode.Style); aFaceIter.More(); aFaceIter.Next())
    {
      theMaterialMap.AddTextures (myWriter.get(), aFaceIter.FaceStyle(), anIsStarted);
    }
  }
  if (anIsStarted)
  {
    myWriter->EndArray();
  }
#else
 (void )theSceneNodeMap;
 (void )theMaterialMap;
#endif
}
