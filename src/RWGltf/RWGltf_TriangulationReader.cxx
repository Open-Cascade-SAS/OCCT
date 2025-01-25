// Author: Kirill Gavrilov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <RWGltf_TriangulationReader.hxx>

#include <Message.hxx>
#include <OSD_FileSystem.hxx>
#include <RWGltf_GltfLatePrimitiveArray.hxx>
#include <RWGltf_GltfPrimArrayData.hxx>
#include <Standard_ArrayStreamBuffer.hxx>
#include <Standard_ReadBuffer.hxx>

#ifdef HAVE_DRACO
  #include <Standard_WarningsDisable.hxx>
  #include <draco/compression/decode.h>
  #include <Standard_WarningsRestore.hxx>
#endif

namespace
{
static const Standard_Integer   THE_LOWER_TRI_INDEX  = 1;
static const Standard_Integer   THE_LOWER_NODE_INDEX = 1;
static const Standard_ShortReal THE_NORMAL_PREC2     = 0.001f;

#ifdef HAVE_DRACO
//! Return array type from Draco attribute type.
static RWGltf_GltfArrayType arrayTypeFromDraco(draco::GeometryAttribute::Type theType)
{
  switch (theType)
  {
    case draco::GeometryAttribute::POSITION:
      return RWGltf_GltfArrayType_Position;
    case draco::GeometryAttribute::NORMAL:
      return RWGltf_GltfArrayType_Normal;
    case draco::GeometryAttribute::COLOR:
      return RWGltf_GltfArrayType_Color;
    case draco::GeometryAttribute::TEX_COORD:
      return RWGltf_GltfArrayType_TCoord0;
    default:
      return RWGltf_GltfArrayType_UNKNOWN;
  }
}

//! Return layout from Draco number of components.
static RWGltf_GltfAccessorLayout layoutFromDraco(int8_t theNbComps)
{
  switch (theNbComps)
  {
    case 1:
      return RWGltf_GltfAccessorLayout_Scalar;
    case 2:
      return RWGltf_GltfAccessorLayout_Vec2;
    case 3:
      return RWGltf_GltfAccessorLayout_Vec3;
    case 4:
      return RWGltf_GltfAccessorLayout_Vec4;
  }
  return RWGltf_GltfAccessorLayout_UNKNOWN;
}

//! Return component type from Draco data type.
static RWGltf_GltfAccessorCompType compTypeFromDraco(draco::DataType theType)
{
  switch (theType)
  {
    case draco::DT_INT8:
      return RWGltf_GltfAccessorCompType_Int8;
    case draco::DT_UINT8:
      return RWGltf_GltfAccessorCompType_UInt8;
    case draco::DT_INT16:
      return RWGltf_GltfAccessorCompType_Int16;
    case draco::DT_UINT16:
      return RWGltf_GltfAccessorCompType_UInt16;
    case draco::DT_INT32:
    case draco::DT_UINT32:
      return RWGltf_GltfAccessorCompType_UInt32;
    // case draco::DT_INT64:
    // case draco::DT_UINT64:
    case draco::DT_FLOAT32:
      return RWGltf_GltfAccessorCompType_Float32;
    // case draco::DT_FLOAT64:
    // case draco::DT_BOOL:
    default:
      return RWGltf_GltfAccessorCompType_UNKNOWN;
  }
}
#endif
} // namespace

IMPLEMENT_STANDARD_RTTIEXT(RWGltf_TriangulationReader, RWMesh_TriangulationReader)

// =======================================================================
// function : RWGltf_TriangulationReader
// purpose  :
// =======================================================================
RWGltf_TriangulationReader::RWGltf_TriangulationReader()
{
  //
}

// =======================================================================
// function : reportError
// purpose  :
// =======================================================================
void RWGltf_TriangulationReader::reportError(const TCollection_AsciiString& theText) const
{
  Message::SendFail(TCollection_AsciiString("File '") + myFileName + "' defines invalid glTF!\n"
                    + theText);
}

// =======================================================================
// function : LoadStreamData
// purpose  :
// =======================================================================
bool RWGltf_TriangulationReader::LoadStreamData(
  const Handle(RWMesh_TriangulationSource)& theSourceMesh,
  const Handle(Poly_Triangulation)&         theDestMesh) const
{
  Standard_ASSERT_RETURN(!theDestMesh.IsNull(),
                         "The destination mesh should be initialized before loading data to it",
                         false);
  theDestMesh->Clear();
  theDestMesh->SetDoublePrecision(myIsDoublePrecision);

  if (!loadStreamData(theSourceMesh, theDestMesh))
  {
    theDestMesh->Clear();
    return false;
  }
  if (!finalizeLoading(theSourceMesh, theDestMesh))
  {
    theDestMesh->Clear();
    return false;
  }
  return true;
}

// =======================================================================
// function : readStreamData
// purpose  :
// =======================================================================
bool RWGltf_TriangulationReader::readStreamData(
  const Handle(RWGltf_GltfLatePrimitiveArray)& theSourceGltfMesh,
  const RWGltf_GltfPrimArrayData&              theGltfData,
  const Handle(Poly_Triangulation)&            theDestMesh) const
{
  Standard_ArrayStreamBuffer aStreamBuffer((const char*)theGltfData.StreamData->Data(),
                                           theGltfData.StreamData->Size());
  std::istream               aStream(&aStreamBuffer);
  aStream.seekg((std::streamoff)theGltfData.StreamOffset, std::ios_base::beg);
  if (!readBuffer(theSourceGltfMesh, theDestMesh, aStream, theGltfData.Accessor, theGltfData.Type))
  {
    return false;
  }
  return true;
}

// =======================================================================
// function : readFileData
// purpose  :
// =======================================================================
bool RWGltf_TriangulationReader::readFileData(
  const Handle(RWGltf_GltfLatePrimitiveArray)& theSourceGltfMesh,
  const RWGltf_GltfPrimArrayData&              theGltfData,
  const Handle(Poly_Triangulation)&            theDestMesh,
  const Handle(OSD_FileSystem)&                theFileSystem) const
{
  const Handle(OSD_FileSystem)& aFileSystem =
    !theFileSystem.IsNull() ? theFileSystem : OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aSharedStream =
    aFileSystem->OpenIStream(theGltfData.StreamUri,
                             std::ios::in | std::ios::binary,
                             theGltfData.StreamOffset);
  if (aSharedStream.get() == NULL)
  {
    reportError(TCollection_AsciiString("Buffer '") + theSourceGltfMesh->Id()
                + "' refers to invalid file '" + theGltfData.StreamUri + "'.");
    return false;
  }

  if (!readBuffer(theSourceGltfMesh,
                  theDestMesh,
                  *aSharedStream.get(),
                  theGltfData.Accessor,
                  theGltfData.Type))
  {
    return false;
  }
  return true;
}

// =======================================================================
// function : loadStreamData
// purpose  :
// =======================================================================
bool RWGltf_TriangulationReader::loadStreamData(
  const Handle(RWMesh_TriangulationSource)& theSourceMesh,
  const Handle(Poly_Triangulation)&         theDestMesh,
  bool                                      theToResetStream) const
{
  const Handle(RWGltf_GltfLatePrimitiveArray) aSourceGltfMesh =
    Handle(RWGltf_GltfLatePrimitiveArray)::DownCast(theSourceMesh);
  if (aSourceGltfMesh.IsNull()
      || aSourceGltfMesh->PrimitiveMode() == RWGltf_GltfPrimitiveMode_UNKNOWN)
  {
    return false;
  }
  bool wasLoaded = false;
  for (NCollection_Sequence<RWGltf_GltfPrimArrayData>::Iterator aDataIter(aSourceGltfMesh->Data());
       aDataIter.More();
       aDataIter.Next())
  {
    RWGltf_GltfPrimArrayData& aData = aDataIter.ChangeValue();
    if (aData.StreamData.IsNull())
    {
      continue;
    }
    if (!readStreamData(aSourceGltfMesh, aData, theDestMesh))
    {
      return false;
    }
    if (theToResetStream)
    {
      aData.StreamData.Nullify();
    }
    wasLoaded = true;
  }
  return wasLoaded;
}

// =======================================================================
// function : readDracoBuffer
// purpose  :
// =======================================================================
bool RWGltf_TriangulationReader::readDracoBuffer(
  const Handle(RWGltf_GltfLatePrimitiveArray)& theSourceGltfMesh,
  const RWGltf_GltfPrimArrayData&              theGltfData,
  const Handle(Poly_Triangulation)&            theDestMesh,
  const Handle(OSD_FileSystem)&                theFileSystem) const
{
  const TCollection_AsciiString& aName = theSourceGltfMesh->Id();
  const Handle(OSD_FileSystem)&  aFileSystem =
    !theFileSystem.IsNull() ? theFileSystem : OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::istream> aSharedStream =
    aFileSystem->OpenIStream(theGltfData.StreamUri,
                             std::ios::in | std::ios::binary,
                             theGltfData.StreamOffset);
  if (aSharedStream.get() == NULL)
  {
    reportError(TCollection_AsciiString("Buffer '") + aName + "' refers to invalid file '"
                + theGltfData.StreamUri + "'.");
    return false;
  }

#ifdef HAVE_DRACO
  std::vector<char> aReadData;
  aReadData.resize((size_t)theGltfData.StreamLength);
  aSharedStream->read(aReadData.data(), (std::streamsize)theGltfData.StreamLength);
  if (!aSharedStream->good())
  {
    reportError(TCollection_AsciiString("Buffer '") + aName
                + "' refers to file that cannot be read '" + theGltfData.StreamUri + "'.");
    return false;
  }

  draco::DecoderBuffer aDracoBuf;
  aDracoBuf.Init(aReadData.data(), aReadData.size());

  draco::Decoder                                aDracoDecoder;
  draco::StatusOr<std::unique_ptr<draco::Mesh>> aDracoStat =
    aDracoDecoder.DecodeMeshFromBuffer(&aDracoBuf);
  if (!aDracoStat.ok() || aDracoStat.value().get() == NULL)
  {
    reportError(TCollection_AsciiString("Buffer '") + aName
                + "' refers to Draco data that cannot be decoded '" + theGltfData.StreamUri + "'.");
    return false;
  }

  const Standard_Integer aNbNodes = (Standard_Integer)aDracoStat.value()->num_points();
  const Standard_Integer aNbTris  = (Standard_Integer)aDracoStat.value()->num_faces();
  if (aNbNodes < 0)
  {
    reportError(TCollection_AsciiString("Buffer '") + aName + "' defines an empty array.");
    return false;
  }
  if ((int64_t)aDracoStat.value()->num_points() > std::numeric_limits<Standard_Integer>::max())
  {
    reportError(TCollection_AsciiString("Buffer '") + aName + "' defines too big array.");
    return false;
  }
  if (!setNbPositionNodes(theDestMesh, aNbNodes))
  {
    return false;
  }

  if (aNbTris > 0 && !setNbTriangles(theDestMesh, aNbTris))
  {
    return false;
  }

  // copy vertex attributes
  for (int32_t anAttrIter = 0; anAttrIter < aDracoStat.value()->num_attributes(); ++anAttrIter)
  {
    const draco::PointAttribute*      anAttrib    = aDracoStat.value()->attribute(anAttrIter);
    const RWGltf_GltfArrayType        aWrapType   = arrayTypeFromDraco(anAttrib->attribute_type());
    const RWGltf_GltfAccessorLayout   aWrapLayout = layoutFromDraco(anAttrib->num_components());
    const RWGltf_GltfAccessorCompType aWrapCompType = compTypeFromDraco(anAttrib->data_type());
    switch (aWrapType)
    {
      case RWGltf_GltfArrayType_Position: {
        if (aWrapCompType != RWGltf_GltfAccessorCompType_Float32
            || aWrapLayout != RWGltf_GltfAccessorLayout_Vec3)
        {
          reportError(TCollection_AsciiString("Buffer '") + aName
                      + "' has unsupported position data type.");
          return false;
        }

        for (Standard_Integer aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          const Graphic3d_Vec3* aVec3 = reinterpret_cast<const Graphic3d_Vec3*>(
            anAttrib->GetAddressOfMappedIndex(draco::PointIndex(aVertIter)));
          if (aVec3 == NULL)
          {
            reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
            return false;
          }

          gp_Pnt anXYZ(aVec3->x(), aVec3->y(), aVec3->z());
          myCoordSysConverter.TransformPosition(anXYZ.ChangeCoord());
          setNodePosition(theDestMesh, THE_LOWER_NODE_INDEX + aVertIter, anXYZ);
        }
        break;
      }
      case RWGltf_GltfArrayType_Normal: {
        if (aWrapCompType != RWGltf_GltfAccessorCompType_Float32
            || aWrapLayout != RWGltf_GltfAccessorLayout_Vec3)
        {
          Message::SendTrace(TCollection_AsciiString()
                             + "Vertex normals in unsupported format have been skipped while "
                               "reading glTF triangulation '"
                             + aName + "'");
          break;
        }

        if (!setNbNormalNodes(theDestMesh, aNbNodes))
        {
          return false;
        }

        for (Standard_Integer aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          const Graphic3d_Vec3* aVec3 = reinterpret_cast<const Graphic3d_Vec3*>(
            anAttrib->GetAddressOfMappedIndex(draco::PointIndex(aVertIter)));
          if (aVec3 == NULL)
          {
            reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
            return false;
          }
          if (aVec3->SquareModulus() >= THE_NORMAL_PREC2)
          {
            Graphic3d_Vec3 aVec3Copy = *aVec3;
            myCoordSysConverter.TransformNormal(aVec3Copy);
            setNodeNormal(theDestMesh, THE_LOWER_NODE_INDEX + aVertIter, aVec3Copy);
          }
          else
          {
            setNodeNormal(theDestMesh, THE_LOWER_NODE_INDEX + aVertIter, gp_Vec3f(0.0, 0.0, 1.0));
          }
        }
        break;
      }
      case RWGltf_GltfArrayType_TCoord0: {
        if (aWrapCompType != RWGltf_GltfAccessorCompType_Float32
            || aWrapLayout != RWGltf_GltfAccessorLayout_Vec2)
        {
          Message::SendTrace(TCollection_AsciiString()
                             + "Vertex UV coordinates in unsupported format have been skipped "
                               "while reading glTF triangulation '"
                             + aName + "'");
          break;
        }

        if (!setNbUVNodes(theDestMesh, aNbNodes))
        {
          return false;
        }

        for (int aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          const Graphic3d_Vec2* aVec2 = reinterpret_cast<const Graphic3d_Vec2*>(
            anAttrib->GetAddressOfMappedIndex(draco::PointIndex(aVertIter)));
          if (aVec2 == NULL)
          {
            reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
            return false;
          }

          // Y should be flipped (relative to image layout used by OCCT)
          float aTexY = 1.0f - aVec2->y();
          setNodeUV(theDestMesh, THE_LOWER_NODE_INDEX + aVertIter, gp_Pnt2d(aVec2->x(), aTexY));
        }
        break;
      }
      default: {
        break;
      }
    }
  }

  // copy triangles
  Standard_Integer aLastTriIndex = 0;
  for (Standard_Integer aFaceIter = 0; aFaceIter < aNbTris; ++aFaceIter)
  {
    const draco::Mesh::Face& aFace = aDracoStat.value()->face(draco::FaceIndex(aFaceIter));
    Poly_Triangle            aVec3;
    aVec3.ChangeValue(1) = THE_LOWER_NODE_INDEX + aFace[0].value();
    aVec3.ChangeValue(2) = THE_LOWER_NODE_INDEX + aFace[1].value();
    aVec3.ChangeValue(3) = THE_LOWER_NODE_INDEX + aFace[2].value();
    const Standard_Integer wasSet =
      setTriangle(theDestMesh, THE_LOWER_TRI_INDEX + aLastTriIndex, aVec3);
    if (!wasSet)
    {
      reportError(TCollection_AsciiString("Buffer '") + aName + "' refers to invalid indices.");
    }
    if (wasSet > 0)
    {
      ++aLastTriIndex;
    }
  }

  const Standard_Integer aNbDegenerate = aNbTris - aLastTriIndex;
  if (aNbDegenerate > 0)
  {
    if (aNbDegenerate == aNbTris)
    {
      Message::SendWarning(TCollection_AsciiString("Buffer '") + aName
                           + "' has been skipped (all elements are degenerative in)");
      return false;
    }
    theSourceGltfMesh->ChangeDegeneratedTriNb() += aNbDegenerate;
    if (myLoadingStatistic == NULL && myToPrintDebugMessages)
    {
      Message::SendTrace(
        TCollection_AsciiString() + aNbDegenerate
        + " degenerate triangles have been skipped while reading glTF triangulation '" + aName
        + "'");
    }
    if (!setNbTriangles(theDestMesh, aLastTriIndex, true))
    {
      return false;
    }
  }
  return true;
#else
  (void)theDestMesh;
  reportError(TCollection_AsciiString("Buffer '") + aName
              + "' refers to unsupported compressed data.");
  return false;
#endif
}

// =======================================================================
// function : load
// purpose  :
// =======================================================================
bool RWGltf_TriangulationReader::load(const Handle(RWMesh_TriangulationSource)& theSourceMesh,
                                      const Handle(Poly_Triangulation)&         theDestMesh,
                                      const Handle(OSD_FileSystem)&             theFileSystem) const
{
  const Handle(RWGltf_GltfLatePrimitiveArray) aSourceGltfMesh =
    Handle(RWGltf_GltfLatePrimitiveArray)::DownCast(theSourceMesh);
  if (aSourceGltfMesh.IsNull()
      || aSourceGltfMesh->PrimitiveMode() == RWGltf_GltfPrimitiveMode_UNKNOWN)
  {
    return false;
  }

  bool hasCompressed = false;
  for (NCollection_Sequence<RWGltf_GltfPrimArrayData>::Iterator aDataIter(aSourceGltfMesh->Data());
       aDataIter.More();
       aDataIter.Next())
  {
    const RWGltf_GltfPrimArrayData& aData = aDataIter.Value();
    const TCollection_AsciiString&  aName = aSourceGltfMesh->Id();
    if (!aData.StreamData.IsNull())
    {
      Message::SendWarning(
        TCollection_AsciiString("Buffer '") + aName
        + "' contains stream data that cannot be loaded during deferred data loading.");
      continue;
    }
    else if (aData.StreamUri.IsEmpty())
    {
      reportError(TCollection_AsciiString("Buffer '") + aName + "' does not define uri.");
      return false;
    }

    if (aData.Accessor.IsCompressed)
    {
      if (hasCompressed)
      {
        // already decoded (compressed stream defines all attributes at once)
        continue;
      }
      if (!readDracoBuffer(aSourceGltfMesh, aData, theDestMesh, theFileSystem))
      {
        return false;
      }

      // keep decoding - there are might be uncompressed attributes in addition to compressed
      hasCompressed = true;
    }
    else if (!readFileData(aSourceGltfMesh, aData, theDestMesh, theFileSystem))
    {
      return false;
    }
  }
  return true;
}

// =======================================================================
// function : finalizeLoading
// purpose  :
// =======================================================================
bool RWGltf_TriangulationReader::finalizeLoading(
  const Handle(RWMesh_TriangulationSource)& theSourceMesh,
  const Handle(Poly_Triangulation)&         theDestMesh) const
{
  if (theDestMesh->NbNodes() < 1)
  {
    return false;
  }
  if (theDestMesh->NbTriangles() < 1)
  {
    const Handle(RWGltf_GltfLatePrimitiveArray) aSourceGltfMesh =
      Handle(RWGltf_GltfLatePrimitiveArray)::DownCast(theSourceMesh);
    if (!aSourceGltfMesh.IsNull()
        && aSourceGltfMesh->PrimitiveMode() == RWGltf_GltfPrimitiveMode_Triangles)
    {
      // reconstruct indexes
      const Standard_Integer aNbTris = theDestMesh->NbNodes() / 3;
      if (!setNbTriangles(theDestMesh, aNbTris))
      {
        return false;
      }
      for (Standard_Integer aTriIter = 0; aTriIter < aNbTris; ++aTriIter)
      {
        if (!setTriangle(theDestMesh,
                         THE_LOWER_TRI_INDEX + aTriIter,
                         Poly_Triangle(THE_LOWER_NODE_INDEX + aTriIter * 3 + 0,
                                       THE_LOWER_NODE_INDEX + aTriIter * 3 + 1,
                                       THE_LOWER_NODE_INDEX + aTriIter * 3 + 2)))
        {
          return false;
        }
      }
    }
  }
  return RWMesh_TriangulationReader::finalizeLoading(theSourceMesh, theDestMesh);
}

// =======================================================================
// function : readBuffer
// purpose  :
// =======================================================================
bool RWGltf_TriangulationReader::readBuffer(
  const Handle(RWGltf_GltfLatePrimitiveArray)& theSourceMesh,
  const Handle(Poly_Triangulation)&            theDestMesh,
  std::istream&                                theStream,
  const RWGltf_GltfAccessor&                   theAccessor,
  RWGltf_GltfArrayType                         theType) const

{
  const TCollection_AsciiString& aName = theSourceMesh->Id();
  if (theSourceMesh->PrimitiveMode() != RWGltf_GltfPrimitiveMode_Triangles)
  {
    Message::SendWarning(TCollection_AsciiString("Buffer '") + aName
                         + "' skipped unsupported primitive array");
    return true;
  }

  switch (theType)
  {
    case RWGltf_GltfArrayType_Indices: {
      if (theAccessor.Type != RWGltf_GltfAccessorLayout_Scalar)
      {
        break;
      }

      Poly_Triangle aVec3;
      if (theAccessor.ComponentType == RWGltf_GltfAccessorCompType_UInt16)
      {
        if ((theAccessor.Count / 3) > std::numeric_limits<Standard_Integer>::max())
        {
          reportError(TCollection_AsciiString("Buffer '") + aName + "' defines too big array.");
          return false;
        }

        const Standard_Integer aNbTris = (Standard_Integer)(theAccessor.Count / 3);
        if (!setNbTriangles(theDestMesh, aNbTris))
        {
          return false;
        }
        const size_t aStride =
          theAccessor.ByteStride != 0 ? theAccessor.ByteStride : sizeof(uint16_t);
        Standard_ReadBuffer aBuffer(theAccessor.Count * aStride, aStride);
        Standard_Integer    aLastTriIndex = 0;
        for (Standard_Integer aTriIter = 0; aTriIter < aNbTris; ++aTriIter)
        {
          if (const uint16_t* anIndex0 = aBuffer.ReadChunk<uint16_t>(theStream))
          {
            aVec3.ChangeValue(1) = THE_LOWER_NODE_INDEX + *anIndex0;
          }
          if (const uint16_t* anIndex1 = aBuffer.ReadChunk<uint16_t>(theStream))
          {
            aVec3.ChangeValue(2) = THE_LOWER_NODE_INDEX + *anIndex1;
          }
          if (const uint16_t* anIndex2 = aBuffer.ReadChunk<uint16_t>(theStream))
          {
            aVec3.ChangeValue(3) = THE_LOWER_NODE_INDEX + *anIndex2;
          }
          else
          {
            reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
            return false;
          }

          const Standard_Integer wasSet =
            setTriangle(theDestMesh, THE_LOWER_TRI_INDEX + aLastTriIndex, aVec3);
          if (!wasSet)
          {
            reportError(TCollection_AsciiString("Buffer '") + aName
                        + "' refers to invalid indices.");
          }
          if (wasSet > 0)
          {
            aLastTriIndex++;
          }
        }
        const Standard_Integer aNbDegenerate = aNbTris - aLastTriIndex;
        if (aNbDegenerate > 0)
        {
          if (aNbDegenerate == aNbTris)
          {
            Message::SendWarning(TCollection_AsciiString("Buffer '") + aName
                                 + "' has been skipped (all elements are degenerative in)");
            return false;
          }
          theSourceMesh->ChangeDegeneratedTriNb() += aNbDegenerate;
          if ((myLoadingStatistic == NULL) && myToPrintDebugMessages)
          {
            Message::SendTrace(
              TCollection_AsciiString() + aNbDegenerate
              + " degenerate triangles have been skipped while reading glTF triangulation '" + aName
              + "'");
          }
          if (!setNbTriangles(theDestMesh, aLastTriIndex, true))
          {
            return false;
          }
        }
      }
      else if (theAccessor.ComponentType == RWGltf_GltfAccessorCompType_UInt32)
      {
        if ((theAccessor.Count / 3) > std::numeric_limits<Standard_Integer>::max())
        {
          reportError(TCollection_AsciiString("Buffer '") + aName + "' defines too big array.");
          return false;
        }

        const int aNbTris = (Standard_Integer)(theAccessor.Count / 3);
        if (!setNbTriangles(theDestMesh, aNbTris))
        {
          return false;
        }
        const size_t aStride =
          theAccessor.ByteStride != 0 ? theAccessor.ByteStride : sizeof(uint32_t);
        Standard_ReadBuffer aBuffer(theAccessor.Count * aStride, aStride);
        Standard_Integer    aLastTriIndex = 0;
        for (Standard_Integer aTriIter = 0; aTriIter < aNbTris; ++aTriIter)
        {
          if (const uint32_t* anIndex0 = aBuffer.ReadChunk<uint32_t>(theStream))
          {
            aVec3.ChangeValue(1) = THE_LOWER_NODE_INDEX + *anIndex0;
          }
          if (const uint32_t* anIndex1 = aBuffer.ReadChunk<uint32_t>(theStream))
          {
            aVec3.ChangeValue(2) = THE_LOWER_NODE_INDEX + *anIndex1;
          }
          if (const uint32_t* anIndex2 = aBuffer.ReadChunk<uint32_t>(theStream))
          {
            aVec3.ChangeValue(3) = THE_LOWER_NODE_INDEX + *anIndex2;
          }
          else
          {
            reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
            return false;
          }

          const Standard_Integer wasSet =
            setTriangle(theDestMesh, THE_LOWER_TRI_INDEX + aLastTriIndex, aVec3);
          if (!wasSet)
          {
            reportError(TCollection_AsciiString("Buffer '") + aName
                        + "' refers to invalid indices.");
          }
          if (wasSet > 0)
          {
            aLastTriIndex++;
          }
        }
        const Standard_Integer aNbDegenerate = aNbTris - aLastTriIndex;
        if (aNbDegenerate > 0)
        {
          if (aNbDegenerate == aNbTris)
          {
            Message::SendWarning(TCollection_AsciiString("Buffer '") + aName
                                 + "' has been skipped (all elements are degenerative in)");
            return false;
          }
          theSourceMesh->ChangeDegeneratedTriNb() += aNbDegenerate;
          if (myLoadingStatistic == NULL && myToPrintDebugMessages)
          {
            Message::SendTrace(
              TCollection_AsciiString() + aNbDegenerate
              + " degenerate triangles have been skipped while reading glTF triangulation '" + aName
              + "'");
          }
          if (!setNbTriangles(theDestMesh, aLastTriIndex, true))
          {
            return false;
          }
        }
      }
      else if (theAccessor.ComponentType == RWGltf_GltfAccessorCompType_UInt8)
      {
        if ((theAccessor.Count / 3) > std::numeric_limits<Standard_Integer>::max())
        {
          reportError(TCollection_AsciiString("Buffer '") + aName + "' defines too big array.");
          return false;
        }

        const Standard_Integer aNbTris = (Standard_Integer)(theAccessor.Count / 3);
        if (!setNbTriangles(theDestMesh, aNbTris))
        {
          return false;
        }
        const size_t aStride =
          theAccessor.ByteStride != 0 ? theAccessor.ByteStride : sizeof(uint8_t);
        Standard_ReadBuffer aBuffer(theAccessor.Count * aStride, aStride);
        Standard_Integer    aLastTriIndex = 0;
        for (Standard_Integer aTriIter = 0; aTriIter < aNbTris; ++aTriIter)
        {
          if (const uint8_t* anIndex0 = aBuffer.ReadChunk<uint8_t>(theStream))
          {
            aVec3.ChangeValue(1) = THE_LOWER_NODE_INDEX + (Standard_Integer)*anIndex0;
          }
          if (const uint8_t* anIndex1 = aBuffer.ReadChunk<uint8_t>(theStream))
          {
            aVec3.ChangeValue(2) = THE_LOWER_NODE_INDEX + (Standard_Integer)*anIndex1;
          }
          if (const uint8_t* anIndex2 = aBuffer.ReadChunk<uint8_t>(theStream))
          {
            aVec3.ChangeValue(3) = THE_LOWER_NODE_INDEX + (Standard_Integer)*anIndex2;
          }
          else
          {
            reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
            return false;
          }

          const Standard_Integer wasSet =
            setTriangle(theDestMesh, THE_LOWER_TRI_INDEX + aLastTriIndex, aVec3);
          if (!wasSet)
          {
            reportError(TCollection_AsciiString("Buffer '") + aName
                        + "' refers to invalid indices.");
          }
          if (wasSet > 0)
          {
            aLastTriIndex++;
          }
        }
        const Standard_Integer aNbDegenerate = aNbTris - aLastTriIndex;
        if (aNbDegenerate > 0)
        {
          if (aNbDegenerate == aNbTris)
          {
            Message::SendWarning(TCollection_AsciiString("Buffer '") + aName
                                 + "' has been skipped (all elements are degenerative in)");
            return false;
          }
          theSourceMesh->ChangeDegeneratedTriNb() += aNbDegenerate;
          if (myLoadingStatistic == NULL && myToPrintDebugMessages)
          {
            Message::SendTrace(
              TCollection_AsciiString() + aNbDegenerate
              + " degenerate triangles have been skipped while reading glTF triangulation '" + aName
              + "'");
          }
          if (!setNbTriangles(theDestMesh, aLastTriIndex, true))
          {
            return false;
          }
        }
      }
      else
      {
        break;
      }

      break;
    }
    case RWGltf_GltfArrayType_Position: {
      if (theAccessor.ComponentType != RWGltf_GltfAccessorCompType_Float32
          || theAccessor.Type != RWGltf_GltfAccessorLayout_Vec3)
      {
        break;
      }
      else if (theAccessor.Count > std::numeric_limits<Standard_Integer>::max())
      {
        reportError(TCollection_AsciiString("Buffer '") + aName + "' defines too big array.");
        return false;
      }

      const size_t aStride =
        theAccessor.ByteStride != 0 ? theAccessor.ByteStride : sizeof(Graphic3d_Vec3);
      const Standard_Integer aNbNodes = (Standard_Integer)theAccessor.Count;
      if (!setNbPositionNodes(theDestMesh, aNbNodes))
      {
        return false;
      }

      Standard_ReadBuffer aBuffer(theAccessor.Count * aStride - (aStride - sizeof(Graphic3d_Vec3)),
                                  aStride,
                                  true);
      if (!myCoordSysConverter.IsEmpty())
      {
        for (Standard_Integer aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          const Graphic3d_Vec3* aVec3 = aBuffer.ReadChunk<Graphic3d_Vec3>(theStream);
          if (aVec3 == NULL)
          {
            reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
            return false;
          }

          gp_Pnt anXYZ(aVec3->x(), aVec3->y(), aVec3->z());
          myCoordSysConverter.TransformPosition(anXYZ.ChangeCoord());
          setNodePosition(theDestMesh, THE_LOWER_NODE_INDEX + aVertIter, anXYZ);
        }
      }
      else
      {
        for (Standard_Integer aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          const Graphic3d_Vec3* aVec3 = aBuffer.ReadChunk<Graphic3d_Vec3>(theStream);
          if (aVec3 == NULL)
          {
            reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
            return false;
          }
          setNodePosition(theDestMesh,
                          THE_LOWER_NODE_INDEX + aVertIter,
                          gp_Pnt(aVec3->x(), aVec3->y(), aVec3->z()));
        }
      }
      break;
    }
    case RWGltf_GltfArrayType_Normal: {
      if (theAccessor.ComponentType != RWGltf_GltfAccessorCompType_Float32
          || theAccessor.Type != RWGltf_GltfAccessorLayout_Vec3)
      {
        break;
      }
      else if (theAccessor.Count > std::numeric_limits<Standard_Integer>::max())
      {
        reportError(TCollection_AsciiString("Buffer '") + aName + "' defines too big array.");
        return false;
      }

      const size_t aStride =
        theAccessor.ByteStride != 0 ? theAccessor.ByteStride : sizeof(Graphic3d_Vec3);
      const Standard_Integer aNbNodes = (Standard_Integer)theAccessor.Count;
      if (!setNbNormalNodes(theDestMesh, aNbNodes))
      {
        return false;
      }
      Standard_ReadBuffer aBuffer(theAccessor.Count * aStride - (aStride - sizeof(Graphic3d_Vec3)),
                                  aStride,
                                  true);
      if (!myCoordSysConverter.IsEmpty())
      {
        for (Standard_Integer aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          Graphic3d_Vec3* aVec3 = aBuffer.ReadChunk<Graphic3d_Vec3>(theStream);
          if (aVec3 == NULL)
          {
            reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
            return false;
          }
          if (aVec3->SquareModulus() >= THE_NORMAL_PREC2)
          {
            myCoordSysConverter.TransformNormal(*aVec3);
            setNodeNormal(theDestMesh, THE_LOWER_NODE_INDEX + aVertIter, *aVec3);
          }
          else
          {
            setNodeNormal(theDestMesh, THE_LOWER_NODE_INDEX + aVertIter, gp_Vec3f(0.0, 0.0, 1.0));
          }
        }
      }
      else
      {
        for (Standard_Integer aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          const Graphic3d_Vec3* aVec3 = aBuffer.ReadChunk<Graphic3d_Vec3>(theStream);
          if (aVec3 == NULL)
          {
            reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
            return false;
          }
          if (aVec3->SquareModulus() >= THE_NORMAL_PREC2)
          {
            setNodeNormal(theDestMesh, THE_LOWER_NODE_INDEX + aVertIter, *aVec3);
          }
          else
          {
            setNodeNormal(theDestMesh, THE_LOWER_NODE_INDEX + aVertIter, gp_Vec3f(0.0, 0.0, 1.0));
          }
        }
      }
      break;
    }
    case RWGltf_GltfArrayType_TCoord0: {
      if (theAccessor.ComponentType != RWGltf_GltfAccessorCompType_Float32
          || theAccessor.Type != RWGltf_GltfAccessorLayout_Vec2)
      {
        break;
      }
      else if (theAccessor.Count > std::numeric_limits<Standard_Integer>::max())
      {
        reportError(TCollection_AsciiString("Buffer '") + aName + "' defines too big array.");
        return false;
      }

      const size_t aStride =
        theAccessor.ByteStride != 0 ? theAccessor.ByteStride : sizeof(Graphic3d_Vec2);
      const Standard_Integer aNbNodes = (Standard_Integer)theAccessor.Count;
      if (!setNbUVNodes(theDestMesh, aNbNodes))
      {
        return false;
      }

      Standard_ReadBuffer aBuffer(theAccessor.Count * aStride - (aStride - sizeof(Graphic3d_Vec2)),
                                  aStride,
                                  true);
      for (int aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
      {
        Graphic3d_Vec2* aVec2 = aBuffer.ReadChunk<Graphic3d_Vec2>(theStream);
        if (aVec2 == NULL)
        {
          reportError(TCollection_AsciiString("Buffer '") + aName + "' reading error.");
          return false;
        }

        // Y should be flipped (relative to image layout used by OCCT)
        aVec2->y() = 1.0f - aVec2->y();
        setNodeUV(theDestMesh, THE_LOWER_NODE_INDEX + aVertIter, gp_Pnt2d(aVec2->x(), aVec2->y()));
      }
      break;
    }
    case RWGltf_GltfArrayType_Color:
    case RWGltf_GltfArrayType_TCoord1:
    case RWGltf_GltfArrayType_Joint:
    case RWGltf_GltfArrayType_Weight: {
      return true;
    }
    case RWGltf_GltfArrayType_UNKNOWN: {
      return false;
    }
  }
  return true;
}
