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

#ifndef _BVH_QuadTree_Header
#define _BVH_QuadTree_Header

#include <BVH_Tree.hxx>

//! Specialization of quad BVH (QBVH) tree.
template<class T, int N>
class BVH_Tree<T, N, BVH_QuadTree> : public BVH_TreeBase<T, N>
{
public: //! @name general methods

  //! Creates new empty BVH tree.
  BVH_Tree() : BVH_TreeBase<T, N>() { }

  //! Returns index of the K-th child of the given inner node.
  //! \tparam K the index of node child (from 0 to 3)
  template<int K>
  int Child (const int theNodeIndex) const
  {
    return BVH::Array<int, 4>::Value (this->myNodeInfoBuffer, theNodeIndex).y() + K;
  }

};

#endif // _BVH_QuadTree_Header
