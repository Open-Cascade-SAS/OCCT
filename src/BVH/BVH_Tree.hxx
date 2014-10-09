// Created on: 2013-12-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef _BVH_Tree_Header
#define _BVH_Tree_Header

#include <Standard_Type.hxx>

#include <BVH_Box.hxx>

template<class T, int N> class BVH_Builder;

//! Stores parameters of bounding volume hierarchy (BVH).
//! Bounding volume hierarchy (BVH) organizes geometric objects in
//! the tree based on spatial relationships. Each node in the tree
//! contains an axis-aligned bounding box of all the objects below
//! it. Bounding volume hierarchies are used in many algorithms to
//! support efficient operations on the sets of geometric objects,
//! such as collision detection, ray-tracing, searching of nearest
//! objects, and view frustum culling.
template<class T, int N>
class BVH_Tree
{
  friend class BVH_Builder<T, N>;

public:

  typedef typename BVH_Box<T, N>::BVH_VecNt BVH_VecNt;

public:

  //! Creates new empty BVH tree.
  BVH_Tree() : myDepth (0)
  {
    //
  }

  //! Returns minimum point of the given node.
  BVH_VecNt& MinPoint (const Standard_Integer theNodeIndex)
  {
    return BVH::Array<T, N>::ChangeValue (myMinPointBuffer, theNodeIndex);
  }

  //! Returns maximum point of the given node.
  BVH_VecNt& MaxPoint (const Standard_Integer theNodeIndex)
  {
    return BVH::Array<T, N>::ChangeValue (myMaxPointBuffer, theNodeIndex);
  }

  //! Returns minimum point of the given node.
  const BVH_VecNt& MinPoint (const Standard_Integer theNodeIndex) const
  {
    return BVH::Array<T, N>::Value (myMinPointBuffer, theNodeIndex);
  }

  //! Returns maximum point of the given node.
  const BVH_VecNt& MaxPoint (const Standard_Integer theNodeIndex) const
  {
    return BVH::Array<T, N>::Value (myMaxPointBuffer, theNodeIndex);
  }

  //! Returns index of left child of the given inner node.
  Standard_Integer& LeftChild (const Standard_Integer theNodeIndex)
  {
    return BVH::Array<Standard_Integer, 4>::ChangeValue (myNodeInfoBuffer, theNodeIndex).y();
  }

  //! Returns index of left child of the given inner node.
  Standard_Integer LeftChild (const Standard_Integer theNodeIndex) const
  {
    return BVH::Array<Standard_Integer, 4>::Value (myNodeInfoBuffer, theNodeIndex).y();
  }

  //! Returns index of right child of the given inner node.
  Standard_Integer& RightChild (const Standard_Integer theNodeIndex)
  {
    return BVH::Array<Standard_Integer, 4>::ChangeValue (myNodeInfoBuffer, theNodeIndex).z();
  }

  //! Returns index of right child of the given inner node.
  Standard_Integer RightChild (const Standard_Integer theNodeIndex) const
  {
    return BVH::Array<Standard_Integer, 4>::Value (myNodeInfoBuffer, theNodeIndex).z();
  }

  //! Returns index of first primitive of the given leaf node.
  Standard_Integer& BegPrimitive (const Standard_Integer theNodeIndex)
  {
    return BVH::Array<Standard_Integer, 4>::ChangeValue (myNodeInfoBuffer, theNodeIndex).y();
  }

  //! Returns index of first primitive of the given leaf node.
  Standard_Integer BegPrimitive (const Standard_Integer theNodeIndex) const
  {
    return BVH::Array<Standard_Integer, 4>::Value (myNodeInfoBuffer, theNodeIndex).y();
  }

  //! Returns index of last primitive of the given leaf node.
  Standard_Integer& EndPrimitive (const Standard_Integer theNodeIndex)
  {
    return BVH::Array<Standard_Integer, 4>::ChangeValue (myNodeInfoBuffer, theNodeIndex).z();
  }

  //! Returns index of last primitive of the given leaf node.
  Standard_Integer EndPrimitive (const Standard_Integer theNodeIndex) const
  {
    return BVH::Array<Standard_Integer, 4>::Value (myNodeInfoBuffer, theNodeIndex).z();
  }

  //! Returns number of primitives for the given tree node.
  Standard_Integer NbPrimitives (const Standard_Integer theNodeIndex) const
  {
    return EndPrimitive (theNodeIndex) - BegPrimitive (theNodeIndex) + 1;
  }

  //! Returns level (depth) of the given node.
  Standard_Integer& Level (const Standard_Integer theNodeIndex)
  {
    return BVH::Array<Standard_Integer, 4>::ChangeValue (myNodeInfoBuffer, theNodeIndex).w();
  }

  //! Returns level (depth) of the given node.
  Standard_Integer Level (const Standard_Integer theNodeIndex) const
  {
    return BVH::Array<Standard_Integer, 4>::Value (myNodeInfoBuffer, theNodeIndex).w();
  }

  //! Is node a leaf (outer)?
  Standard_Boolean IsOuter (const Standard_Integer theNodeIndex) const
  {
    return BVH::Array<Standard_Integer, 4>::Value (myNodeInfoBuffer, theNodeIndex).x() > 0;
  }

  //! Sets node type to 'outer'.
  void SetOuter (const Standard_Integer theNodeIndex)
  {
    BVH::Array<Standard_Integer, 4>::ChangeValue (myNodeInfoBuffer, theNodeIndex).x() = 1;
  }

  //! Sets node type to 'inner'.
  void SetInner (const Standard_Integer theNodeIndex)
  {
    BVH::Array<Standard_Integer, 4>::ChangeValue (myNodeInfoBuffer, theNodeIndex).x() = 0;
  }

  //! Returns total number of BVH nodes.
  Standard_Integer Length() const
  {
    return BVH::Array<Standard_Integer, 4>::Size (myNodeInfoBuffer);
  }

  //! Returns depth of BVH tree from last build.
  Standard_Integer Depth() const
  {
    return myDepth;
  }

public:

  //! Removes all BVH nodes.
  void Clear();

  //! Adds new leaf node to the BVH.
  Standard_Integer AddLeafNode (const BVH_VecNt&       theMinPoint,
                                const BVH_VecNt&       theMaxPoint,
                                const Standard_Integer theBegElem,
                                const Standard_Integer theEndElem);

  //! Adds new inner node to the BVH.
  Standard_Integer AddInnerNode (const BVH_VecNt&       theMinPoint,
                                 const BVH_VecNt&       theMaxPoint,
                                 const Standard_Integer theLftChild,
                                 const Standard_Integer theRghChild);

  //! Adds new leaf node to the BVH.
  Standard_Integer AddLeafNode (const BVH_Box<T, N>&   theAABB,
                                const Standard_Integer theBegElem,
                                const Standard_Integer theEndElem);

  //! Adds new inner node to the BVH.
  Standard_Integer AddInnerNode (const BVH_Box<T, N>&   theAABB,
                                 const Standard_Integer theLftChild,
                                 const Standard_Integer theRghChild);

  //! Adds new leaf node to the BVH with UNINITIALIZED bounds.
  Standard_Integer AddLeafNode (const Standard_Integer theBegElem,
                                const Standard_Integer theEndElem);

  //! Adds new inner node to the BVH with UNINITIALIZED bounds.
  Standard_Integer AddInnerNode (const Standard_Integer theLftChild,
                                 const Standard_Integer theRghChild);

  //! Returns value of SAH (surface area heuristic).
  //! Allows to compare the quality of BVH trees constructed for
  //! the same sets of geometric objects with different methods.
  T EstimateSAH() const;

public:

  //! Returns array of node min points.
  typename BVH::ArrayType<T, N>::Type& MinPointBuffer()
  {
    return myMinPointBuffer;
  }

  //! Returns array of node min points.
  const typename BVH::ArrayType<T, N>::Type& MinPointBuffer() const
  {
    return myMinPointBuffer;
  }

  //! Returns array of node max points.
  typename BVH::ArrayType<T, N>::Type& MaxPointBuffer()
  {
    return myMaxPointBuffer;
  }

  //! Returns array of node max points.
  const typename BVH::ArrayType<T, N>::Type& MaxPointBuffer() const
  {
    return myMaxPointBuffer;
  }

  //! Returns array of node data records.
  BVH_Array4i& NodeInfoBuffer()
  {
    return myNodeInfoBuffer;
  }

  //! Returns array of node data records.
  const BVH_Array4i& NodeInfoBuffer() const
  {
    return myNodeInfoBuffer;
  }

protected:

  //! Array of node minimum points.
  typename BVH::ArrayType<T, N>::Type myMinPointBuffer;

  //! Array of node maximum points.
  typename BVH::ArrayType<T, N>::Type myMaxPointBuffer;

  //! Array of node data records.
  BVH_Array4i myNodeInfoBuffer;

  //! Depth of constructed tree.
  Standard_Integer myDepth;

};

#include <BVH_Tree.lxx>

#endif // _BVH_Tree_Header
