// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _BVH_Constants_Header
#define _BVH_Constants_Header

enum
{
  //! The optimal tree depth.
  //! Should be in sync with maximum stack size while traversing the tree - don't pass the trees of
  //! greater depth to OCCT algorithms!
  BVH_Constants_MaxTreeDepth = 32,

  //! Leaf node size optimal for complex nodes,
  //! e.g. for upper-level BVH trees within multi-level structure (nodes point to another BVH
  //! trees).
  BVH_Constants_LeafNodeSizeSingle = 1,
  //! Average leaf node size (4 primitive per leaf), optimal for average tree nodes.
  BVH_Constants_LeafNodeSizeAverage = 4,
  //! Default leaf node size (5 primitives per leaf).
  BVH_Constants_LeafNodeSizeDefault = 5,
  //! Leaf node size (8 primitives per leaf), optimal for small tree nodes (e.g. triangles).
  BVH_Constants_LeafNodeSizeSmall = 8,

  //! The optimal number of bins for binned builder.
  BVH_Constants_NbBinsOptimal = 32,
  //! The maximum number of bins for binned builder (giving the best traversal time at cost of
  //! longer tree construction time).
  BVH_Constants_NbBinsBest = 48,
};

namespace BVH
{
//! Minimum node size to split.
const double THE_NODE_MIN_SIZE = 1e-5;
} // namespace BVH

#endif // _BVH_Constants_Header
