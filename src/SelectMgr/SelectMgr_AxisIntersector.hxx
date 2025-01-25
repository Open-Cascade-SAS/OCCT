// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _SelectMgr_AxisIntersector_HeaderFile
#define _SelectMgr_AxisIntersector_HeaderFile

#include <SelectMgr_BaseIntersector.hxx>

#include <gp_Ax1.hxx>

//! This class contains representation of selecting axis, created in case of point selection
//! and algorithms for overlap detection between this axis and sensitive entities.
class SelectMgr_AxisIntersector : public SelectMgr_BaseIntersector
{
public:
  //! Empty constructor
  Standard_EXPORT SelectMgr_AxisIntersector();

  //! Destructor
  Standard_EXPORT virtual ~SelectMgr_AxisIntersector();

  //! Initializes selecting axis according to the input one
  Standard_EXPORT void Init(const gp_Ax1& theAxis);

  //! Builds axis according to internal parameters.
  //! NOTE: it should be called after Init() method
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;

  //! Saves camera definition.
  //! Do nothing for axis intersector (not applicable to this volume).
  Standard_EXPORT virtual void SetCamera(const Handle(Graphic3d_Camera)& theCamera)
    Standard_OVERRIDE;

  //! Returns FALSE (not applicable to this volume).
  virtual Standard_Boolean IsScalable() const Standard_OVERRIDE { return false; }

  //! IMPORTANT: Scaling doesn't make sense for this intersector.
  //! Returns a copy of the intersector transformed using the matrix given.
  //! Builder is an optional argument that represents corresponding settings for re-constructing
  //! transformed frustum from scratch. Can be null if reconstruction is not expected furthermore.
  Standard_EXPORT virtual Handle(SelectMgr_BaseIntersector) ScaleAndTransform(
    const Standard_Integer                  theScaleFactor,
    const gp_GTrsf&                         theTrsf,
    const Handle(SelectMgr_FrustumBuilder)& theBuilder) const Standard_OVERRIDE;

  //! Returns a copy of the intersector transformed using the builder configuration given.
  //! Builder is an argument that represents corresponding settings for re-constructing transformed
  //! frustum from scratch. In this class, builder is not used and theBuilder parameter is ignored.
  Standard_EXPORT virtual Handle(SelectMgr_BaseIntersector) CopyWithBuilder(
    const Handle(SelectMgr_FrustumBuilder)& theBuilder) const Standard_OVERRIDE;

public:
  //! Intersection test between defined axis and given axis-aligned box
  Standard_EXPORT virtual Standard_Boolean OverlapsBox(const SelectMgr_Vec3&          theBoxMin,
                                                       const SelectMgr_Vec3&          theBoxMax,
                                                       const SelectMgr_ViewClipRange& theClipRange,
                                                       SelectBasics_PickResult& thePickResult) const
    Standard_OVERRIDE;

  //! Returns true if selecting axis intersects axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_EXPORT virtual Standard_Boolean OverlapsBox(const SelectMgr_Vec3& theBoxMin,
                                                       const SelectMgr_Vec3& theBoxMax,
                                                       Standard_Boolean*     theInside) const
    Standard_OVERRIDE;

  //! Intersection test between defined axis and given point
  Standard_EXPORT virtual Standard_Boolean OverlapsPoint(
    const gp_Pnt&                  thePnt,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const Standard_OVERRIDE;

  //! Intersection test between defined axis and given point
  Standard_EXPORT virtual Standard_Boolean OverlapsPoint(const gp_Pnt& thePnt) const
    Standard_OVERRIDE;

  //! Intersection test between defined axis and given ordered set of points,
  //! representing line segments. The test may be considered of interior part or
  //! boundary line defined by segments depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean OverlapsPolygon(
    const TColgp_Array1OfPnt&      theArrayOfPnts,
    Select3D_TypeOfSensitivity     theSensType,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const Standard_OVERRIDE;

  //! Checks if selecting axis intersects line segment
  Standard_EXPORT virtual Standard_Boolean OverlapsSegment(
    const gp_Pnt&                  thePnt1,
    const gp_Pnt&                  thePnt2,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const Standard_OVERRIDE;

  //! Intersection test between defined axis and given triangle. The test may
  //! be considered of interior part or boundary line defined by triangle vertices
  //! depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean OverlapsTriangle(
    const gp_Pnt&                  thePnt1,
    const gp_Pnt&                  thePnt2,
    const gp_Pnt&                  thePnt3,
    Select3D_TypeOfSensitivity     theSensType,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const Standard_OVERRIDE;

  //! Intersection test between defined axis and given sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT virtual Standard_Boolean OverlapsSphere(const gp_Pnt&       theCenter,
                                                          const Standard_Real theRadius,
                                                          Standard_Boolean* theInside = NULL) const
    Standard_OVERRIDE;

  //! Intersection test between defined axis and given sphere with center theCenter
  //! and radius theRadius
  Standard_EXPORT virtual Standard_Boolean OverlapsSphere(
    const gp_Pnt&                  theCenter,
    const Standard_Real            theRadius,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT virtual Standard_Boolean OverlapsCylinder(
    const Standard_Real            theBottomRad,
    const Standard_Real            theTopRad,
    const Standard_Real            theHeight,
    const gp_Trsf&                 theTrsf,
    const Standard_Boolean         theIsHollow,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT virtual Standard_Boolean OverlapsCylinder(
    const Standard_Real    theBottomRad,
    const Standard_Real    theTopRad,
    const Standard_Real    theHeight,
    const gp_Trsf&         theTrsf,
    const Standard_Boolean theIsHollow,
    Standard_Boolean*      theInside = NULL) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  Standard_EXPORT virtual Standard_Boolean OverlapsCircle(
    const Standard_Real            theRadius,
    const gp_Trsf&                 theTrsf,
    const Standard_Boolean         theIsFilled,
    const SelectMgr_ViewClipRange& theClipRange,
    SelectBasics_PickResult&       thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  Standard_EXPORT virtual Standard_Boolean OverlapsCircle(const Standard_Real    theRadius,
                                                          const gp_Trsf&         theTrsf,
                                                          const Standard_Boolean theIsFilled,
                                                          Standard_Boolean* theInside = NULL) const
    Standard_OVERRIDE;

public:
  //! Measures distance between start axis point and given point theCOG.
  Standard_EXPORT virtual Standard_Real DistToGeometryCenter(const gp_Pnt& theCOG) const
    Standard_OVERRIDE;

  //! Calculates the point on a axis ray that was detected during the run of selection algo by given
  //! depth
  Standard_EXPORT virtual gp_Pnt DetectedPoint(const Standard_Real theDepth) const
    Standard_OVERRIDE;

  //! Returns near point along axis.
  Standard_EXPORT virtual const gp_Pnt& GetNearPnt() const Standard_OVERRIDE;

  //! Returns far point along axis (infinite).
  Standard_EXPORT virtual const gp_Pnt& GetFarPnt() const Standard_OVERRIDE;

  //! Returns axis direction.
  Standard_EXPORT virtual const gp_Dir& GetViewRayDirection() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

protected:
  //! Returns true if selecting axis intersects axis-aligned bounding box
  //! with minimum corner at point theBoxMin and maximum at point theBoxMax.
  //! Also returns enter and leave time of axis-box intersection.
  Standard_EXPORT Standard_Boolean hasIntersection(const SelectMgr_Vec3& theBoxMin,
                                                   const SelectMgr_Vec3& theBoxMax,
                                                   Standard_Real&        theTimeEnter,
                                                   Standard_Real&        theTimeLeave) const;

  //! Returns true if selecting axis intersects point.
  //! Also returns time of axis-point intersection.
  Standard_EXPORT Standard_Boolean hasIntersection(const gp_Pnt&  thePnt,
                                                   Standard_Real& theDepth) const;

  //! Returns true if selecting axis intersects segment.
  //! Also saves time of axis-segment intersection and intersection point as pick result.
  Standard_EXPORT Standard_Boolean raySegmentDistance(const gp_Pnt&            theSegPnt1,
                                                      const gp_Pnt&            theSegPnt2,
                                                      SelectBasics_PickResult& thePickResult) const;

  //! Returns true if selecting axis intersects plane.
  //! Also saves time of axis-plane intersection and intersection point as pick result.
  Standard_EXPORT Standard_Boolean
    rayPlaneIntersection(const gp_Vec&            thePlane,
                         const gp_Pnt&            thePntOnPlane,
                         SelectBasics_PickResult& thePickResult) const;

private:
  gp_Ax1 myAxis;
};

#endif // _SelectMgr_AxisIntersector_HeaderFile
