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

public: //! @name Box-Box Square distance

  //! Computes Square distance between Axis aligned bounding boxes
  static T BoxBoxSquareDistance (const BVH_Box<T, N>& theBox1,
                                 const BVH_Box<T, N>& theBox2)
  {
    if (!theBox1.IsValid() || !theBox2.IsValid())
    {
      return static_cast<T>(0);
    }
    return BoxBoxSquareDistance (theBox1.CornerMin(), theBox1.CornerMax(),
                                 theBox2.CornerMin(), theBox2.CornerMax());
  }

  //! Computes Square distance between Axis aligned bounding boxes
  static T BoxBoxSquareDistance (const BVH_VecNt& theCMin1,
                                 const BVH_VecNt& theCMax1,
                                 const BVH_VecNt& theCMin2,
                                 const BVH_VecNt& theCMax2)
  {
    T aDist = 0;
    for (int i = 0; i < N; ++i)
    {
      if      (theCMin1[i] > theCMax2[i]) { T d = theCMin1[i] - theCMax2[i]; d *= d; aDist += d; }
      else if (theCMax1[i] < theCMin2[i]) { T d = theCMin2[i] - theCMax1[i]; d *= d; aDist += d; }
    }
    return aDist;
  }

public: //! @name Point-Box Square distance

  //! Computes square distance between point and bounding box
  static T PointBoxSquareDistance (const BVH_VecNt& thePoint,
                                   const BVH_Box<T, N>& theBox)
  {
    if (!theBox.IsValid())
    {
      return static_cast<T>(0);
    }
    return PointBoxSquareDistance (thePoint,
                                   theBox.CornerMin(),
                                   theBox.CornerMax());
  }

  //! Computes square distance between point and bounding box
  static T PointBoxSquareDistance (const BVH_VecNt& thePoint,
                                   const BVH_VecNt& theCMin,
                                   const BVH_VecNt& theCMax)
  {
    T aDist = 0;
    for (int i = 0; i < N; ++i)
    {
      if      (thePoint[i] < theCMin[i]) { T d = theCMin[i] - thePoint[i]; d *= d; aDist += d; }
      else if (thePoint[i] > theCMax[i]) { T d = thePoint[i] - theCMax[i]; d *= d; aDist += d; }
    }
    return aDist;
  }

public: //! @name Point-Box projection

  //! Computes projection of point on bounding box
  static BVH_VecNt PointBoxProjection (const BVH_VecNt& thePoint,
                                       const BVH_Box<T, N>& theBox)
  {
    if (!theBox.IsValid())
    {
      return thePoint;
    }
    return PointBoxProjection (thePoint,
                               theBox.CornerMin(),
                               theBox.CornerMax());
  }

  //! Computes projection of point on bounding box
  static BVH_VecNt PointBoxProjection (const BVH_VecNt& thePoint,
                                       const BVH_VecNt& theCMin,
                                       const BVH_VecNt& theCMax)
  {
    return thePoint.cwiseMax (theCMin).cwiseMin (theCMax);
  }

public: //! @name Point-Triangle Square distance

  //! Computes square distance between point and triangle
  static T PointTriangleSquareDistance (const BVH_VecNt& thePoint,
                                        const BVH_VecNt& theNode0,
                                        const BVH_VecNt& theNode1,
                                        const BVH_VecNt& theNode2)
  {
    const BVH_VecNt aAB = theNode1 - theNode0;
    const BVH_VecNt aAC = theNode2 - theNode0;
    const BVH_VecNt aAP = thePoint - theNode0;
  
    T aABdotAP = aAB.Dot(aAP);
    T aACdotAP = aAC.Dot(aAP);
  
    if (aABdotAP <= 0. && aACdotAP <= 0.)
    {
      return aAP.Dot(aAP);
    }
  
    const BVH_VecNt aBC = theNode2 - theNode1;
    const BVH_VecNt aBP = thePoint - theNode1;
  
    T aBAdotBP = -(aAB.Dot(aBP));
    T aBCdotBP =  (aBC.Dot(aBP));
  
    if (aBAdotBP <= 0. && aBCdotBP <= 0.)
    {
      return (aBP.Dot(aBP));
    }
  
    const BVH_VecNt aCP = thePoint - theNode2;
  
    T aCBdotCP = -(aBC.Dot(aCP));
    T aCAdotCP = -(aAC.Dot(aCP));
  
    if (aCAdotCP <= 0. && aCBdotCP <= 0.)
    {
      return (aCP.Dot(aCP));
    }
  
    T aACdotBP = (aAC.Dot(aBP));
  
    T aVC = aABdotAP * aACdotBP + aBAdotBP * aACdotAP;
  
    if (aVC <= 0. && aABdotAP > 0. && aBAdotBP > 0.)
    {
      const BVH_VecNt aDirect = aAP - aAB * (aABdotAP / (aABdotAP + aBAdotBP));
  
      return (aDirect.Dot(aDirect));
    }
  
    T aABdotCP = (aAB.Dot(aCP));
  
    T aVA = aBAdotBP * aCAdotCP - aABdotCP * aACdotBP;
  
    if (aVA <= 0. && aBCdotBP > 0. && aCBdotCP > 0.)
    {
      const BVH_VecNt aDirect = aBP - aBC * (aBCdotBP / (aBCdotBP + aCBdotCP));
  
      return (aDirect.Dot(aDirect));
    }
  
    T aVB = aABdotCP * aACdotAP + aABdotAP * aCAdotCP;
  
    if (aVB <= 0. && aACdotAP > 0. && aCAdotCP > 0.)
    {
      const BVH_VecNt aDirect = aAP - aAC * (aACdotAP / (aACdotAP + aCAdotCP));
  
      return (aDirect.Dot(aDirect));
    }
  
    T aNorm = aVA + aVB + aVC;
  
    const BVH_VecNt& aDirect = thePoint - (theNode0 * aVA +
                                           theNode1 * aVB +
                                           theNode2 * aVC) / aNorm;
  
    return (aDirect.Dot(aDirect));
  }

public: //! @name Ray-Box Intersection

  //! Computes hit time of ray-box intersection
  static Standard_Boolean RayBoxIntersection (const BVH_Ray<T, N>& theRay,
                                              const BVH_Box<T, N>& theBox,
                                              T& theTimeEnter,
                                              T& theTimeLeave)
  {
    if (!theBox.IsValid())
    {
      return Standard_False;
    }
    return RayBoxIntersection (theRay, theBox.CornerMin(), theBox.CornerMax(), theTimeEnter, theTimeLeave);
  }

  //! Computes hit time of ray-box intersection
  static Standard_Boolean RayBoxIntersection (const BVH_Ray<T, N>& theRay,
                                              const BVH_VecNt& theBoxCMin,
                                              const BVH_VecNt& theBoxCMax,
                                              T& theTimeEnter,
                                              T& theTimeLeave)
  {
    return RayBoxIntersection (theRay.Origin, theRay.Direct,
                               theBoxCMin, theBoxCMax, theTimeEnter, theTimeLeave);
  }

  //! Computes hit time of ray-box intersection
  static Standard_Boolean RayBoxIntersection (const BVH_VecNt& theRayOrigin,
                                              const BVH_VecNt& theRayDirection,
                                              const BVH_Box<T, N>& theBox,
                                              T& theTimeEnter,
                                              T& theTimeLeave)
  {
    if (!theBox.IsValid())
    {
      return Standard_False;
    }
    return RayBoxIntersection (theRayOrigin, theRayDirection,
                               theBox.CornerMin(), theBox.CornerMax(),
                               theTimeEnter, theTimeLeave);
  }

  //! Computes hit time of ray-box intersection
  static Standard_Boolean RayBoxIntersection (const BVH_VecNt& theRayOrigin,
                                              const BVH_VecNt& theRayDirection,
                                              const BVH_VecNt& theBoxCMin,
                                              const BVH_VecNt& theBoxCMax,
                                              T& theTimeEnter,
                                              T& theTimeLeave)
  {
    BVH_VecNt aNodeMin, aNodeMax;
    for (int i = 0; i < N; ++i)
    {
      if (theRayDirection[i] == 0)
      {
        aNodeMin[i] = (theBoxCMin[i] - theRayOrigin[i]) <= 0 ?
                       (std::numeric_limits<T>::min)() : (std::numeric_limits<T>::max)();
        aNodeMax[i] = (theBoxCMax[i] - theRayOrigin[i]) < 0 ?
                       (std::numeric_limits<T>::min)() : (std::numeric_limits<T>::max)();
      }
      else
      {
        aNodeMin[i] = (theBoxCMin[i] - theRayOrigin[i]) / theRayDirection[i];
        aNodeMax[i] = (theBoxCMax[i] - theRayOrigin[i]) / theRayDirection[i];
      }
    }

    BVH_VecNt aTimeMin, aTimeMax;
    for (int i = 0; i < N; ++i)
    {
      aTimeMin[i] = Min (aNodeMin[i], aNodeMax[i]);
      aTimeMax[i] = Max (aNodeMin[i], aNodeMax[i]);
    }

    T aTimeEnter = Max (aTimeMin[0], Max (aTimeMin[1], aTimeMin[2]));
    T aTimeLeave = Min (aTimeMax[0], Min (aTimeMax[1], aTimeMax[2]));

    Standard_Boolean hasIntersection = aTimeEnter <= aTimeLeave && aTimeLeave >= 0;
    if (hasIntersection)
    {
      theTimeEnter = aTimeEnter;
      theTimeLeave = aTimeLeave;
    }

    return hasIntersection;
  }
};

#endif