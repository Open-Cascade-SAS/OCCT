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

#ifndef _Select3D_SensitivePrimitiveArray_Header
#define _Select3D_SensitivePrimitiveArray_Header

#include <Graphic3d_IndexBuffer.hxx>
#include <Graphic3d_TypeOfPrimitiveArray.hxx>
#include <Select3D_SensitiveSet.hxx>
#include <Select3D_BVHIndexBuffer.hxx>
#include <TColStd_HPackedMapOfInteger.hxx>

//! Sensitive for triangulation or point set defined by Primitive Array.
//! The primitives can be optionally combined into patches within BVH tree
//! to reduce its building time in expense of extra traverse time.
class Select3D_SensitivePrimitiveArray : public Select3D_SensitiveSet
{

public:

  //! Constructs an empty sensitive object.
  Standard_EXPORT Select3D_SensitivePrimitiveArray (const Handle(SelectMgr_EntityOwner)& theOwnerId);

  //! Return patch size limit (1 by default).
  Standard_Integer PatchSizeMax() const { return myPatchSizeMax; }

  //! Assign patch size limit.
  //! Should be set before initialization.
  void SetPatchSizeMax (const Standard_Integer thePatchSizeMax) { myPatchSizeMax = thePatchSizeMax; }

  //! Maximum allowed distance between consequential elements in patch (ShortRealLast() by default).
  //! Has no effect on indexed triangulation.
  float PatchDistance() const { return myPatchDistance; }

  //! Assign patch distance limit.
  //! Should be set before initialization.
  void SetPatchDistance (const float thePatchDistMax) { myPatchDistance = thePatchDistMax; }

  //! Initialize the sensitive object from triangualtion.
  //! The sub-triangulation can be specified by arguments theIndexLower and theIndexUpper
  //! (these are for iterating theIndices, not to restrict the actual index values!).
  //! @param theVerts        attributes array containing Graphic3d_TOA_POS with type Graphic3d_TOD_VEC3 or Graphic3d_TOD_VEC2
  //! @param theIndices      index array defining triangulation
  //! @param theInitLoc      location
  //! @param theIndexLower   the theIndices range - first value (inclusive), starting from 0 and multiple by 3
  //! @param theIndexUpper   the theIndices range - last  value (inclusive), upto theIndices->NbElements-1 and multiple by 3
  //! @param theToEvalMinMax compute bounding box within initialization
  //! @param theNbGroups     number of groups to split the vertex array into several parts
  Standard_EXPORT bool InitTriangulation (const Handle(Graphic3d_Buffer)&      theVerts,
                                          const Handle(Graphic3d_IndexBuffer)& theIndices,
                                          const TopLoc_Location&               theInitLoc,
                                          const Standard_Integer               theIndexLower,
                                          const Standard_Integer               theIndexUpper,
                                          const bool                           theToEvalMinMax = true,
                                          const Standard_Integer               theNbGroups = 1);

  //! Initialize the sensitive object from triangualtion.
  //! @param theVerts        attributes array containing Graphic3d_TOA_POS with type Graphic3d_TOD_VEC3 or Graphic3d_TOD_VEC2
  //! @param theIndices      index array defining triangulation
  //! @param theInitLoc      location
  //! @param theToEvalMinMax compute bounding box within initialization
  //! @param theNbGroups     number of groups to split the vertex array into several parts
  bool InitTriangulation (const Handle(Graphic3d_Buffer)&      theVerts,
                          const Handle(Graphic3d_IndexBuffer)& theIndices,
                          const TopLoc_Location&               theInitLoc,
                          const bool                           theToEvalMinMax = true,
                          const Standard_Integer               theNbGroups = 1)
  {
    const Standard_Integer anUpper = !theIndices.IsNull() ? (theIndices->NbElements - 1)
                                                          : (!theVerts.IsNull() ? (theVerts->NbElements - 1) : 0);
    return InitTriangulation (theVerts, theIndices, theInitLoc, 0, anUpper, theToEvalMinMax, theNbGroups);
  }

  //! Initialize the sensitive object from point set.
  //! The sub-set of points can be specified by arguments theIndexLower and theIndexUpper
  //! (these are for iterating theIndices, not to restrict the actual index values!).
  //! @param theVerts        attributes array containing Graphic3d_TOA_POS with type Graphic3d_TOD_VEC3 or Graphic3d_TOD_VEC2
  //! @param theIndices      index array defining points
  //! @param theInitLoc      location
  //! @param theIndexLower   the theIndices range - first value (inclusive), starting from 0
  //! @param theIndexUpper   the theIndices range - last  value (inclusive), upto theIndices->NbElements-1
  //! @param theToEvalMinMax compute bounding box within initialization
  //! @param theNbGroups     number of groups to split the vertex array into several parts
  Standard_EXPORT bool InitPoints (const Handle(Graphic3d_Buffer)&      theVerts,
                                   const Handle(Graphic3d_IndexBuffer)& theIndices,
                                   const TopLoc_Location&               theInitLoc,
                                   const Standard_Integer               theIndexLower,
                                   const Standard_Integer               theIndexUpper,
                                   const bool                           theToEvalMinMax = true,
                                   const Standard_Integer               theNbGroups = 1);

  //! Initialize the sensitive object from point set.
  //! @param theVerts        attributes array containing Graphic3d_TOA_POS with type Graphic3d_TOD_VEC3 or Graphic3d_TOD_VEC2
  //! @param theIndices      index array to define subset of points
  //! @param theInitLoc      location
  //! @param theToEvalMinMax compute bounding box within initialization
  //! @param theNbGroups     number of groups to split the vertex array into several parts
  bool InitPoints (const Handle(Graphic3d_Buffer)&      theVerts,
                   const Handle(Graphic3d_IndexBuffer)& theIndices,
                   const TopLoc_Location&               theInitLoc,
                   const bool                           theToEvalMinMax = true,
                   const Standard_Integer               theNbGroups = 1)
  {
    const Standard_Integer anUpper = !theIndices.IsNull() ? (theIndices->NbElements - 1)
                                                          : (!theVerts.IsNull() ? (theVerts->NbElements - 1) : 0);
    return InitPoints (theVerts, theIndices, theInitLoc, 0, anUpper, theToEvalMinMax, theNbGroups);
  }

  //! Initialize the sensitive object from point set.
  //! @param theVerts        attributes array containing Graphic3d_TOA_POS with type Graphic3d_TOD_VEC3 or Graphic3d_TOD_VEC2
  //! @param theInitLoc      location
  //! @param theToEvalMinMax compute bounding box within initialization
  //! @param theNbGroups     number of groups to split the vertex array into several parts
  bool InitPoints (const Handle(Graphic3d_Buffer)& theVerts,
                   const TopLoc_Location&          theInitLoc,
                   const bool                      theToEvalMinMax = true,
                   const Standard_Integer          theNbGroups = 1)
  {
    const Standard_Integer anUpper = !theVerts.IsNull() ? (theVerts->NbElements - 1) : 0;
    return InitPoints (theVerts, Handle(Graphic3d_IndexBuffer)(), theInitLoc, 0, anUpper, theToEvalMinMax, theNbGroups);
  }

  //! Assign new not transformed bounding box.
  void SetMinMax (double theMinX, double theMinY, double theMinZ,
                  double theMaxX, double theMaxY, double theMaxZ)
  {
    myBndBox = Select3D_BndBox3d (SelectMgr_Vec3 (theMinX, theMinY, theMinZ),
                                  SelectMgr_Vec3 (theMaxX, theMaxY, theMaxZ));
    if (!myGroups.IsNull())
    {
      for (Select3D_PrimArraySubGroupArray::Iterator aGroupIter (*myGroups); aGroupIter.More(); aGroupIter.Next())
      {
        aGroupIter.Value()->myBndBox = myBndBox;
      }
    }
  }

  //! Return flag to keep index of last topmost detected element, TRUE by default.
  bool ToDetectElements() const { return myToDetectElem; }

  //! Setup keeping of the index of last topmost detected element (axis picking).
  void SetDetectElements (bool theToDetect) { myToDetectElem = theToDetect; }

  //! Return flag to keep index map of last detected elements, FALSE by default (rectangle selection).
  bool ToDetectElementMap() const { return !myDetectedElemMap.IsNull(); }

  //! Setup keeping of the index map of last detected elements (rectangle selection).
  Standard_EXPORT void SetDetectElementMap (bool theToDetect);

  //! Return flag to keep index of last topmost detected node, FALSE by default.
  bool ToDetectNodes() const { return myToDetectNode; }

  //! Setup keeping of the index of last topmost detected node (for axis picking).
  void SetDetectNodes (bool theToDetect) { myToDetectNode = theToDetect; }

  //! Return flag to keep index map of last detected nodes, FALSE by default (rectangle selection).
  bool ToDetectNodeMap() const { return !myDetectedNodeMap.IsNull(); }

  //! Setup keeping of the index map of last detected nodes (rectangle selection).
  Standard_EXPORT void SetDetectNodeMap (bool theToDetect);

  //! Return flag to keep index of last topmost detected edge, FALSE by default.
  bool ToDetectEdges() const { return myToDetectEdge; }

  //! Setup keeping of the index of last topmost detected edge (axis picking).
  void SetDetectEdges (bool theToDetect) { myToDetectEdge = theToDetect; }

  //! Return last topmost detected element or -1 if undefined (axis picking).
  Standard_Integer LastDetectedElement() const { return myDetectedElem; }

  //! Return the index map of last detected elements (rectangle selection).
  const Handle(TColStd_HPackedMapOfInteger)& LastDetectedElementMap() const { return myDetectedElemMap; }

  //! Return last topmost detected node or -1 if undefined (axis picking).
  Standard_Integer LastDetectedNode() const { return myDetectedNode; }

  //! Return the index map of last detected nodes (rectangle selection).
  const Handle(TColStd_HPackedMapOfInteger)& LastDetectedNodeMap() const { return myDetectedNodeMap; }

  //! Return the first node of last topmost detected edge or -1 if undefined (axis picking).
  Standard_Integer LastDetectedEdgeNode1() const { return myDetectedEdgeNode1; }

  //! Return the second node of last topmost detected edge or -1 if undefined (axis picking).
  Standard_Integer LastDetectedEdgeNode2() const { return myDetectedEdgeNode2; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

public:

  //! Checks whether the sensitive entity is overlapped by current selecting volume.
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult&             thePickResult) Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Returns the length of array of triangles or edges
  Standard_EXPORT virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns the amount of nodes in triangulation
  virtual Standard_Integer NbSubElements() const Standard_OVERRIDE
  {
    return !myGroups.IsNull() ? myGroups->Size() : myBvhIndices.NbElements;
  }

  //! Returns bounding box of triangle/edge with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box (const Standard_Integer theIdx) const Standard_OVERRIDE;

  //! Returns geometry center of triangle/edge with index theIdx
  //! in array along the given axis theAxis
  Standard_EXPORT virtual Standard_Real Center (const Standard_Integer theIdx,
                                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps items with indexes theIdx1 and theIdx2 in array
  Standard_EXPORT virtual void Swap (const Standard_Integer theIdx1,
                                     const Standard_Integer theIdx2) Standard_OVERRIDE;

  //! Returns bounding box of the triangulation. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of triangulation. If location transformation
  //! is set, it will be applied
  virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE
  {
    return myCDG3D;
  }

  //! Returns true if the shape corresponding to the entity has init location
  virtual Standard_Boolean HasInitLocation() const Standard_OVERRIDE
  {
    return !myInitLocation.IsIdentity();
  }

  //! Returns inversed location transformation matrix if the shape corresponding
  //! to this entity has init location set. Otherwise, returns identity matrix.
  virtual gp_GTrsf InvInitLocation() const Standard_OVERRIDE
  {
    return myInvInitLocation;
  }

  //! Sets the owner for all entities in group
  Standard_EXPORT virtual void Set (const Handle(SelectMgr_EntityOwner)& theOwnerId) Standard_OVERRIDE;

  //! Builds BVH tree for sensitive set.
  Standard_EXPORT virtual void BVH() Standard_OVERRIDE;

protected:

  //! Compute bounding box.
  Standard_EXPORT void computeBoundingBox();

  //! Inner function for transformation application to bounding
  //! box of the triangulation
  Standard_EXPORT Select3D_BndBox3d applyTransformation();

  //! Auxiliary getter.
  const Graphic3d_Vec3& getPosVec3 (const Standard_Integer theIndex) const
  {
    return *reinterpret_cast<const Graphic3d_Vec3* >(myPosData + myPosStride * theIndex);
  }

  //! Auxiliary getter.
  const Graphic3d_Vec2& getPosVec2 (const Standard_Integer theIndex) const
  {
    return *reinterpret_cast<const Graphic3d_Vec2* >(myPosData + myPosStride * theIndex);
  }

  //! Checks whether the element with index theIdx overlaps the current selecting volume
  Standard_EXPORT virtual Standard_Boolean overlapsElement (SelectBasics_PickResult& thePickResult,
                                                            SelectBasics_SelectingVolumeManager& theMgr,
                                                            Standard_Integer theElemIdx,
                                                            Standard_Boolean theIsFullInside) Standard_OVERRIDE;

  //! Calculates distance from the 3d projection of used-picked screen point to center of the geometry
  Standard_EXPORT virtual Standard_Real distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr) Standard_OVERRIDE;

  //! Checks whether the entity with index theIdx is inside the current selecting volume
  Standard_EXPORT virtual Standard_Boolean elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                                            Standard_Integer theElemIdx,
                                                            Standard_Boolean theIsFullInside) Standard_OVERRIDE;

private:

  typedef NCollection_Shared<NCollection_Array1<Handle(Select3D_SensitivePrimitiveArray)> > Select3D_PrimArraySubGroupArray;
  struct Select3D_SensitivePrimitiveArray_InitFunctor;
  struct Select3D_SensitivePrimitiveArray_BVHFunctor;

private:

  Handle(Select3D_PrimArraySubGroupArray) myGroups;         //!< sub-groups of sensitive entities

  Handle(Graphic3d_Buffer)            myVerts;              //!< source data - nodes position
  Handle(Graphic3d_IndexBuffer)       myIndices;            //!< source data - primitive indexes
  const Standard_Byte*                myPosData;            //!< position vertex attribute data
  Standard_Size                       myPosStride;          //!< position vertex attribute stride in bytes
  Graphic3d_TypeOfPrimitiveArray      myPrimType;           //!< primitives type
  Standard_Integer                    myIndexLower;         //!< index range - first index in myIndices (inclusive)
  Standard_Integer                    myIndexUpper;         //!< index range - last  index in myIndices (inclusive)
  Standard_Integer                    myPatchSizeMax;       //!< patch size limit (1 by default)
  float                               myPatchDistance;      //!< distance between elements in patch
  bool                                myIs3d;               //!< flag indicating that position attribute has 3 components
  TopLoc_Location                     myInitLocation;
  gp_Pnt                              myCDG3D;              //!< Center of the whole triangulation
  Select3D_BVHIndexBuffer             myBvhIndices;         //!< Indexes of edges or triangles for BVH tree
  mutable Select3D_BndBox3d           myBndBox;             //!< Bounding box of the whole triangulation
  gp_GTrsf                            myInvInitLocation;
  Handle(TColStd_HPackedMapOfInteger) myDetectedElemMap;    //!< index map of last detected elements
  Handle(TColStd_HPackedMapOfInteger) myDetectedNodeMap;    //!< index map of last detected nodes
  Standard_Real                       myMinDepthElem;       //!< the depth of nearest detected element
  Standard_Real                       myMinDepthNode;       //!< the depth of nearest detected node
  Standard_Real                       myMinDepthEdge;       //!< the depth of nearest detected edge
  Standard_Integer                    myDetectedElem;       //!< index of last detected element
  Standard_Integer                    myDetectedNode;       //!< index of last detected node
  Standard_Integer                    myDetectedEdgeNode1;  //!< index of last detected edge node 1
  Standard_Integer                    myDetectedEdgeNode2;  //!< index of last detected edge node 2
  bool                                myToDetectElem;       //!< flag to keep info about last detected element
  bool                                myToDetectNode;       //!< flag to keep info about last detected node
  bool                                myToDetectEdge;       //!< flag to keep info about last detected edge

public:

  DEFINE_STANDARD_RTTIEXT(Select3D_SensitivePrimitiveArray, Select3D_SensitiveSet)

};

DEFINE_STANDARD_HANDLE(Select3D_SensitivePrimitiveArray, Select3D_SensitiveSet)

#endif // _Select3D_SensitivePrimitiveArray_Header
