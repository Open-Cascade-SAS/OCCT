// Created on: 2016-02-20
// Created by: Kirill Gavrilov
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Select3D_SensitivePrimitiveArray.hxx>

#include <NCollection_AlignedAllocator.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitivePrimitiveArray, Select3D_SensitiveSet)

namespace
{

  //! Auxiliary converter.
  static inline gp_Pnt vecToPnt (const Graphic3d_Vec3& theVec)
  {
    return gp_Pnt (theVec.x(), theVec.y(), theVec.z());
  }

  //! Auxiliary converter.
  static inline gp_Pnt vecToPnt (const Graphic3d_Vec2& theVec)
  {
    return gp_Pnt (theVec.x(), theVec.y(), 0.0);
  }


  //! Auxiliary function to find shared node between two triangles.
  static inline bool hasSharedNode (const Standard_Integer* theTri1,
                                    const Standard_Integer* theTri2)
  {
    return theTri1[0] == theTri2[0]
        || theTri1[1] == theTri2[0]
        || theTri1[2] == theTri2[0]
        || theTri1[0] == theTri2[1]
        || theTri1[1] == theTri2[1]
        || theTri1[2] == theTri2[1]
        || theTri1[0] == theTri2[2]
        || theTri1[1] == theTri2[2]
        || theTri1[2] == theTri2[2];
  }

  //! Fill in the triangle nodes indices.
  static inline void getTriIndices (const Handle(Graphic3d_IndexBuffer)& theIndices,
                                    const Standard_Integer theIndexOffset,
                                    Standard_Integer* theNodes)
  {
    if (!theIndices.IsNull())
    {
      theNodes[0] = theIndices->Index (theIndexOffset + 0);
      theNodes[1] = theIndices->Index (theIndexOffset + 1);
      theNodes[2] = theIndices->Index (theIndexOffset + 2);
    }
    else
    {
      theNodes[0] = theIndexOffset + 0;
      theNodes[1] = theIndexOffset + 1;
      theNodes[2] = theIndexOffset + 2;
    }
  }

}

// =======================================================================
// function : Select3D_SensitivePrimitiveArray
// purpose  :
// =======================================================================
Select3D_SensitivePrimitiveArray::Select3D_SensitivePrimitiveArray (const Handle(SelectBasics_EntityOwner)& theOwnerId)
: Select3D_SensitiveSet (theOwnerId),
  myPrimType (Graphic3d_TOPA_UNDEFINED),
  myIndexLower (0),
  myIndexUpper (0),
  myPosOffset (Standard_Size(-1)),
  myPatchSizeMax (1),
  myPatchDistance (ShortRealLast()),
  myIs3d (false),
  myBvhIndices (new NCollection_AlignedAllocator(16)),
  myMinDepthElem (RealLast()),
  myMinDepthNode (RealLast()),
  myMinDepthEdge (RealLast()),
  myDetectedElem (-1),
  myDetectedNode (-1),
  myDetectedEdgeNode1 (-1),
  myDetectedEdgeNode2 (-1),
  myToDetectElem (true),
  myToDetectNode (false),
  myToDetectEdge (false)
{
  //
}

// =======================================================================
// function : InitTriangulation
// purpose  :
// =======================================================================
bool Select3D_SensitivePrimitiveArray::InitTriangulation (const Handle(Graphic3d_Buffer)&      theVerts,
                                                          const Handle(Graphic3d_IndexBuffer)& theIndices,
                                                          const TopLoc_Location&               theInitLoc,
                                                          const Standard_Integer               theIndexLower,
                                                          const Standard_Integer               theIndexUpper,
                                                          const bool                           theToEvalMinMax)
{
  MarkDirty();
  myPrimType = Graphic3d_TOPA_TRIANGLES;
  myBndBox.Clear();
  myVerts.Nullify();
  myIndices.Nullify();
  myIndexLower = 0;
  myIndexUpper = 0;
  myPosOffset = Standard_Size(-1);
  myBvhIndices.release();
  myIs3d = false;
  myInitLocation = theInitLoc;
  if (theVerts.IsNull()
   || theVerts->NbElements == 0)
  {
    return false;
  }

  for (Standard_Integer anAttribIter = 0; anAttribIter < theVerts->NbAttributes; ++anAttribIter)
  {
    const Graphic3d_Attribute& anAttrib = theVerts->Attribute (anAttribIter);
    if (anAttrib.Id == Graphic3d_TOA_POS)
    {
      if (anAttrib.DataType == Graphic3d_TOD_VEC3
       || anAttrib.DataType == Graphic3d_TOD_VEC4)
      {
        myIs3d = true;
      }
      else if (anAttrib.DataType != Graphic3d_TOD_VEC2)
      {
        return false;
      }

      myPosOffset = theVerts->AttributeOffset (anAttribIter);
      break;
    }
  }
  if (myPosOffset == Standard_Size(-1))
  {
    return false;
  }

  if (!theIndices.IsNull())
  {
    if (theIndexLower < 0
     || theIndexUpper >= theIndices->NbElements
     || theIndices->NbElements == 0)
    {
      return false;
    }
  }
  else
  {
    if (theIndexLower < 0
     || theIndexUpper >= theVerts->NbElements)
    {
      return false;
    }
  }

  Standard_Integer aTriFrom = theIndexLower / 3;
  Standard_Integer aNbTris  = (theIndexUpper - theIndexLower + 1) / 3;
  if (aNbTris < 1)
  {
    return false;
  }
  if (!myBvhIndices.Init (aNbTris, myPatchSizeMax > 1))
  {
    return false;
  }

  myVerts      = theVerts;
  myIndices    = theIndices;
  myIndexLower = theIndexLower;
  myIndexUpper = theIndexUpper;
  myInvInitLocation = myInitLocation.Transformation().Inverted();

  Graphic3d_Vec3 aCenter (0.0f, 0.0f, 0.0f);
  Standard_Integer  aTriNodes1[3] = { -1, -1, -1 };
  Standard_Integer  aTriNodes2[3] = { -1, -1, -1 };
  Standard_Integer* aTriNodesPrev = aTriNodes1;
  Standard_Integer* aTriNodes     = aTriNodes2;
  Standard_Integer  aPatchFrom    = 0;
  Standard_Integer  aPatchSize    = 0;
  if (myBvhIndices.HasPatches())
  {
    myBvhIndices.NbElements = 0;
  }
  for (Standard_Integer aTriIter = 0; aTriIter < aNbTris; ++aTriIter)
  {
    const Standard_Integer anIndexOffset = (aTriFrom + aTriIter) * 3;
    getTriIndices (myIndices, anIndexOffset, aTriNodes);
    if (myIs3d)
    {
      const Graphic3d_Vec3& aNode1 = getPosVec3 (aTriNodes[0]);
      const Graphic3d_Vec3& aNode2 = getPosVec3 (aTriNodes[1]);
      const Graphic3d_Vec3& aNode3 = getPosVec3 (aTriNodes[2]);
      aCenter += (aNode1 + aNode2 + aNode3) / 3.0;
    }
    else
    {
      const Graphic3d_Vec2& aNode1 = getPosVec2 (aTriNodes[0]);
      const Graphic3d_Vec2& aNode2 = getPosVec2 (aTriNodes[1]);
      const Graphic3d_Vec2& aNode3 = getPosVec2 (aTriNodes[2]);
      aCenter.xy() += (aNode1 + aNode2 + aNode3) / 3.0;
    }
    if (myBvhIndices.HasPatches())
    {
      std::swap (aTriNodes, aTriNodesPrev);
      if (aPatchSize < myPatchSizeMax
       && hasSharedNode (aTriNodes, aTriNodesPrev))
      {
        ++aPatchSize;
        continue;
      }
      else
      {
        myBvhIndices.SetIndex (myBvhIndices.NbElements++, aTriFrom + aPatchFrom, aPatchSize);
        aPatchFrom = aTriIter;
        aPatchSize = 0;
      }
    }
    else
    {
      myBvhIndices.SetIndex (aTriIter, aTriFrom + aTriIter);
    }
  }
  if (aPatchSize != 0)
  {
    myBvhIndices.SetIndex (myBvhIndices.NbElements++, aTriFrom + aPatchFrom, aPatchSize);
  }
  aCenter /= float(aNbTris);

  myCDG3D = vecToPnt (aCenter);
  if (theToEvalMinMax)
  {
    computeBoundingBox();
  }
  return true;
}

// =======================================================================
// function : InitPoints
// purpose  :
// =======================================================================
bool Select3D_SensitivePrimitiveArray::InitPoints (const Handle(Graphic3d_Buffer)&      theVerts,
                                                   const Handle(Graphic3d_IndexBuffer)& theIndices,
                                                   const TopLoc_Location&               theInitLoc,
                                                   const Standard_Integer               theIndexLower,
                                                   const Standard_Integer               theIndexUpper,
                                                   const bool                           theToEvalMinMax)
{
  MarkDirty();
  myPrimType = Graphic3d_TOPA_POINTS;
  myBndBox.Clear();
  myVerts.Nullify();
  myIndices.Nullify();
  myIndexLower = 0;
  myIndexUpper = 0;
  myPosOffset = Standard_Size(-1);
  myBvhIndices.release();
  myIs3d = false;
  myInitLocation = theInitLoc;
  if (theVerts.IsNull()
   || theVerts->NbElements == 0)
  {
    return false;
  }

  for (Standard_Integer anAttribIter = 0; anAttribIter < theVerts->NbAttributes; ++anAttribIter)
  {
    const Graphic3d_Attribute& anAttrib = theVerts->Attribute (anAttribIter);
    if (anAttrib.Id == Graphic3d_TOA_POS)
    {
      if (anAttrib.DataType == Graphic3d_TOD_VEC3
       || anAttrib.DataType == Graphic3d_TOD_VEC4)
      {
        myIs3d = true;
      }
      else if (anAttrib.DataType != Graphic3d_TOD_VEC2)
      {
        return false;
      }

      myPosOffset = theVerts->AttributeOffset (anAttribIter);
      break;
    }
  }
  if (myPosOffset == Standard_Size(-1))
  {
    return false;
  }

  if (!theIndices.IsNull())
  {
    if (theIndexLower < 0
     || theIndexUpper >= theIndices->NbElements
     || theIndices->NbElements == 0)
    {
      return false;
    }
  }
  else
  {
    if (theIndexLower < 0
     || theIndexUpper >= theVerts->NbElements)
    {
      return false;
    }
  }

  Standard_Integer aNbPoints = theIndexUpper - theIndexLower + 1;
  if (aNbPoints < 1)
  {
    return false;
  }
  if (!myBvhIndices.Init (aNbPoints, myPatchSizeMax > 1))
  {
    return false;
  }

  myVerts      = theVerts;
  myIndices    = theIndices;
  myIndexLower = theIndexLower;
  myIndexUpper = theIndexUpper;
  myInvInitLocation = myInitLocation.Transformation().Inverted();

  Graphic3d_Vec3 aCenter (0.0f, 0.0f, 0.0f);
  Standard_Integer aPatchFrom = 0;
  Standard_Integer aPatchSize = 0;
  if (myBvhIndices.HasPatches())
  {
    myBvhIndices.NbElements = 0;
  }
  const float aPatchSize2 = myPatchDistance < ShortRealLast()
                          ? myPatchDistance * myPatchDistance
                          : myPatchDistance;
  const Graphic3d_Vec3* aPnt3dPrev = NULL;
  const Graphic3d_Vec3* aPnt3d     = NULL;
  const Graphic3d_Vec2* aPnt2dPrev = NULL;
  const Graphic3d_Vec2* aPnt2d     = NULL;
  for (Standard_Integer aPointIter = 0; aPointIter < aNbPoints; ++aPointIter)
  {
    const Standard_Integer anIndexOffset = (theIndexLower + aPointIter);
    const Standard_Integer aPointIndex   = !myIndices.IsNull()
                                          ? myIndices->Index (anIndexOffset)
                                          : anIndexOffset;
    if (myIs3d)
    {
      aPnt3d = &getPosVec3 (aPointIndex);
      aCenter += *aPnt3d;
    }
    else
    {
      aPnt2d = &getPosVec2 (aPointIndex);
      aCenter.xy() += *aPnt2d;
    }

    if (myBvhIndices.HasPatches())
    {
      if (myIs3d)
      {
        std::swap (aPnt3d, aPnt3dPrev);
        if (aPatchSize < myPatchSizeMax
         && aPnt3d != NULL
         && (*aPnt3dPrev - *aPnt3d).SquareModulus() < aPatchSize2)
        {
          ++aPatchSize;
          continue;
        }
      }
      else
      {
        std::swap (aPnt2d, aPnt2dPrev);
        if (aPatchSize < myPatchSizeMax
         && aPnt2d != NULL
         && (*aPnt2dPrev - *aPnt2d).SquareModulus() < aPatchSize2)
        {
          ++aPatchSize;
          continue;
        }
      }

      myBvhIndices.SetIndex (myBvhIndices.NbElements++, theIndexLower + aPatchFrom,
                              aPatchSize != 0 ? aPatchSize : 1);
      aPatchFrom = aPointIter;
      aPatchSize = 0;
    }
    else
    {
      myBvhIndices.SetIndex (aPointIter, theIndexLower + aPointIter);
    }
  }
  if (aPatchSize != 0)
  {
    myBvhIndices.SetIndex (myBvhIndices.NbElements++, theIndexLower + aPatchFrom, aPatchSize);
  }
  aCenter /= float(aNbPoints);

  myCDG3D = vecToPnt (aCenter);
  if (theToEvalMinMax)
  {
    computeBoundingBox();
  }
  return true;
}

// =======================================================================
// function : GetConnected
// purpose  :
// =======================================================================
Handle(Select3D_SensitiveEntity) Select3D_SensitivePrimitiveArray::GetConnected()
{
  Handle(Select3D_SensitivePrimitiveArray) aNewEntity = new Select3D_SensitivePrimitiveArray (myOwnerId);
  switch (myPrimType)
  {
    case Graphic3d_TOPA_POINTS:
    {
      aNewEntity->InitPoints        (myVerts, myIndices, myInitLocation, myIndexLower, myIndexUpper);
      break;
    }
    case Graphic3d_TOPA_TRIANGLES:
    {
      aNewEntity->InitTriangulation (myVerts, myIndices, myInitLocation, myIndexLower, myIndexUpper);
      break;
    }
    default: break;
  }
  return aNewEntity;
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
Standard_Integer Select3D_SensitivePrimitiveArray::Size() const
{
  return myBvhIndices.NbElements;
}

// =======================================================================
// function : Box
// purpose  :
// =======================================================================
Select3D_BndBox3d Select3D_SensitivePrimitiveArray::Box (const Standard_Integer theIdx) const
{
  const Standard_Integer anElemIdx  = myBvhIndices.Index (theIdx);
  const Standard_Integer aPatchSize = myBvhIndices.PatchSize (theIdx);
  Select3D_BndBox3d aBox;
  switch (myPrimType)
  {
    case Graphic3d_TOPA_POINTS:
    {
      for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
      {
        const Standard_Integer anIndexOffset = (anElemIdx + anElemIter);
        const Standard_Integer aPointIndex   = !myIndices.IsNull()
                                             ?  myIndices->Index (anIndexOffset)
                                             :  anIndexOffset;
        if (myIs3d)
        {
          const Graphic3d_Vec3& aPoint = getPosVec3 (aPointIndex);
          aBox.Add (SelectMgr_Vec3 (aPoint.x(), aPoint.y(), aPoint.z()));
        }
        else
        {
          const Graphic3d_Vec2& aPoint = getPosVec2 (aPointIndex);
          aBox.Add (SelectMgr_Vec3 (aPoint.x(), aPoint.y(), 0.0));
        }
      }
      break;
    }
    case Graphic3d_TOPA_TRIANGLES:
    {
      Standard_Integer aTriNodes[3];
      if (myIs3d)
      {
        for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
        {
          const Standard_Integer anIndexOffset = (anElemIdx + anElemIter) * 3;
          getTriIndices (myIndices, anIndexOffset, aTriNodes);
          const Graphic3d_Vec3& aNode1 = getPosVec3 (aTriNodes[0]);
          const Graphic3d_Vec3& aNode2 = getPosVec3 (aTriNodes[1]);
          const Graphic3d_Vec3& aNode3 = getPosVec3 (aTriNodes[2]);
          Graphic3d_Vec3 aMinPnt = (aNode1.cwiseMin (aNode2)).cwiseMin (aNode3);
          Graphic3d_Vec3 aMaxPnt = (aNode1.cwiseMax (aNode2)).cwiseMax (aNode3);
          aBox.Add (SelectMgr_Vec3 (aMinPnt.x(), aMinPnt.y(), aMinPnt.z()));
          aBox.Add (SelectMgr_Vec3 (aMaxPnt.x(), aMaxPnt.y(), aMaxPnt.z()));
        }
      }
      else
      {
        for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
        {
          const Standard_Integer anIndexOffset = (anElemIdx + anElemIter) * 3;
          getTriIndices (myIndices, anIndexOffset, aTriNodes);
          const Graphic3d_Vec2& aNode1 = getPosVec2 (aTriNodes[0]);
          const Graphic3d_Vec2& aNode2 = getPosVec2 (aTriNodes[1]);
          const Graphic3d_Vec2& aNode3 = getPosVec2 (aTriNodes[2]);
          Graphic3d_Vec2 aMinPnt = (aNode1.cwiseMin (aNode2)).cwiseMin (aNode3);
          Graphic3d_Vec2 aMaxPnt = (aNode1.cwiseMax (aNode2)).cwiseMax (aNode3);
          aBox.Add (SelectMgr_Vec3 (aMinPnt.x(), aMinPnt.y(), 0.0));
          aBox.Add (SelectMgr_Vec3 (aMaxPnt.x(), aMaxPnt.y(), 0.0));
        }
      }
      break;
    }
    default:
    {
      return aBox;
    }
  }
  return aBox;
}

// =======================================================================
// function : Center
// purpose  :
// =======================================================================
Standard_Real Select3D_SensitivePrimitiveArray::Center (const Standard_Integer theIdx,
                                                        const Standard_Integer theAxis) const
{
  const Select3D_BndBox3d& aBox = Box (theIdx);
  SelectMgr_Vec3 aCenter = (aBox.CornerMin() + aBox.CornerMax()) * 0.5;
  return theAxis == 0 ? aCenter.x() : (theAxis == 1 ? aCenter.y() : aCenter.z());
}

// =======================================================================
// function : Swap
// purpose  :
// =======================================================================
void Select3D_SensitivePrimitiveArray::Swap (const Standard_Integer theIdx1,
                                             const Standard_Integer theIdx2)
{
  Standard_Integer anElemIdx1 = myBvhIndices.Index (theIdx1);
  Standard_Integer anElemIdx2 = myBvhIndices.Index (theIdx2);
  if (myBvhIndices.HasPatches())
  {
    Standard_Integer aPatchSize1 = myBvhIndices.PatchSize (theIdx1);
    Standard_Integer aPatchSize2 = myBvhIndices.PatchSize (theIdx2);
    myBvhIndices.SetIndex (theIdx1, anElemIdx2, aPatchSize2);
    myBvhIndices.SetIndex (theIdx2, anElemIdx1, aPatchSize1);
  }
  else
  {
    myBvhIndices.SetIndex (theIdx1, anElemIdx2);
    myBvhIndices.SetIndex (theIdx2, anElemIdx1);
  }
}

// =======================================================================
// function : BoundingBox
// purpose  :
// =======================================================================
Select3D_BndBox3d Select3D_SensitivePrimitiveArray::BoundingBox()
{
  if (!myBndBox.IsValid())
  {
    computeBoundingBox();
  }
  return applyTransformation();
}

// =======================================================================
// function : computeBoundingBox
// purpose  :
// =======================================================================
void Select3D_SensitivePrimitiveArray::computeBoundingBox()
{
  myBndBox.Clear();
  if (myVerts.IsNull())
  {
    return;
  }

  const Standard_Integer aNbVerts = myVerts->NbElements;
  if (myIs3d)
  {
    for (Standard_Integer aVertIter = 0; aVertIter < aNbVerts; ++aVertIter)
    {
      const Graphic3d_Vec3& aVert = getPosVec3 (aVertIter);
      myBndBox.Add (SelectMgr_Vec3 (aVert.x(), aVert.y(), aVert.z()));
    }
  }
  else
  {
    for (Standard_Integer aVertIter = 0; aVertIter < aNbVerts; ++aVertIter)
    {
      const Graphic3d_Vec2& aVert = getPosVec2 (aVertIter);
      myBndBox.Add (SelectMgr_Vec3 (aVert.x(), aVert.y(), 0.0));
    }
  }
}

// =======================================================================
// function : applyTransformation
// purpose  :
// =======================================================================
Select3D_BndBox3d Select3D_SensitivePrimitiveArray::applyTransformation()
{
  if (!HasInitLocation())
  {
    return myBndBox;
  }

  Select3D_BndBox3d aBndBox;
  for (Standard_Integer aX = 0; aX <=1; ++aX)
  {
    for (Standard_Integer aY = 0; aY <=1; ++aY)
    {
      for (Standard_Integer aZ = 0; aZ <= 1; ++aZ)
      {
        gp_Pnt aVertex = gp_Pnt (aX == 0 ? myBndBox.CornerMin().x() : myBndBox.CornerMax().x(),
                                 aY == 0 ? myBndBox.CornerMin().y() : myBndBox.CornerMax().y(),
                                 aZ == 0 ? myBndBox.CornerMin().z() : myBndBox.CornerMax().z());
        aVertex.Transform (myInitLocation.Transformation());
        aBndBox.Add (Select3D_Vec3 (aVertex.X(), aVertex.Y(), aVertex.Z()));
      }
    }
  }
  return aBndBox;
}

// =======================================================================
// function : Matches
// purpose  :
// =======================================================================
Standard_Boolean Select3D_SensitivePrimitiveArray::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                            SelectBasics_PickResult& thePickResult)
{
  myMinDepthElem      = RealLast();
  myMinDepthNode      = RealLast();
  myMinDepthEdge      = RealLast();
  myDetectedElem      = -1;
  myDetectedNode      = -1;
  myDetectedEdgeNode1 = -1;
  myDetectedEdgeNode2 = -1;
  return Select3D_SensitiveSet::Matches (theMgr, thePickResult);
}

// =======================================================================
// function : overlapsElement
// purpose  :
// =======================================================================
Standard_Boolean Select3D_SensitivePrimitiveArray::overlapsElement (SelectBasics_SelectingVolumeManager& theMgr,
                                                                    Standard_Integer theElemIdx,
                                                                    Standard_Real& theMatchDepth)
{
  const Standard_Integer anElemIdx  = myBvhIndices.Index (theElemIdx);
  const Standard_Integer aPatchSize = myBvhIndices.PatchSize (theElemIdx);
  Select3D_BndBox3d aBox;
  Standard_Boolean aResult = Standard_False;
  Standard_Real aMinDepth  = RealLast();
  switch (myPrimType)
  {
    case Graphic3d_TOPA_POINTS:
    {
      for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
      {
        const Standard_Integer anIndexOffset = (anElemIdx + anElemIter);
        const Standard_Integer aPointIndex   = !myIndices.IsNull()
                                             ?  myIndices->Index (anIndexOffset)
                                             :  anIndexOffset;
        gp_Pnt aPoint;
        if (myIs3d)
        {
          aPoint = vecToPnt (getPosVec3 (aPointIndex));
        }
        else
        {
          aPoint = vecToPnt (getPosVec2 (aPointIndex));
        }

        Standard_Real aCurrentDepth = RealLast();
        if (myToDetectNode
         || myToDetectElem)
        {
          if (theMgr.Overlaps (aPoint, aCurrentDepth))
          {
            if (aCurrentDepth <= myMinDepthNode)
            {
              myDetectedElem = myDetectedNode = aPointIndex;
              myMinDepthElem = myMinDepthNode = aCurrentDepth;
            }
            aResult = Standard_True;
          }
        }
        aMinDepth = Min (aMinDepth, aCurrentDepth);
      }
      break;
    }
    case Graphic3d_TOPA_TRIANGLES:
    {
      Graphic3d_Vec3i aTriNodes;
      for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
      {
        const Standard_Integer anIndexOffset = (anElemIdx + anElemIter) * 3;
        getTriIndices (myIndices, anIndexOffset, aTriNodes);
        gp_Pnt aPnts[3];
        if (myIs3d)
        {
          aPnts[0] = vecToPnt (getPosVec3 (aTriNodes[0]));
          aPnts[1] = vecToPnt (getPosVec3 (aTriNodes[1]));
          aPnts[2] = vecToPnt (getPosVec3 (aTriNodes[2]));
        }
        else
        {
          aPnts[0] = vecToPnt (getPosVec2 (aTriNodes[0]));
          aPnts[1] = vecToPnt (getPosVec2 (aTriNodes[1]));
          aPnts[2] = vecToPnt (getPosVec2 (aTriNodes[2]));
        }

        Standard_Real aCurrentDepth = RealLast();
        if (myToDetectElem)
        {
          if (theMgr.Overlaps (aPnts[0], aPnts[1], aPnts[2], Select3D_TOS_INTERIOR, aCurrentDepth))
          {
            if (aCurrentDepth <= myMinDepthElem)
            {
              myDetectedElem = anElemIdx + anElemIter;
              myMinDepthElem = aCurrentDepth;
            }
            aResult = Standard_True;
          }
        }
        if (myToDetectNode)
        {
          for (int aNodeIter = 0; aNodeIter < 3; ++aNodeIter)
          {
            if (theMgr.Overlaps (aPnts[aNodeIter], aCurrentDepth))
            {
              if (aCurrentDepth <= myMinDepthNode)
              {
                myDetectedNode = aTriNodes[aNodeIter];
                myMinDepthNode = aCurrentDepth;
              }
              aResult = Standard_True;
            }
          }
        }
        if (myToDetectEdge)
        {
          for (int aNodeIter = 0; aNodeIter < 3; ++aNodeIter)
          {
            int aNode1 = aNodeIter == 0 ? 2 : (aNodeIter - 1);
            int aNode2 = aNodeIter;
            if (theMgr.Overlaps (aPnts[aNode1], aPnts[aNode2], aCurrentDepth))
            {
              if (aCurrentDepth <= myMinDepthEdge)
              {
                myDetectedEdgeNode1 = aTriNodes[aNode1];
                myDetectedEdgeNode2 = aTriNodes[aNode2];
                myMinDepthEdge = aCurrentDepth;
              }
              aResult = Standard_True;
            }
          }
        }
        aMinDepth = Min (aMinDepth, aCurrentDepth);
      }
      break;
    }
    default:
    {
      return Standard_False;
    }
  }

  theMatchDepth = aMinDepth;
  return aResult;
}

// =======================================================================
// function : distanceToCOG
// purpose  :
// =======================================================================
Standard_Real Select3D_SensitivePrimitiveArray::distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter (myCDG3D);
}

// =======================================================================
// function : elementIsInside
// purpose  :
// =======================================================================
Standard_Boolean Select3D_SensitivePrimitiveArray::elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                                                    const Standard_Integer               theElemIdx)
{
  const Standard_Integer anElemIdx  = myBvhIndices.Index (theElemIdx);
  const Standard_Integer aPatchSize = myBvhIndices.PatchSize (theElemIdx);
  switch (myPrimType)
  {
    case Graphic3d_TOPA_POINTS:
    {
      for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
      {
        const Standard_Integer anIndexOffset = (anElemIdx + anElemIter);
        const Standard_Integer aPointIndex   = !myIndices.IsNull()
                                             ?  myIndices->Index (anIndexOffset)
                                             :  anIndexOffset;
        gp_Pnt aPoint;
        if (myIs3d)
        {
          aPoint = vecToPnt (getPosVec3 (aPointIndex));
        }
        else
        {
          aPoint = vecToPnt (getPosVec2 (aPointIndex));
        }
        if (!theMgr.Overlaps (aPoint))
        {
          return Standard_False;
        }
      }
      return Standard_True;
    }
    case Graphic3d_TOPA_TRIANGLES:
    {
      Graphic3d_Vec3i aTriNodes;
      for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
      {
        const Standard_Integer anIndexOffset = (anElemIdx + anElemIter) * 3;
        getTriIndices (myIndices, anIndexOffset, aTriNodes);
        gp_Pnt aPnts[3];
        if (myIs3d)
        {
          aPnts[0] = vecToPnt (getPosVec3 (aTriNodes[0]));
          aPnts[1] = vecToPnt (getPosVec3 (aTriNodes[1]));
          aPnts[2] = vecToPnt (getPosVec3 (aTriNodes[2]));
        }
        else
        {
          aPnts[0] = vecToPnt (getPosVec2 (aTriNodes[0]));
          aPnts[1] = vecToPnt (getPosVec2 (aTriNodes[1]));
          aPnts[2] = vecToPnt (getPosVec2 (aTriNodes[2]));
        }

        if (!theMgr.Overlaps (aPnts[0])
         || !theMgr.Overlaps (aPnts[1])
         || !theMgr.Overlaps (aPnts[2]))
        {
          return Standard_False;
        }
      }
      return Standard_True;
    }
    default:
    {
      return Standard_False;
    }
  }
}
