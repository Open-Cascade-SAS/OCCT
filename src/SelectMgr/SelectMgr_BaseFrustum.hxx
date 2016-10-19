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

#ifndef _SelectMgr_BaseFrustum_HeaderFile
#define _SelectMgr_BaseFrustum_HeaderFile

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>

#include <Graphic3d_Camera.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_WorldViewProjState.hxx>

#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <Select3D_BndBox3d.hxx>
#include <SelectMgr_FrustumBuilder.hxx>
#include <Select3D_TypeOfSensitivity.hxx>
#include <SelectMgr_VectorTypes.hxx>

//! This class is an interface for different types of selecting frustums,
//! defining different selection types, like point, box or polyline
//! selection. It contains signatures of functions for detection of
//! overlap by sensitive entity and initializes some data for building
//! the selecting frustum
class SelectMgr_BaseFrustum : public Standard_Transient
{
public:

  //! Creates new selecting volume with pixel toletance set to 2,
  //! orthographic camera and empty frustum builder
  Standard_EXPORT SelectMgr_BaseFrustum();

  virtual ~SelectMgr_BaseFrustum() {}

  //! Return camera definition.
  const Handle(Graphic3d_Camera)& Camera() const { return myCamera; }

  //! Passes camera projection and orientation matrices to builder
  Standard_EXPORT void SetCamera (const Handle(Graphic3d_Camera)& theCamera);

  //! Passes camera projection and orientation matrices to builder
  Standard_EXPORT void SetCamera (const Graphic3d_Mat4d& theProjection,
                                  const Graphic3d_Mat4d& theWorldView,
                                  const Standard_Boolean theIsOrthographic,
                                  const Graphic3d_WorldViewProjState& theWVPState = Graphic3d_WorldViewProjState());

  //! @return current camera projection transformation common for all selecting volumes
  Standard_EXPORT const Graphic3d_Mat4d& ProjectionMatrix() const;

  //! @return current camera world view transformation common for all selecting volumes
  Standard_EXPORT const Graphic3d_Mat4d& WorldViewMatrix() const;

  //! @return current camera world view projection transformation state
  Standard_EXPORT const Graphic3d_WorldViewProjState& WorldViewProjState() const;

  Standard_EXPORT void SetPixelTolerance (const Standard_Integer theTol);

  Standard_EXPORT void SetWindowSize (const Standard_Integer theWidth, 
                                      const Standard_Integer theHeight);

  Standard_EXPORT void WindowSize (Standard_Integer& theWidth,
                                   Standard_Integer& theHeight) const;

  //! Passes viewport parameters to builder
  Standard_EXPORT void SetViewport (const Standard_Real theX,
                                    const Standard_Real theY,
                                    const Standard_Real theWidth,
                                    const Standard_Real theHeight);

  //! Nullifies the builder created in the constructor and copies the pointer given
  Standard_EXPORT void SetBuilder (const Handle(SelectMgr_FrustumBuilder)& theBuilder);


  //! Builds volume according to the point and given pixel tolerance
  virtual void Build (const gp_Pnt2d& /*thePoint*/) {}

  //! Builds volume according to the selected rectangle
  virtual void Build (const gp_Pnt2d& /*theMinPt*/,
                      const gp_Pnt2d& /*theMaxPt*/) {}

  //! Builds volume according to the triangle given
  virtual void Build (const gp_Pnt2d& /*theP1*/,
                      const gp_Pnt2d& /*theP2*/,
                      const gp_Pnt2d& /*theP3*/) {}

  //! Builds selecting volumes set according to polyline points
  virtual void Build (const TColgp_Array1OfPnt2d& /*thePoints*/) {}

  //! IMPORTANT: Scaling makes sense only for frustum built on a single point!
  //!            Note that this method does not perform any checks on type of the frustum.
  //! Returns a copy of the frustum resized according to the scale factor given
  //! and transforms it using the matrix given.
  //! There are no default parameters, but in case if:
  //!    - transformation only is needed: @theScaleFactor must be initialized as any negative value;
  //!    - scale only is needed: @theTrsf must be set to gp_Identity.
  Standard_EXPORT virtual Handle(SelectMgr_BaseFrustum) ScaleAndTransform (const Standard_Integer /*theScaleFactor*/,
                                                                           const gp_GTrsf& /*theTrsf*/) const { return NULL; }

  //! SAT intersection test between defined volume and given axis-aligned box
  Standard_EXPORT virtual Standard_Boolean Overlaps (const SelectMgr_Vec3& theBoxMin,
                                                     const SelectMgr_Vec3& theBoxMax,
                                                     Standard_Real& theDepth);

  //! Returns true if selecting volume is overlapped by axis-aligned bounding box
  //! with minimum corner at point theMinPt and maximum at point theMaxPt
  Standard_EXPORT virtual Standard_Boolean Overlaps (const SelectMgr_Vec3& theBoxMin,
                                                     const SelectMgr_Vec3& theBoxMax,
                                                     Standard_Boolean*     theInside = NULL);

  //! Intersection test between defined volume and given point
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt,
                                                     Standard_Real& theDepth);

  //! Intersection test between defined volume and given point
  //! Does not perform depth calculation, so this method is defined as
  //! helper function for inclusion test. Therefore, its implementation
  //! makes sense only for rectangular frustum with box selection mode activated.
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt);

  //! SAT intersection test between defined volume and given ordered set of points,
  //! representing line segments. The test may be considered of interior part or
  //! boundary line defined by segments depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean Overlaps (const TColgp_Array1OfPnt& theArrayOfPnts,
                                                     Select3D_TypeOfSensitivity theSensType,
                                                     Standard_Real& theDepth);

  //! Checks if line segment overlaps selecting frustum
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePnt1,
                                                     const gp_Pnt& thePnt2,
                                                     Standard_Real& theDepth);

  //! SAT intersection test between defined volume and given triangle. The test may
  //! be considered of interior part or boundary line defined by triangle vertices
  //! depending on given sensitivity type
  Standard_EXPORT virtual Standard_Boolean Overlaps (const gp_Pnt& thePt1,
                                                     const gp_Pnt& thePt2,
                                                     const gp_Pnt& thePt3,
                                                     Select3D_TypeOfSensitivity theSensType,
                                                     Standard_Real& theDepth);

  //! Measures distance between 3d projection of user-picked
  //! screen point and given point theCOG
  Standard_EXPORT virtual Standard_Real DistToGeometryCenter (const gp_Pnt& theCOG);

  Standard_EXPORT virtual gp_Pnt DetectedPoint (const Standard_Real theDepth) const;

  //! Checks if the point of sensitive in which selection was detected belongs
  //! to the region defined by clipping planes
  Standard_EXPORT virtual Standard_Boolean IsClipped (const Graphic3d_SequenceOfHClipPlane& thePlanes,
                                                      const Standard_Real theDepth);

  //! Valid for point selection only!
  //! Computes depth range for global (defined for the whole view) clipping planes.
  virtual void SetViewClipping (const Handle(Graphic3d_SequenceOfHClipPlane)& /*thePlanes*/) {};

  //! Set if view clipping plane is enabled or not.
  //! @return previous value of the flag
  virtual Standard_Boolean SetViewClippingEnabled (const Standard_Boolean /*theToEnable*/) { return Standard_False; }

  //! Stores plane equation coefficients (in the following form:
  //! Ax + By + Cz + D = 0) to the given vector
  virtual void GetPlanes (NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const
  {
    thePlaneEquations.Clear();
    return;
  }

  DEFINE_STANDARD_RTTIEXT(SelectMgr_BaseFrustum,Standard_Transient)

protected:
  Standard_Integer    myPixelTolerance;      //!< Pixel tolerance
  Standard_Boolean    myIsOrthographic;      //!< Defines if current camera is orthographic

  Handle(SelectMgr_FrustumBuilder) myBuilder; //!< A tool implementing methods for volume build
  Handle(Graphic3d_Camera)         myCamera;  //!< camera definition
};

#endif // _SelectMgr_BaseFrustum_HeaderFile
