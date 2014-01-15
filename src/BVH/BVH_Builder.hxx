// Created on: 2013-12-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BVH_Builder_Header
#define _BVH_Builder_Header

#include <BVH_Set.hxx>
#include <BVH_Tree.hxx>

#include <NCollection_Vector.hxx>

namespace
{
  //! Minimum node size to split.
  const Standard_Real THE_NODE_MIN_SIZE = 1e-5;
}

//! Performs building of BVH tree.
template<class T, int N>
class BVH_Builder
{
public:

  //! Creates abstract BVH builder.
  BVH_Builder (const Standard_Integer theLeafNodeSize,
               const Standard_Integer theMaxTreeDepth);

  //! Releases resources of BVH builder.
  virtual ~BVH_Builder() = 0;

public:

  //! Builds BVH using specified algorithm.
  void Build (BVH_Set<T, N>*       theSet,
              BVH_Tree<T, N>*      theBVH,
              const BVH_Box<T, N>& theBox);

protected:

  //! Builds BVH node for specified task info.
  virtual void BuildNode (BVH_Set<T, N>*         theSet,
                          BVH_Tree<T, N>*        theBVH,
                          const Standard_Integer theTask);

protected:

  Standard_Integer                     myMaxTreeDepth; //!< Maximum depth of constructed BVH
  Standard_Integer                     myLeafNodeSize; //!< Maximum number of primitives per leaf
  NCollection_Vector<Standard_Integer> myTasksQueue;   //!< Queue to manage BVH node building tasks

};

#include <BVH_Builder.lxx>

#endif // _BVH_Builder_Header
