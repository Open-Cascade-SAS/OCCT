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

#ifndef BVH_TraverseWide_HeaderFile
#define BVH_TraverseWide_HeaderFile

#include <BVH_Constants.hxx>
#include <BVH_Ray.hxx>
#include <BVH_SIMDDispatch.hxx>
#include <BVH_Tree.hxx>
#include <BVH_WideTree.hxx>

#include <Standard_Handle.hxx>

namespace BVH
{
namespace SIMD
{

namespace detail
{

//! Loads up to W child AABBs of an inner node into a SoA box record.
//! Lanes beyond theNumChildren are filled with an "always-miss" box
//! (max < min) so the SIMD slab test rejects them naturally without
//! requiring a separate count check on the kernel side.
template <int W>
inline void LoadChildBoxesN(const opencascade::handle<BVH_Tree<float, 3, BVH_WideTree<W>>>& theTree,
                            int               theBaseIdx,
                            int               theNumChildren,
                            BVH_BoxNf_SoA<W>& theBoxes) noexcept
{
  for (int aK = 0; aK < theNumChildren; ++aK)
  {
    const auto& aMin  = theTree->MinPoint(theBaseIdx + aK);
    const auto& aMax  = theTree->MaxPoint(theBaseIdx + aK);
    theBoxes.minX[aK] = aMin.x();
    theBoxes.minY[aK] = aMin.y();
    theBoxes.minZ[aK] = aMin.z();
    theBoxes.maxX[aK] = aMax.x();
    theBoxes.maxY[aK] = aMax.y();
    theBoxes.maxZ[aK] = aMax.z();
  }
  // Padding lanes: max < min on every axis. The slab test produces
  // tEnter > tLeave and the lane gets dropped via the hit-mask check.
  for (int aK = theNumChildren; aK < W; ++aK)
  {
    theBoxes.minX[aK] = 1.0f;
    theBoxes.minY[aK] = 1.0f;
    theBoxes.minZ[aK] = 1.0f;
    theBoxes.maxX[aK] = -1.0f;
    theBoxes.maxY[aK] = -1.0f;
    theBoxes.maxZ[aK] = -1.0f;
  }
}

//! Builds a W-lane splat record from a single ray.
template <int W>
inline BVH_RayNf_Splat<W> MakeRaySplatN(const BVH_Ray<float, 3>& theRay) noexcept
{
  BVH_RayNf_Splat<W> aSplat;
  for (int i = 0; i < W; ++i)
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

//! Traverses a W-wide BVH (BVH8 / BVH16) with one ray, invoking theAcceptor
//! for each primitive in any leaf the ray intersects.
//!
//! @tparam W        node fan-out (8 for BVH8 / AVX2, 16 for BVH16 / AVX-512)
//! @tparam Acceptor must be callable as `void(int primIdx, float tEnter)`.
//! @param theTree     wide BVH built via BVH_BinaryTree::CollapseToWide<W>()
//! @param theRay      ray to trace; must have valid InvDirect (use BVH_Ray ctor)
//! @param theAcceptor invoked once per candidate primitive in any hit leaf;
//!                    user code does the precise primitive-level test
//!
//! Kernel selection (scalar / AVX2 for W=8 / AVX-512 for W=16) is delegated
//! to BVH::SIMD::GetRayBoxN<W>(), picked once at process start by CPU
//! detection.
template <int W, class Acceptor>
inline void TraverseWide(const opencascade::handle<BVH_Tree<float, 3, BVH_WideTree<W>>>& theTree,
                         const BVH_Ray<float, 3>&                                        theRay,
                         Acceptor& theAcceptor) noexcept
{
  if (theTree.IsNull() || theTree->Length() == 0)
  {
    return;
  }

  const BVH_RayNf_Splat<W> aRaySplat = detail::MakeRaySplatN<W>(theRay);
  const RayBoxN_Fn<W>      aKernel   = GetRayBoxN<W>();

  // Each level can push at most W children; depth is bounded by
  // BVH_Constants_MaxTreeDepth (the binary tree's depth, since a wide
  // BVH built by log2(W)-level collapse is shallower, never deeper).
  int aStack[BVH_Constants_MaxTreeDepth * W];
  int aTop       = 0;
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

    const auto& aInfo        = theTree->NodeInfoBuffer()[aNode];
    const int   aBaseIdx     = aInfo.y();
    const int   aNumChildren = aInfo.z() + 1;

    BVH_BoxNf_SoA<W> aBoxes;
    detail::LoadChildBoxesN<W>(theTree, aBaseIdx, aNumChildren, aBoxes);

    float aTEnter[W]{};
    float aTLeave[W]{};
    int   aMask = aKernel(aRaySplat, aBoxes, aTEnter, aTLeave);

    // Drop padding-lane bits for inner nodes with fewer than W children.
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

#endif // BVH_TraverseWide_HeaderFile
