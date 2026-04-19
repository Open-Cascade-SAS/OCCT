// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _BVH_OctTree_Header
#define _BVH_OctTree_Header

#include <BVH_Tree.hxx>

//! Type tag for an 8-wide BVH (BVH8 / OBVH).
//! Inner nodes hold up to 8 children stored at consecutive indices, which
//! lets a single AVX2 (__m256) ray-box test cover the whole fan-out in one
//! instruction. Built from a binary tree via
//! BVH_Tree<T,N,BVH_BinaryTree>::CollapseToOctTree().
struct BVH_OctTree
{
};

//! Specialization of the BVH tree for an 8-wide layout.
template <class T, int N>
class BVH_Tree<T, N, BVH_OctTree> : public BVH_TreeBase<T, N>
{
public: //! @name general methods
  //! Creates a new empty BVH tree.
  BVH_Tree()
      : BVH_TreeBase<T, N>()
  {
  }

  //! Returns index of the K-th child of the given inner node.
  //! Children of an inner node are stored at consecutive indices starting
  //! at NodeInfo[node].y(); the actual child count is NodeInfo[node].z() + 1
  //! (range 1..8).
  //! @tparam K the index of the node child (from 0 to 7)
  template <int K>
  int Child(const int theNodeIndex) const
  {
    return BVH::Array<int, 4>::Value(this->myNodeInfoBuffer, theNodeIndex).y() + K;
  }
};

#endif // _BVH_OctTree_Header
