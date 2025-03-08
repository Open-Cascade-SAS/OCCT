// Created on: 2015-03-16
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _SelectMgr_Frustum_HeaderFile
#define _SelectMgr_Frustum_HeaderFile

#include <SelectMgr_BaseFrustum.hxx>

//! This is an internal class containing representation of rectangular selecting frustum, created in
//! case of point and box selection, and algorithms for overlap detection between selecting frustum
//! and sensitive entities. The principle of frustum calculation:
//! - for point selection: on a near view frustum plane rectangular neighborhood of
//!                        user-picked point is created according to the pixel tolerance
//!                        given and then this rectangle is projected onto far view frustum
//!                        plane. This rectangles define the parallel bases of selecting frustum;
//! - for box selection: box points are projected onto near and far view frustum planes.
//!                      These 2 projected rectangles define parallel bases of selecting frustum.
//! Overlap detection tests are implemented according to the terms of separating axis
//! theorem (SAT).
//! Vertex order:
//! - for triangular frustum: V0_Near, V1_Near, V2_Near,
//!                           V0_Far, V1_Far, V2_Far;
//! - for rectangular frustum: LeftTopNear, LeftTopFar,
//!                            LeftBottomNear,LeftBottomFar,
//!                            RightTopNear, RightTopFar,
//!                            RightBottomNear, RightBottomFar.
//! Plane order in array:
//! - for triangular frustum: V0V1, V1V2, V0V2, Near, Far;
//! - for rectangular frustum: Top, Bottom, Left, Right, Near, Far.
//! Uncollinear edge directions order:
//! - for rectangular frustum: Horizontal, Vertical,
//!                            LeftLower, RightLower,
//!                            LeftUpper, RightUpper;
//! - for triangular frustum: V0_Near - V0_Far, V1_Near - V1_Far, V2_Near - V2_Far,
//!                           V1_Near - V0_Near, V2_Near - V1_Near, V2_Near - V0_Near.
template <int N>
class SelectMgr_Frustum : public SelectMgr_BaseFrustum
{
public:
  SelectMgr_Frustum()
      : SelectMgr_BaseFrustum()
  {
    memset(myMaxOrthoVertsProjections, 0, sizeof(myMaxOrthoVertsProjections));
    memset(myMinOrthoVertsProjections, 0, sizeof(myMinOrthoVertsProjections));
    memset(myMaxVertsProjections, 0, sizeof(myMaxVertsProjections));
    memset(myMinVertsProjections, 0, sizeof(myMinVertsProjections));
  }

  //! Dumps the content of me into the stream
  inline virtual void DumpJson(Standard_OStream& theOStream,
                               Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

protected:
  // SAT Tests for different objects

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_Boolean hasBoxOverlap(const SelectMgr_Vec3& theBoxMin,
                                 const SelectMgr_Vec3& theBoxMax,
                                 Standard_Boolean*     theInside = NULL) const;

  //! SAT intersection test between defined volume and given point
  Standard_Boolean hasPointOverlap(const gp_Pnt& thePnt) const;

  //! SAT intersection test between defined volume and given segment
  Standard_Boolean hasSegmentOverlap(const gp_Pnt& thePnt1, const gp_Pnt& thePnt2) const;

  //! SAT intersection test between frustum given and planar convex polygon represented as ordered
  //! point set
  Standard_Boolean hasPolygonOverlap(const TColgp_Array1OfPnt& theArrayOfPnts,
                                     gp_Vec&                   theNormal) const;

  //! SAT intersection test between defined volume and given triangle
  Standard_Boolean hasTriangleOverlap(const gp_Pnt& thePnt1,
                                      const gp_Pnt& thePnt2,
                                      const gp_Pnt& thePnt3,
                                      gp_Vec&       theNormal) const;

  //! Intersection test between defined volume and given sphere
  Standard_Boolean hasSphereOverlap(const gp_Pnt&       thePnt1,
                                    const Standard_Real theRadius,
                                    Standard_Boolean*   theInside = NULL) const;

  //! Intersection test between defined volume and given cylinder (or cone).
  Standard_Boolean hasCylinderOverlap(const Standard_Real    theBottomRad,
                                      const Standard_Real    theTopRad,
                                      const Standard_Real    theHeight,
                                      const gp_Trsf&         theTrsf,
                                      const Standard_Boolean theIsHollow,
                                      Standard_Boolean*      theInside = NULL) const;

  //! Intersection test between defined volume and given circle.
  Standard_Boolean hasCircleOverlap(const Standard_Real    theRadius,
                                    const gp_Trsf&         theTrsf,
                                    const Standard_Boolean theIsFilled,
                                    Standard_Boolean*      theInside = NULL) const;

  //! Returns True if all vertices (theVertices) are inside the top and bottom sides of the
  //! cylinder.
  Standard_Boolean isInsideCylinderEndFace(const Standard_Real       theBottomRad,
                                           const Standard_Real       theTopRad,
                                           const Standard_Real       theHeight,
                                           const gp_Trsf&            theTrsf,
                                           const TColgp_Array1OfPnt& theVertices) const;

  //! Checking whether the point thePnt is inside the shape with borders theVertices.
  //! thePnt and theVertices lie in the same plane.
  Standard_Boolean isDotInside(const gp_Pnt& thePnt, const TColgp_Array1OfPnt& theVertices) const;

private:
  //! Return true if one segment enclosed between the points thePnt1Seg1 and thePnt2Seg1
  //! intersects another segment that enclosed between thePnt1Seg2 and thePnt2Seg2.
  Standard_Boolean isSegmentsIntersect(const gp_Pnt& thePnt1Seg1,
                                       const gp_Pnt& thePnt2Seg1,
                                       const gp_Pnt& thePnt1Seg2,
                                       const gp_Pnt& thePnt2Seg2) const;

  //! Checking whether the borders theVertices of the shape intersect
  //! the cylinder (or cone) end face with the center theCenter and radius theRadius
  Standard_Boolean isIntersectCircle(const Standard_Real       theRadius,
                                     const gp_Pnt&             theCenter,
                                     const gp_Trsf&            theTrsf,
                                     const TColgp_Array1OfPnt& theVertices) const;

  //! Checks if AABB and frustum are separated along the given axis
  Standard_Boolean isSeparated(const SelectMgr_Vec3& theBoxMin,
                               const SelectMgr_Vec3& theBoxMax,
                               const gp_XYZ&         theDirect,
                               Standard_Boolean*     theInside) const;

  //! Checks if triangle and frustum are separated along the given axis
  Standard_Boolean isSeparated(const gp_Pnt& thePnt1,
                               const gp_Pnt& thePnt2,
                               const gp_Pnt& thePnt3,
                               const gp_XYZ& theAxis) const;

protected:
  gp_Vec myPlanes[N + 2];   //!< Plane equations
  gp_Pnt myVertices[N * 2]; //!< Vertices coordinates

  // clang-format off
  Standard_Real myMaxVertsProjections[N + 2];      //!< Cached projections of vertices onto frustum plane directions
  Standard_Real myMinVertsProjections[N + 2];      //!< Cached projections of vertices onto frustum plane directions
  Standard_Real myMaxOrthoVertsProjections[3];     //!< Cached projections of vertices onto directions of ortho unit vectors
  Standard_Real myMinOrthoVertsProjections[3];     //!< Cached projections of vertices onto directions of ortho unit vectors
  // clang-format on

  gp_Vec myEdgeDirs[6]; //!< Cached edge directions
};

#include <SelectMgr_Frustum.lxx>

#endif // _SelectMgr_Frustum_HeaderFile
