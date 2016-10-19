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

#include <NCollection_Handle.hxx>

#include <Graphic3d_Camera.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_WorldViewProjState.hxx>

#include <SelectMgr_BaseFrustum.hxx>
#include <SelectMgr_RectangularFrustum.hxx>
#include <SelectMgr_TriangularFrustumSet.hxx>
#include <SelectBasics_SelectingVolumeManager.hxx>

//! This class is used to switch between active selecting volumes depending
//! on selection type chosen by the user
class SelectMgr_SelectingVolumeManager : public SelectBasics_SelectingVolumeManager
{
public:

  //! Creates instances of all available selecting volume types
  Standard_EXPORT SelectMgr_SelectingVolumeManager (Standard_Boolean theToAllocateFrustums = Standard_True);

  virtual ~SelectMgr_SelectingVolumeManager() {};

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
                                                                              const Handle(SelectMgr_FrustumBuilder)& theBuilder = NULL) const;

  Standard_EXPORT virtual Standard_Integer GetActiveSelectionType() const Standard_OVERRIDE;

  Standard_EXPORT void SetActiveSelectionType (const SelectionType& theType);

  //! Returns current camera definition.
  const Handle(Graphic3d_Camera)& Camera() const { return mySelectingVolumes[Frustum]->Camera(); }

  //! Updates camera projection and orientation matrices in all selecting volumes
  Standard_EXPORT void SetCamera (const Handle(Graphic3d_Camera) theCamera);

  //! Updates camera projection and orientation matrices in all selecting volumes
  Standard_EXPORT void SetCamera (const Graphic3d_Mat4d& theProjection,
                                  const Graphic3d_Mat4d& theWorldView,
                                  const Standard_Boolean theIsOrthographic,
                                  const Graphic3d_WorldViewProjState& theWVPState = Graphic3d_WorldViewProjState());

  //! @return current projection transformation common for all selecting volumes
  Standard_EXPORT const Graphic3d_Mat4d& ProjectionMatrix() const;

  //! @return current world view transformation common for all selecting volumes
  Standard_EXPORT const Graphic3d_Mat4d& WorldViewMatrix() const;

  Standard_EXPORT void WindowSize (Standard_Integer& theWidth, Standard_Integer& theHeight) const;

  //! @return current camera world view projection transformation state common for all selecting volumes
  Standard_EXPORT const Graphic3d_WorldViewProjState& WorldViewProjState() const;

  //! Updates viewport in all selecting volumes
  Standard_EXPORT void SetViewport (const Standard_Real theX,
                                    const Standard_Real theY,
                                    const Standard_Real theWidth,
                                    const Standard_Real theHeight);

  //! Updates pixel tolerance in all selecting volumes
  Standard_EXPORT void SetPixelTolerance (const Standard_Integer theTolerance);

  //! Updates window size in all selecting volumes
  Standard_EXPORT void SetWindowSize (const Standard_Integer theWidth, const Standard_Integer theHeight);


  //! Builds rectangular selecting frustum for point selection
  Standard_EXPORT void BuildSelectingVolume (const gp_Pnt2d& thePoint);

  //! Builds rectangular selecting frustum for box selection
  Standard_EXPORT void BuildSelectingVolume (const gp_Pnt2d& theMinPt,
                                             const gp_Pnt2d& theMaxPt);

  //! Builds set of triangular selecting frustums for polyline selection
  Standard_EXPORT void BuildSelectingVolume (const TColgp_Array1OfPnt2d& thePoints);


  //! SAT intersection test between defined volume and given axis-aligned box
  Standard_EXPORT virtual Standard_Boolean Overlaps (const SelectMgr_Vec3& theBoxMin,
                                                      const SelectMgr_Vec3& theBoxMax,
                                                     Standard_Real& theDepth) Standard_OVERRIDE;

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_EXPORT  virtual Standard_Boolean Overlaps (const SelectMgr_Vec3& theBoxMin,
                                                      const SelectMgr_Vec3& theBoxMax,
                                                      Standard_Boolean*     theInside = NULL) Standard_OVERRIDE;

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt,
                                                     Standard_Real& theDepth) Standard_OVERRIDE;

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt) Standard_OVERRIDE;

  //! SAT intersection test between defined volume and given ordered set of points,
  //! representing line segments. The test may be considered of interior part or
  //! boundary line defined by segments depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean Overlaps (const Handle(TColgp_HArray1OfPnt)& theArrayOfPts,
                                                     Standard_Integer theSensType,
                                                     Standard_Real& theDepth) Standard_OVERRIDE;

  //! SAT intersection test between defined volume and given ordered set of points,
  //! representing line segments. The test may be considered of interior part or
  //! boundary line defined by segments depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean Overlaps (const TColgp_Array1OfPnt& theArrayOfPts,
                                                     Standard_Integer theSensType,
                                                     Standard_Real& theDepth) Standard_OVERRIDE;

  //! Checks if line segment overlaps selecting frustum
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt1,
                                                     const gp_Pnt& thePnt2,
                                                     Standard_Real& theDepth) Standard_OVERRIDE;

  //! SAT intersection test between defined volume and given triangle. The test may
  //! be considered of interior part or boundary line defined by triangle vertices
  //! depending on given sensitivity type
  Standard_EXPORT  virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt1,
                                                      const gp_Pnt& thePnt2,
                                                      const gp_Pnt& thePnt3,
                                                      Standard_Integer theSensType,
                                                      Standard_Real& theDepth) Standard_OVERRIDE;


  //! Measures distance between 3d projection of user-picked
  //! screen point and given point theCOG
  Standard_EXPORT virtual Standard_Real DistToGeometryCenter (const gp_Pnt& theCOG) Standard_OVERRIDE;

  //! Calculates the point on a view ray that was detected during the run of selection algo by given depth.
  //! Throws exception if active selection type is not Point.
  Standard_EXPORT virtual gp_Pnt DetectedPoint (const Standard_Real theDepth) const Standard_OVERRIDE;

  //! Checks if the point of sensitive in which selection was detected belongs
  //! to the region defined by clipping planes
  Standard_EXPORT virtual Standard_Boolean IsClipped (const Graphic3d_SequenceOfHClipPlane& thePlanes,
                                                      const Standard_Real& theDepth);

  //! Is used for rectangular selection only
  //! If theIsToAllow is false, only fully included sensitives will be detected, otherwise the algorithm will
  //! mark both included and overlapped entities as matched
  Standard_EXPORT virtual void AllowOverlapDetection (const Standard_Boolean theIsToAllow);

  Standard_EXPORT virtual Standard_Boolean IsOverlapAllowed() const Standard_OVERRIDE;

  //! Return view clipping planes.
  const Handle(Graphic3d_SequenceOfHClipPlane)& ViewClipping() const { return myViewClipPlanes; }

  //! Valid for point selection only!
  //! Computes depth range for global (defined for the whole view) clipping planes.
  Standard_EXPORT void SetViewClipping (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes);

  //! Set if view clipping plane is enabled or not.
  //! @return previous flag value
  Standard_EXPORT Standard_Boolean SetViewClippingEnabled (const Standard_Boolean theToEnable);

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

  //! Returns active selecting volume that was built during last
  //! run of OCCT selection mechanism
  Handle(SelectMgr_BaseFrustum) ActiveVolume() const
  {
    if (myActiveSelectionType == Unknown)
      return Handle(SelectMgr_BaseFrustum)();

    return mySelectingVolumes[myActiveSelectionType / 2];
  }

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  virtual void GetPlanes (NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const Standard_OVERRIDE
  {
    if (myActiveSelectionType == Unknown)
    {
      thePlaneEquations.Clear();
      return;
    }

    return mySelectingVolumes[myActiveSelectionType / 2]->GetPlanes (thePlaneEquations);
  }

private:
  enum { Frustum, FrustumSet, VolumeTypesNb };       //!< Defines the amount of available selecting volumes

  Handle(SelectMgr_BaseFrustum)          mySelectingVolumes[VolumeTypesNb]; //!< Array of selecting volumes
  Handle(Graphic3d_SequenceOfHClipPlane) myViewClipPlanes;                  //!< view clipping planes
  Standard_Boolean                       myToAllowOverlap;                  //!< Defines if partially overlapped entities will me detected or not
};

#endif
