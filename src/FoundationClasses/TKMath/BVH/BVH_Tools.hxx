// Created by: Eugeny MALTCHIKOV
// Created on: 2019-04-17
// Copyright (c) 2019 OPEN CASCADE SAS
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
                             Standard_Integer*       theFirstNode,
                             Standard_Integer*       theLastNode,
                             Standard_Integer        theVertexIndex)
  {
    if (thePrjState != nullptr)
    {
      *thePrjState  = BVH_PrjStateInTriangle_VERTEX;
      *theFirstNode = theVertexIndex;
      *theLastNode  = theVertexIndex;
    }
  }

  //! Helper to set projection state for edge
  static void SetEdgeState(BVH_PrjStateInTriangle* thePrjState,
                           Standard_Integer*       theFirstNode,
                           Standard_Integer*       theLastNode,
                           Standard_Integer        theStartVertex,
                           Standard_Integer        theEndVertex)
  {
    if (thePrjState != nullptr)
    {
      *thePrjState  = BVH_PrjStateInTriangle_EDGE;
      *theFirstNode = theStartVertex;
      *theLastNode  = theEndVertex;
    }
  }

  //! Helper to compute projection onto edge
  static BVH_VecNt ProjectToEdge(const BVH_VecNt& theEdgeStart,
                                 const BVH_VecNt& theEdge,
                                 T                theDot1,
                                 T                theDot2)
  {
    T aT = theDot1 / (theDot1 + theDot2);
    return theEdgeStart + theEdge * aT;
  }

public: //! @name Point-Triangle Square distance
  //! Find nearest point on a triangle for the given point.
  //! Uses Voronoi region testing to determine closest feature (vertex, edge, or interior).
  static BVH_VecNt PointTriangleProjection(const BVH_VecNt&        thePoint,
                                           const BVH_VecNt&        theNode0,
                                           const BVH_VecNt&        theNode1,
                                           const BVH_VecNt&        theNode2,
                                           BVH_PrjStateInTriangle* thePrjState          = nullptr,
                                           Standard_Integer*       theNumberOfFirstNode = nullptr,
                                           Standard_Integer*       theNumberOfLastNode  = nullptr)
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

    // Compute barycentric coordinates for edge/interior tests
    const T aACdotBP = aAC.Dot(aBP);
    const T aVC      = aABdotAP * aACdotBP + aBAdotBP * aACdotAP;

    // Check if P is in edge region of AB
    if (aVC <= T(0) && aABdotAP > T(0) && aBAdotBP > T(0))
    {
      SetEdgeState(thePrjState, theNumberOfFirstNode, theNumberOfLastNode, 0, 1);
      return ProjectToEdge(theNode0, aAB, aABdotAP, aBAdotBP);
    }

    const T aABdotCP = aAB.Dot(aCP);
    const T aVA      = aBAdotBP * aCAdotCP - aABdotCP * aACdotBP;

    // Check if P is in edge region of BC
    if (aVA <= T(0) && aBCdotBP > T(0) && aCBdotCP > T(0))
    {
      SetEdgeState(thePrjState, theNumberOfFirstNode, theNumberOfLastNode, 1, 2);
      return ProjectToEdge(theNode1, aBC, aBCdotBP, aCBdotCP);
    }

    const T aVB = aABdotCP * aACdotAP + aABdotAP * aCAdotCP;

    // Check if P is in edge region of CA
    if (aVB <= T(0) && aACdotAP > T(0) && aCAdotCP > T(0))
    {
      SetEdgeState(thePrjState, theNumberOfFirstNode, theNumberOfLastNode, 2, 0);
      return ProjectToEdge(theNode0, aAC, aACdotAP, aCAdotCP);
    }

    // P is inside triangle - compute barycentric coordinates
    const T aNorm = aVA + aVB + aVC;

    // Handle degenerate triangle (zero or near-zero area)
    if (aNorm
        <= std::numeric_limits<T>::epsilon() * (std::abs(aVA) + std::abs(aVB) + std::abs(aVC)))
    {
      SetVertexState(thePrjState, theNumberOfFirstNode, theNumberOfLastNode, 0);
      return (theNode0 + theNode1 + theNode2) / T(3);
    }

    if (thePrjState != nullptr)
    {
      *thePrjState = BVH_PrjStateInTriangle_INNER;
    }

    return (theNode0 * aVA + theNode1 * aVB + theNode2 * aVC) / aNorm;
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
  //! Computes hit time of ray-box intersection
  static Standard_Boolean RayBoxIntersection(const BVH_Ray<T, N>& theRay,
                                             const BVH_Box<T, N>& theBox,
                                             T&                   theTimeEnter,
                                             T&                   theTimeLeave)
  {
    if (!theBox.IsValid())
    {
      return Standard_False;
    }
    return RayBoxIntersection(theRay,
                              theBox.CornerMin(),
                              theBox.CornerMax(),
                              theTimeEnter,
                              theTimeLeave);
  }

  //! Computes hit time of ray-box intersection.
  static Standard_Boolean RayBoxIntersection(const BVH_Ray<T, N>& theRay,
                                             const BVH_VecNt&     theBoxCMin,
                                             const BVH_VecNt&     theBoxCMax,
                                             T&                   theTimeEnter,
                                             T&                   theTimeLeave)
  {
    return RayBoxIntersection(theRay.Origin,
                              theRay.Direct,
                              theBoxCMin,
                              theBoxCMax,
                              theTimeEnter,
                              theTimeLeave);
  }

  //! Computes hit time of ray-box intersection
  static Standard_Boolean RayBoxIntersection(const BVH_VecNt&     theRayOrigin,
                                             const BVH_VecNt&     theRayDirection,
                                             const BVH_Box<T, N>& theBox,
                                             T&                   theTimeEnter,
                                             T&                   theTimeLeave)
  {
    if (!theBox.IsValid())
    {
      return Standard_False;
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
  static Standard_Boolean RayBoxIntersection(const BVH_VecNt& theRayOrigin,
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
          return Standard_False; // Ray misses the slab entirely
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
        return Standard_False;
      }
    }

    // Check if intersection is behind the ray origin
    if (aTimeLeave < T(0))
    {
      return Standard_False;
    }

    theTimeEnter = aTimeEnter;
    theTimeLeave = aTimeLeave;
    return Standard_True;
  }
};

#endif