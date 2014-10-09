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

#ifndef _BVH_Builder_Header
#define _BVH_Builder_Header

#include <BVH_Set.hxx>
#include <BVH_Tree.hxx>

namespace BVH
{
  //! Minimum node size to split.
  const Standard_Real THE_NODE_MIN_SIZE = 1e-5;
}

//! Performs construction of BVH tree using bounding
//! boxes (AABBs) of abstract objects.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template<class T, int N>
class BVH_Builder
{
public:

  //! Creates new abstract BVH builder.
  BVH_Builder (const Standard_Integer theLeafNodeSize,
               const Standard_Integer theMaxTreeDepth);

  //! Releases resources of BVH builder.
  virtual ~BVH_Builder();

  //! Builds BVH using specific algorithm.
  virtual void Build (BVH_Set<T, N>*       theSet,
                      BVH_Tree<T, N>*      theBVH,
                      const BVH_Box<T, N>& theBox) = 0;

protected:

  //! Updates depth of constructed BVH tree.
  void UpdateDepth (BVH_Tree<T, N>*        theBVH,
                    const Standard_Integer theLevel)
  {
    if (theLevel > theBVH->myDepth)
    {
      theBVH->myDepth = theLevel;
    }
  }

protected:

  Standard_Integer myMaxTreeDepth; //!< Maximum depth of constructed BVH
  Standard_Integer myLeafNodeSize; //!< Maximum number of objects per leaf

};

#include <BVH_Builder.lxx>

#endif // _BVH_Builder_Header
