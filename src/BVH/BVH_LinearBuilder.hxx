// Created on: 2014-09-11
// Created by: Danila ULYANOV
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

#ifndef _BVH_LinearBuilder_Header
#define _BVH_LinearBuilder_Header

#include <BVH_Builder.hxx>

typedef std::pair<Standard_Integer, Standard_Integer> BVH_EncodedLink;

//! Performs fast BVH construction using LBVH building approach.
//! Algorithm uses spatial Morton codes to reduce the BVH construction
//! problem to a sorting problem (radix sort -- O(N) complexity). This
//! Linear Bounding Volume Hierarchy (LBVH) builder produces BVH trees
//! of lower quality compared to SAH-based BVH builders but it is over
//! an order of magnitude faster (up to 3M triangles per second).
//! 
//! For more details see:
//! C. Lauterbach, M. Garland, S. Sengupta, D. Luebke, and D. Manocha.
//! Fast BVH construction on GPUs. Eurographics, 2009.
template<class T, int N>
class BVH_LinearBuilder : public BVH_Builder<T, N>
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates binned LBVH builder.
  BVH_LinearBuilder (const Standard_Integer theLeafNodeSize = 5,
                     const Standard_Integer theMaxTreeDepth = 32);

  //! Releases resources of LBVH builder.
  virtual ~BVH_LinearBuilder();

  //! Builds BVH.
  void Build (BVH_Set<T, N>*       theSet,
              BVH_Tree<T, N>*      theBVH,
              const BVH_Box<T, N>& theBox);

protected:

  //! Emits hierarchy from sorted Morton codes.
  Standard_Integer EmitHierachy (BVH_Tree<T, N>*                        theBVH,
                                 const Standard_Integer                 theBit,
                                 const Standard_Integer                 theShift,
                                 std::vector<BVH_EncodedLink>::iterator theStart,
                                 std::vector<BVH_EncodedLink>::iterator theFinal);

};

#include <BVH_LinearBuilder.lxx>

#endif // _BVH_LinearBuilder_Header
