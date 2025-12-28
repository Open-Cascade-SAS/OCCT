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

#ifndef _SelectMgr_RectangularFrustum_HeaderFile
#define _SelectMgr_RectangularFrustum_HeaderFile

#include <SelectMgr_Frustum.hxx>

//! This class contains representation of rectangular selecting frustum, created in case
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
class SelectMgr_RectangularFrustum : public SelectMgr_Frustum<4>
{
public:
  //! Auxiliary structure to define selection primitive (point or box)
  //! In case of point selection min and max points are identical.
  struct SelectionRectangle
  {
    SelectionRectangle()
        : myMinPnt(gp_Pnt2d(RealLast(), RealLast())),
          myMaxPnt(gp_Pnt2d(RealLast(), RealLast()))
    {
    }

    const gp_Pnt2d& MousePos() const { return myMinPnt; }

    void SetMousePos(const gp_Pnt2d& thePos)
    {
      myMinPnt = thePos;
      myMaxPnt = thePos;
    }

    const gp_Pnt2d& MinPnt() const { return myMinPnt; }

    void SetMinPnt(const gp_Pnt2d& theMinPnt) { myMinPnt = theMinPnt; }

    const gp_Pnt2d& MaxPnt() const { return myMaxPnt; }

    void SetMaxPnt(const gp_Pnt2d& theMaxPnt) { myMaxPnt = theMaxPnt; }

  private:
    gp_Pnt2d myMinPnt;
    gp_Pnt2d myMaxPnt;
  };

  //! Creates rectangular selecting frustum.
  Standard_EXPORT SelectMgr_RectangularFrustum();

  //! Initializes volume according to the point and given pixel tolerance
  Standard_EXPORT void Init(const gp_Pnt2d& thePoint);

  //! Initializes volume according to the selected rectangle
  Standard_EXPORT void Init(const gp_Pnt2d& theMinPnt, const gp_Pnt2d& theMaxPnt);

  //! Returns True if Frustum (theVertices) intersects the circle.
  Standard_EXPORT bool isIntersectCircle(const double                      theRadius,
                                         const gp_Pnt&                     theCenter,
                                         const gp_Trsf&                    theTrsf,
                                         const NCollection_Array1<gp_Pnt>& theVertices) const;

  //! Returns True if Seg1 (thePnt1Seg1, thePnt2Seg1) and Seg2 (thePnt1Seg2, thePnt2Seg2) intersect.
  Standard_EXPORT bool isSegmentsIntersect(const gp_Pnt& thePnt1Seg1,
                                           const gp_Pnt& thePnt2Seg1,
                                           const gp_Pnt& thePnt1Seg2,
                                           const gp_Pnt& thePnt2Seg2) const;

  //! Builds volume according to internal parameters.
  //! NOTE: it should be called after Init() method
  Standard_EXPORT virtual void Build() override;

  //! Checks if it is possible to scale this frustum.
  //! It is true for frustum built on a single point.
  Standard_EXPORT virtual bool IsScalable() const override;

  //! IMPORTANT: Scaling makes sense only for frustum built on a single point!
  //!            Note that this method does not perform any checks on type of the frustum.
  //! Returns a copy of the frustum resized according to the scale factor given
  //! and transforms it using the matrix given.
  //! There are no default parameters, but in case if:
  //!    - transformation only is needed: @theScaleFactor must be initialized as any negative value;
  //!    - scale only is needed: @theTrsf must be set to gp_Identity.
  //! Builder is an optional argument that represents corresponding settings for re-constructing
  //! transformed frustum from scratch. Can be null if reconstruction is not expected furthermore.
  Standard_EXPORT virtual occ::handle<SelectMgr_BaseIntersector> ScaleAndTransform(
    const int                                    theScaleFactor,
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

  // SAT Tests for different objects

  //! SAT intersection test between defined volume and given axis-aligned box
  Standard_EXPORT virtual bool OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                                           const NCollection_Vec3<double>& theBoxMax,
                                           const SelectMgr_ViewClipRange&  theClipRange,
                                           SelectBasics_PickResult& thePickResult) const override;

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_EXPORT virtual bool OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                                           const NCollection_Vec3<double>& theBoxMax,
                                           bool* theInside) const override;

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual bool OverlapsPoint(const gp_Pnt&                  thePnt,
                                             const SelectMgr_ViewClipRange& theClipRange,
                                             SelectBasics_PickResult& thePickResult) const override;

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual bool OverlapsPoint(const gp_Pnt& thePnt) const override;

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

  //! Intersection test between defined volume and given sphere
  Standard_EXPORT virtual bool OverlapsSphere(
    const gp_Pnt&                  theCenter,
    const double                   theRadius,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  //! Intersection test between defined volume and given sphere
  Standard_EXPORT virtual bool OverlapsSphere(const gp_Pnt& theCenter,
                                              const double  theRadius,
                                              bool*         theInside) const override;

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
    const double                   theBottomRad,
    const gp_Trsf&                 theTrsf,
    const bool                     theIsFilled,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const override;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  Standard_EXPORT virtual bool OverlapsCircle(const double   theBottomRad,
                                              const gp_Trsf& theTrsf,
                                              const bool     theIsFilled,
                                              bool*          theInside = NULL) const override;

  //! Measures distance between 3d projection of user-picked
  //! screen point and given point theCOG.
  //! It makes sense only for frustums built on a single point.
  Standard_EXPORT virtual double DistToGeometryCenter(const gp_Pnt& theCOG) const override;

  //! Calculates the point on a view ray that was detected during the run of selection algo by given
  //! depth
  Standard_EXPORT virtual gp_Pnt DetectedPoint(const double theDepth) const override;

  //! A set of helper functions that return rectangular selecting frustum data
  const gp_Pnt* GetVertices() const { return myVertices; }

  //! Returns projection of 2d mouse picked point or projection
  //! of center of 2d rectangle (for point and rectangular selection
  //! correspondingly) onto near view frustum plane
  virtual const gp_Pnt& GetNearPnt() const override { return myNearPickedPnt; }

  //! Returns projection of 2d mouse picked point or projection
  //! of center of 2d rectangle (for point and rectangular selection
  //! correspondingly) onto far view frustum plane
  virtual const gp_Pnt& GetFarPnt() const override { return myFarPickedPnt; }

  //! Returns view ray direction.
  virtual const gp_Dir& GetViewRayDirection() const override { return myViewRayDir; }

  //! Returns current mouse coordinates.
  Standard_EXPORT virtual const gp_Pnt2d& GetMousePosition() const override;

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  Standard_EXPORT virtual void GetPlanes(
    NCollection_Vector<NCollection_Vec4<double>>& thePlaneEquations) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

protected:
  Standard_EXPORT void segmentSegmentDistance(const gp_Pnt&            theSegPnt1,
                                              const gp_Pnt&            theSegPnt2,
                                              SelectBasics_PickResult& thePickResult) const;

  Standard_EXPORT bool segmentPlaneIntersection(const gp_Vec&            thePlane,
                                                const gp_Pnt&            thePntOnPlane,
                                                SelectBasics_PickResult& thePickResult) const;

private:
  void cacheVertexProjections(SelectMgr_RectangularFrustum* theFrustum) const;

private:
  enum
  {
    LeftTopNear,
    LeftTopFar,
    LeftBottomNear,
    LeftBottomFar,
    RightTopNear,
    RightTopFar,
    RightBottomNear,
    RightBottomFar
  };

private:
  // clang-format off
  SelectionRectangle      mySelRectangle;              //!< parameters for selection by point or box (it is used to build frustum)
  gp_Pnt                  myNearPickedPnt;             //!< 3d projection of user-picked selection point onto near view plane
  gp_Pnt                  myFarPickedPnt;              //!< 3d projection of user-picked selection point onto far view plane
  gp_Dir                  myViewRayDir;                //!< view ray direction
  double           myScale;                     //!< Scale factor of applied transformation, if there was any
  // clang-format on
};

#endif // _SelectMgr_RectangularFrustum_HeaderFile
