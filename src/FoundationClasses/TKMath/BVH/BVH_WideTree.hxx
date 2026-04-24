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

#ifndef _BVH_WideTree_Header
#define _BVH_WideTree_Header

#include <BVH_Tree.hxx>

//! Type tag for a W-wide BVH (BVH8 / BVH16, also called OBVH / HBVH).
//! Inner nodes hold up to W children stored at consecutive indices, which
//! lets a single SIMD ray-box test cover the whole fan-out in one instruction
//! when the kernel width matches the BVH width:
//!   - W=8  pairs with AVX2     (__m256, 8 fp32 lanes in ymm)
//!   - W=16 pairs with AVX-512  (__m512, 16 fp32 lanes in zmm)
//! BVH4 uses BVH_QuadTree separately; SSE2 (__m128) is the natural fit there.
//! Built from a binary tree via BVH_Tree<T,N,BVH_BinaryTree>::CollapseToWide<W>().
template <int W>
struct BVH_WideTree
{
  static_assert(W == 8 || W == 16,
                "BVH_WideTree only supports W=8 or W=16; use BVH_QuadTree for W=4");
  static constexpr int Width = W;
  // Number of binary levels collapsed into one wide inner node (2^Log2W = W).
  static constexpr int Log2W = (W == 8) ? 3 : 4;
};

//! Specialization of the BVH tree for a W-wide layout.
template <class T, int N, int W>
class BVH_Tree<T, N, BVH_WideTree<W>> : public BVH_TreeBase<T, N>
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
  //! (range 1..W).
  //! @tparam K the index of the node child (from 0 to W-1)
  template <int K>
  int Child(const int theNodeIndex) const
  {
    static_assert(K >= 0 && K < W, "Child index out of range for BVH_WideTree<W>");
    return BVH::Array<int, 4>::Value(this->myNodeInfoBuffer, theNodeIndex).y() + K;
  }
};

#endif // _BVH_WideTree_Header
