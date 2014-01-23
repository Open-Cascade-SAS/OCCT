// Created on: 2013-05-29
// Created by: Anton POLETAEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _Graphic3d_Camera_HeaderFile
#define _Graphic3d_Camera_HeaderFile

#include <Graphic3d_Mat4.hxx>
#include <Graphic3d_Vec3.hxx>

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <Standard_Macro.hxx>
#include <Standard_TypeDef.hxx>

DEFINE_STANDARD_HANDLE (Graphic3d_Camera, Standard_Transient)

//! Camera class provides object-oriented approach to setting up projection
//! and orientation properties of 3D view.
class Graphic3d_Camera : public Standard_Transient
{

public:

  //! Enumerates supported monographic projections.
  //! - Projection_Orthographic : orthographic projection.
  //! - Projection_Perspective  : perspective projection.
  //! - Projection_Stere        : stereographic projection.
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
  //! ZNear (0.1); ZFar (100); Aspect(1);
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

  //! Returns modification state of camera projection matrix
  Standard_Size ProjectionState() const
  {
    return myProjectionState;
  }

  //! Returns modification state of camera model-view matrix
  Standard_Size ModelViewState() const
  {
    return myOrientationState;
  }

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

  //! Sets camera Up direction vector.
  //! @param theUp [in] the Up direction vector.
  Standard_EXPORT void SetUp (const gp_Dir& theUp);

  //! Get camera Up direction vector.
  //! @return Camera's Up direction vector.
  const gp_Dir& Up() const
  {
    return myUp;
  }

  //! Set camera projection shift vector.<br>
  //! Used for compatibility with older view mechanics. Applied after
  //! view transform and before projection step (P * Shift * V).
  //! @param theProjShift [in] the projection shift vector.
  Standard_EXPORT void SetProjectionShift (const gp_Pnt& theProjShift);

  //! Get camera projection shift vector.
  //! @return Camera's projection shift vector.
  const gp_Pnt& ProjectionShift() const
  {
    return myProjectionShift;
  }

  //! Set camera axial scale.<br>
  //! @param theAxialScale [in] the axial scale vector.
  Standard_EXPORT void SetAxialScale (const gp_Pnt& theAxialScale);

  //! Get camera axial scale.
  //! @return Camera's axial scale.
  const gp_Pnt& AxialScale() const
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
  //! distance.
  //! @param theScale [in] the scale factor.
  Standard_EXPORT void SetScale (const Standard_Real theScale);

  //! Get camera scale.
  //! @return camera scale factor.
  Standard_EXPORT Standard_Real Scale() const;

  //! Change camera projection type.
  //! While switching between perspective and ortho projection types
  //! ZNear and ZFar value conversion is performed due to different 
  //! coordinate systems (made for compatibility, to be improved..)
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

  //! Change the Near Z-clipping plane position.
  //! @param theZNear [in] the distance of the plane from the Eye.
  Standard_EXPORT void SetZNear (const Standard_Real theZNear);

  //! Get the Near Z-clipping plane position.
  //! @return the distance of the plane from the Eye.
  Standard_Real ZNear() const
  {
    return myZNear;
  }

  //! Change the Far Z-clipping plane position.
  //! @param theZFar [in] the distance of the plane from the Eye.
  Standard_EXPORT void SetZFar (const Standard_Real theZFar);

  //! Get the Far Z-clipping plane position.
  //! @return the distance of the plane from the Eye.
  Standard_Real ZFar() const
  {
    return myZFar;
  }

  //! Change display ratio.
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

  //! Get orientation matrix.
  //! @return camera orientation matrix.
  const Graphic3d_Mat4& OrientationMatrix() const
  {
    return myOrientation;
  }

  //! Get monographic or middle point projection matrix used for monographic
  //! rendering and for point projection / unprojection.
  //! @return monographic projection matrix.
  const Graphic3d_Mat4& ProjectionMatrix() const
  {
    return myMProjection;
  }

  //! @return stereographic matrix computed for left eye. Please note
  //! that this method is used for rendering for <i>Projection_Stereo</i>.
  const Graphic3d_Mat4& ProjectionStereoLeft() const
  {
    return myLProjection;
  }

  //! @return stereographic matrix computed for right eye. Please note
  //! that this method is used for rendering for <i>Projection_Stereo</i>.
  const Graphic3d_Mat4& ProjectionStereoRight() const
  {
    return myRProjection;
  }

public:

  //! Orthogonalize up direction vector.
  Standard_EXPORT void OrthogonalizeUp();

  //! Suspend internal data recalculation when changing set of camera
  //! properties. This method is optional and can be used for pieces
  //! of code which are critical to performance. Note that the method
  //! supports stacked calls (carried out by internal counter).
  Standard_EXPORT void BeginUpdate();

  //! Unset lock set by <i>BeginUpdate</i> and invoke data recalculation when
  //! there are no more locks left. This method is optional and can be used
  //! for pieces of code which are critical to performance.
  Standard_EXPORT void EndUpdate();

  // Basic camera operations
public:

  //! Transform orientation components of the camera:
  //! Eye, Up and Center points.
  //! @param theTrsf [in] the transformation to apply.
  Standard_EXPORT void Transform (const gp_Trsf& theTrsf);

  //! Calculate view plane size at center (target) point
  //! and distance between ZFar and ZNear planes.
  //! @return values in form of gp_Pnt (Width, Height, Depth).
  Standard_EXPORT gp_Pnt ViewDimensions () const;

  //! Calculate view plane dimensions with projection shift applied.
  //! Analog to old ViewMapping.WindowLimit() function.
  //! @param theUMin [out] the u component of min corner of the rect.
  //! @param theVMin [out] the v component of min corner of the rect.
  //! @param theUMax [out] the u component of max corner of the rect.
  //! @param theVMax [out] the v component of max corner of the rect.
  Standard_EXPORT void WindowLimit (Standard_Real& theUMin,
                                    Standard_Real& theVMin,
                                    Standard_Real& theUMax,
                                    Standard_Real& theVMax) const;

  // Projection methods
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

  // managing projection and orientation cache:
public:

  //! Compute and cache projection matrices.
  void UpdateProjection();

  //! Compute and cache orientation matrix.
  void UpdateOrientation();

private:

  //! Compose orthographic projection matrix for
  //! the passed camera volume mapping.
  //! @param theLeft [in] the left mapping (clipping) coordinate.
  //! @param theRight [in] the right mapping (clipping) coordinate.
  //! @param theBottom [in] the bottom mapping (clipping) coordinate.
  //! @param theTop [in] the top mapping (clipping) coordinate.
  //! @param theNear [in] the near mapping (clipping) coordinate.
  //! @param theFar [in] the far mapping (clipping) coordinate.
  //! @param theShiftX [in] the shift x coordinate.
  //! @param theShiftY [in] the shift y coordinate.
  //! @param theOutMx [out] the projection matrix.
  static void 
    OrthoProj (const Standard_ShortReal theLeft,
               const Standard_ShortReal theRight,
               const Standard_ShortReal theBottom,
               const Standard_ShortReal theTop,
               const Standard_ShortReal theNear,
               const Standard_ShortReal theFar,
               const Standard_ShortReal theShiftX,
               const Standard_ShortReal theShiftY,
               Graphic3d_Mat4&          theOutMx);

  //! Compose perspective projection matrix for
  //! the passed camera volume mapping.
  //! @param theLeft [in] the left mapping (clipping) coordinate.
  //! @param theRight [in] the right mapping (clipping) coordinate.
  //! @param theBottom [in] the bottom mapping (clipping) coordinate.
  //! @param theTop [in] the top mapping (clipping) coordinate.
  //! @param theNear [in] the near mapping (clipping) coordinate.
  //! @param theFar [in] the far mapping (clipping) coordinate.
  //! @param theShiftX [in] the shift x coordinate.
  //! @param theShiftY [in] the shift y coordinate.
  //! @param theOutMx [out] the projection matrix.
  static void
    PerspectiveProj (const Standard_ShortReal theLeft,
                     const Standard_ShortReal theRight,
                     const Standard_ShortReal theBottom,
                     const Standard_ShortReal theTop,
                     const Standard_ShortReal theNear,
                     const Standard_ShortReal theFar,
                     const Standard_ShortReal theShiftX,
                     const Standard_ShortReal theShiftY,
                     Graphic3d_Mat4&          theOutMx);

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
  //! @param theShiftX [in] the shift x coordinate.
  //! @param theShiftY [in] the shift y coordinate.
  //! @param theIsLeft [in] boolean flag to choose between L/R eyes.
  //! @param theOutMx [out] the projection matrix.
  static void
    StereoEyeProj (const Standard_ShortReal theLeft,
                   const Standard_ShortReal theRight,
                   const Standard_ShortReal theBottom,
                   const Standard_ShortReal theTop,
                   const Standard_ShortReal theNear,
                   const Standard_ShortReal theFar,
                   const Standard_ShortReal theIOD,
                   const Standard_ShortReal theZFocus,
                   const Standard_ShortReal theShiftX,
                   const Standard_ShortReal theShiftY,
                   const Standard_Boolean   theIsLeft,
                   Graphic3d_Mat4&          theOutMx);

  //! Construct "look at" orientation transformation.
  //! Reference point differs for perspective and ortho modes 
  //! (made for compatibility, to be improved..).
  //! @param theEye [in] the eye coordinates in 3D space.
  //! @param theLookAt [in] the point the camera looks at.
  //! @param theUpDir [in] the up direction vector.
  //! @param theAxialScale [in] the axial scale vector.
  //! @param theOutMx [in/out] the orientation matrix.
  static void
    LookOrientation (const Graphic3d_Vec3& theEye,
                     const Graphic3d_Vec3& theLookAt,
                     Graphic3d_Vec3& theUpDir,
                     const Graphic3d_Vec3& theAxialScale,
                     Graphic3d_Mat4& theOutMx);

private:

  gp_Dir myUp;     //!< Camera up direction vector.
  gp_Pnt myEye;    //!< Camera eye position.
  gp_Pnt myCenter; //!< Camera center.

  gp_Pnt myProjectionShift; //!< Camera projection shift for compatibility.
  gp_Pnt myAxialScale;      //!< Camera axial scale.

  Projection myProjType;  //!< Projection type used for rendering.
  Standard_Real myFOVy;   //!< Field Of View in y axis.
  Standard_Real myZNear;  //!< Distance to near clipping plane.
  Standard_Real myZFar;   //!< Distance to far clipping plane.
  Standard_Real myAspect; //!< Width to height display ratio.

  Standard_Real myScale;      //!< Specifies parallel scale for orthographic projection.
  Standard_Real myZFocus;     //!< Stereographic focus value.
  FocusType     myZFocusType; //!< Stereographic focus definition type.

  Standard_Real myIOD;     //!< Intraocular distance value.
  IODType       myIODType; //!< Intraocular distance definition type.

  //! Number of locks set up on internal data recalculation by
  //! <i>(BeginUpdate, EndUpdate)</i> pairs. The counter provides effective
  //! use of the mentioned methods when camera properties are modified
  //! in stacked functions.
  Standard_Integer myNbUpdateLocks;

  Graphic3d_Mat4 myOrientation; //!< Camera orientation matrix.
  Graphic3d_Mat4 myMProjection; //!< Monographic projection matrix.
  Graphic3d_Mat4 myLProjection; //!< Projection matrix for left eye.
  Graphic3d_Mat4 myRProjection; //!< Projection matrix for right eye.

  Standard_Size myProjectionState;
  Standard_Size myOrientationState;

public:

  DEFINE_STANDARD_RTTI(Graphic3d_Camera);

};

#endif
