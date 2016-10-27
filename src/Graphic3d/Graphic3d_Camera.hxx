// Created on: 2013-05-29
// Created by: Anton POLETAEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_Camera_HeaderFile
#define _Graphic3d_Camera_HeaderFile

#include <Graphic3d_CameraTile.hxx>
#include <Graphic3d_Mat4d.hxx>
#include <Graphic3d_Mat4.hxx>
#include <Graphic3d_Vec3.hxx>
#include <Graphic3d_WorldViewProjState.hxx>
#include <NCollection_Lerp.hxx>

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <Standard_Macro.hxx>
#include <Standard_TypeDef.hxx>

#include <Bnd_Box.hxx>

//! Forward declaration
class Graphic3d_WorldViewProjState;

//! Camera class provides object-oriented approach to setting up projection
//! and orientation properties of 3D view.
class Graphic3d_Camera : public Standard_Transient
{
private:

  //! Template container for cached matrices or Real/ShortReal types.
  template<typename Elem_t>
  struct TransformMatrices
  {

    //! Default constructor.
    TransformMatrices() : myIsOrientationValid (Standard_False), myIsProjectionValid (Standard_False) {}

    //! Initialize orientation.
    void InitOrientation()
    {
      myIsOrientationValid = Standard_True;
      Orientation.InitIdentity();
    }

    //! Initialize projection.
    void InitProjection()
    {
      myIsProjectionValid = Standard_True;
      MProjection.InitIdentity();
      LProjection.InitIdentity();
      RProjection.InitIdentity();
    }

    //! Invalidate orientation.
    void ResetOrientation() { myIsOrientationValid = Standard_False; }

    //! Invalidate projection.
    void ResetProjection()  { myIsProjectionValid  = Standard_False; }

    //! Return true if Orientation was not invalidated.
    Standard_Boolean IsOrientationValid() const { return myIsOrientationValid; }

    //! Return true if Projection was not invalidated.
    Standard_Boolean IsProjectionValid()  const { return myIsProjectionValid;  }

  public:

    NCollection_Mat4<Elem_t> Orientation;
    NCollection_Mat4<Elem_t> MProjection;
    NCollection_Mat4<Elem_t> LProjection;
    NCollection_Mat4<Elem_t> RProjection;

  private:

    Standard_Boolean myIsOrientationValid;
    Standard_Boolean myIsProjectionValid;

  };

public:

  //! Enumerates supported monographic projections.
  //! - Projection_Orthographic : orthographic projection.
  //! - Projection_Perspective  : perspective projection.
  //! - Projection_Stereo       : stereographic projection.
  //! - Projection_MonoLeftEye  : mono projection for stereo left eye.
  //! - Projection_MonoRightEye : mono projection for stereo right eye.
  enum Projection
  {
    Projection_Orthographic,
    Projection_Perspective,
    Projection_Stereo,
    Projection_MonoLeftEye,
    Projection_MonoRightEye
  };

  //! Enumerates approaches to define stereographic focus.
  //! - FocusType_Absolute : focus is specified as absolute value.
  //! - FocusType_Relative : focus is specified relative to
  //! (as coefficient of) camera focal length.
  enum FocusType
  {
    FocusType_Absolute,
    FocusType_Relative
  };

  //! Enumerates approaches to define Intraocular distance.
  //! - IODType_Absolute : Intraocular distance is defined as absolute value.
  //! - IODType_Relative : Intraocular distance is defined relative to
  //! (as coefficient of) camera focal length.
  enum IODType
  {
    IODType_Absolute,
    IODType_Relative
  };

public:

  //! Default constructor.
  //! Initializes camera with the following properties:
  //! Eye (0, 0, -2); Center (0, 0, 0); Up (0, 1, 0);
  //! Type (Orthographic); FOVy (45); Scale (1000); IsStereo(false);
  //! ZNear (0.001); ZFar (3000.0); Aspect(1);
  //! ZFocus(1.0); ZFocusType(Relative); IOD(0.05); IODType(Relative)
  Standard_EXPORT Graphic3d_Camera();

  //! Copy constructor.
  //! @param theOther [in] the camera to copy from.
  Standard_EXPORT Graphic3d_Camera (const Handle(Graphic3d_Camera)& theOther);

  //! Initialize mapping related parameters from other camera handle.
  Standard_EXPORT void CopyMappingData (const Handle(Graphic3d_Camera)& theOtherCamera);

  //! Initialize orientation related parameters from other camera handle.
  Standard_EXPORT void CopyOrientationData (const Handle(Graphic3d_Camera)& theOtherCamera);

  //! Copy properties of another camera.
  //! @param theOther [in] the camera to copy from.
  Standard_EXPORT void Copy (const Handle(Graphic3d_Camera)& theOther);

//! @name Public camera properties
public:

  //! Sets camera Eye position.
  //! @param theEye [in] the location of camera's Eye.
  Standard_EXPORT void SetEye (const gp_Pnt& theEye);

  //! Get camera Eye position.
  //! @return camera eye location.
  const gp_Pnt& Eye() const
  {
    return myEye;
  }

  //! Sets Center of the camera.
  //! @param theCenter [in] the point where the camera looks at.
  Standard_EXPORT void SetCenter (const gp_Pnt& theCenter);

  //! Get Center of the camera.
  //! @return the point where the camera looks at.
  const gp_Pnt& Center() const
  {
    return myCenter;
  }

  //! Sets camera Up direction vector, orthogonal to camera direction.
  //! @param theUp [in] the Up direction vector.
  Standard_EXPORT void SetUp (const gp_Dir& theUp);

  //! Orthogonalize up direction vector.
  Standard_EXPORT void OrthogonalizeUp();

  //! Return a copy of orthogonalized up direction vector.
  Standard_EXPORT gp_Dir OrthogonalizedUp() const;

  //! Get camera Up direction vector.
  //! @return Camera's Up direction vector.
  const gp_Dir& Up() const
  {
    return myUp;
  }

  //! Set camera axial scale.
  //! @param theAxialScale [in] the axial scale vector.
  Standard_EXPORT void SetAxialScale (const gp_XYZ& theAxialScale);

  //! Get camera axial scale.
  //! @return Camera's axial scale.
  const gp_XYZ& AxialScale() const
  {
    return myAxialScale;
  }

  //! Set distance of Eye from camera Center.
  //! @param theDistance [in] the distance.
  Standard_EXPORT void SetDistance (const Standard_Real theDistance);

  //! Get distance of Eye from camera Center.
  //! @return the distance.
  Standard_EXPORT Standard_Real Distance() const;

  //! Sets camera look direction.
  //! @param theDir [in] the direction.
  Standard_EXPORT void SetDirection (const gp_Dir& theDir);

  //! Get camera look direction.
  //! @return camera look direction.
  Standard_EXPORT gp_Dir Direction() const;

  //! Sets camera scale. For orthographic projection the scale factor
  //! corresponds to parallel scale of view mapping  (i.e. size
  //! of viewport). For perspective camera scale is converted to
  //! distance. The scale specifies equal size of the view projection in
  //! both dimensions assuming that the aspect is 1.0. The projection height
  //! and width are specified with the scale and correspondingly multiplied
  //! by the aspect.
  //! @param theScale [in] the scale factor.
  Standard_EXPORT void SetScale (const Standard_Real theScale);

  //! Get camera scale.
  //! @return camera scale factor.
  Standard_EXPORT Standard_Real Scale() const;

  //! Change camera projection type.
  //! When switching to perspective projection from orthographic one,
  //! the ZNear and ZFar are reset to default values (0.001, 3000.0)
  //! if less than 0.0.
  //! @param theProjectionType [in] the camera projection type.
  Standard_EXPORT void SetProjectionType (const Projection theProjection);

  //! @return camera projection type.
  Projection ProjectionType() const
  {
    return myProjType;
  }

  //! Check that the camera projection is orthographic.
  //! @return boolean flag that indicates whether the camera's projection is
  //! orthographic or not.
  Standard_Boolean IsOrthographic() const
  {
    return (myProjType == Projection_Orthographic);
  }

  //! Check whether the camera projection is stereo.
  //! Please note that stereo rendering is now implemented with support of
  //! Quad buffering.
  //! @return boolean flag indicating whether the stereographic L/R projection
  //! is chosen.
  Standard_Boolean IsStereo() const
  {
    return (myProjType == Projection_Stereo);
  }

  //! Set Field Of View (FOV) in y axis for perspective projection.
  //! @param theFOVy [in] the FOV in degrees.
  Standard_EXPORT void SetFOVy (const Standard_Real theFOVy);

  //! Get Field Of View (FOV) in y axis.
  //! @return the FOV value in degrees.
  Standard_Real FOVy() const
  {
    return myFOVy;
  }

  //! Estimate Z-min and Z-max planes of projection volume to match the
  //! displayed objects. The methods ensures that view volume will
  //! be close by depth range to the displayed objects. Fitting assumes that
  //! for orthogonal projection the view volume contains the displayed objects
  //! completely. For zoomed perspective view, the view volume is adjusted such
  //! that it contains the objects or their parts, located in front of the camera.
  //! @param theScaleFactor [in] the scale factor for Z-range.
  //!   The range between Z-min, Z-max projection volume planes
  //!   evaluated by z fitting method will be scaled using this coefficient.
  //!   Program error exception is thrown if negative or zero value is passed.
  //! @param theMinMax [in] applicative min max boundaries.
  //! @param theScaleFactor [in] real graphical boundaries (not accounting infinite flag).
  Standard_EXPORT bool ZFitAll (const Standard_Real theScaleFactor,
                                const Bnd_Box&      theMinMax,
                                const Bnd_Box&      theGraphicBB,
                                Standard_Real&      theZNear,
                                Standard_Real&      theZFar) const;

  //! Change Z-min and Z-max planes of projection volume to match the displayed objects.
  void ZFitAll (const Standard_Real theScaleFactor, const Bnd_Box& theMinMax, const Bnd_Box& theGraphicBB)
  {
    Standard_Real aZNear = 0.0, aZFar = 1.0;
    ZFitAll (theScaleFactor, theMinMax, theGraphicBB, aZNear, aZFar);
    SetZRange (aZNear, aZFar);
  }

  //! Change the Near and Far Z-clipping plane positions.
  //! For orthographic projection, theZNear, theZFar can be negative or positive.
  //! For perspective projection, only positive values are allowed.
  //! Program error exception is raised if non-positive values are
  //! specified for perspective projection or theZNear >= theZFar.
  //! @param theZNear [in] the distance of the plane from the Eye.
  //! @param theZFar [in] the distance of the plane from the Eye.
  Standard_EXPORT void SetZRange (const Standard_Real theZNear, const Standard_Real theZFar);

  //! Get the Near Z-clipping plane position.
  //! @return the distance of the plane from the Eye.
  Standard_Real ZNear() const
  {
    return myZNear;
  }

  //! Get the Far Z-clipping plane position.
  //! @return the distance of the plane from the Eye.
  Standard_Real ZFar() const
  {
    return myZFar;
  }

  //! Changes width / height display ratio.
  //! @param theAspect [in] the display ratio.
  Standard_EXPORT void SetAspect (const Standard_Real theAspect);

  //! Get camera display ratio.
  //! @return display ratio.
  Standard_Real Aspect() const
  {
    return myAspect;
  }

  //! Sets stereographic focus distance.
  //! @param theType [in] the focus definition type. Focus can be defined
  //! as absolute value or relatively to (as coefficient of) coefficient of
  //! camera focal length.
  //! @param theZFocus [in] the focus absolute value or coefficient depending
  //! on the passed definition type.
  Standard_EXPORT void SetZFocus (const FocusType theType, const Standard_Real theZFocus);

  //! Get stereographic focus value.
  //! @return absolute or relative stereographic focus value
  //! depending on its definition type.
  Standard_Real ZFocus() const
  {
    return myZFocus;
  }

  //! Get stereographic focus definition type.
  //! @return definition type used for stereographic focus.
  FocusType ZFocusType() const
  {
    return myZFocusType;
  }

  //! Sets Intraocular distance.
  //! @param theType [in] the IOD definition type. IOD can be defined as
  //! absolute value or relatively to (as coefficient of) camera focal length.
  //! @param theIOD [in] the Intraocular distance.
  Standard_EXPORT void SetIOD (const IODType theType, const Standard_Real theIOD);

  //! Get Intraocular distance value.
  //! @return absolute or relative IOD value depending on its definition type.
  Standard_Real IOD() const
  {
    return myIOD;
  }

  //! Get Intraocular distance definition type.
  //! @return definition type used for Intraocular distance.
  IODType GetIODType() const
  {
    return myIODType;
  }

  //! Get current tile.
  const Graphic3d_CameraTile& Tile() const { return myTile; }

  //! Sets the Tile defining the drawing sub-area within View.
  //! Note that tile defining a region outside the view boundaries is also valid - use method Graphic3d_CameraTile::Cropped() to assign a cropped copy.
  //! @param theTile tile definition
  Standard_EXPORT void SetTile (const Graphic3d_CameraTile& theTile);

//! @name Basic camera operations
public:

  //! Transform orientation components of the camera:
  //! Eye, Up and Center points.
  //! @param theTrsf [in] the transformation to apply.
  Standard_EXPORT void Transform (const gp_Trsf& theTrsf);

  //! Calculate view plane size at center (target) point
  //! and distance between ZFar and ZNear planes.
  //! @return values in form of gp_Pnt (Width, Height, Depth).
  gp_XYZ ViewDimensions() const
  {
    return ViewDimensions (Distance());
  }

  //! Calculate view plane size at center point with specified Z offset
  //! and distance between ZFar and ZNear planes.
  //! @param theZValue [in] the distance from the eye in eye-to-center direction
  //! @return values in form of gp_Pnt (Width, Height, Depth).
  Standard_EXPORT gp_XYZ ViewDimensions (const Standard_Real theZValue) const;

  //! Calculate WCS frustum planes for the camera projection volume.
  //! Frustum is a convex volume determined by six planes directing
  //! inwards.
  //! The frustum planes are usually used as inputs for camera algorithms.
  //! Thus, if any changes to projection matrix calculation are necessary,
  //! the frustum planes calculation should be also touched.
  //! @param theLeft [out] the frustum plane for left side of view.
  //! @param theRight [out] the frustum plane for right side of view.
  //! @param theBottom [out] the frustum plane for bottom side of view.
  //! @param theTop [out] the frustum plane for top side of view.
  //! @param theNear [out] the frustum plane for near side of view.
  //! @param theFar [out] the frustum plane for far side of view.
  Standard_EXPORT void Frustum (gp_Pln& theLeft,
                                gp_Pln& theRight,
                                gp_Pln& theBottom,
                                gp_Pln& theTop,
                                gp_Pln& theNear,
                                gp_Pln& theFar) const;

//! @name Projection methods
public:

  //! Project point from world coordinate space to
  //! normalized device coordinates (mapping).
  //! @param thePnt [in] the 3D point in WCS.
  //! @return mapped point in NDC.
  Standard_EXPORT gp_Pnt Project (const gp_Pnt& thePnt) const;

  //! Unproject point from normalized device coordinates
  //! to world coordinate space.
  //! @param thePnt [in] the NDC point.
  //! @return 3D point in WCS.
  Standard_EXPORT gp_Pnt UnProject (const gp_Pnt& thePnt) const;

  //! Convert point from view coordinate space to
  //! projection coordinate space.
  //! @param thePnt [in] the point in VCS.
  //! @return point in NDC.
  Standard_EXPORT gp_Pnt ConvertView2Proj (const gp_Pnt& thePnt) const;

  //! Convert point from projection coordinate space
  //! to view coordinate space.
  //! @param thePnt [in] the point in NDC.
  //! @return point in VCS.
  Standard_EXPORT gp_Pnt ConvertProj2View (const gp_Pnt& thePnt) const;

  //! Convert point from world coordinate space to
  //! view coordinate space.
  //! @param thePnt [in] the 3D point in WCS.
  //! @return point in VCS.
  Standard_EXPORT gp_Pnt ConvertWorld2View (const gp_Pnt& thePnt) const;

  //! Convert point from view coordinate space to
  //! world coordinates.
  //! @param thePnt [in] the 3D point in VCS.
  //! @return point in WCS.
  Standard_EXPORT gp_Pnt ConvertView2World (const gp_Pnt& thePnt) const;

//! @name Camera modification state
public:

  //! @return projection modification state of the camera.
  const Graphic3d_WorldViewProjState& WorldViewProjState() const
  {
    return myWorldViewProjState;
  }


  //! Returns modification state of camera projection matrix
  Standard_Size ProjectionState() const
  {
    return myWorldViewProjState.ProjectionState();
  }

  //! Returns modification state of camera world view transformation matrix.
  Standard_Size WorldViewState() const
  {
    return myWorldViewProjState.WorldViewState();
  }

//! @name Lazily-computed orientation and projection matrices derived from camera parameters
public:

  //! Get orientation matrix.
  //! @return camera orientation matrix.
  Standard_EXPORT const Graphic3d_Mat4d& OrientationMatrix() const;

  //! Get orientation matrix of Standard_ShortReal precision.
  //! @return camera orientation matrix.
  Standard_EXPORT const Graphic3d_Mat4& OrientationMatrixF() const;

  //! Get monographic or middle point projection matrix used for monographic
  //! rendering and for point projection / unprojection.
  //! @return monographic projection matrix.
  Standard_EXPORT const Graphic3d_Mat4d& ProjectionMatrix() const;

  //! Get monographic or middle point projection matrix of Standard_ShortReal precision used for monographic
  //! rendering and for point projection / unprojection.
  //! @return monographic projection matrix.
  Standard_EXPORT const Graphic3d_Mat4& ProjectionMatrixF() const;

  //! @return stereographic matrix computed for left eye. Please note
  //! that this method is used for rendering for <i>Projection_Stereo</i>.
  Standard_EXPORT const Graphic3d_Mat4d& ProjectionStereoLeft() const;

  //! @return stereographic matrix of Standard_ShortReal precision computed for left eye.
  //! Please note that this method is used for rendering for <i>Projection_Stereo</i>.
  Standard_EXPORT const Graphic3d_Mat4& ProjectionStereoLeftF() const;

  //! @return stereographic matrix computed for right eye. Please note
  //! that this method is used for rendering for <i>Projection_Stereo</i>.
  Standard_EXPORT const Graphic3d_Mat4d& ProjectionStereoRight() const;

  //! @return stereographic matrix of Standard_ShortReal precision computed for right eye.
  //! Please note that this method is used for rendering for <i>Projection_Stereo</i>.
  Standard_EXPORT const Graphic3d_Mat4& ProjectionStereoRightF() const;

  //! Invalidate state of projection matrix.
  //! The matrix will be updated on request.
  Standard_EXPORT void InvalidateProjection();

  //! Invalidate orientation matrix.
  //! The matrix will be updated on request.
  Standard_EXPORT void InvalidateOrientation();

//! @name Managing projection and orientation cache
private:

  //! Compute projection matrices.
  //! @param theMatrices [in] the matrices data container.
  template <typename Elem_t>
  Standard_EXPORT
    TransformMatrices<Elem_t>& UpdateProjection (TransformMatrices<Elem_t>& theMatrices) const;

  //! Compute orientation matrix.
  //! @param theMatrices [in] the matrices data container.
  template <typename Elem_t>
  Standard_EXPORT
    TransformMatrices<Elem_t>& UpdateOrientation (TransformMatrices<Elem_t>& theMatrices) const;

private:

  //! Compose orthographic projection matrix for
  //! the passed camera volume mapping.
  //! @param theLeft [in] the left mapping (clipping) coordinate.
  //! @param theRight [in] the right mapping (clipping) coordinate.
  //! @param theBottom [in] the bottom mapping (clipping) coordinate.
  //! @param theTop [in] the top mapping (clipping) coordinate.
  //! @param theNear [in] the near mapping (clipping) coordinate.
  //! @param theFar [in] the far mapping (clipping) coordinate.
  //! @param theOutMx [out] the projection matrix.
  template <typename Elem_t>
  static void 
    OrthoProj (const Elem_t              theLeft,
               const Elem_t              theRight,
               const Elem_t              theBottom,
               const Elem_t              theTop,
               const Elem_t              theNear,
               const Elem_t              theFar,
               NCollection_Mat4<Elem_t>& theOutMx);

  //! Compose perspective projection matrix for
  //! the passed camera volume mapping.
  //! @param theLeft [in] the left mapping (clipping) coordinate.
  //! @param theRight [in] the right mapping (clipping) coordinate.
  //! @param theBottom [in] the bottom mapping (clipping) coordinate.
  //! @param theTop [in] the top mapping (clipping) coordinate.
  //! @param theNear [in] the near mapping (clipping) coordinate.
  //! @param theFar [in] the far mapping (clipping) coordinate.
  //! @param theOutMx [out] the projection matrix.
  template <typename Elem_t>
  static void
    PerspectiveProj (const Elem_t              theLeft,
                     const Elem_t              theRight,
                     const Elem_t              theBottom,
                     const Elem_t              theTop,
                     const Elem_t              theNear,
                     const Elem_t              theFar,
                     NCollection_Mat4<Elem_t>& theOutMx);

  //! Compose projection matrix for L/R stereo eyes.
  //! @param theLeft [in] the left mapping (clipping) coordinate.
  //! @param theRight [in] the right mapping (clipping) coordinate.
  //! @param theBottom [in] the bottom mapping (clipping) coordinate.
  //! @param theTop [in] the top mapping (clipping) coordinate.
  //! @param theNear [in] the near mapping (clipping) coordinate.
  //! @param theFar [in] the far mapping (clipping) coordinate.
  //! @param theIOD [in] the Intraocular distance.
  //! @param theZFocus [in] the z coordinate of off-axis
  //! projection plane with zero parallax.
  //! @param theIsLeft [in] boolean flag to choose between L/R eyes.
  //! @param theOutMx [out] the projection matrix.
  template <typename Elem_t>
  static void
    StereoEyeProj (const Elem_t              theLeft,
                   const Elem_t              theRight,
                   const Elem_t              theBottom,
                   const Elem_t              theTop,
                   const Elem_t              theNear,
                   const Elem_t              theFar,
                   const Elem_t              theIOD,
                   const Elem_t              theZFocus,
                   const Standard_Boolean    theIsLeft,
                   NCollection_Mat4<Elem_t>& theOutMx);

  //! Construct "look at" orientation transformation.
  //! Reference point differs for perspective and ortho modes 
  //! (made for compatibility, to be improved..).
  //! @param theEye [in] the eye coordinates in 3D space.
  //! @param theLookAt [in] the point the camera looks at.
  //! @param theUpDir [in] the up direction vector.
  //! @param theAxialScale [in] the axial scale vector.
  //! @param theOutMx [in/out] the orientation matrix.
  template <typename Elem_t>
  static void
    LookOrientation (const NCollection_Vec3<Elem_t>& theEye,
                     const NCollection_Vec3<Elem_t>& theLookAt,
                     const NCollection_Vec3<Elem_t>& theUpDir,
                     const NCollection_Vec3<Elem_t>& theAxialScale,
                     NCollection_Mat4<Elem_t>&       theOutMx);

private:

  gp_Dir myUp;     //!< Camera up direction vector.
  gp_Pnt myEye;    //!< Camera eye position.
  gp_Pnt myCenter; //!< Camera center.

  gp_XYZ myAxialScale; //!< World axial scale.

  Projection    myProjType; //!< Projection type used for rendering.
  Standard_Real myFOVy;     //!< Field Of View in y axis.
  Standard_Real myFOVyTan;  //!< Field Of View as Tan(DTR_HALF * myFOVy)
  Standard_Real myZNear;    //!< Distance to near clipping plane.
  Standard_Real myZFar;     //!< Distance to far clipping plane.
  Standard_Real myAspect;   //!< Width to height display ratio.

  Standard_Real myScale;      //!< Specifies parallel scale for orthographic projection.
  Standard_Real myZFocus;     //!< Stereographic focus value.
  FocusType     myZFocusType; //!< Stereographic focus definition type.

  Standard_Real myIOD;     //!< Intraocular distance value.
  IODType       myIODType; //!< Intraocular distance definition type.

  Graphic3d_CameraTile myTile;//!< Tile defining sub-area for drawing

  mutable TransformMatrices<Standard_Real>      myMatricesD;
  mutable TransformMatrices<Standard_ShortReal> myMatricesF;

  mutable Graphic3d_WorldViewProjState myWorldViewProjState;

public:

  DEFINE_STANDARD_RTTIEXT(Graphic3d_Camera,Standard_Transient)
};

DEFINE_STANDARD_HANDLE (Graphic3d_Camera, Standard_Transient)

//! Linear interpolation tool for camera orientation and position.
//! This tool interpolates camera parameters scale, eye, center, rotation (up and direction vectors) independently.
//!
//! Eye/Center interpolation is performed through defining an anchor point in-between Center and Eye.
//! The anchor position is defined as point near to the camera point which has smaller translation part.
//! The main idea is to keep the distance between Center and Eye
//! (which will change if Center and Eye translation will be interpolated independently).
//! E.g.:
//!  - When both Center and Eye are moved at the same vector -> both will be just translated by straight line
//!  - When Center is not moved -> camera Eye    will move around Center through arc
//!  - When Eye    is not moved -> camera Center will move around Eye    through arc
//!  - When both Center and Eye are move by different vectors -> transformation will be something in between,
//!    and will try interpolate linearly the distance between Center and Eye.
//!
//! This transformation might be not in line with user expectations.
//! In this case, application might define intermediate camera positions for interpolation
//! or implement own interpolation logic.
template<>
Standard_EXPORT void NCollection_Lerp<Handle(Graphic3d_Camera)>::Interpolate (const double theT,
                                                                              Handle(Graphic3d_Camera)& theResult) const;
typedef NCollection_Lerp<Handle(Graphic3d_Camera)> Graphic3d_CameraLerp;

#endif
