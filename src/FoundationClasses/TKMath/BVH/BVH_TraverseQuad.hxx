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

#ifndef BVH_TraverseQuad_HeaderFile
#define BVH_TraverseQuad_HeaderFile

#include <BVH_Constants.hxx>
#include <BVH_QuadTree.hxx>
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

//! Loads up to 4 child AABBs of an inner node into a SoA box record.
//! Lanes beyond theNumChildren are filled with an "always-miss" box
//! (max < min) so subsequent SIMD tests reject them naturally.
inline void LoadChildBoxes(const opencascade::handle<BVH_Tree<float, 3, BVH_QuadTree>>& theTree,
                           int                                                          theBaseIdx,
                           int            theNumChildren,
                           BVH_Box4f_SoA& theBoxes) noexcept
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
  // Padding lanes get an "always-miss" box: max < min on every axis.
  // The slab test produces tEnter > tLeave and the lane is rejected.
  for (int aK = theNumChildren; aK < 4; ++aK)
  {
    theBoxes.minX[aK] = 1.0f;
    theBoxes.minY[aK] = 1.0f;
    theBoxes.minZ[aK] = 1.0f;
    theBoxes.maxX[aK] = -1.0f;
    theBoxes.maxY[aK] = -1.0f;
    theBoxes.maxZ[aK] = -1.0f;
  }
}

//! Builds a 4-lane splat record from a single ray.
inline BVH_Ray4f_Splat MakeRaySplat(const BVH_Ray<float, 3>& theRay) noexcept
{
  BVH_Ray4f_Splat aSplat;
  for (int i = 0; i < 4; ++i)
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

//! Traverses a quad-BVH tree with one ray, invoking theAcceptor for each
//! primitive in any leaf the ray hits.
//!
//! @tparam Acceptor must be callable as `void(int primIdx, float tEnter)`.
//! @param theTree     QBVH tree built via BVH_BinaryTree::CollapseToQuadTree
//! @param theRay      ray to trace; must have valid InvDirect (use BVH_Ray ctor)
//! @param theAcceptor invoked for each candidate primitive in hit leaves.
//!                    The acceptor decides whether the primitive truly
//!                    intersects (this routine only does AABB culling).
//!
//! The kernel selection (scalar / SSE2 / AVX2 / AVX-512) is delegated to
//! BVH::SIMD::GetRayBox4(), which picks the best implementation at process
//! start based on CPU detection.
template <class Acceptor>
inline void TraverseQuad(const opencascade::handle<BVH_Tree<float, 3, BVH_QuadTree>>& theTree,
                         const BVH_Ray<float, 3>&                                     theRay,
                         Acceptor& theAcceptor) noexcept
{
  if (theTree.IsNull() || theTree->Length() == 0)
  {
    return;
  }

  const BVH_Ray4f_Splat aRaySplat = detail::MakeRaySplat(theRay);
  const RayBox4_Fn      aKernel   = GetRayBox4();

  // Stack of nodes to visit; QBVH is at most BVH_Constants_MaxTreeDepth deep
  // and each level adds up to 4 children, so a per-level slot of 4 suffices.
  int aStack[BVH_Constants_MaxTreeDepth * 4];
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

    // Inner node: child indices = NodeInfo[aNode].y() + K, K in [0, NumChildren).
    // NumChildren = NodeInfo[aNode].z() + 1 (range 1..4).
    const auto& aInfo        = theTree->NodeInfoBuffer()[aNode];
    const int   aBaseIdx     = aInfo.y();
    const int   aNumChildren = aInfo.z() + 1;

    BVH_Box4f_SoA aBoxes;
    detail::LoadChildBoxes(theTree, aBaseIdx, aNumChildren, aBoxes);

    float aTEnter[4]{};
    float aTLeave[4]{};
    int   aMask = aKernel(aRaySplat, aBoxes, aTEnter, aTLeave);

    // Mask away padding lanes (children that don't exist in this node).
    aMask &= (1 << aNumChildren) - 1;

    // Push hit children. Order does not affect correctness (only traversal
    // efficiency); MVP uses simple order: highest-index lanes first so they
    // get popped in lane order.
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

#endif // BVH_TraverseQuad_HeaderFile
