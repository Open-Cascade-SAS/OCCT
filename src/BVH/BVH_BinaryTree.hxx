// Created on: 2016-06-20
// Created by: Denis BOGOLEPOV
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

#ifndef _BVH_BinaryTree_Header
#define _BVH_BinaryTree_Header

#include <BVH_QuadTree.hxx>

//! Specialization of binary BVH tree.
template<class T, int N>
class BVH_Tree<T, N, BVH_BinaryTree> : public BVH_TreeBase<T, N>
{
public: //! @name custom data types

  typedef typename BVH_TreeBase<T, N>::BVH_VecNt BVH_VecNt;

public: //! @name methods for accessing individual nodes

  //! Creates new empty BVH tree.
  BVH_Tree() : BVH_TreeBase<T, N>() { }

  //! Sets node type to 'outer'.
  void SetOuter (const int theNodeIndex);

  //! Sets node type to 'inner'.
  void SetInner (const int theNodeIndex);

  //! Returns index of the K-th child of the given inner node.
  //! \tparam K the index of node child (0 or 1)
  template<int K>
  int Child (const int theNodeIndex) const;

  //! Returns index of the K-th child of the given inner node.
  //! \tparam K the index of node child (0 or 1)
  template<int K>
  int& Child (const int theNodeIndex);

public: //! @name methods for adding/removing tree nodes

  //! Removes all nodes from the tree.
  void Clear();

  //! Reserves internal BVH storage, so that it
  //! can contain the given number of BVH nodes.
  void Reserve (const int theNbNodes);

  //! Adds new leaf node to the BVH.
  int AddLeafNode (const BVH_VecNt& theMinPoint,
                   const BVH_VecNt& theMaxPoint,
                   const int        theBegElem,
                   const int        theEndElem);

  //! Adds new inner node to the BVH.
  int AddInnerNode (const BVH_VecNt& theMinPoint,
                    const BVH_VecNt& theMaxPoint,
                    const int        theLftChild,
                    const int        theRghChild);

  //! Adds new leaf node to the BVH.
  int AddLeafNode (const BVH_Box<T, N>& theAABB,
                   const int            theBegElem,
                   const int            theEndElem);

  //! Adds new inner node to the BVH.
  int AddInnerNode (const BVH_Box<T, N>& theAABB,
                    const int            theLftChild,
                    const int            theRghChild);

  //! Adds new leaf node to the BVH with UNINITIALIZED bounds.
  int AddLeafNode (const int theBegElem,
                   const int theEndElem);

  //! Adds new inner node to the BVH with UNINITIALIZED bounds.
  int AddInnerNode (const int theLftChild,
                    const int theRghChild);

public: //! @name methods specific to binary BVH

  //! Returns value of SAH (surface area heuristic).
  //! Allows to compare the quality of BVH trees constructed for
  //! the same sets of geometric objects with different methods.
  T EstimateSAH() const;

  //! Collapses the tree into QBVH an returns it. As a result, each
  //! 2-nd level of current tree is kept and the rest are discarded.
  BVH_Tree<T, N, BVH_QuadTree>* CollapseToQuadTree() const;

};

#include <BVH_BinaryTree.lxx>

#endif // _BVH_BinaryTree_Header
