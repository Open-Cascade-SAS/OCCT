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

#ifndef _SelectMgr_SelectingVolumeManager_HeaderFile
#define _SelectMgr_SelectingVolumeManager_HeaderFile

#include <SelectBasics_SelectingVolumeManager.hxx>

#include <SelectMgr_BaseIntersector.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Mat4.hxx>
#include <NCollection_Vec3.hxx>
#include <NCollection_Vec4.hxx>
#include <SelectMgr_ViewClipRange.hxx>

//! This class is used to switch between active selecting volumes depending
//! on selection type chosen by the user.
//! The sample of correct selection volume initialization procedure:
//! @code
//!   aMgr.InitPointSelectingVolume (aMousePos);
//!   aMgr.SetPixelTolerance (aTolerance);
//!   aMgr.SetCamera (aCamera);
//!   aMgr.SetWindowSize (aWidth, aHeight);
//!   aMgr.BuildSelectingVolume();
//! @endcode
class SelectMgr_SelectingVolumeManager : public SelectBasics_SelectingVolumeManager
{
public:
  //! Creates instances of all available selecting volume types
  Standard_EXPORT SelectMgr_SelectingVolumeManager();

  ~SelectMgr_SelectingVolumeManager() override = default;

  //! Creates, initializes and activates rectangular selecting frustum for point selection
  Standard_EXPORT void InitPointSelectingVolume(const gp_Pnt2d& thePoint);

  //! Creates, initializes and activates rectangular selecting frustum for box selection
  Standard_EXPORT void InitBoxSelectingVolume(const gp_Pnt2d& theMinPt, const gp_Pnt2d& theMaxPt);

  //! Creates, initializes and activates set of triangular selecting frustums for polyline selection
  Standard_EXPORT void InitPolylineSelectingVolume(const NCollection_Array1<gp_Pnt2d>& thePoints);

  //! Creates and activates axis selector for point selection
  Standard_EXPORT void InitAxisSelectingVolume(const gp_Ax1& theAxis);

  //! Sets as active the custom selecting volume
  Standard_EXPORT void InitSelectingVolume(const occ::handle<SelectMgr_BaseIntersector>& theVolume);

  //! Builds previously initialized selecting volume.
  Standard_EXPORT void BuildSelectingVolume();

  //! Returns active selecting volume that was built during last
  //! run of OCCT selection mechanism
  const occ::handle<SelectMgr_BaseIntersector>& ActiveVolume() const
  {
    return myActiveSelectingVolume;
  }

  // Returns active selection type (point, box, polyline)
  Standard_EXPORT int GetActiveSelectionType() const override;

  //! IMPORTANT: Scaling makes sense only for frustum built on a single point!
  //!            Note that this method does not perform any checks on type of the frustum.
  //!
  //! Returns a copy of the frustum resized according to the scale factor given
  //! and transforms it using the matrix given.
  //! There are no default parameters, but in case if:
  //!    - transformation only is needed: @theScaleFactor must be initialized as any negative value;
  //!    - scale only is needed: @theTrsf must be set to gp_Identity.
  //! Builder is an optional argument that represents corresponding settings for re-constructing
  //! transformed frustum from scratch. Can be null if reconstruction is not expected furthermore.
  Standard_EXPORT virtual SelectMgr_SelectingVolumeManager ScaleAndTransform(
    const int                                    theScaleFactor,
    const gp_GTrsf&                              theTrsf,
    const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const;

  //! Returns a copy of the selecting volume manager and its active frustum re-constructed using the
  //! passed builder. Builder is an argument that represents corresponding settings for
  //! re-constructing transformed frustum from scratch.
  Standard_EXPORT virtual SelectMgr_SelectingVolumeManager CopyWithBuilder(
    const occ::handle<SelectMgr_FrustumBuilder>& theBuilder) const;

public:
  //! Returns current camera definition.
  Standard_EXPORT const occ::handle<Graphic3d_Camera>& Camera() const;

  //! Updates camera projection and orientation matrices in all selecting volumes
  //! Note: this method should be called after selection volume building
  //! else exception will be thrown
  Standard_EXPORT void SetCamera(const occ::handle<Graphic3d_Camera>& theCamera);

  //! Updates viewport in all selecting volumes
  //! Note: this method should be called after selection volume building
  //! else exception will be thrown
  Standard_EXPORT void SetViewport(const double theX,
                                   const double theY,
                                   const double theWidth,
                                   const double theHeight);

  //! Updates pixel tolerance in all selecting volumes
  //! Note: this method should be called after selection volume building
  //! else exception will be thrown
  Standard_EXPORT void SetPixelTolerance(const int theTolerance);

  //! Returns window size
  Standard_EXPORT void WindowSize(int& theWidth, int& theHeight) const;

  //! Updates window size in all selecting volumes
  //! Note: this method should be called after selection volume building
  //! else exception will be thrown
  Standard_EXPORT void SetWindowSize(const int theWidth, const int theHeight);

  //! SAT intersection test between defined volume and given axis-aligned box
  Standard_EXPORT bool OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                                           const NCollection_Vec3<double>& theBoxMax,
                                           SelectBasics_PickResult& thePickResult) const override;

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_EXPORT bool OverlapsBox(const NCollection_Vec3<double>& theBoxMin,
                                           const NCollection_Vec3<double>& theBoxMax,
                                           bool* theInside = nullptr) const override;

  //! Intersection test between defined volume and given point
  Standard_EXPORT bool OverlapsPoint(const gp_Pnt&            thePnt,
                                             SelectBasics_PickResult& thePickResult) const override;

  //! Intersection test between defined volume and given point
  Standard_EXPORT bool OverlapsPoint(const gp_Pnt& thePnt) const override;

  //! SAT intersection test between defined volume and given ordered set of points,
  //! representing line segments. The test may be considered of interior part or
  //! boundary line defined by segments depending on given sensitivity type
  Standard_EXPORT bool OverlapsPolygon(
    const NCollection_Array1<gp_Pnt>& theArrayOfPts,
    int                               theSensType,
    SelectBasics_PickResult&          thePickResult) const override;

  //! Checks if line segment overlaps selecting frustum
  Standard_EXPORT bool OverlapsSegment(
    const gp_Pnt&            thePnt1,
    const gp_Pnt&            thePnt2,
    SelectBasics_PickResult& thePickResult) const override;

  //! SAT intersection test between defined volume and given triangle. The test may
  //! be considered of interior part or boundary line defined by triangle vertices
  //! depending on given sensitivity type
  Standard_EXPORT bool OverlapsTriangle(
    const gp_Pnt&            thePnt1,
    const gp_Pnt&            thePnt2,
    const gp_Pnt&            thePnt3,
    int                      theSensType,
    SelectBasics_PickResult& thePickResult) const override;

  //! Intersection test between defined volume and given sphere
  Standard_EXPORT bool OverlapsSphere(
    const gp_Pnt&            theCenter,
    const double             theRadius,
    SelectBasics_PickResult& thePickResult) const override;

  //! Intersection test between defined volume and given sphere
  Standard_EXPORT bool OverlapsSphere(const gp_Pnt& theCenter,
                                              const double  theRadius,
                                              bool*         theInside = nullptr) const override;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT bool OverlapsCylinder(
    const double             theBottomRad,
    const double             theTopRad,
    const double             theHeight,
    const gp_Trsf&           theTrsf,
    const bool               theIsHollow,
    SelectBasics_PickResult& thePickResult) const override;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses
  //! theBottomRad and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT bool OverlapsCylinder(const double   theBottomRad,
                                                const double   theTopRad,
                                                const double   theHeight,
                                                const gp_Trsf& theTrsf,
                                                const bool     theIsHollow,
                                                bool*          theInside = nullptr) const override;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  Standard_EXPORT bool OverlapsCircle(
    const double             theBottomRad,
    const gp_Trsf&           theTrsf,
    const bool               theIsFilled,
    SelectBasics_PickResult& thePickResult) const override;

  //! Returns true if selecting volume is overlapped by circle with radius theRadius,
  //! boolean theIsFilled and transformation to apply theTrsf.
  //! The position and orientation of the circle are specified
  //! via theTrsf transformation for gp::XOY() with center in gp::Origin().
  Standard_EXPORT bool OverlapsCircle(const double   theBottomRad,
                                              const gp_Trsf& theTrsf,
                                              const bool     theIsFilled,
                                              bool*          theInside = nullptr) const override;

  //! Measures distance between 3d projection of user-picked
  //! screen point and given point theCOG
  Standard_EXPORT double DistToGeometryCenter(const gp_Pnt& theCOG) const override;

  //! Calculates the point on a view ray that was detected during the run of selection algo by given
  //! depth. Throws exception if active selection type is not Point.
  Standard_EXPORT gp_Pnt DetectedPoint(const double theDepth) const override;

  //! If theIsToAllow is false, only fully included sensitives will be detected, otherwise the
  //! algorithm will mark both included and overlapped entities as matched
  Standard_EXPORT virtual void AllowOverlapDetection(const bool theIsToAllow);

  Standard_EXPORT bool IsOverlapAllowed() const override;

  //! Return view clipping planes.
  const occ::handle<Graphic3d_SequenceOfHClipPlane>& ViewClipping() const
  {
    return myViewClipPlanes;
  }

  //! Return object clipping planes.
  const occ::handle<Graphic3d_SequenceOfHClipPlane>& ObjectClipping() const
  {
    return myObjectClipPlanes;
  }

  //! Valid for point selection only!
  //! Computes depth range for clipping planes.
  //! @param[in] theViewPlanes   global view planes
  //! @param[in] theObjPlanes    object planes
  //! @param[in] theWorldSelMgr  selection volume in world space for computing clipping plane ranges
  Standard_EXPORT void SetViewClipping(
    const occ::handle<Graphic3d_SequenceOfHClipPlane>& theViewPlanes,
    const occ::handle<Graphic3d_SequenceOfHClipPlane>& theObjPlanes,
    const SelectMgr_SelectingVolumeManager*            theWorldSelMgr);

  //! Copy clipping planes from another volume manager.
  Standard_EXPORT void SetViewClipping(const SelectMgr_SelectingVolumeManager& theOther);

  //! Return clipping range.
  const SelectMgr_ViewClipRange& ViewClipRanges() const { return myViewClipRange; }

  //! Set clipping range.
  void SetViewClipRanges(const SelectMgr_ViewClipRange& theRange) { myViewClipRange = theRange; }

  //! A set of helper functions that return rectangular selecting frustum data
  Standard_EXPORT const gp_Pnt* GetVertices() const;

  //! Valid only for point and rectangular selection.
  //! Returns projection of 2d mouse picked point or projection
  //! of center of 2d rectangle (for point and rectangular selection
  //! correspondingly) onto near view frustum plane
  Standard_EXPORT gp_Pnt GetNearPickedPnt() const override;

  //! Valid only for point and rectangular selection.
  //! Returns projection of 2d mouse picked point or projection
  //! of center of 2d rectangle (for point and rectangular selection
  //! correspondingly) onto far view frustum plane
  Standard_EXPORT gp_Pnt GetFarPickedPnt() const override;

  //! Valid only for point and rectangular selection.
  //! Returns view ray direction
  Standard_EXPORT gp_Dir GetViewRayDirection() const override;

  //! Checks if it is possible to scale current active selecting volume
  Standard_EXPORT bool IsScalableActiveVolume() const override;

  //! Returns mouse coordinates for Point selection mode.
  //! @return infinite point in case of unsupport of mouse position for this active selection
  //! volume.
  Standard_EXPORT gp_Pnt2d GetMousePosition() const override;

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  Standard_EXPORT void GetPlanes(
    NCollection_Vector<NCollection_Vec4<double>>& thePlaneEquations) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

public:
  Standard_DEPRECATED(
    "Deprecated method - InitPointSelectingVolume() and Build() methods should be used instead")
  Standard_EXPORT void BuildSelectingVolume(const gp_Pnt2d& thePoint);

  Standard_DEPRECATED(
    "Deprecated method - InitBoxSelectingVolume() and Build() should be used instead")
  Standard_EXPORT void BuildSelectingVolume(const gp_Pnt2d& theMinPt, const gp_Pnt2d& theMaxPt);

  Standard_DEPRECATED(
    "Deprecated method - InitPolylineSelectingVolume() and Build() should be used instead")
  Standard_EXPORT void BuildSelectingVolume(const NCollection_Array1<gp_Pnt2d>& thePoints);

private:
  occ::handle<SelectMgr_BaseIntersector> myActiveSelectingVolume;
  // clang-format off
  occ::handle<Graphic3d_SequenceOfHClipPlane> myViewClipPlanes;                  //!< view clipping planes
  occ::handle<Graphic3d_SequenceOfHClipPlane> myObjectClipPlanes;                //!< object clipping planes
  SelectMgr_ViewClipRange                myViewClipRange;
  bool                       myToAllowOverlap;                  //!< Defines if partially overlapped entities will me detected or not
  // clang-format on
};

#endif
