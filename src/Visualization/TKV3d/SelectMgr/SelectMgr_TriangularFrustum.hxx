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
//! of polyline selection, and algorithms for overlap detection between selecting frustum and
//! sensitive entities. Overlap detection tests are implemented according to the terms of separating
//! axis theorem (SAT). NOTE: the object of this class can be created only as part of
//! SelectMgr_TriangularFrustumSet.
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
  Standard_EXPORT void Init(const gp_Pnt2d& theP1, const gp_Pnt2d& theP2, const gp_Pnt2d& theP3);

  //! Creates new triangular frustum with bases of triangles with vertices theP1, theP2 and theP3
  //! projections onto near and far view frustum planes (only for triangular frustums)
  //! NOTE: it should be called after Init() method
  Standard_EXPORT virtual void Build() override;

  //! Returns FALSE (not applicable to this volume).
  virtual bool IsScalable() const override { return false; }

  //! Returns a copy of the frustum transformed according to the matrix given
  Standard_EXPORT virtual occ::handle<SelectMgr_BaseIntersector> ScaleAndTransform(
    const int                                    theScale,
    const gp_GTrsf&                              theTrsf,
    const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const override;

  //! Returns a copy of the frustum using the given frustum builder configuration.
  //! Returned frustum should be re-constructed before being used.
  //! @param[in] theBuilder  argument that represents corresponding settings for re-constructing
  //! transformed frustum from scratch;
  //!                        should NOT be NULL.
  //! @return a copy of the frustum with the input builder assigned
  Standard_EXPORT virtual occ::handle<SelectMgr_BaseIntersector> CopyWithBuilder(
    const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const override;

public: //! @name SAT Tests for different objects
  //! SAT intersection test between defined volume and given axis-aligned box
  Standard_EXPORT virtual bool OverlapsBox(const NCollection_Vec3<double>& theMinPnt,
                                           const NCollection_Vec3<double>& theMaxPnt,
                                           const SelectMgr_ViewClipRange&  theClipRange,
                                           SelectBasics_PickResult& thePickResult) const override;

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_EXPORT virtual bool OverlapsBox(const NCollection_Vec3<double>& theMinPt,
                                           const NCollection_Vec3<double>& theMaxPt,
                                           bool* theInside) const override;

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual bool OverlapsPoint(const gp_Pnt&                  thePnt,
                                             const SelectMgr_ViewClipRange& theClipRange,
                                             SelectBasics_PickResult& thePickResult) const override;

  //! Always returns FALSE (not applicable to this selector).
  virtual bool OverlapsPoint(const gp_Pnt&) const override { return false; }

  //! SAT intersection test between defined volume and given ordered set of points,
  //! representing line segments. The test may be considered of interior part or
  //! boundary line defined by segments depending on given sensitivity type
  Standard_EXPORT virtual bool OverlapsPolygon(
    const NCollection_Array1<gp_Pnt>& theArrayOfPnts,
    Select3D_TypeOfSensitivity        theSensType,
    const SelectMgr_ViewClipRange&    theClipRange,
    SelectBasics_PickResult&          thePickResult) const override;

  //! Checks if line segment overlaps selecting frustum
  Standard_EXPORT virtual bool OverlapsSegment(
    const gp_Pnt&                  thePnt1,
    const gp_Pnt&                  thePnt2,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  //! SAT intersection test between defined volume and given triangle. The test may
  //! be considered of interior part or boundary line defined by triangle vertices
  //! depending on given sensitivity type
  Standard_EXPORT virtual bool OverlapsTriangle(
    const gp_Pnt&                  thePnt1,
    const gp_Pnt&                  thePnt2,
    const gp_Pnt&                  thePnt3,
    Select3D_TypeOfSensitivity     theSensType,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT virtual bool OverlapsSphere(const gp_Pnt& theCenter,
                                              const double  theRadius,
                                              bool*         theInside = NULL) const override;

  //! Returns true if selecting volume is overlapped by sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT virtual bool OverlapsSphere(
    const gp_Pnt&                  theCenter,
    const double                   theRadius,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT virtual bool OverlapsCylinder(
    const double                   theBottomRad,
    const double                   theTopRad,
    const double                   theHeight,
    const gp_Trsf&                 theTrsf,
    const bool                     theIsHollow,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT virtual bool OverlapsCylinder(const double   theBottomRad,
                                                const double   theTopRad,
                                                const double   theHeight,
                                                const gp_Trsf& theTrsf,
                                                const bool     theIsHollow,
                                                bool*          theInside = NULL) const override;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  Standard_EXPORT virtual bool OverlapsCircle(
    const double                   theRadius,
    const gp_Trsf&                 theTrsf,
    const bool                     theIsFilled,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  Standard_EXPORT virtual bool OverlapsCircle(const double   theRadius,
                                              const gp_Trsf& theTrsf,
                                              const bool     theIsFilled,
                                              bool*          theInside = NULL) const override;

public:
  //! Nullifies the handle to corresponding builder instance to prevent memory leaks
  Standard_EXPORT void Clear();

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  Standard_EXPORT virtual void GetPlanes(
    NCollection_Vector<NCollection_Vec4<double>>& thePlaneEquations) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

protected:
  //! Creates an empty triangular frustum.
  Standard_EXPORT SelectMgr_TriangularFrustum();

private:
  void cacheVertexProjections(SelectMgr_TriangularFrustum* theFrustum) const;

protected:
  // clang-format off
  SelectionTriangle mySelTriangle; //!< parameters of selection triangle (it is used to build triangle frustum)
  // clang-format on

public:
  DEFINE_STANDARD_RTTIEXT(SelectMgr_TriangularFrustum, SelectMgr_Frustum<3>)
  friend class SelectMgr_TriangularFrustumSet;
};

#endif // _SelectMgr_TriangularFrustum_HeaderFile
