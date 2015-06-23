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

#ifndef _BVH_BinnedBuilder_Header
#define _BVH_BinnedBuilder_Header

#include <BVH_QueueBuilder.hxx>

#include <algorithm>

//! Stores parameters of single bin (slice of AABB).
template<class T, int N>
struct BVH_Bin
{
  //! Creates new node bin.
  BVH_Bin() : Count (0) {}

  Standard_Integer Count; //!< Number of primitives in the bin
  BVH_Box<T, N>    Box;   //!< AABB of primitives in the bin
};

//! Performs construction of BVH tree using binned SAH algorithm. Number
//! of bins controls BVH quality in cost of construction time (greater -
//! better). For optimal results, use 32 - 48 bins. However, reasonable
//! performance is provided even for 4 - 8 bins (it is only 10-20% lower
//! in comparison with optimal settings). Note that multiple threads can
//! be used only with thread safe BVH primitive sets.
template<class T, int N, int Bins = 32>
class BVH_BinnedBuilder : public BVH_QueueBuilder<T, N>
{
public:

  //! Type of the array of bins of BVH tree node.
  typedef BVH_Bin<T, N> BVH_BinVector[Bins];

  //! Describes split plane candidate.
  struct BVH_SplitPlane
  {
    BVH_Bin<T, N> LftVoxel;
    BVH_Bin<T, N> RghVoxel;
  };

  //! Type of the array of split plane candidates.
  typedef BVH_SplitPlane BVH_SplitPlanes[Bins + 1];

public:

  //! Creates binned SAH BVH builder.
  BVH_BinnedBuilder (const Standard_Integer theLeafNodeSize = 5,
                     const Standard_Integer theMaxTreeDepth = 32,
                     const Standard_Boolean theDoMainSplits = 0,
                     const Standard_Integer theNumOfThreads = 1);

  //! Releases resources of binned SAH BVH builder.
  virtual ~BVH_BinnedBuilder();

protected:

  //! Performs splitting of the given BVH node.
  typename BVH_QueueBuilder<T, N>::BVH_ChildNodes BuildNode (BVH_Set<T, N>*         theSet,
                                                             BVH_Tree<T, N>*        theBVH,
                                                             const Standard_Integer theNode);

  //! Arranges node primitives into bins.
  virtual void GetSubVolumes (BVH_Set<T, N>*         theSet,
                              BVH_Tree<T, N>*        theBVH,
                              const Standard_Integer theNode,
                              BVH_BinVector&         theBins,
                              const Standard_Integer theAxis);

private:

  Standard_Boolean myUseMainAxis; //!< Defines whether to search for the best split or use the widest axis

};

#include <BVH_BinnedBuilder.lxx>

#endif // _BVH_BinnedBuilder_Header
