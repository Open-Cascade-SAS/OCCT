// Created on: 2014-01-09
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

#ifndef _BVH_SweepPlaneBuilder_Header
#define _BVH_SweepPlaneBuilder_Header

#include <BVH_QueueBuilder.hxx>

//! Performs building of BVH tree using sweep plane SAH algorithm.
template<class T, int N>
class BVH_SweepPlaneBuilder : public BVH_QueueBuilder<T, N>
{
public:

  //! Creates sweep plane SAH BVH builder.
  BVH_SweepPlaneBuilder (const Standard_Integer theLeafNodeSize = 5,
                         const Standard_Integer theMaxTreeDepth = 32);

  //! Releases resources of sweep plane SAH BVH builder.
  virtual ~BVH_SweepPlaneBuilder();

protected:

  //! Builds specified BVH node.
  virtual void BuildNode (BVH_Set<T, N>*         theSet,
                          BVH_Tree<T, N>*        theBVH,
                          const Standard_Integer theNode);

};

#include <BVH_SweepPlaneBuilder.lxx>

#endif // _BVH_SweepPlaneBuilder_Header
