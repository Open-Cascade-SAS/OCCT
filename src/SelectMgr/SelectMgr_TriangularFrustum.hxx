// Created on: 2014-05-22
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

#ifndef _SelectMgr_TriangularFrustum_HeaderFile
#define _SelectMgr_TriangularFrustum_HeaderFile

#include <SelectMgr_Frustum.hxx>

//! This class contains representation of triangular selecting frustum, created in case
//! of polyline selection, and algorithms for overlap detection between selecting frustum and sensitive entities.
//! Overlap detection tests are implemented according to the terms of separating axis theorem (SAT).
//! NOTE: the object of this class can be created only as part of SelectMgr_TriangularFrustumSet.
class SelectMgr_TriangularFrustum : public SelectMgr_Frustum<3>
{
public:

  //! Auxiliary structure to define selection triangle
  struct SelectionTriangle
  {
    gp_Pnt2d Points[3];
  };

public:

  //! Destructor.
  Standard_EXPORT virtual ~SelectMgr_TriangularFrustum();

  //! Initializes selection triangle by input points
  Standard_EXPORT void Init (const gp_Pnt2d& theP1,
                             const gp_Pnt2d& theP2,
                             const gp_Pnt2d& theP3);

  //! Creates new triangular frustum with bases of triangles with vertices theP1, theP2 and theP3
  //! projections onto near and far view frustum planes (only for triangular frustums)
  //! NOTE: it should be called after Init() method
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;

  //! Returns FALSE (not applicable to this volume).
  virtual Standard_Boolean IsScalable() const Standard_OVERRIDE { return false; }

  //! Returns a copy of the frustum transformed according to the matrix given
  Standard_EXPORT virtual Handle(SelectMgr_BaseIntersector) ScaleAndTransform (const Standard_Integer theScale,
                                                                               const gp_GTrsf& theTrsf,
                                                                               const Handle(SelectMgr_FrustumBuilder)& theBuilder) const Standard_OVERRIDE;

public: //! @name SAT Tests for different objects

  //! SAT intersection test between defined volume and given axis-aligned box
  Standard_EXPORT virtual Standard_Boolean OverlapsBox (const SelectMgr_Vec3& theMinPnt,
                                                        const SelectMgr_Vec3& theMaxPnt,
                                                        const SelectMgr_ViewClipRange& theClipRange,
                                                        SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_EXPORT virtual Standard_Boolean OverlapsBox (const SelectMgr_Vec3& theMinPt,
                                                        const SelectMgr_Vec3& theMaxPt,
                                                        Standard_Boolean* theInside) const Standard_OVERRIDE;

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual Standard_Boolean OverlapsPoint (const gp_Pnt& thePnt,
                                                          const SelectMgr_ViewClipRange& theClipRange,
                                                          SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Always returns FALSE (not applicable to this selector).
  virtual Standard_Boolean OverlapsPoint (const gp_Pnt& ) const Standard_OVERRIDE
  {
    return Standard_False;
  }

  //! SAT intersection test between defined volume and given ordered set of points,
  //! representing line segments. The test may be considered of interior part or
  //! boundary line defined by segments depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean OverlapsPolygon (const TColgp_Array1OfPnt& theArrayOfPnts,
                                                            Select3D_TypeOfSensitivity theSensType,
                                                            const SelectMgr_ViewClipRange& theClipRange,
                                                            SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Checks if line segment overlaps selecting frustum
  Standard_EXPORT virtual Standard_Boolean OverlapsSegment (const gp_Pnt& thePnt1,
                                                            const gp_Pnt& thePnt2,
                                                            const SelectMgr_ViewClipRange& theClipRange,
                                                            SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! SAT intersection test between defined volume and given triangle. The test may
  //! be considered of interior part or boundary line defined by triangle vertices
  //! depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean OverlapsTriangle (const gp_Pnt& thePnt1,
                                                             const gp_Pnt& thePnt2,
                                                             const gp_Pnt& thePnt3,
                                                             Select3D_TypeOfSensitivity theSensType,
                                                             const SelectMgr_ViewClipRange& theClipRange,
                                                             SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT virtual Standard_Boolean OverlapsSphere (const gp_Pnt& theCenter,
                                                           const Standard_Real theRadius,
                                                           Standard_Boolean* theInside = NULL) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT virtual Standard_Boolean OverlapsSphere (const gp_Pnt& theCenter,
                                                           const Standard_Real theRadius,
                                                           const SelectMgr_ViewClipRange& theClipRange,
                                                           SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses theBottomRad
  //! and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT virtual Standard_Boolean OverlapsCylinder (const Standard_Real theBottomRad,
                                                             const Standard_Real theTopRad,
                                                             const Standard_Real theHeight,
                                                             const gp_Trsf& theTrsf,
                                                             const Standard_Boolean theIsHollow,
                                                             const SelectMgr_ViewClipRange& theClipRange,
                                                             SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses theBottomRad
  //! and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT virtual Standard_Boolean OverlapsCylinder (const Standard_Real theBottomRad,
                                                             const Standard_Real theTopRad,
                                                             const Standard_Real theHeight,
                                                             const gp_Trsf& theTrsf,
                                                             const Standard_Boolean theIsHollow,
                                                             Standard_Boolean* theInside = NULL) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  Standard_EXPORT virtual Standard_Boolean OverlapsCircle (const Standard_Real theRadius,
                                                           const gp_Trsf& theTrsf,
                                                           const Standard_Boolean theIsFilled,
                                                           const SelectMgr_ViewClipRange& theClipRange,
                                                           SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  Standard_EXPORT virtual Standard_Boolean OverlapsCircle (const Standard_Real theRadius,
                                                           const gp_Trsf& theTrsf,
                                                           const Standard_Boolean theIsFilled,
                                                           Standard_Boolean* theInside = NULL) const Standard_OVERRIDE;

public:

  //! Nullifies the handle to corresponding builder instance to prevent memory leaks
  Standard_EXPORT void Clear();

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  Standard_EXPORT virtual void GetPlanes (NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

protected:

  //! Creates an empty triangular frustum.
  Standard_EXPORT SelectMgr_TriangularFrustum();

private:

  void cacheVertexProjections (SelectMgr_TriangularFrustum* theFrustum) const;

protected:

  SelectionTriangle mySelTriangle; //!< parameters of selection triangle (it is used to build triangle frustum)

public:

  DEFINE_STANDARD_RTTIEXT(SelectMgr_TriangularFrustum, SelectMgr_Frustum<3>)
  friend class SelectMgr_TriangularFrustumSet;
};

#endif // _SelectMgr_TriangularFrustum_HeaderFile
