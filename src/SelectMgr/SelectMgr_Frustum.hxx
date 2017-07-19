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

#include <BVH_Box.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <SelectMgr_BaseFrustum.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

//! This is an internal class containing representation of rectangular selecting frustum, created in case
//! of point and box selection, and algorithms for overlap detection between selecting
//! frustum and sensitive entities. The principle of frustum calculation:
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

  SelectMgr_Frustum() : SelectMgr_BaseFrustum() {};

protected:

  // SAT Tests for different objects

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_Boolean hasOverlap (const SelectMgr_Vec3& theBoxMin,
                               const SelectMgr_Vec3& theBoxMax,
                               Standard_Boolean*     theInside = NULL);

  //! SAT intersection test between defined volume and given point
  Standard_Boolean hasOverlap (const gp_Pnt& thePnt);

  //! SAT intersection test between defined volume and given segment
  Standard_Boolean hasOverlap (const gp_Pnt& thePnt1,
                               const gp_Pnt& thePnt2);

  //! SAT intersection test between frustum given and planar convex polygon represented as ordered point set
  Standard_Boolean hasOverlap (const TColgp_Array1OfPnt& theArrayOfPnts,
                               gp_Vec& theNormal);

  //! SAT intersection test between defined volume and given triangle
  Standard_Boolean hasOverlap (const gp_Pnt& thePnt1,
                               const gp_Pnt& thePnt2,
                               const gp_Pnt& thePnt3,
                               gp_Vec& theNormal);

private:

  //! Checks if AABB and frustum are separated along the given axis
  Standard_Boolean isSeparated (const SelectMgr_Vec3& theBoxMin,
                                const SelectMgr_Vec3& theBoxMax,
                                const gp_XYZ&         theDirect,
                                Standard_Boolean*     theInside) const;

  //! Checks if triangle and frustum are separated along the given axis
  Standard_Boolean isSeparated (const gp_Pnt& thePnt1,
                                const gp_Pnt& thePnt2,
                                const gp_Pnt& thePnt3,
                                const gp_XYZ& theAxis) const;

protected:

  gp_Vec myPlanes[N + 2];        //!< Plane equations
  gp_Pnt myVertices[N * 2];      //!< Vertices coordinates

  Standard_Real myMaxVertsProjections[N + 2];      //!< Cached projections of vertices onto frustum plane directions
  Standard_Real myMinVertsProjections[N + 2];      //!< Cached projections of vertices onto frustum plane directions
  Standard_Real myMaxOrthoVertsProjections[3];     //!< Cached projections of vertices onto directions of ortho unit vectors
  Standard_Real myMinOrthoVertsProjections[3];     //!< Cached projections of vertices onto directions of ortho unit vectors

  gp_Vec myEdgeDirs[6];                    //!< Cached edge directions
};

#include <SelectMgr_Frustum.lxx>

#endif // _SelectMgr_Frustum_HeaderFile
