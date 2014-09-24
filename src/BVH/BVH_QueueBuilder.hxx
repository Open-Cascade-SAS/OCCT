// Created on: 2014-09-15
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

#ifndef _BVH_QueueBuilder_Header
#define _BVH_QueueBuilder_Header

#include <BVH_Builder.hxx>

#include <NCollection_Vector.hxx>

//! Abstract BVH builder based on the concept of work queue.
template<class T, int N>
class BVH_QueueBuilder : public BVH_Builder<T, N>
{
public:

  //! Creates new BVH queue based builder.
  BVH_QueueBuilder (const Standard_Integer theLeafNodeSize,
                    const Standard_Integer theMaxTreeDepth);

  //! Releases resources of BVH queue based builder.
  virtual ~BVH_QueueBuilder() = 0;

public:

  //! Builds BVH using specific algorithm.
  virtual void Build (BVH_Set<T, N>*       theSet,
                      BVH_Tree<T, N>*      theBVH,
                      const BVH_Box<T, N>& theBox);

protected:

  //! Builds BVH node for the given task info.
  virtual void BuildNode (BVH_Set<T, N>*         theSet,
                          BVH_Tree<T, N>*        theBVH,
                          const Standard_Integer theTask);

protected:

  NCollection_Vector<Standard_Integer> myTasksQueue;   //!< Queue to manage BVH node building tasks

};

#include <BVH_QueueBuilder.lxx>

#endif // _BVH_QueueBuilder_Header
