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

#include <RWMesh_CoordinateSystemConverter.hxx>
#include <Standard_ReadBuffer.hxx>

#include <BRep_Builder.hxx>
#include <Graphic3d_Vec.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>

namespace
{
  static const Standard_Integer   THE_LOWER_TRI_INDEX  = 1;
  static const Standard_Integer   THE_LOWER_NODE_INDEX = 1;
  static const Standard_ShortReal THE_NORMAL_PREC2 = 0.001f;
}

IMPLEMENT_STANDARD_RTTIEXT(RWGltf_TriangulationReader, RWGltf_PrimitiveArrayReader)

// =======================================================================
// function : RWGltf_TriangulationReader
// purpose  :
// =======================================================================
RWGltf_TriangulationReader::RWGltf_TriangulationReader()
{
  //
}

// =======================================================================
// function : reset
// purpose  :
// =======================================================================
void RWGltf_TriangulationReader::reset()
{
  myTriangulation = new Poly_Triangulation (1, 1, true);
  {
    TColgp_Array1OfPnt anEmpty;
    myTriangulation->ChangeNodes().Move (anEmpty);
  }
  {
    TColgp_Array1OfPnt2d anEmpty;
    myTriangulation->ChangeUVNodes().Move (anEmpty);
  }
  {
    Poly_Array1OfTriangle anEmpty;
    myTriangulation->ChangeTriangles().Move (anEmpty);
  }
}

// =======================================================================
// function : result
// purpose  :
// =======================================================================
Handle(Poly_Triangulation) RWGltf_TriangulationReader::result()
{
  if (myTriangulation->NbNodes() < 1)
  {
    return Handle(Poly_Triangulation)();
  }
  if (myTriangulation->UVNodes().Size() != myTriangulation->NbNodes())
  {
    myTriangulation->RemoveUVNodes();
  }

  if (myTriangulation->NbTriangles() < 1)
  {
    // reconstruct indexes
    const Standard_Integer aNbTris = myTriangulation->NbNodes() / 3;
    if (!setNbTriangles (aNbTris))
    {
      return Handle(Poly_Triangulation)();
    }

    for (Standard_Integer aTriIter = 0; aTriIter < aNbTris; ++aTriIter)
    {
      setTriangle (THE_LOWER_TRI_INDEX + aTriIter,
                   Poly_Triangle (THE_LOWER_NODE_INDEX + aTriIter * 3 + 0,
                                  THE_LOWER_NODE_INDEX + aTriIter * 3 + 1,
                                  THE_LOWER_NODE_INDEX + aTriIter * 3 + 2));
    }
  }

  return myTriangulation;
}

// =======================================================================
// function : readBuffer
// purpose  :
// =======================================================================
bool RWGltf_TriangulationReader::readBuffer (std::istream& theStream,
                                             const TCollection_AsciiString& theName,
                                             const RWGltf_GltfAccessor& theAccessor,
                                             RWGltf_GltfArrayType theType,
                                             RWGltf_GltfPrimitiveMode theMode)
{
  if (theMode != RWGltf_GltfPrimitiveMode_Triangles)
  {
    Message::SendWarning (TCollection_AsciiString("Buffer '") + theName + "' skipped unsupported primitive array");
    return true;
  }

  switch (theType)
  {
    case RWGltf_GltfArrayType_Indices:
    {
      if (theAccessor.Type != RWGltf_GltfAccessorLayout_Scalar)
      {
        break;
      }

      Poly_Triangle aVec3;
      if (theAccessor.ComponentType == RWGltf_GltfAccessorCompType_UInt16)
      {
        if ((theAccessor.Count / 3) > std::numeric_limits<Standard_Integer>::max())
        {
          reportError (TCollection_AsciiString ("Buffer '") + theName + "' defines too big array.");
          return false;
        }

        const Standard_Integer aNbTris = (Standard_Integer )(theAccessor.Count / 3);
        if (!setNbTriangles (aNbTris))
        {
          return false;
        }
        const size_t aStride = theAccessor.ByteStride != 0
                             ? theAccessor.ByteStride
                             : sizeof(uint16_t);
        Standard_ReadBuffer aBuffer (theAccessor.Count * aStride, aStride);
        for (Standard_Integer aTriIter = 0; aTriIter < aNbTris; ++aTriIter)
        {
          if (const uint16_t* anIndex0 = aBuffer.ReadChunk<uint16_t> (theStream))
          {
            aVec3.ChangeValue (1) = THE_LOWER_NODE_INDEX + *anIndex0;
          }
          if (const uint16_t* anIndex1 = aBuffer.ReadChunk<uint16_t> (theStream))
          {
            aVec3.ChangeValue (2) = THE_LOWER_NODE_INDEX + *anIndex1;
          }
          if (const uint16_t* anIndex2 = aBuffer.ReadChunk<uint16_t> (theStream))
          {
            aVec3.ChangeValue (3) = THE_LOWER_NODE_INDEX + *anIndex2;
          }
          else
          {
            reportError (TCollection_AsciiString ("Buffer '") + theName + "' reading error.");
            return false;
          }

          if (!setTriangle (THE_LOWER_TRI_INDEX + aTriIter, aVec3))
          {
            reportError (TCollection_AsciiString ("Buffer '") + theName + "' refers to invalid indices.");
          }
        }
      }
      else if (theAccessor.ComponentType == RWGltf_GltfAccessorCompType_UInt32)
      {
        if ((theAccessor.Count / 3) > std::numeric_limits<Standard_Integer>::max())
        {
          reportError (TCollection_AsciiString ("Buffer '") + theName + "' defines too big array.");
          return false;
        }

        const int aNbTris = (Standard_Integer )(theAccessor.Count / 3);
        if (!setNbTriangles (aNbTris))
        {
          return false;
        }
        const size_t aStride = theAccessor.ByteStride != 0
                             ? theAccessor.ByteStride
                             : sizeof(uint32_t);
        Standard_ReadBuffer aBuffer (theAccessor.Count * aStride, aStride);
        for (Standard_Integer aTriIter = 0; aTriIter < aNbTris; ++aTriIter)
        {
          if (const uint32_t* anIndex0 = aBuffer.ReadChunk<uint32_t> (theStream))
          {
            aVec3.ChangeValue (1) = THE_LOWER_NODE_INDEX + *anIndex0;
          }
          if (const uint32_t* anIndex1 = aBuffer.ReadChunk<uint32_t> (theStream))
          {
            aVec3.ChangeValue (2) = THE_LOWER_NODE_INDEX + *anIndex1;
          }
          if (const uint32_t* anIndex2 = aBuffer.ReadChunk<uint32_t> (theStream))
          {
            aVec3.ChangeValue (3) = THE_LOWER_NODE_INDEX + *anIndex2;
          }
          else
          {
            reportError (TCollection_AsciiString ("Buffer '") + theName + "' reading error.");
            return false;
          }

          if (!setTriangle (THE_LOWER_TRI_INDEX + aTriIter, aVec3))
          {
            reportError (TCollection_AsciiString ("Buffer '") + theName + "' refers to invalid indices.");
          }
        }
      }
      else if (theAccessor.ComponentType == RWGltf_GltfAccessorCompType_UInt8)
      {
        if ((theAccessor.Count / 3) > std::numeric_limits<Standard_Integer>::max())
        {
          reportError (TCollection_AsciiString ("Buffer '") + theName + "' defines too big array.");
          return false;
        }

        const Standard_Integer aNbTris = (Standard_Integer )(theAccessor.Count / 3);
        if (!setNbTriangles (aNbTris))
        {
          return false;
        }
        const size_t aStride = theAccessor.ByteStride != 0
                             ? theAccessor.ByteStride
                             : sizeof(uint8_t);
        Standard_ReadBuffer aBuffer (theAccessor.Count * aStride, aStride);
        for (Standard_Integer aTriIter = 0; aTriIter < aNbTris; ++aTriIter)
        {
          if (const uint8_t* anIndex0 = aBuffer.ReadChunk<uint8_t> (theStream))
          {
            aVec3.ChangeValue (1) = THE_LOWER_NODE_INDEX + (Standard_Integer )*anIndex0;
          }
          if (const uint8_t* anIndex1 = aBuffer.ReadChunk<uint8_t> (theStream))
          {
            aVec3.ChangeValue (2) = THE_LOWER_NODE_INDEX + (Standard_Integer )*anIndex1;
          }
          if (const uint8_t* anIndex2 = aBuffer.ReadChunk<uint8_t> (theStream))
          {
            aVec3.ChangeValue (3) = THE_LOWER_NODE_INDEX + (Standard_Integer )*anIndex2;
          }
          else
          {
            reportError (TCollection_AsciiString ("Buffer '") + theName + "' reading error.");
            return false;
          }

          if (!setTriangle (THE_LOWER_TRI_INDEX + aTriIter, aVec3))
          {
            reportError (TCollection_AsciiString ("Buffer '") + theName + "' refers to invalid indices.");
          }
        }
      }
      else
      {
        break;
      }

      break;
    }
    case RWGltf_GltfArrayType_Position:
    {
      if (theAccessor.ComponentType != RWGltf_GltfAccessorCompType_Float32
       || theAccessor.Type != RWGltf_GltfAccessorLayout_Vec3)
      {
        break;
      }
      else if (theAccessor.Count > std::numeric_limits<Standard_Integer>::max())
      {
        reportError (TCollection_AsciiString ("Buffer '") + theName + "' defines too big array.");
        return false;
      }

      const size_t aStride = theAccessor.ByteStride != 0
                           ? theAccessor.ByteStride
                           : sizeof(Graphic3d_Vec3);
      const Standard_Integer aNbNodes = (Standard_Integer )theAccessor.Count;
      if (!setNbPositionNodes (aNbNodes))
      {
        return false;
      }

      Standard_ReadBuffer aBuffer (theAccessor.Count * aStride - (aStride - sizeof(Graphic3d_Vec3)), aStride, true);
      if (!myCoordSysConverter.IsEmpty())
      {
        for (Standard_Integer aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          const Graphic3d_Vec3* aVec3 = aBuffer.ReadChunk<Graphic3d_Vec3> (theStream);
          if (aVec3 == NULL)
          {
            reportError (TCollection_AsciiString ("Buffer '") + theName + "' reading error.");
            return false;
          }

          gp_Pnt anXYZ (aVec3->x(), aVec3->y(), aVec3->z());
          myCoordSysConverter.TransformPosition (anXYZ.ChangeCoord());
          setNodePosition (THE_LOWER_NODE_INDEX + aVertIter, anXYZ);
        }
      }
      else
      {
        for (Standard_Integer aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          const Graphic3d_Vec3* aVec3 = aBuffer.ReadChunk<Graphic3d_Vec3> (theStream);
          if (aVec3 == NULL)
          {
            reportError (TCollection_AsciiString ("Buffer '") + theName + "' reading error.");
            return false;
          }
          setNodePosition (THE_LOWER_NODE_INDEX + aVertIter, gp_Pnt (aVec3->x(), aVec3->y(), aVec3->z()));
        }
      }
      break;
    }
    case RWGltf_GltfArrayType_Normal:
    {
      if (theAccessor.ComponentType != RWGltf_GltfAccessorCompType_Float32
       || theAccessor.Type != RWGltf_GltfAccessorLayout_Vec3)
      {
        break;
      }
      else if (theAccessor.Count > std::numeric_limits<Standard_Integer>::max())
      {
        reportError (TCollection_AsciiString ("Buffer '") + theName + "' defines too big array.");
        return false;
      }

      const size_t aStride = theAccessor.ByteStride != 0
                           ? theAccessor.ByteStride
                           : sizeof(Graphic3d_Vec3);
      const Standard_Integer aNbNodes = (Standard_Integer )theAccessor.Count;
      if (!setNbNormalNodes (aNbNodes))
      {
        return false;
      }
      Standard_ReadBuffer aBuffer (theAccessor.Count * aStride - (aStride - sizeof(Graphic3d_Vec3)), aStride, true);
      if (!myCoordSysConverter.IsEmpty())
      {
        for (Standard_Integer aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          Graphic3d_Vec3* aVec3 = aBuffer.ReadChunk<Graphic3d_Vec3> (theStream);
          if (aVec3 == NULL)
          {
            reportError (TCollection_AsciiString ("Buffer '") + theName + "' reading error.");
            return false;
          }
          if (aVec3->SquareModulus() >= THE_NORMAL_PREC2)
          {
            myCoordSysConverter.TransformNormal (*aVec3);
            setNodeNormal (THE_LOWER_NODE_INDEX + aVertIter, gp_Dir (aVec3->x(), aVec3->y(), aVec3->z()));
          }
          else
          {
            setNodeNormal (THE_LOWER_NODE_INDEX + aVertIter, gp_Dir (0.0, 0.0, 1.0));
          }
        }
      }
      else
      {
        for (Standard_Integer aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
        {
          const Graphic3d_Vec3* aVec3 = aBuffer.ReadChunk<Graphic3d_Vec3> (theStream);
          if (aVec3 == NULL)
          {
            reportError (TCollection_AsciiString ("Buffer '") + theName + "' reading error.");
            return false;
          }
          if (aVec3->SquareModulus() >= THE_NORMAL_PREC2)
          {
            setNodeNormal (THE_LOWER_NODE_INDEX + aVertIter, gp_Dir (aVec3->x(), aVec3->y(), aVec3->z()));
          }
          else
          {
            setNodeNormal (THE_LOWER_NODE_INDEX + aVertIter, gp_Dir (0.0, 0.0, 1.0));
          }
        }
      }
      break;
    }
    case RWGltf_GltfArrayType_TCoord0:
    {
      if (theAccessor.ComponentType != RWGltf_GltfAccessorCompType_Float32
       || theAccessor.Type != RWGltf_GltfAccessorLayout_Vec2)
      {
        break;
      }
      else if (theAccessor.Count > std::numeric_limits<Standard_Integer>::max())
      {
        reportError (TCollection_AsciiString ("Buffer '") + theName + "' defines too big array.");
        return false;
      }

      const size_t aStride = theAccessor.ByteStride != 0
                           ? theAccessor.ByteStride
                           : sizeof(Graphic3d_Vec2);
      const Standard_Integer aNbNodes = (Standard_Integer )theAccessor.Count;
      if (!setNbUVNodes (aNbNodes))
      {
        return false;
      }

      Standard_ReadBuffer aBuffer (theAccessor.Count * aStride - (aStride - sizeof(Graphic3d_Vec2)), aStride, true);
      for (int aVertIter = 0; aVertIter < aNbNodes; ++aVertIter)
      {
        Graphic3d_Vec2* aVec2 = aBuffer.ReadChunk<Graphic3d_Vec2> (theStream);
        if (aVec2 == NULL)
        {
          reportError (TCollection_AsciiString ("Buffer '") + theName + "' reading error.");
          return false;
        }

        // Y should be flipped (relative to image layout used by OCCT)
        aVec2->y() = 1.0f - aVec2->y();
        setNodeUV (THE_LOWER_NODE_INDEX + aVertIter, gp_Pnt2d (aVec2->x(), aVec2->y()));
      }
      break;
    }
    case RWGltf_GltfArrayType_Color:
    case RWGltf_GltfArrayType_TCoord1:
    case RWGltf_GltfArrayType_Joint:
    case RWGltf_GltfArrayType_Weight:
    {
      return true;
    }
    case RWGltf_GltfArrayType_UNKNOWN:
    {
      return false;
    }
  }
  return true;
}
