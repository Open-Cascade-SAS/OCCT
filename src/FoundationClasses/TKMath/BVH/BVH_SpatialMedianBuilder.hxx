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

#ifndef BVH_SpatialMedianBuilder_HeaderFile
#define BVH_SpatialMedianBuilder_HeaderFile

#include <BVH_BinnedBuilder.hxx>
#include <BVH_Box.hxx>

//! Performs building of BVH tree using spatial median split algorithm.
template <class T, int N>
class BVH_SpatialMedianBuilder : public BVH_BinnedBuilder<T, N, 2>
{
public:
  //! Creates spatial median split builder.
  BVH_SpatialMedianBuilder(
    const Standard_Integer theLeafNodeSize  = BVH_Constants_LeafNodeSizeDefault,
    const Standard_Integer theMaxTreeDepth  = BVH_Constants_MaxTreeDepth,
    const Standard_Boolean theToUseMainAxis = Standard_False)
      : BVH_BinnedBuilder<T, N, 2>(theLeafNodeSize, theMaxTreeDepth, theToUseMainAxis)
  {
  }

  //! Releases resources of spatial median split builder.
  virtual ~BVH_SpatialMedianBuilder() {}
};

#endif // _BVH_SpatialMedianBuilder_Header
