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
#include <SelectMgr_VectorTypes.hxx>
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

  virtual ~SelectMgr_SelectingVolumeManager() {}

  //! Creates, initializes and activates rectangular selecting frustum for point selection
  Standard_EXPORT void InitPointSelectingVolume (const gp_Pnt2d& thePoint);

  //! Creates, initializes and activates rectangular selecting frustum for box selection
  Standard_EXPORT void InitBoxSelectingVolume (const gp_Pnt2d& theMinPt,
                                               const gp_Pnt2d& theMaxPt);

  //! Creates, initializes and activates set of triangular selecting frustums for polyline selection
  Standard_EXPORT void InitPolylineSelectingVolume (const TColgp_Array1OfPnt2d& thePoints);

  //! Creates and activates axis selector for point selection
  Standard_EXPORT void InitAxisSelectingVolume (const gp_Ax1& theAxis);

  //! Sets as active the custom selecting volume
  Standard_EXPORT void InitSelectingVolume (const Handle(SelectMgr_BaseIntersector)& theVolume);

  //! Builds previously initialized selecting volume.
  Standard_EXPORT void BuildSelectingVolume();

  //! Returns active selecting volume that was built during last
  //! run of OCCT selection mechanism
  const Handle(SelectMgr_BaseIntersector)& ActiveVolume() const { return myActiveSelectingVolume; }

  // Returns active selection type (point, box, polyline)
  Standard_EXPORT virtual Standard_Integer GetActiveSelectionType() const Standard_OVERRIDE;

  //! IMPORTANT: Scaling makes sense only for frustum built on a single point!
  //!            Note that this method does not perform any checks on type of the frustum.
  //!
  //! Returns a copy of the frustum resized according to the scale factor given
  //! and transforms it using the matrix given.
  //! There are no default parameters, but in case if:
  //!    - transformation only is needed: @theScaleFactor must be initialized as any negative value;
  //!    - scale only is needed: @theTrsf must be set to gp_Identity.
  //! Builder is an optional argument that represents corresponding settings for re-constructing transformed
  //! frustum from scratch. Can be null if reconstruction is not expected furthermore.
  Standard_EXPORT virtual SelectMgr_SelectingVolumeManager ScaleAndTransform (const Standard_Integer theScaleFactor,
                                                                              const gp_GTrsf& theTrsf,
                                                                              const Handle(SelectMgr_FrustumBuilder)& theBuilder) const;

public:

  //! Returns current camera definition.
  Standard_EXPORT const Handle(Graphic3d_Camera)& Camera() const;

  //! Updates camera projection and orientation matrices in all selecting volumes
  //! Note: this method should be called after selection volume building
  //! else exception will be thrown
  Standard_EXPORT void SetCamera (const Handle(Graphic3d_Camera) theCamera);

  //! Updates viewport in all selecting volumes
  //! Note: this method should be called after selection volume building
  //! else exception will be thrown
  Standard_EXPORT void SetViewport (const Standard_Real theX,
                                    const Standard_Real theY,
                                    const Standard_Real theWidth,
                                    const Standard_Real theHeight);

  //! Updates pixel tolerance in all selecting volumes
  //! Note: this method should be called after selection volume building
  //! else exception will be thrown
  Standard_EXPORT void SetPixelTolerance (const Standard_Integer theTolerance);

  //! Returns window size
  Standard_EXPORT void WindowSize (Standard_Integer& theWidth, Standard_Integer& theHeight) const;

  //! Updates window size in all selecting volumes
  //! Note: this method should be called after selection volume building
  //! else exception will be thrown
  Standard_EXPORT void SetWindowSize (const Standard_Integer theWidth, const Standard_Integer theHeight);


  //! SAT intersection test between defined volume and given axis-aligned box
  Standard_EXPORT virtual Standard_Boolean OverlapsBox (const SelectMgr_Vec3& theBoxMin,
                                                        const SelectMgr_Vec3& theBoxMax,
                                                        SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_EXPORT  virtual Standard_Boolean OverlapsBox (const SelectMgr_Vec3& theBoxMin,
                                                         const SelectMgr_Vec3& theBoxMax,
                                                         Standard_Boolean*     theInside = NULL) const Standard_OVERRIDE;

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual Standard_Boolean OverlapsPoint (const gp_Pnt& thePnt,
                                                          SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual Standard_Boolean OverlapsPoint (const gp_Pnt& thePnt) const Standard_OVERRIDE;

  //! SAT intersection test between defined volume and given ordered set of points,
  //! representing line segments. The test may be considered of interior part or
  //! boundary line defined by segments depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean OverlapsPolygon (const TColgp_Array1OfPnt& theArrayOfPts,
                                                            Standard_Integer theSensType,
                                                            SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Checks if line segment overlaps selecting frustum
  Standard_EXPORT virtual Standard_Boolean OverlapsSegment (const gp_Pnt& thePnt1,
                                                            const gp_Pnt& thePnt2,
                                                            SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! SAT intersection test between defined volume and given triangle. The test may
  //! be considered of interior part or boundary line defined by triangle vertices
  //! depending on given sensitivity type
  Standard_EXPORT  virtual Standard_Boolean OverlapsTriangle (const gp_Pnt& thePnt1,
                                                              const gp_Pnt& thePnt2,
                                                              const gp_Pnt& thePnt3,
                                                              Standard_Integer theSensType,
                                                              SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Intersection test between defined volume and given sphere
  Standard_EXPORT virtual Standard_Boolean OverlapsSphere (const gp_Pnt& theCenter,
                                                           const Standard_Real theRadius,
                                                           SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Intersection test between defined volume and given sphere
  Standard_EXPORT virtual Standard_Boolean OverlapsSphere (const gp_Pnt& theCenter,
                                                           const Standard_Real theRadius,
                                                           Standard_Boolean* theInside = NULL) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses theBottomRad
  //! and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT virtual Standard_Boolean OverlapsCylinder (const Standard_Real theBottomRad,
                                                             const Standard_Real theTopRad,
                                                             const Standard_Real theHeight,
                                                             const gp_Trsf& theTrsf,
                                                             SelectBasics_PickResult& thePickResult) const Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by cylinder (or cone) with radiuses theBottomRad
  //! and theTopRad, height theHeight and transformation to apply theTrsf.
  Standard_EXPORT virtual Standard_Boolean OverlapsCylinder (const Standard_Real theBottomRad,
                                                             const Standard_Real theTopRad,
                                                             const Standard_Real theHeight,
                                                             const gp_Trsf& theTrsf,
                                                             Standard_Boolean* theInside = NULL) const Standard_OVERRIDE;

  //! Measures distance between 3d projection of user-picked
  //! screen point and given point theCOG
  Standard_EXPORT virtual Standard_Real DistToGeometryCenter (const gp_Pnt& theCOG) const Standard_OVERRIDE;

  //! Calculates the point on a view ray that was detected during the run of selection algo by given depth.
  //! Throws exception if active selection type is not Point.
  Standard_EXPORT virtual gp_Pnt DetectedPoint (const Standard_Real theDepth) const Standard_OVERRIDE;

  //! If theIsToAllow is false, only fully included sensitives will be detected, otherwise the algorithm will
  //! mark both included and overlapped entities as matched
  Standard_EXPORT virtual void AllowOverlapDetection (const Standard_Boolean theIsToAllow);

  Standard_EXPORT virtual Standard_Boolean IsOverlapAllowed() const Standard_OVERRIDE;

  //! Return view clipping planes.
  const Handle(Graphic3d_SequenceOfHClipPlane)& ViewClipping() const { return myViewClipPlanes; }

  //! Return object clipping planes.
  const Handle(Graphic3d_SequenceOfHClipPlane)& ObjectClipping() const { return myObjectClipPlanes; }

  //! Valid for point selection only!
  //! Computes depth range for clipping planes.
  //! @param theViewPlanes  [in] global view planes
  //! @param theObjPlanes   [in] object planes
  //! @param theWorldSelMgr [in] selection volume in world space for computing clipping plane ranges
  Standard_EXPORT void SetViewClipping (const Handle(Graphic3d_SequenceOfHClipPlane)& theViewPlanes,
                                        const Handle(Graphic3d_SequenceOfHClipPlane)& theObjPlanes,
                                        const SelectMgr_SelectingVolumeManager* theWorldSelMgr);

  //! Copy clipping planes from another volume manager.
  Standard_EXPORT void SetViewClipping (const SelectMgr_SelectingVolumeManager& theOther);

  //! Return clipping range.
  const SelectMgr_ViewClipRange& ViewClipRanges() const { return myViewClipRange; }

  //! Set clipping range.
  void SetViewClipRanges (const SelectMgr_ViewClipRange& theRange) { myViewClipRange = theRange; }

  //! A set of helper functions that return rectangular selecting frustum data
  Standard_EXPORT const gp_Pnt* GetVertices() const;

  //! Valid only for point and rectangular selection.
  //! Returns projection of 2d mouse picked point or projection
  //! of center of 2d rectangle (for point and rectangular selection
  //! correspondingly) onto near view frustum plane
  Standard_EXPORT virtual gp_Pnt GetNearPickedPnt() const Standard_OVERRIDE;

  //! Valid only for point and rectangular selection.
  //! Returns projection of 2d mouse picked point or projection
  //! of center of 2d rectangle (for point and rectangular selection
  //! correspondingly) onto far view frustum plane
  Standard_EXPORT virtual gp_Pnt GetFarPickedPnt() const Standard_OVERRIDE;

  //! Valid only for point and rectangular selection.
  //! Returns view ray direction
  Standard_EXPORT virtual gp_Dir GetViewRayDirection() const Standard_OVERRIDE;

  //! Checks if it is possible to scale current active selecting volume
  Standard_EXPORT virtual Standard_Boolean IsScalableActiveVolume() const Standard_OVERRIDE;

  //! Returns mouse coordinates for Point selection mode.
  //! @return infinite point in case of unsupport of mouse position for this active selection volume.
  Standard_EXPORT virtual gp_Pnt2d GetMousePosition() const Standard_OVERRIDE;

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  Standard_EXPORT virtual void GetPlanes (NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

public:

  Standard_DEPRECATED("Deprecated method - InitPointSelectingVolume() and Build() methods should be used instead")
  Standard_EXPORT void BuildSelectingVolume (const gp_Pnt2d& thePoint);

  Standard_DEPRECATED("Deprecated method - InitBoxSelectingVolume() and Build() should be used instead")
  Standard_EXPORT void BuildSelectingVolume (const gp_Pnt2d& theMinPt,
                                             const gp_Pnt2d& theMaxPt);

  Standard_DEPRECATED("Deprecated method - InitPolylineSelectingVolume() and Build() should be used instead")
  Standard_EXPORT void BuildSelectingVolume (const TColgp_Array1OfPnt2d& thePoints);

private:
  Handle(SelectMgr_BaseIntersector)      myActiveSelectingVolume;
  Handle(Graphic3d_SequenceOfHClipPlane) myViewClipPlanes;                  //!< view clipping planes
  Handle(Graphic3d_SequenceOfHClipPlane) myObjectClipPlanes;                //!< object clipping planes
  SelectMgr_ViewClipRange                myViewClipRange;
  Standard_Boolean                       myToAllowOverlap;                  //!< Defines if partially overlapped entities will me detected or not
};

#endif
