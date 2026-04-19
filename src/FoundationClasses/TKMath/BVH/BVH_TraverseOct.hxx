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

#ifndef BVH_TraverseOct_HeaderFile
#define BVH_TraverseOct_HeaderFile

#include <BVH_Constants.hxx>
#include <BVH_OctTree.hxx>
#include <BVH_Ray.hxx>
#include <BVH_SIMDDispatch.hxx>
#include <BVH_Tree.hxx>

#include <Standard_Handle.hxx>

namespace BVH
{
namespace SIMD
{

namespace detail
{

//! Loads up to 8 child AABBs of an inner node into a SoA record.
//! Lanes beyond theNumChildren are filled with an "always-miss" box
//! (max < min) so the SIMD slab test rejects them naturally without
//! requiring a separate count check on the kernel side.
inline void LoadChildBoxes8(const opencascade::handle<BVH_Tree<float, 3, BVH_OctTree>>& theTree,
                            int                                                          theBaseIdx,
                            int                                                          theNumChildren,
                            BVH_Box8f_SoA& theBoxes) noexcept
{
  for (int aK = 0; aK < theNumChildren; ++aK)
  {
    const auto& aMin     = theTree->MinPoint(theBaseIdx + aK);
    const auto& aMax     = theTree->MaxPoint(theBaseIdx + aK);
    theBoxes.minX[aK]    = aMin.x();
    theBoxes.minY[aK]    = aMin.y();
    theBoxes.minZ[aK]    = aMin.z();
    theBoxes.maxX[aK]    = aMax.x();
    theBoxes.maxY[aK]    = aMax.y();
    theBoxes.maxZ[aK]    = aMax.z();
  }
  for (int aK = theNumChildren; aK < 8; ++aK)
  {
    theBoxes.minX[aK]    = 1.0f;
    theBoxes.minY[aK]    = 1.0f;
    theBoxes.minZ[aK]    = 1.0f;
    theBoxes.maxX[aK]    = -1.0f;
    theBoxes.maxY[aK]    = -1.0f;
    theBoxes.maxZ[aK]    = -1.0f;
  }
}

//! Builds an 8-lane splat record from a single ray.
inline BVH_Ray8f_Splat MakeRaySplat8(const BVH_Ray<float, 3>& theRay) noexcept
{
  BVH_Ray8f_Splat aSplat;
  for (int i = 0; i < 8; ++i)
  {
    aSplat.ox[i]  = theRay.Origin.x();
    aSplat.oy[i]  = theRay.Origin.y();
    aSplat.oz[i]  = theRay.Origin.z();
    aSplat.idx[i] = theRay.InvDirect.x();
    aSplat.idy[i] = theRay.InvDirect.y();
    aSplat.idz[i] = theRay.InvDirect.z();
  }
  return aSplat;
}

} // namespace detail

//! Traverses a BVH8 tree with one ray, invoking theAcceptor for each
//! primitive in any leaf the ray intersects.
//!
//! @tparam Acceptor must be callable as `void(int primIdx, float tEnter)`.
//! @param theTree     OBVH tree built via BVH_BinaryTree::CollapseToOctTree
//! @param theRay      ray to trace; must have valid InvDirect (use BVH_Ray ctor)
//! @param theAcceptor invoked once per candidate primitive in any hit leaf;
//!                    user code does the precise primitive-level test
//!
//! Kernel selection (scalar / SSE2 / AVX2 / AVX-512) is delegated to
//! BVH::SIMD::GetRayBox8(), picked once at process start by CPU detection.
template <class Acceptor>
inline void TraverseOct(
  const opencascade::handle<BVH_Tree<float, 3, BVH_OctTree>>& theTree,
  const BVH_Ray<float, 3>&                                    theRay,
  Acceptor&                                                   theAcceptor) noexcept
{
  if (theTree.IsNull() || theTree->Length() == 0)
  {
    return;
  }

  const BVH_Ray8f_Splat aRaySplat = detail::MakeRaySplat8(theRay);
  const RayBox8_Fn      aKernel   = GetRayBox8();

  // Each level can push at most 8 children; depth is bounded by
  // BVH_Constants_MaxTreeDepth (the binary tree's depth, since a BVH8
  // built by 3-level collapse is a third as deep, never deeper).
  int aStack[BVH_Constants_MaxTreeDepth * 8];
  int aTop      = 0;
  aStack[aTop++] = 0; // root

  while (aTop > 0)
  {
    const int aNode = aStack[--aTop];
    if (theTree->IsOuter(aNode))
    {
      const int aBeg = theTree->BegPrimitive(aNode);
      const int aEnd = theTree->EndPrimitive(aNode);
      for (int aPrim = aBeg; aPrim <= aEnd; ++aPrim)
      {
        theAcceptor(aPrim, 0.0f);
      }
      continue;
    }

    const auto& aInfo         = theTree->NodeInfoBuffer()[aNode];
    const int   aBaseIdx      = aInfo.y();
    const int   aNumChildren  = aInfo.z() + 1;

    BVH_Box8f_SoA aBoxes;
    detail::LoadChildBoxes8(theTree, aBaseIdx, aNumChildren, aBoxes);

    float aTEnter[8]{};
    float aTLeave[8]{};
    int   aMask = aKernel(aRaySplat, aBoxes, aTEnter, aTLeave);

    // Drop padding-lane bits for inner nodes with fewer than 8 children.
    aMask &= (1 << aNumChildren) - 1;

    // Push hit children. MVP uses simple highest-first so they pop in
    // lane order; a tNear-sorted push is a future optimisation.
    for (int aK = aNumChildren - 1; aK >= 0; --aK)
    {
      if ((aMask >> aK) & 1)
      {
        aStack[aTop++] = aBaseIdx + aK;
      }
    }
  }
}

} // namespace SIMD
} // namespace BVH

#endif // BVH_TraverseOct_HeaderFile
