// Copyright (c) 2019-2026 OPEN CASCADE SAS
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

#ifndef _BVH_Tools_Header
#define _BVH_Tools_Header

#include <BVH_Box.hxx>
#include <BVH_Ray.hxx>
#include <BVH_Types.hxx>

#include <algorithm>
#include <cmath>
#include <limits>

//! Defines a set of static methods operating with points and bounding boxes.
//! \tparam T Numeric data type
//! \tparam N Vector dimension
template <class T, int N>
class BVH_Tools
{
public: //! @name public types
  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:
  enum BVH_PrjStateInTriangle
  {
    BVH_PrjStateInTriangle_VERTEX,
    BVH_PrjStateInTriangle_EDGE,
    BVH_PrjStateInTriangle_INNER
  };

public: //! @name Box-Box Square distance
  //! Computes Square distance between Axis aligned bounding boxes
  static T BoxBoxSquareDistance(const BVH_Box<T, N>& theBox1, const BVH_Box<T, N>& theBox2)
  {
    if (!theBox1.IsValid() || !theBox2.IsValid())
    {
      return static_cast<T>(0);
    }
    return BoxBoxSquareDistance(theBox1.CornerMin(),
                                theBox1.CornerMax(),
                                theBox2.CornerMin(),
                                theBox2.CornerMax());
  }

  //! Computes Square distance between Axis aligned bounding boxes
  static T BoxBoxSquareDistance(const BVH_VecNt& theCMin1,
                                const BVH_VecNt& theCMax1,
                                const BVH_VecNt& theCMin2,
                                const BVH_VecNt& theCMax2)
  {
    T aDist = T(0);
    for (int i = 0; i < N; ++i)
    {
      if (theCMin1[i] > theCMax2[i])
      {
        T d = theCMin1[i] - theCMax2[i];
        aDist += d * d;
      }
      else if (theCMax1[i] < theCMin2[i])
      {
        T d = theCMin2[i] - theCMax1[i];
        aDist += d * d;
      }
    }
    return aDist;
  }

public: //! @name Point-Box Square distance
  //! Computes square distance between point and bounding box
  static T PointBoxSquareDistance(const BVH_VecNt& thePoint, const BVH_Box<T, N>& theBox)
  {
    if (!theBox.IsValid())
    {
      return static_cast<T>(0);
    }
    return PointBoxSquareDistance(thePoint, theBox.CornerMin(), theBox.CornerMax());
  }

  //! Computes square distance between point and bounding box
  static T PointBoxSquareDistance(const BVH_VecNt& thePoint,
                                  const BVH_VecNt& theCMin,
                                  const BVH_VecNt& theCMax)
  {
    T aDist = T(0);
    for (int i = 0; i < N; ++i)
    {
      if (thePoint[i] < theCMin[i])
      {
        T d = theCMin[i] - thePoint[i];
        aDist += d * d;
      }
      else if (thePoint[i] > theCMax[i])
      {
        T d = thePoint[i] - theCMax[i];
        aDist += d * d;
      }
    }
    return aDist;
  }

public: //! @name Point-Box projection
  //! Computes projection of point on bounding box
  static BVH_VecNt PointBoxProjection(const BVH_VecNt& thePoint, const BVH_Box<T, N>& theBox)
  {
    if (!theBox.IsValid())
    {
      return thePoint;
    }
    return PointBoxProjection(thePoint, theBox.CornerMin(), theBox.CornerMax());
  }

  //! Computes projection of point on bounding box
  static BVH_VecNt PointBoxProjection(const BVH_VecNt& thePoint,
                                      const BVH_VecNt& theCMin,
                                      const BVH_VecNt& theCMax)
  {
    return thePoint.cwiseMax(theCMin).cwiseMin(theCMax);
  }

private: //! @name Internal helpers for point-triangle projection
  //! Helper to set projection state for vertex
  static void SetVertexState(BVH_PrjStateInTriangle* thePrjState,
                             int*                    theFirstNode,
                             int*                    theLastNode,
                             int                     theVertexIndex)
  {
    if (thePrjState != nullptr)
    {
      *thePrjState = BVH_PrjStateInTriangle_VERTEX;
    }
    if (theFirstNode != nullptr)
    {
      *theFirstNode = theVertexIndex;
    }
    if (theLastNode != nullptr)
    {
      *theLastNode = theVertexIndex;
    }
  }

  //! Helper to set projection state for edge
  static void SetEdgeState(BVH_PrjStateInTriangle* thePrjState,
                           int*                    theFirstNode,
                           int*                    theLastNode,
                           int                     theStartVertex,
                           int                     theEndVertex)
  {
    if (thePrjState != nullptr)
    {
      *thePrjState = BVH_PrjStateInTriangle_EDGE;
    }
    if (theFirstNode != nullptr)
    {
      *theFirstNode = theStartVertex;
    }
    if (theLastNode != nullptr)
    {
      *theLastNode = theEndVertex;
    }
  }

  //! Helper to compute projection onto edge
  static BVH_VecNt ProjectToEdge(const BVH_VecNt& theEdgeStart,
                                 const BVH_VecNt& theEdge,
                                 T                theDot1,
                                 T                theDot2)
  {
    const T aParameter = theDot1 / (theDot1 + theDot2);
    return theEdgeStart + theEdge * aParameter;
  }

  //! Computes square distance between two points.
  static T squareDistance(const BVH_VecNt& thePoint1, const BVH_VecNt& thePoint2)
  {
    const BVH_VecNt aDelta = thePoint1 - thePoint2;
    return aDelta.Dot(aDelta);
  }

  //! Computes area-weighted coordinates using exterior products.
  //! This form avoids subtraction of nearly equal squared dot products for thin triangles.
  static void triangleCoordinates(const BVH_VecNt& theAB,
                                  const BVH_VecNt& theAC,
                                  const BVH_VecNt& theAP,
                                  T&               theNorm,
                                  T&               theWeightB,
                                  T&               theWeightC)
  {
    theNorm = theWeightB = theWeightC = T(0);
    for (int aFirst = 0; aFirst < N; ++aFirst)
    {
      for (int aSecond = aFirst + 1; aSecond < N; ++aSecond)
      {
        const T anArea = theAB[aFirst] * theAC[aSecond] - theAB[aSecond] * theAC[aFirst];
        theNorm += anArea * anArea;
        theWeightB +=
          (theAP[aFirst] * theAC[aSecond] - theAP[aSecond] * theAC[aFirst]) * anArea;
        theWeightC +=
          (theAB[aFirst] * theAP[aSecond] - theAB[aSecond] * theAP[aFirst]) * anArea;
      }
    }
  }

  //! Computes closest point for a numerically thin or degenerate triangle.
  //! Edge projections cover collapsed triangles; exterior-product coordinates are used
  //! when an interior projection is still numerically resolvable.
  static BVH_VecNt projectToThinTriangle(const BVH_VecNt&        thePoint,
                                         const BVH_VecNt&        theNode0,
                                         const BVH_VecNt&        theNode1,
                                         const BVH_VecNt&        theNode2,
                                         BVH_PrjStateInTriangle* thePrjState,
                                         int*                    theFirstNode,
                                         int*                    theLastNode)
  {
    const BVH_VecNt* aNodes[] = {&theNode0, &theNode1, &theNode2};

    BVH_VecNt                aClosest  = theNode0;
    T                        aDistance = squareDistance(thePoint, theNode0);
    BVH_PrjStateInTriangle   aPrjState = BVH_PrjStateInTriangle_VERTEX;
    int                      aFirstNode = 0;
    int                      aLastNode  = 0;

    for (int aSide = 0; aSide < 3; ++aSide)
    {
      const int       aNext         = (aSide + 1) % 3;
      const BVH_VecNt aEdge         = *aNodes[aNext] - *aNodes[aSide];
      const T         aSquareLength = aEdge.Dot(aEdge);
      const T         aParameter =
        aSquareLength > T(0)
          ? std::clamp((thePoint - *aNodes[aSide]).Dot(aEdge) / aSquareLength, T(0), T(1))
          : T(0);
      const BVH_VecNt aCandidate         = *aNodes[aSide] + aEdge * aParameter;
      const T         aCandidateDistance = squareDistance(thePoint, aCandidate);
      if (aCandidateDistance < aDistance)
      {
        aClosest  = aCandidate;
        aDistance = aCandidateDistance;
        if (aParameter == T(0) || aParameter == T(1))
        {
          aPrjState  = BVH_PrjStateInTriangle_VERTEX;
          aFirstNode = aLastNode = aParameter == T(0) ? aSide : aNext;
        }
        else
        {
          aPrjState  = BVH_PrjStateInTriangle_EDGE;
          aFirstNode = aSide;
          aLastNode  = aNext;
        }
      }
    }

    const BVH_VecNt aAB = theNode1 - theNode0;
    const BVH_VecNt aAC = theNode2 - theNode0;
    const BVH_VecNt aAP = thePoint - theNode0;
    T               aNorm;
    T               aWeightB;
    T               aWeightC;
    triangleCoordinates(aAB, aAC, aAP, aNorm, aWeightB, aWeightC);
    if (aNorm > T(0) && aWeightB > T(0) && aWeightC > T(0) && aWeightB + aWeightC < aNorm)
    {
      const BVH_VecNt aCandidate =
        theNode0 + aAB * (aWeightB / aNorm) + aAC * (aWeightC / aNorm);
      if (squareDistance(thePoint, aCandidate) <= aDistance)
      {
        aClosest  = aCandidate;
        aPrjState = BVH_PrjStateInTriangle_INNER;
      }
    }

    if (thePrjState != nullptr)
    {
      *thePrjState = aPrjState;
    }
    if (aPrjState != BVH_PrjStateInTriangle_INNER)
    {
      if (theFirstNode != nullptr)
      {
        *theFirstNode = aFirstNode;
      }
      if (theLastNode != nullptr)
      {
        *theLastNode = aLastNode;
      }
    }
    return aClosest;
  }

public: //! @name Point-Triangle Square distance
  //! Find nearest point on a triangle for the given point.
  //! Uses Voronoi region testing to determine closest feature (vertex, edge, or interior).
  static BVH_VecNt PointTriangleProjection(const BVH_VecNt&        thePoint,
                                           const BVH_VecNt&        theNode0,
                                           const BVH_VecNt&        theNode1,
                                           const BVH_VecNt&        theNode2,
                                           BVH_PrjStateInTriangle* thePrjState          = nullptr,
                                           int*                    theNumberOfFirstNode = nullptr,
                                           int*                    theNumberOfLastNode  = nullptr)
  {
    // Compute edge vectors
    const BVH_VecNt aAB = theNode1 - theNode0;
    const BVH_VecNt aAC = theNode2 - theNode0;
    const BVH_VecNt aBC = theNode2 - theNode1;

    // Compute point-to-vertex vectors
    const BVH_VecNt aAP = thePoint - theNode0;
    const BVH_VecNt aBP = thePoint - theNode1;
    const BVH_VecNt aCP = thePoint - theNode2;

    // Compute dot products for Voronoi region tests
    const T aABdotAP = aAB.Dot(aAP);
    const T aACdotAP = aAC.Dot(aAP);

    // Check if P is in vertex region outside A
    if (aABdotAP <= T(0) && aACdotAP <= T(0))
    {
      SetVertexState(thePrjState, theNumberOfFirstNode, theNumberOfLastNode, 0);
      return theNode0;
    }

    const T aBAdotBP = -aAB.Dot(aBP);
    const T aBCdotBP = aBC.Dot(aBP);

    // Check if P is in vertex region outside B
    if (aBAdotBP <= T(0) && aBCdotBP <= T(0))
    {
      SetVertexState(thePrjState, theNumberOfFirstNode, theNumberOfLastNode, 1);
      return theNode1;
    }

    const T aCBdotCP = -aBC.Dot(aCP);
    const T aCAdotCP = -aAC.Dot(aCP);

    // Check if P is in vertex region outside C
    if (aCAdotCP <= T(0) && aCBdotCP <= T(0))
    {
      SetVertexState(thePrjState, theNumberOfFirstNode, theNumberOfLastNode, 2);
      return theNode2;
    }

    // Barycentric weights use differences of products. Detect cancellation before
    // the edge-region tests, where an unreliable sign can misclassify a thin triangle.
    const T aACdotBP = aAC.Dot(aBP);
    const T aABdotCP = aAB.Dot(aCP);

    const T aVC1 = aABdotAP * aACdotBP;
    const T aVC2 = aBAdotBP * aACdotAP;
    const T aVA1 = aBAdotBP * aCAdotCP;
    const T aVA2 = aABdotCP * aACdotBP;
    const T aVB1 = aABdotCP * aACdotAP;
    const T aVB2 = aABdotAP * aCAdotCP;

    const T aVC   = aVC1 + aVC2;
    const T aVA   = aVA1 - aVA2;
    const T aVB   = aVB1 + aVB2;
    const T aNorm = aVA + aVB + aVC;

    const T aVCProducts = std::abs(aVC1) + std::abs(aVC2);
    const T aVAProducts = std::abs(aVA1) + std::abs(aVA2);
    const T aVBProducts = std::abs(aVB1) + std::abs(aVB2);
    const T aProducts   = aVCProducts + aVAProducts + aVBProducts;

    // Each dot product uses approximately 2*N operations; account also for
    // the products and summations forming the barycentric weights.
    constexpr T THE_ERROR_FACTOR = T(2 * N + 3) * std::numeric_limits<T>::epsilon();
    constexpr T THE_ROUNDOFF     = THE_ERROR_FACTOR / (T(1) - THE_ERROR_FACTOR);

    // The normalization can still be well above its roundoff bound while an
    // individual barycentric weight loses significant digits through cancellation.
    // In that case use exterior-product coordinates, which are substantially more
    // stable for thin triangles.
    const T aCancellationLimit = std::sqrt(std::numeric_limits<T>::epsilon());
    if (aNorm <= THE_ROUNDOFF * aProducts
        || std::abs(aVA) <= aCancellationLimit * aVAProducts
        || std::abs(aVB) <= aCancellationLimit * aVBProducts
        || std::abs(aVC) <= aCancellationLimit * aVCProducts)
    {
      return projectToThinTriangle(thePoint,
                                   theNode0,
                                   theNode1,
                                   theNode2,
                                   thePrjState,
                                   theNumberOfFirstNode,
                                   theNumberOfLastNode);
    }

    // Check if P is in edge region of AB
    if (aVC <= T(0) && aABdotAP > T(0) && aBAdotBP > T(0))
    {
      SetEdgeState(thePrjState, theNumberOfFirstNode, theNumberOfLastNode, 0, 1);
      return ProjectToEdge(theNode0, aAB, aABdotAP, aBAdotBP);
    }

    // Check if P is in edge region of BC
    if (aVA <= T(0) && aBCdotBP > T(0) && aCBdotCP > T(0))
    {
      SetEdgeState(thePrjState, theNumberOfFirstNode, theNumberOfLastNode, 1, 2);
      return ProjectToEdge(theNode1, aBC, aBCdotBP, aCBdotCP);
    }

    // Check if P is in edge region of CA
    if (aVB <= T(0) && aACdotAP > T(0) && aCAdotCP > T(0))
    {
      SetEdgeState(thePrjState, theNumberOfFirstNode, theNumberOfLastNode, 2, 0);
      return ProjectToEdge(theNode0, aAC, aACdotAP, aCAdotCP);
    }

    // If roundoff produced inconsistent barycentric signs, use the robust path.
    if (aVA <= T(0) || aVB <= T(0) || aVC <= T(0))
    {
      return projectToThinTriangle(thePoint,
                                   theNode0,
                                   theNode1,
                                   theNode2,
                                   thePrjState,
                                   theNumberOfFirstNode,
                                   theNumberOfLastNode);
    }

    if (thePrjState != nullptr)
    {
      *thePrjState = BVH_PrjStateInTriangle_INNER;
    }

    // aVB and aVC are barycentric weights of nodes 1 and 2 respectively.
    return theNode0 + aAB * (aVB / aNorm) + aAC * (aVC / aNorm);
  }

  //! Computes square distance between point and triangle
  static T PointTriangleSquareDistance(const BVH_VecNt& thePoint,
                                       const BVH_VecNt& theNode0,
                                       const BVH_VecNt& theNode1,
                                       const BVH_VecNt& theNode2)
  {
    const BVH_VecNt aProj = PointTriangleProjection(thePoint, theNode0, theNode1, theNode2);
    const BVH_VecNt aPP   = aProj - thePoint;
    return aPP.Dot(aPP);
  }

public: //! @name Ray-Box Intersection
  //! Computes hit time of ray-box intersection.
  //! Uses precomputed reciprocal direction from BVH_Ray for optimal performance.
  static bool RayBoxIntersection(const BVH_Ray<T, N>& theRay,
                                 const BVH_Box<T, N>& theBox,
                                 T&                   theTimeEnter,
                                 T&                   theTimeLeave)
  {
    if (!theBox.IsValid())
    {
      return false;
    }
    return RayBoxIntersection(theRay,
                              theBox.CornerMin(),
                              theBox.CornerMax(),
                              theTimeEnter,
                              theTimeLeave);
  }

  //! Computes hit time of ray-box intersection.
  //! Uses precomputed reciprocal direction from BVH_Ray for optimal performance.
  //! Handles parallel rays (infinite inverse direction) explicitly to avoid NaN from 0*inf
  //! when ray origin is exactly on a slab boundary.
  static bool RayBoxIntersection(const BVH_Ray<T, N>& theRay,
                                 const BVH_VecNt&     theBoxCMin,
                                 const BVH_VecNt&     theBoxCMax,
                                 T&                   theTimeEnter,
                                 T&                   theTimeLeave)
  {
    T aTimeEnter = (std::numeric_limits<T>::lowest)();
    T aTimeLeave = (std::numeric_limits<T>::max)();

    for (int i = 0; i < N; ++i)
    {
      // Handle parallel rays (infinite inverse direction) to avoid NaN from 0*inf
      if (std::isinf(theRay.InvDirect[i]))
      {
        if (theRay.Origin[i] < theBoxCMin[i] || theRay.Origin[i] > theBoxCMax[i])
        {
          return false;
        }
        continue;
      }
      T aT1      = (theBoxCMin[i] - theRay.Origin[i]) * theRay.InvDirect[i];
      T aT2      = (theBoxCMax[i] - theRay.Origin[i]) * theRay.InvDirect[i];
      aTimeEnter = (std::max)(aTimeEnter, (std::min)(aT1, aT2));
      aTimeLeave = (std::min)(aTimeLeave, (std::max)(aT1, aT2));
      if (aTimeEnter > aTimeLeave)
      {
        return false;
      }
    }

    // Check if intersection is behind the ray origin
    if (aTimeLeave < T(0))
    {
      return false;
    }

    theTimeEnter = aTimeEnter;
    theTimeLeave = aTimeLeave;
    return true;
  }

  //! Computes hit time of ray-box intersection
  static bool RayBoxIntersection(const BVH_VecNt&     theRayOrigin,
                                 const BVH_VecNt&     theRayDirection,
                                 const BVH_Box<T, N>& theBox,
                                 T&                   theTimeEnter,
                                 T&                   theTimeLeave)
  {
    if (!theBox.IsValid())
    {
      return false;
    }
    return RayBoxIntersection(theRayOrigin,
                              theRayDirection,
                              theBox.CornerMin(),
                              theBox.CornerMax(),
                              theTimeEnter,
                              theTimeLeave);
  }

  //! Computes hit time of ray-box intersection.
  //! Uses optimized single-pass algorithm with early exit.
  //! @param theRayOrigin ray origin point
  //! @param theRayDirection ray direction vector
  //! @param theBoxCMin minimum corner of the box
  //! @param theBoxCMax maximum corner of the box
  //! @param theTimeEnter time of ray entering the box
  //! @param theTimeLeave time of ray leaving the box
  //! @return true if ray intersects the box
  static bool RayBoxIntersection(const BVH_VecNt& theRayOrigin,
                                 const BVH_VecNt& theRayDirection,
                                 const BVH_VecNt& theBoxCMin,
                                 const BVH_VecNt& theBoxCMax,
                                 T&               theTimeEnter,
                                 T&               theTimeLeave)
  {
    T aTimeEnter = (std::numeric_limits<T>::lowest)();
    T aTimeLeave = (std::numeric_limits<T>::max)();

    for (int i = 0; i < N; ++i)
    {
      if (theRayDirection[i] == T(0))
      {
        // Ray is parallel to this axis slab - check if origin is within bounds
        if (theRayOrigin[i] < theBoxCMin[i] || theRayOrigin[i] > theBoxCMax[i])
        {
          return false; // Ray misses the slab entirely
        }
        // Ray is within the slab, doesn't constrain the intersection interval
        continue;
      }

      // Compute intersection distances for this axis
      T aT1 = (theBoxCMin[i] - theRayOrigin[i]) / theRayDirection[i];
      T aT2 = (theBoxCMax[i] - theRayOrigin[i]) / theRayDirection[i];

      // Ensure aT1 <= aT2 (handle negative direction)
      T aTMin = (std::min)(aT1, aT2);
      T aTMax = (std::max)(aT1, aT2);

      // Update intersection interval
      aTimeEnter = (std::max)(aTimeEnter, aTMin);
      aTimeLeave = (std::min)(aTimeLeave, aTMax);

      // Early exit if no intersection
      if (aTimeEnter > aTimeLeave)
      {
        return false;
      }
    }

    // Check if intersection is behind the ray origin
    if (aTimeLeave < T(0))
    {
      return false;
    }

    theTimeEnter = aTimeEnter;
    theTimeLeave = aTimeLeave;
    return true;
  }
};

#endif // _BVH_Tools_Header
