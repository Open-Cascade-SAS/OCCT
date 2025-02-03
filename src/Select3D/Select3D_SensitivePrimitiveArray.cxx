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

#include <OSD_Parallel.hxx>
#include <Standard_Atomic.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitivePrimitiveArray, Select3D_SensitiveSet)

namespace
{

//! Auxiliary converter.
static inline gp_Pnt vecToPnt(const Graphic3d_Vec3& theVec)
{
  return gp_Pnt(theVec.x(), theVec.y(), theVec.z());
}

//! Auxiliary converter.
static inline gp_Pnt vecToPnt(const Graphic3d_Vec2& theVec)
{
  return gp_Pnt(theVec.x(), theVec.y(), 0.0);
}

//! Auxiliary function to find shared node between two triangles.
static inline bool hasSharedNode(const Standard_Integer* theTri1, const Standard_Integer* theTri2)
{
  return theTri1[0] == theTri2[0] || theTri1[1] == theTri2[0] || theTri1[2] == theTri2[0]
         || theTri1[0] == theTri2[1] || theTri1[1] == theTri2[1] || theTri1[2] == theTri2[1]
         || theTri1[0] == theTri2[2] || theTri1[1] == theTri2[2] || theTri1[2] == theTri2[2];
}

//! Fill in the triangle nodes indices.
static inline void getTriIndices(const Handle(Graphic3d_IndexBuffer)& theIndices,
                                 const Standard_Integer               theIndexOffset,
                                 Standard_Integer*                    theNodes)
{
  if (!theIndices.IsNull())
  {
    theNodes[0] = theIndices->Index(theIndexOffset + 0);
    theNodes[1] = theIndices->Index(theIndexOffset + 1);
    theNodes[2] = theIndices->Index(theIndexOffset + 2);
  }
  else
  {
    theNodes[0] = theIndexOffset + 0;
    theNodes[1] = theIndexOffset + 1;
    theNodes[2] = theIndexOffset + 2;
  }
}

} // namespace

//! Functor for initializing groups in parallel threads.
struct Select3D_SensitivePrimitiveArray::Select3D_SensitivePrimitiveArray_InitFunctor
{
  Select3D_SensitivePrimitiveArray_InitFunctor(Select3D_SensitivePrimitiveArray& thePrimArray,
                                               Standard_Integer                  theDivStep,
                                               Standard_Boolean                  theToEvalMinMax)
      : myPrimArray(thePrimArray),
        myDivStep(theDivStep),
        myToEvalMinMax(theToEvalMinMax),
        myToComputeBvh(Standard_True),
        myNbFailures(0)
  {
  }

  void operator()(const Standard_Integer& theIndex) const
  {
    Handle(Select3D_SensitivePrimitiveArray)& anEntity =
      myPrimArray.myGroups->ChangeValue(theIndex);
    const Standard_Integer aLower  = myPrimArray.myIndexLower + theIndex * myDivStep;
    const Standard_Integer anUpper = Min(aLower + myDivStep - 1, myPrimArray.myIndexUpper);
    anEntity                       = new Select3D_SensitivePrimitiveArray(myPrimArray.myOwnerId);
    anEntity->SetPatchSizeMax(myPrimArray.myPatchSizeMax);
    anEntity->SetPatchDistance(myPrimArray.myPatchDistance);
    anEntity->SetDetectElements(myPrimArray.myToDetectElem);
    anEntity->SetDetectElementMap(myPrimArray.ToDetectElementMap());
    anEntity->SetDetectNodes(myPrimArray.myToDetectNode);
    anEntity->SetDetectNodeMap(myPrimArray.ToDetectNodeMap());
    anEntity->SetSensitivityFactor(myPrimArray.SensitivityFactor());
    switch (myPrimArray.myPrimType)
    {
      case Graphic3d_TOPA_POINTS: {
        if (!anEntity->InitPoints(myPrimArray.myVerts,
                                  myPrimArray.myIndices,
                                  myPrimArray.myInitLocation,
                                  aLower,
                                  anUpper,
                                  myToEvalMinMax,
                                  1))
        {
          Standard_Atomic_Increment(&myNbFailures);
          return;
        }
        break;
      }
      case Graphic3d_TOPA_TRIANGLES: {
        if (!anEntity->InitTriangulation(myPrimArray.myVerts,
                                         myPrimArray.myIndices,
                                         myPrimArray.myInitLocation,
                                         aLower,
                                         anUpper,
                                         myToEvalMinMax,
                                         1))
        {
          Standard_Atomic_Increment(&myNbFailures);
          return;
        }
        break;
      }
      default: {
        Standard_Atomic_Increment(&myNbFailures);
        return;
      }
    }

    if (myToComputeBvh)
    {
      anEntity->BVH();
    }
  }

  Standard_Boolean IsDone() const { return myNbFailures == 0; }

private:
  Select3D_SensitivePrimitiveArray_InitFunctor operator=(
    Select3D_SensitivePrimitiveArray_InitFunctor&);

private:
  Select3D_SensitivePrimitiveArray& myPrimArray;
  Standard_Integer                  myDivStep;
  Standard_Boolean                  myToEvalMinMax;
  Standard_Boolean                  myToComputeBvh;
  mutable volatile Standard_Integer myNbFailures;
};

//! Functor for computing BVH in parallel threads.
struct Select3D_SensitivePrimitiveArray::Select3D_SensitivePrimitiveArray_BVHFunctor
{
  Select3D_SensitivePrimitiveArray_BVHFunctor(
    NCollection_Array1<Handle(Select3D_SensitivePrimitiveArray)>& theGroups)
      : myGroups(theGroups)
  {
  }

  void operator()(const Standard_Integer& theIndex) const { myGroups.ChangeValue(theIndex)->BVH(); }

private:
  Select3D_SensitivePrimitiveArray_BVHFunctor operator=(
    Select3D_SensitivePrimitiveArray_BVHFunctor&);

private:
  NCollection_Array1<Handle(Select3D_SensitivePrimitiveArray)>& myGroups;
};

//=================================================================================================

Select3D_SensitivePrimitiveArray::Select3D_SensitivePrimitiveArray(
  const Handle(SelectMgr_EntityOwner)& theOwnerId)
    : Select3D_SensitiveSet(theOwnerId),
      myPosData(NULL),
      myPosStride(Standard_Size(-1)),
      myPrimType(Graphic3d_TOPA_UNDEFINED),
      myIndexLower(0),
      myIndexUpper(0),
      myPatchSizeMax(1),
      myPatchDistance(ShortRealLast()),
      myIs3d(false),
      myBvhIndices(Graphic3d_Buffer::DefaultAllocator()),
      myMinDepthElem(RealLast()),
      myMinDepthNode(RealLast()),
      myMinDepthEdge(RealLast()),
      myDetectedElem(-1),
      myDetectedNode(-1),
      myDetectedEdgeNode1(-1),
      myDetectedEdgeNode2(-1),
      myToDetectElem(true),
      myToDetectNode(false),
      myToDetectEdge(false)
{
  //
}

//=================================================================================================

void Select3D_SensitivePrimitiveArray::SetDetectElementMap(bool theToDetect)
{
  if (!theToDetect)
  {
    myDetectedElemMap.Nullify();
    return;
  }

  if (myDetectedElemMap.IsNull())
  {
    myDetectedElemMap = new TColStd_HPackedMapOfInteger();
  }
  else
  {
    myDetectedElemMap->ChangeMap().Clear();
  }
}

//=================================================================================================

void Select3D_SensitivePrimitiveArray::SetDetectNodeMap(bool theToDetect)
{
  if (!theToDetect)
  {
    myDetectedNodeMap.Nullify();
    return;
  }

  if (myDetectedNodeMap.IsNull())
  {
    myDetectedNodeMap = new TColStd_HPackedMapOfInteger();
  }
  else
  {
    myDetectedNodeMap->ChangeMap().Clear();
  }
}

//=================================================================================================

bool Select3D_SensitivePrimitiveArray::InitTriangulation(
  const Handle(Graphic3d_Buffer)&      theVerts,
  const Handle(Graphic3d_IndexBuffer)& theIndices,
  const TopLoc_Location&               theInitLoc,
  const Standard_Integer               theIndexLower,
  const Standard_Integer               theIndexUpper,
  const bool                           theToEvalMinMax,
  const Standard_Integer               theNbGroups)
{
  MarkDirty();
  myGroups.Nullify();
  myPrimType = Graphic3d_TOPA_TRIANGLES;
  myBndBox.Clear();
  myVerts.Nullify();
  myIndices.Nullify();
  myIndexLower = 0;
  myIndexUpper = 0;
  myPosData    = NULL;
  myPosStride  = Standard_Size(-1);
  myBvhIndices.release();
  myIs3d         = false;
  myInitLocation = theInitLoc;
  myCDG3D.SetCoord(0.0, 0.0, 0.0);
  if (theVerts.IsNull() || theVerts->NbElements == 0)
  {
    return false;
  }

  Standard_Integer aPosAttribIndex = 0;
  myPosData = theVerts->AttributeData(Graphic3d_TOA_POS, aPosAttribIndex, myPosStride);
  if (myPosData == NULL)
  {
    return false;
  }

  const Graphic3d_Attribute& anAttrib = theVerts->Attribute(aPosAttribIndex);
  myIs3d = anAttrib.DataType == Graphic3d_TOD_VEC3 || anAttrib.DataType == Graphic3d_TOD_VEC4;
  if (!myIs3d && anAttrib.DataType != Graphic3d_TOD_VEC2)
  {
    myPosData = NULL;
    return false;
  }

  if (!theIndices.IsNull())
  {
    if (theIndexLower < 0 || theIndexUpper >= theIndices->NbElements || theIndices->NbElements == 0)
    {
      return false;
    }
  }
  else
  {
    if (theIndexLower < 0 || theIndexUpper >= theVerts->NbElements)
    {
      return false;
    }
  }

  Standard_Integer aTriFrom  = theIndexLower / 3;
  Standard_Integer aNbTris   = (theIndexUpper - theIndexLower + 1) / 3;
  const bool       hasGroups = (theNbGroups > 1) && (aNbTris / theNbGroups > 10);
  if (aNbTris < 1)
  {
    return false;
  }
  if (!myBvhIndices.Init(hasGroups ? theNbGroups : aNbTris, !hasGroups && myPatchSizeMax > 1))
  {
    return false;
  }

  myVerts           = theVerts;
  myIndices         = theIndices;
  myIndexLower      = theIndexLower;
  myIndexUpper      = theIndexUpper;
  myInvInitLocation = myInitLocation.Transformation().Inverted();
  if (hasGroups)
  {
    myGroups = new Select3D_PrimArraySubGroupArray(0, theNbGroups - 1);
    const Standard_Integer                       aDivStep = (aNbTris / theNbGroups) * 3;
    Select3D_SensitivePrimitiveArray_InitFunctor anInitFunctor(*this, aDivStep, theToEvalMinMax);
    OSD_Parallel::For(myGroups->Lower(), myGroups->Upper() + 1, anInitFunctor);
    if (!anInitFunctor.IsDone())
    {
      return false;
    }
    for (Standard_Integer aGroupIter = 0; aGroupIter < theNbGroups; ++aGroupIter)
    {
      Handle(Select3D_SensitivePrimitiveArray)& anEntity = myGroups->ChangeValue(aGroupIter);
      myBndBox.Combine(anEntity->BoundingBox());
      myBvhIndices.SetIndex(aGroupIter, aGroupIter);
      myCDG3D.ChangeCoord() += anEntity->CenterOfGeometry().XYZ();
    }
    myCDG3D.ChangeCoord().Divide(static_cast<Standard_Real>(myGroups->Size()));
    if (theToEvalMinMax)
    {
      computeBoundingBox();
    }
    return true;
  }

  Graphic3d_Vec3    aCenter(0.0f, 0.0f, 0.0f);
  Standard_Integer  aTriNodes1[3] = {-1, -1, -1};
  Standard_Integer  aTriNodes2[3] = {-1, -1, -1};
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
    getTriIndices(myIndices, anIndexOffset, aTriNodes);
    if (myIs3d)
    {
      const Graphic3d_Vec3& aNode1 = getPosVec3(aTriNodes[0]);
      const Graphic3d_Vec3& aNode2 = getPosVec3(aTriNodes[1]);
      const Graphic3d_Vec3& aNode3 = getPosVec3(aTriNodes[2]);
      aCenter += (aNode1 + aNode2 + aNode3) / 3.0;
    }
    else
    {
      const Graphic3d_Vec2& aNode1 = getPosVec2(aTriNodes[0]);
      const Graphic3d_Vec2& aNode2 = getPosVec2(aTriNodes[1]);
      const Graphic3d_Vec2& aNode3 = getPosVec2(aTriNodes[2]);
      aCenter += Graphic3d_Vec3((aNode1 + aNode2 + aNode3) / 3.0);
    }
    if (myBvhIndices.HasPatches())
    {
      std::swap(aTriNodes, aTriNodesPrev);
      if (aPatchSize < myPatchSizeMax && hasSharedNode(aTriNodes, aTriNodesPrev))
      {
        ++aPatchSize;
        continue;
      }
      else
      {
        myBvhIndices.SetIndex(myBvhIndices.NbElements++, aTriFrom + aPatchFrom, aPatchSize);
        aPatchFrom = aTriIter;
        aPatchSize = 0;
      }
    }
    else
    {
      myBvhIndices.SetIndex(aTriIter, aTriFrom + aTriIter);
    }
  }
  if (aPatchSize != 0)
  {
    myBvhIndices.SetIndex(myBvhIndices.NbElements++, aTriFrom + aPatchFrom, aPatchSize);
  }
  aCenter /= float(aNbTris);

  myCDG3D = vecToPnt(aCenter);
  if (theToEvalMinMax)
  {
    computeBoundingBox();
  }
  return true;
}

//=================================================================================================

bool Select3D_SensitivePrimitiveArray::InitPoints(const Handle(Graphic3d_Buffer)&      theVerts,
                                                  const Handle(Graphic3d_IndexBuffer)& theIndices,
                                                  const TopLoc_Location&               theInitLoc,
                                                  const Standard_Integer theIndexLower,
                                                  const Standard_Integer theIndexUpper,
                                                  const bool             theToEvalMinMax,
                                                  const Standard_Integer theNbGroups)
{
  MarkDirty();
  myGroups.Nullify();
  myPrimType = Graphic3d_TOPA_POINTS;
  myBndBox.Clear();
  myVerts.Nullify();
  myIndices.Nullify();
  myIndexLower = 0;
  myIndexUpper = 0;
  myPosData    = NULL;
  myPosStride  = Standard_Size(-1);
  myBvhIndices.release();
  myIs3d         = false;
  myInitLocation = theInitLoc;
  if (theVerts.IsNull() || theVerts->NbElements == 0)
  {
    return false;
  }

  Standard_Integer aPosAttribIndex = 0;
  myPosData = theVerts->AttributeData(Graphic3d_TOA_POS, aPosAttribIndex, myPosStride);
  if (myPosData == NULL)
  {
    return false;
  }

  const Graphic3d_Attribute& anAttrib = theVerts->Attribute(aPosAttribIndex);
  myIs3d = anAttrib.DataType == Graphic3d_TOD_VEC3 || anAttrib.DataType == Graphic3d_TOD_VEC4;
  if (!myIs3d && anAttrib.DataType != Graphic3d_TOD_VEC2)
  {
    myPosData = NULL;
    return false;
  }

  if (!theIndices.IsNull())
  {
    if (theIndexLower < 0 || theIndexUpper >= theIndices->NbElements || theIndices->NbElements == 0)
    {
      return false;
    }
  }
  else
  {
    if (theIndexLower < 0 || theIndexUpper >= theVerts->NbElements)
    {
      return false;
    }
  }

  const Standard_Integer aNbPoints = theIndexUpper - theIndexLower + 1;
  const bool             hasGroups = (theNbGroups > 1) && (aNbPoints / theNbGroups > 10);
  if (aNbPoints < 1)
  {
    return false;
  }
  if (!myBvhIndices.Init(hasGroups ? theNbGroups : aNbPoints, !hasGroups && myPatchSizeMax > 1))
  {
    return false;
  }

  myVerts           = theVerts;
  myIndices         = theIndices;
  myIndexLower      = theIndexLower;
  myIndexUpper      = theIndexUpper;
  myInvInitLocation = myInitLocation.Transformation().Inverted();
  if (hasGroups)
  {
    myGroups = new Select3D_PrimArraySubGroupArray(0, theNbGroups - 1);
    const Standard_Integer                       aDivStep = aNbPoints / theNbGroups;
    Select3D_SensitivePrimitiveArray_InitFunctor anInitFunctor(*this, aDivStep, theToEvalMinMax);
    OSD_Parallel::For(myGroups->Lower(), myGroups->Upper() + 1, anInitFunctor);
    if (!anInitFunctor.IsDone())
    {
      return false;
    }
    for (Standard_Integer aGroupIter = 0; aGroupIter < theNbGroups; ++aGroupIter)
    {
      Handle(Select3D_SensitivePrimitiveArray)& anEntity = myGroups->ChangeValue(aGroupIter);
      myBndBox.Combine(anEntity->BoundingBox());
      myBvhIndices.SetIndex(aGroupIter, aGroupIter);
      myCDG3D.ChangeCoord() += anEntity->CenterOfGeometry().XYZ();
    }
    myCDG3D.ChangeCoord().Divide(static_cast<Standard_Real>(myGroups->Size()));
    if (theToEvalMinMax)
    {
      computeBoundingBox();
    }
    return true;
  }

  Graphic3d_Vec3   aCenter(0.0f, 0.0f, 0.0f);
  Standard_Integer aPatchFrom = 0;
  Standard_Integer aPatchSize = 0;
  if (myBvhIndices.HasPatches())
  {
    myBvhIndices.NbElements = 0;
  }
  const float aPatchSize2 =
    myPatchDistance < ShortRealLast() ? myPatchDistance * myPatchDistance : myPatchDistance;
  const Graphic3d_Vec3* aPnt3dPrev = NULL;
  const Graphic3d_Vec3* aPnt3d     = NULL;
  const Graphic3d_Vec2* aPnt2dPrev = NULL;
  const Graphic3d_Vec2* aPnt2d     = NULL;
  for (Standard_Integer aPointIter = 0; aPointIter < aNbPoints; ++aPointIter)
  {
    const Standard_Integer anIndexOffset = (theIndexLower + aPointIter);
    const Standard_Integer aPointIndex =
      !myIndices.IsNull() ? myIndices->Index(anIndexOffset) : anIndexOffset;
    if (myIs3d)
    {
      aPnt3d = &getPosVec3(aPointIndex);
      aCenter += *aPnt3d;
    }
    else
    {
      aPnt2d = &getPosVec2(aPointIndex);
      aCenter += Graphic3d_Vec3(*aPnt2d);
    }

    if (myBvhIndices.HasPatches())
    {
      if (myIs3d)
      {
        std::swap(aPnt3d, aPnt3dPrev);
        if (aPatchSize < myPatchSizeMax && aPnt3d != NULL
            && (*aPnt3dPrev - *aPnt3d).SquareModulus() < aPatchSize2)
        {
          ++aPatchSize;
          continue;
        }
      }
      else
      {
        std::swap(aPnt2d, aPnt2dPrev);
        if (aPatchSize < myPatchSizeMax && aPnt2d != NULL
            && (*aPnt2dPrev - *aPnt2d).SquareModulus() < aPatchSize2)
        {
          ++aPatchSize;
          continue;
        }
      }

      myBvhIndices.SetIndex(myBvhIndices.NbElements++,
                            theIndexLower + aPatchFrom,
                            aPatchSize != 0 ? aPatchSize : 1);
      aPatchFrom = aPointIter;
      aPatchSize = 0;
    }
    else
    {
      myBvhIndices.SetIndex(aPointIter, theIndexLower + aPointIter);
    }
  }
  if (aPatchSize != 0)
  {
    myBvhIndices.SetIndex(myBvhIndices.NbElements++, theIndexLower + aPatchFrom, aPatchSize);
  }
  aCenter /= float(aNbPoints);

  myCDG3D = vecToPnt(aCenter);
  if (theToEvalMinMax)
  {
    computeBoundingBox();
  }
  return true;
}

//=================================================================================================

Handle(Select3D_SensitiveEntity) Select3D_SensitivePrimitiveArray::GetConnected()
{
  Handle(Select3D_SensitivePrimitiveArray) aNewEntity =
    new Select3D_SensitivePrimitiveArray(myOwnerId);
  switch (myPrimType)
  {
    case Graphic3d_TOPA_POINTS: {
      aNewEntity->InitPoints(myVerts,
                             myIndices,
                             myInitLocation,
                             myIndexLower,
                             myIndexUpper,
                             true,
                             !myGroups.IsNull() ? myGroups->Size() : 1);
      break;
    }
    case Graphic3d_TOPA_TRIANGLES: {
      aNewEntity->InitTriangulation(myVerts,
                                    myIndices,
                                    myInitLocation,
                                    myIndexLower,
                                    myIndexUpper,
                                    true,
                                    !myGroups.IsNull() ? myGroups->Size() : 1);
      break;
    }
    default:
      break;
  }
  return aNewEntity;
}

//=================================================================================================

void Select3D_SensitivePrimitiveArray::Set(const Handle(SelectMgr_EntityOwner)& theOwnerId)
{
  base_type::Set(theOwnerId);
  if (!myGroups.IsNull())
  {
    for (Select3D_PrimArraySubGroupArray::Iterator aGroupIter(*myGroups); aGroupIter.More();
         aGroupIter.Next())
    {
      aGroupIter.Value()->Set(theOwnerId);
    }
  }
}

//=================================================================================================

void Select3D_SensitivePrimitiveArray::BVH()
{
  if (!myContent.IsDirty())
  {
    return;
  }

  base_type::BVH();
  if (myGroups.IsNull())
  {
    return;
  }

  Standard_Integer aNbToUpdate = 0;
  for (Select3D_PrimArraySubGroupArray::Iterator aGroupIter(*myGroups); aGroupIter.More();
       aGroupIter.Next())
  {
    if (aGroupIter.Value()->myContent.IsDirty())
    {
      ++aNbToUpdate;
    }
  }

  if (aNbToUpdate > 0)
  {
    Select3D_SensitivePrimitiveArray_BVHFunctor aFunctor(*myGroups);
    OSD_Parallel::For(myGroups->Lower(), myGroups->Upper() + 1, aFunctor, aNbToUpdate <= 1);
  }
}

//=================================================================================================

Standard_Integer Select3D_SensitivePrimitiveArray::Size() const
{
  return myBvhIndices.NbElements;
}

//=================================================================================================

Select3D_BndBox3d Select3D_SensitivePrimitiveArray::Box(const Standard_Integer theIdx) const
{
  const Standard_Integer anElemIdx  = myBvhIndices.Index(theIdx);
  const Standard_Integer aPatchSize = myBvhIndices.PatchSize(theIdx);
  if (!myGroups.IsNull())
  {
    return myGroups->Value(anElemIdx)->BoundingBox();
  }

  Select3D_BndBox3d aBox;
  switch (myPrimType)
  {
    case Graphic3d_TOPA_POINTS: {
      for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
      {
        const Standard_Integer anIndexOffset = (anElemIdx + anElemIter);
        const Standard_Integer aPointIndex =
          !myIndices.IsNull() ? myIndices->Index(anIndexOffset) : anIndexOffset;
        if (myIs3d)
        {
          const Graphic3d_Vec3& aPoint = getPosVec3(aPointIndex);
          aBox.Add(SelectMgr_Vec3(aPoint.x(), aPoint.y(), aPoint.z()));
        }
        else
        {
          const Graphic3d_Vec2& aPoint = getPosVec2(aPointIndex);
          aBox.Add(SelectMgr_Vec3(aPoint.x(), aPoint.y(), 0.0));
        }
      }
      break;
    }
    case Graphic3d_TOPA_TRIANGLES: {
      Standard_Integer aTriNodes[3];
      if (myIs3d)
      {
        for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
        {
          const Standard_Integer anIndexOffset = (anElemIdx + anElemIter) * 3;
          getTriIndices(myIndices, anIndexOffset, aTriNodes);
          const Graphic3d_Vec3& aNode1  = getPosVec3(aTriNodes[0]);
          const Graphic3d_Vec3& aNode2  = getPosVec3(aTriNodes[1]);
          const Graphic3d_Vec3& aNode3  = getPosVec3(aTriNodes[2]);
          Graphic3d_Vec3        aMinPnt = (aNode1.cwiseMin(aNode2)).cwiseMin(aNode3);
          Graphic3d_Vec3        aMaxPnt = (aNode1.cwiseMax(aNode2)).cwiseMax(aNode3);
          aBox.Add(SelectMgr_Vec3(aMinPnt.x(), aMinPnt.y(), aMinPnt.z()));
          aBox.Add(SelectMgr_Vec3(aMaxPnt.x(), aMaxPnt.y(), aMaxPnt.z()));
        }
      }
      else
      {
        for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
        {
          const Standard_Integer anIndexOffset = (anElemIdx + anElemIter) * 3;
          getTriIndices(myIndices, anIndexOffset, aTriNodes);
          const Graphic3d_Vec2& aNode1  = getPosVec2(aTriNodes[0]);
          const Graphic3d_Vec2& aNode2  = getPosVec2(aTriNodes[1]);
          const Graphic3d_Vec2& aNode3  = getPosVec2(aTriNodes[2]);
          Graphic3d_Vec2        aMinPnt = (aNode1.cwiseMin(aNode2)).cwiseMin(aNode3);
          Graphic3d_Vec2        aMaxPnt = (aNode1.cwiseMax(aNode2)).cwiseMax(aNode3);
          aBox.Add(SelectMgr_Vec3(aMinPnt.x(), aMinPnt.y(), 0.0));
          aBox.Add(SelectMgr_Vec3(aMaxPnt.x(), aMaxPnt.y(), 0.0));
        }
      }
      break;
    }
    default: {
      return aBox;
    }
  }
  return aBox;
}

//=================================================================================================

Standard_Real Select3D_SensitivePrimitiveArray::Center(const Standard_Integer theIdx,
                                                       const Standard_Integer theAxis) const
{
  if (!myGroups.IsNull())
  {
    const Standard_Integer anElemIdx = myBvhIndices.Index(theIdx);
    const gp_Pnt           aCenter   = myGroups->Value(anElemIdx)->CenterOfGeometry();
    return theAxis == 0 ? aCenter.X() : (theAxis == 1 ? aCenter.Y() : aCenter.Z());
  }

  const Select3D_BndBox3d& aBox    = Box(theIdx);
  SelectMgr_Vec3           aCenter = (aBox.CornerMin() + aBox.CornerMax()) * 0.5;
  return theAxis == 0 ? aCenter.x() : (theAxis == 1 ? aCenter.y() : aCenter.z());
}

//=================================================================================================

void Select3D_SensitivePrimitiveArray::Swap(const Standard_Integer theIdx1,
                                            const Standard_Integer theIdx2)
{
  Standard_Integer anElemIdx1 = myBvhIndices.Index(theIdx1);
  Standard_Integer anElemIdx2 = myBvhIndices.Index(theIdx2);
  if (myBvhIndices.HasPatches())
  {
    Standard_Integer aPatchSize1 = myBvhIndices.PatchSize(theIdx1);
    Standard_Integer aPatchSize2 = myBvhIndices.PatchSize(theIdx2);
    myBvhIndices.SetIndex(theIdx1, anElemIdx2, aPatchSize2);
    myBvhIndices.SetIndex(theIdx2, anElemIdx1, aPatchSize1);
  }
  else
  {
    myBvhIndices.SetIndex(theIdx1, anElemIdx2);
    myBvhIndices.SetIndex(theIdx2, anElemIdx1);
  }
}

//=================================================================================================

Select3D_BndBox3d Select3D_SensitivePrimitiveArray::BoundingBox()
{
  if (!myBndBox.IsValid())
  {
    computeBoundingBox();
  }
  return applyTransformation();
}

//=================================================================================================

void Select3D_SensitivePrimitiveArray::computeBoundingBox()
{
  myBndBox.Clear();
  if (!myGroups.IsNull())
  {
    for (Select3D_PrimArraySubGroupArray::Iterator aGroupIter(*myGroups); aGroupIter.More();
         aGroupIter.Next())
    {
      myBndBox.Combine(aGroupIter.Value()->BoundingBox());
    }
    return;
  }

  if (myVerts.IsNull())
  {
    return;
  }

  const Standard_Integer aNbVerts = myVerts->NbElements;
  if (myIs3d)
  {
    for (Standard_Integer aVertIter = 0; aVertIter < aNbVerts; ++aVertIter)
    {
      const Graphic3d_Vec3& aVert = getPosVec3(aVertIter);
      myBndBox.Add(SelectMgr_Vec3(aVert.x(), aVert.y(), aVert.z()));
    }
  }
  else
  {
    for (Standard_Integer aVertIter = 0; aVertIter < aNbVerts; ++aVertIter)
    {
      const Graphic3d_Vec2& aVert = getPosVec2(aVertIter);
      myBndBox.Add(SelectMgr_Vec3(aVert.x(), aVert.y(), 0.0));
    }
  }
}

//=================================================================================================

Select3D_BndBox3d Select3D_SensitivePrimitiveArray::applyTransformation()
{
  if (!HasInitLocation())
  {
    return myBndBox;
  }

  Select3D_BndBox3d aBndBox;
  for (Standard_Integer aX = 0; aX <= 1; ++aX)
  {
    for (Standard_Integer aY = 0; aY <= 1; ++aY)
    {
      for (Standard_Integer aZ = 0; aZ <= 1; ++aZ)
      {
        gp_Pnt aVertex = gp_Pnt(aX == 0 ? myBndBox.CornerMin().x() : myBndBox.CornerMax().x(),
                                aY == 0 ? myBndBox.CornerMin().y() : myBndBox.CornerMax().y(),
                                aZ == 0 ? myBndBox.CornerMin().z() : myBndBox.CornerMax().z());
        aVertex.Transform(myInitLocation.Transformation());
        aBndBox.Add(Select3D_Vec3(aVertex.X(), aVertex.Y(), aVertex.Z()));
      }
    }
  }
  return aBndBox;
}

//=================================================================================================

Standard_Boolean Select3D_SensitivePrimitiveArray::Matches(
  SelectBasics_SelectingVolumeManager& theMgr,
  SelectBasics_PickResult&             thePickResult)
{
  if (!myDetectedElemMap.IsNull())
  {
    myDetectedElemMap->ChangeMap().Clear();
  }
  if (!myDetectedNodeMap.IsNull())
  {
    myDetectedNodeMap->ChangeMap().Clear();
  }
  myMinDepthElem           = RealLast();
  myMinDepthNode           = RealLast();
  myMinDepthEdge           = RealLast();
  myDetectedElem           = -1;
  myDetectedNode           = -1;
  myDetectedEdgeNode1      = -1;
  myDetectedEdgeNode2      = -1;
  const bool toDetectRange = !myDetectedElemMap.IsNull() || !myDetectedNodeMap.IsNull();
  if (myGroups.IsNull() || theMgr.GetActiveSelectionType() == SelectMgr_SelectionType_Point
      || !toDetectRange)
  {
    if (!matches(theMgr, thePickResult, toDetectRange))
    {
      return Standard_False;
    }

    if (!myGroups.IsNull() && myDetectedIdx != -1)
    {
      const Standard_Integer                          anIndex = myBvhIndices.Index(myDetectedIdx);
      const Handle(Select3D_SensitivePrimitiveArray)& aLastGroup = myGroups->Value(anIndex);
      myMinDepthElem                                             = aLastGroup->myMinDepthElem;
      myMinDepthNode                                             = aLastGroup->myMinDepthNode;
      myMinDepthEdge                                             = aLastGroup->myMinDepthEdge;
      myDetectedElem                                             = aLastGroup->myDetectedElem;
      myDetectedNode                                             = aLastGroup->myDetectedNode;
      myDetectedEdgeNode1                                        = aLastGroup->myDetectedEdgeNode1;
      myDetectedEdgeNode2                                        = aLastGroup->myDetectedEdgeNode2;
    }
    return Standard_True;
  }

  SelectBasics_PickResult aPickResult;
  bool                    hasResults = false;
  for (Standard_Integer aGroupIter = 0; aGroupIter < myBvhIndices.NbElements; ++aGroupIter)
  {
    const Standard_Integer                    anElemIdx = myBvhIndices.Index(aGroupIter);
    Handle(Select3D_SensitivePrimitiveArray)& aChild    = myGroups->ChangeValue(anElemIdx);
    if (aChild->Matches(theMgr, aPickResult))
    {
      hasResults = true;
      if (!myDetectedElemMap.IsNull())
      {
        myDetectedElemMap->ChangeMap().Unite(aChild->myDetectedElemMap->Map());
      }
      if (!myDetectedNodeMap.IsNull())
      {
        myDetectedNodeMap->ChangeMap().Unite(aChild->myDetectedNodeMap->Map());
      }
      if (thePickResult.Depth() > aPickResult.Depth())
      {
        myDetectedIdx = aGroupIter;
        thePickResult = aPickResult;
      }
    }
  }
  if (!hasResults)
  {
    return Standard_False;
  }
  thePickResult.SetDistToGeomCenter(theMgr.DistToGeometryCenter(CenterOfGeometry()));
  return Standard_True;
}

//=================================================================================================

Standard_Boolean Select3D_SensitivePrimitiveArray::overlapsElement(
  SelectBasics_PickResult&             thePickResult,
  SelectBasics_SelectingVolumeManager& theMgr,
  Standard_Integer                     theElemIdx,
  Standard_Boolean                     theIsFullInside)
{
  const Standard_Integer anElemIdx = myBvhIndices.Index(theElemIdx);
  if (!myGroups.IsNull())
  {
    return myGroups->Value(anElemIdx)->Matches(theMgr, thePickResult);
  }

  const Standard_Integer  aPatchSize = myBvhIndices.PatchSize(theElemIdx);
  Select3D_BndBox3d       aBox;
  Standard_Boolean        aResult = Standard_False;
  SelectBasics_PickResult aPickResult;
  switch (myPrimType)
  {
    case Graphic3d_TOPA_POINTS: {
      for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
      {
        const Standard_Integer anIndexOffset = (anElemIdx + anElemIter);
        const Standard_Integer aPointIndex =
          !myIndices.IsNull() ? myIndices->Index(anIndexOffset) : anIndexOffset;
        gp_Pnt aPoint;
        if (myIs3d)
        {
          aPoint = vecToPnt(getPosVec3(aPointIndex));
        }
        else
        {
          aPoint = vecToPnt(getPosVec2(aPointIndex));
        }

        if (myToDetectNode || myToDetectElem)
        {
          if (theIsFullInside || theMgr.OverlapsPoint(aPoint, aPickResult))
          {
            if (aPickResult.Depth() <= myMinDepthNode)
            {
              myDetectedElem = myDetectedNode = aPointIndex;
              myMinDepthElem = myMinDepthNode = aPickResult.Depth();
            }
            if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
            {
              if (!myDetectedElemMap.IsNull())
              {
                myDetectedElemMap->ChangeMap().Add(aPointIndex);
              }
              if (!myDetectedNodeMap.IsNull())
              {
                myDetectedNodeMap->ChangeMap().Add(aPointIndex);
              }
            }
            aResult = Standard_True;
          }
        }
        thePickResult = SelectBasics_PickResult::Min(thePickResult, aPickResult);
      }
      break;
    }
    case Graphic3d_TOPA_TRIANGLES: {
      Graphic3d_Vec3i aTriNodes;
      for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
      {
        const Standard_Integer aTriIndex     = anElemIdx + anElemIter;
        const Standard_Integer anIndexOffset = aTriIndex * 3;
        getTriIndices(myIndices, anIndexOffset, aTriNodes);
        gp_Pnt aPnts[3];
        if (myIs3d)
        {
          aPnts[0] = vecToPnt(getPosVec3(aTriNodes[0]));
          aPnts[1] = vecToPnt(getPosVec3(aTriNodes[1]));
          aPnts[2] = vecToPnt(getPosVec3(aTriNodes[2]));
        }
        else
        {
          aPnts[0] = vecToPnt(getPosVec2(aTriNodes[0]));
          aPnts[1] = vecToPnt(getPosVec2(aTriNodes[1]));
          aPnts[2] = vecToPnt(getPosVec2(aTriNodes[2]));
        }

        if (myToDetectElem)
        {
          if (theIsFullInside
              || theMgr.OverlapsTriangle(aPnts[0],
                                         aPnts[1],
                                         aPnts[2],
                                         Select3D_TOS_INTERIOR,
                                         aPickResult))
          {
            if (aPickResult.Depth() <= myMinDepthElem)
            {
              myDetectedElem = aTriIndex;
              myMinDepthElem = aPickResult.Depth();
            }
            aResult = Standard_True;
            if (!myDetectedElemMap.IsNull()
                && theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
            {
              myDetectedElemMap->ChangeMap().Add(aTriIndex);
            }
          }
        }
        if (myToDetectNode)
        {
          for (int aNodeIter = 0; aNodeIter < 3; ++aNodeIter)
          {
            if (theIsFullInside || theMgr.OverlapsPoint(aPnts[aNodeIter], aPickResult))
            {
              if (aPickResult.Depth() <= myMinDepthNode)
              {
                myDetectedNode = aTriNodes[aNodeIter];
                myMinDepthNode = aPickResult.Depth();
              }
              if (!myDetectedNodeMap.IsNull()
                  && theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
              {
                myDetectedNodeMap->ChangeMap().Add(aTriNodes[aNodeIter]);
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
            if (theIsFullInside
                || theMgr.OverlapsSegment(aPnts[aNode1], aPnts[aNode2], aPickResult))
            {
              if (aPickResult.Depth() <= myMinDepthEdge)
              {
                myDetectedEdgeNode1 = aTriNodes[aNode1];
                myDetectedEdgeNode2 = aTriNodes[aNode2];
                myMinDepthEdge      = aPickResult.Depth();
              }
              aResult = Standard_True;
            }
          }
        }
        thePickResult = SelectBasics_PickResult::Min(thePickResult, aPickResult);
      }
      break;
    }
    default: {
      return Standard_False;
    }
  }

  return aResult;
}

//=================================================================================================

Standard_Real Select3D_SensitivePrimitiveArray::distanceToCOG(
  SelectBasics_SelectingVolumeManager& theMgr)
{
  return theMgr.DistToGeometryCenter(myCDG3D);
}

//=================================================================================================

Standard_Boolean Select3D_SensitivePrimitiveArray::elementIsInside(
  SelectBasics_SelectingVolumeManager& theMgr,
  Standard_Integer                     theElemIdx,
  Standard_Boolean                     theIsFullInside)
{
  const Standard_Integer anElemIdx = myBvhIndices.Index(theElemIdx);
  if (!myGroups.IsNull())
  {
    SelectBasics_PickResult aDummy;
    return myGroups->Value(anElemIdx)->Matches(theMgr, aDummy);
  }

  const Standard_Integer aPatchSize = myBvhIndices.PatchSize(theElemIdx);
  switch (myPrimType)
  {
    case Graphic3d_TOPA_POINTS: {
      for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
      {
        const Standard_Integer anIndexOffset = (anElemIdx + anElemIter);
        const Standard_Integer aPointIndex =
          !myIndices.IsNull() ? myIndices->Index(anIndexOffset) : anIndexOffset;
        gp_Pnt aPoint;
        if (myIs3d)
        {
          aPoint = vecToPnt(getPosVec3(aPointIndex));
        }
        else
        {
          aPoint = vecToPnt(getPosVec2(aPointIndex));
        }
        if (!theIsFullInside && !theMgr.OverlapsPoint(aPoint))
        {
          return Standard_False;
        }

        if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
        {
          if (!myDetectedElemMap.IsNull())
          {
            myDetectedElemMap->ChangeMap().Add(aPointIndex);
          }
          if (!myDetectedNodeMap.IsNull())
          {
            myDetectedNodeMap->ChangeMap().Add(aPointIndex);
          }
        }
      }
      return Standard_True;
    }
    case Graphic3d_TOPA_TRIANGLES: {
      Graphic3d_Vec3i aTriNodes;
      for (Standard_Integer anElemIter = 0; anElemIter < aPatchSize; ++anElemIter)
      {
        const Standard_Integer aTriIndex     = anElemIdx + anElemIter;
        const Standard_Integer anIndexOffset = aTriIndex * 3;
        getTriIndices(myIndices, anIndexOffset, aTriNodes);
        gp_Pnt aPnts[3];
        if (myIs3d)
        {
          aPnts[0] = vecToPnt(getPosVec3(aTriNodes[0]));
          aPnts[1] = vecToPnt(getPosVec3(aTriNodes[1]));
          aPnts[2] = vecToPnt(getPosVec3(aTriNodes[2]));
        }
        else
        {
          aPnts[0] = vecToPnt(getPosVec2(aTriNodes[0]));
          aPnts[1] = vecToPnt(getPosVec2(aTriNodes[1]));
          aPnts[2] = vecToPnt(getPosVec2(aTriNodes[2]));
        }

        if (!theIsFullInside
            && (!theMgr.OverlapsPoint(aPnts[0]) || !theMgr.OverlapsPoint(aPnts[1])
                || !theMgr.OverlapsPoint(aPnts[2])))
        {
          return Standard_False;
        }

        if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
        {
          if (!myDetectedElemMap.IsNull())
          {
            myDetectedElemMap->ChangeMap().Add(aTriIndex);
          }
          if (!myDetectedNodeMap.IsNull())
          {
            myDetectedNodeMap->ChangeMap().Add(aTriNodes[0]);
            myDetectedNodeMap->ChangeMap().Add(aTriNodes[1]);
            myDetectedNodeMap->ChangeMap().Add(aTriNodes[2]);
          }
        }
      }
      return Standard_True;
    }
    default: {
      return Standard_False;
    }
  }
}

//=================================================================================================

void Select3D_SensitivePrimitiveArray::DumpJson(Standard_OStream& theOStream,
                                                Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Select3D_SensitiveSet)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPosStride)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPrimType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIndexLower)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIndexUpper)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPatchSizeMax)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPatchDistance)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIs3d)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myInitLocation)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBndBox)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMinDepthElem)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMinDepthNode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMinDepthEdge)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDetectedElem)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDetectedNode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDetectedEdgeNode1)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDetectedEdgeNode2)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myToDetectElem)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myToDetectNode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myToDetectEdge)
}
