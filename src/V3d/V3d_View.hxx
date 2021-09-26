// Created on: 1992-01-15
// Created by: GG
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _V3d_View_HeaderFile
#define _V3d_View_HeaderFile

#include <Aspect_Background.hxx>
#include <Aspect_FillMethod.hxx>
#include <Aspect_GradientBackground.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_RenderingContext.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>
#include <Bnd_Box.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_BufferType.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_GraduatedTrihedron.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_TypeOfShadingModel.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Image_PixMap.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <V3d_ImageDumpOptions.hxx>
#include <V3d_ListOfLight.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_Trihedron.hxx>
#include <V3d_TypeOfAxe.hxx>
#include <V3d_TypeOfBackfacingModel.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <V3d_TypeOfView.hxx>
#include <V3d_TypeOfVisualization.hxx>

class Aspect_Grid;
class Aspect_Window;
class Graphic3d_Group;
class Graphic3d_Structure;
class Graphic3d_TextureEnv;

DEFINE_STANDARD_HANDLE(V3d_View, Standard_Transient)

//! Defines the application object VIEW for the
//! VIEWER application.
//! The methods of this class allow the editing
//! and inquiring the parameters linked to the view.
//! Provides a set of services common to all types of view.
//! Warning: The default parameters are defined by the class
//! Viewer (Example : SetDefaultViewSize()).
//! Certain methods are mouse oriented, and it is
//! necessary to know the difference between the start and
//! the continuation of this gesture in putting the method
//! into operation.
//! Example : Shifting the eye-view along the screen axes.
//!
//! View->Move(10.,20.,0.,True)     (Starting motion)
//! View->Move(15.,-5.,0.,False)    (Next motion)
class V3d_View : public Standard_Transient
{

public:

  //! Initializes the view.
  Standard_EXPORT V3d_View (const Handle(V3d_Viewer)& theViewer, const V3d_TypeOfView theType = V3d_ORTHOGRAPHIC);

  //! Initializes the view by copying.
  Standard_EXPORT V3d_View (const Handle(V3d_Viewer)& theViewer, const Handle(V3d_View)& theView);

  //! Default destructor.
  Standard_EXPORT virtual ~V3d_View();

  //! Activates the view in the specified Window
  //! If <aContext> is not NULL the graphic context is used
  //! to draw something in this view.
  //! Otherwise an internal graphic context is created.
  //! Warning: The view is centered and resized to preserve
  //! the height/width ratio of the window.
  Standard_EXPORT void SetWindow (const Handle(Aspect_Window)& theWindow,
                                  const Aspect_RenderingContext theContext = NULL);

  Standard_EXPORT void SetMagnify (const Handle(Aspect_Window)& theWindow,
                                   const Handle(V3d_View)& thePreviousView,
                                   const Standard_Integer theX1,
                                   const Standard_Integer theY1,
                                   const Standard_Integer theX2,
                                   const Standard_Integer theY2);

  //! Destroys the view.
  Standard_EXPORT void Remove();

  //! Deprecated, Redraw() should be used instead.
  Standard_EXPORT void Update() const;

  //! Redisplays the view even if there has not
  //! been any modification.
  //! Must be called if the view is shown.
  //! (Ex: DeIconification ) .
  Standard_EXPORT virtual void Redraw() const;

  //! Updates layer of immediate presentations.
  Standard_EXPORT virtual void RedrawImmediate() const;

  //! Invalidates view content but does not redraw it.
  Standard_EXPORT void Invalidate() const;

  //! Returns true if cached view content has been invalidated.
  Standard_EXPORT Standard_Boolean IsInvalidated() const;

  //! Returns true if immediate layer content has been invalidated.
  Standard_Boolean IsInvalidatedImmediate() const { return myIsInvalidatedImmediate; }

  //! Invalidates view content within immediate layer but does not redraw it.
  void InvalidateImmediate() { myIsInvalidatedImmediate = Standard_True; }

  //! Must be called when the window supporting the
  //! view changes size.
  //! if the view is not mapped on a window.
  //! Warning: The view is centered and resized to preserve
  //! the height/width ratio of the window.
  Standard_EXPORT void MustBeResized();

  //! Must be called when the window supporting the
  //! view is mapped or unmapped.
  Standard_EXPORT void DoMapping();

  //! Returns the status of the view regarding
  //! the displayed structures inside
  //! Returns True is The View is empty
  Standard_EXPORT Standard_Boolean IsEmpty() const;

  //! Updates the lights of the view.
  Standard_EXPORT void UpdateLights() const;

  //! Sets the automatic z-fit mode and its parameters.
  //! The auto z-fit has extra parameters which can controlled from application level
  //! to ensure that the size of viewing volume will be sufficiently large to cover
  //! the depth of unmanaged objects, for example, transformation persistent ones.
  //! @param theScaleFactor [in] the scale factor for Z-range.
  //! The range between Z-min, Z-max projection volume planes
  //! evaluated by z fitting method will be scaled using this coefficient.
  //! Program error exception is thrown if negative or zero value
  //! is passed.
  Standard_EXPORT void SetAutoZFitMode (const Standard_Boolean theIsOn, const Standard_Real theScaleFactor = 1.0);

  //! returns TRUE if automatic z-fit mode is turned on.
  Standard_Boolean AutoZFitMode() const { return myAutoZFitIsOn; }

  //! returns scale factor parameter of automatic z-fit mode.
  Standard_Real AutoZFitScaleFactor() const { return myAutoZFitScaleFactor; }

  //! If automatic z-range fitting is turned on, adjusts Z-min and Z-max
  //! projection volume planes with call to ZFitAll.
  Standard_EXPORT void AutoZFit() const;

  //! Change Z-min and Z-max planes of projection volume to match the
  //! displayed objects.
  Standard_EXPORT void ZFitAll (const Standard_Real theScaleFactor = 1.0) const;

public:

  //! Defines the background color of the view by the color definition type and the three corresponding values.
  Standard_EXPORT void SetBackgroundColor (const Quantity_TypeOfColor theType,
                                           const Standard_Real theV1,
                                           const Standard_Real theV2,
                                           const Standard_Real theV3);

  //! Defines the background color of the view.
  Standard_EXPORT void SetBackgroundColor (const Quantity_Color& theColor);

  //! Defines the gradient background colors of the view by supplying the colors
  //! and the fill method (horizontal by default).
  Standard_EXPORT void SetBgGradientColors (const Quantity_Color& theColor1,
                                            const Quantity_Color& theColor2,
                                            const Aspect_GradientFillMethod theFillStyle = Aspect_GradientFillMethod_Horizontal,
                                            const Standard_Boolean theToUpdate = Standard_False);

  //! Defines the gradient background fill method of the view.
  Standard_EXPORT void SetBgGradientStyle (const Aspect_GradientFillMethod theMethod = Aspect_GradientFillMethod_Horizontal,
                                           const Standard_Boolean theToUpdate = Standard_False);

  //! Defines the background texture of the view by supplying the texture image file name
  //! and fill method (centered by default).
  Standard_EXPORT void SetBackgroundImage (const Standard_CString theFileName,
                                           const Aspect_FillMethod theFillStyle = Aspect_FM_CENTERED,
                                           const Standard_Boolean theToUpdate = Standard_False);

  //! Defines the background texture of the view by supplying the texture and fill method (centered by default)
  Standard_EXPORT void SetBackgroundImage (const Handle(Graphic3d_Texture2D)& theTexture,
                                           const Aspect_FillMethod theFillStyle = Aspect_FM_CENTERED,
                                           const Standard_Boolean theToUpdate = Standard_False);

  //! Defines the textured background fill method of the view.
  Standard_EXPORT void SetBgImageStyle (const Aspect_FillMethod theFillStyle,
                                        const Standard_Boolean theToUpdate = Standard_False);

  //! Sets environment cubemap as background.
  //! @param theCubeMap cubemap source to be set as background
  //! @param theToUpdatePBREnv defines whether IBL maps will be generated or not (see 'GeneratePBREnvironment')
  Standard_EXPORT void SetBackgroundCubeMap (const Handle(Graphic3d_CubeMap)& theCubeMap,
                                             Standard_Boolean                 theToUpdatePBREnv = Standard_True,
                                             Standard_Boolean                 theToUpdate = Standard_False);

  //! Returns TRUE if IBL (Image Based Lighting) from background cubemap is enabled.
  Standard_EXPORT Standard_Boolean IsImageBasedLighting() const;

  //! Enables or disables IBL (Image Based Lighting) from background cubemap.
  //! Has no effect if PBR is not used.
  //! @param[in] theToEnableIBL enable or disable IBL from background cubemap
  //! @param[in] theToUpdate redraw the view
  Standard_EXPORT void SetImageBasedLighting (Standard_Boolean theToEnableIBL,
                                              Standard_Boolean theToUpdate = Standard_False);

  //! Activates IBL from background cubemap.
  void GeneratePBREnvironment (Standard_Boolean theToUpdate = Standard_False) { SetImageBasedLighting (Standard_True, theToUpdate); }

  //! Disables IBL from background cubemap; fills PBR specular probe and irradiance map with white color.
  void ClearPBREnvironment (Standard_Boolean theToUpdate = Standard_False) { SetImageBasedLighting (Standard_True, theToUpdate); }

  //! Sets the environment texture to use. No environment texture by default.
  Standard_EXPORT void SetTextureEnv (const Handle(Graphic3d_TextureEnv)& theTexture);

  //! Definition of an axis from its origin and
  //! its orientation .
  //! This will be the current axis for rotations and movements.
  //! Warning! raises BadValue from V3d if the vector normal is NULL. .
  Standard_EXPORT void SetAxis (const Standard_Real X, const Standard_Real Y, const Standard_Real Z,
                                const Standard_Real Vx, const Standard_Real Vy, const Standard_Real Vz);

public:

  //! Defines the visualization type in the view.
  Standard_EXPORT void SetVisualization (const V3d_TypeOfVisualization theType);

  //! Activates theLight in the view.
  Standard_EXPORT void SetLightOn (const Handle(V3d_Light)& theLight);

  //! Activates all the lights defined in this view.
  Standard_EXPORT void SetLightOn();

  //! Deactivate theLight in this view.
  Standard_EXPORT void SetLightOff (const Handle(V3d_Light)& theLight);

  //! Deactivate all the Lights defined in this view.
  Standard_EXPORT void SetLightOff();

  //! Returns TRUE when the light is active in this view.
  Standard_EXPORT Standard_Boolean IsActiveLight (const Handle(V3d_Light)& theLight) const;

  //! sets the immediate update mode and returns the previous one.
  Standard_EXPORT Standard_Boolean SetImmediateUpdate (const Standard_Boolean theImmediateUpdate);

  //! Returns trihedron object.
  const Handle(V3d_Trihedron)& Trihedron (bool theToCreate = true)
  {
    if (myTrihedron.IsNull() && theToCreate)
    {
      myTrihedron = new V3d_Trihedron();
    }
    return myTrihedron;
  }

  //! Customization of the ZBUFFER Triedron.
  //! XColor,YColor,ZColor - colors of axis
  //! SizeRatio - ratio of decreasing of the trihedron size when its physical
  //! position comes out of the view
  //! AxisDiametr - diameter relatively to axis length
  //! NbFacettes - number of facets of cylinders and cones
  Standard_EXPORT void ZBufferTriedronSetup (const Quantity_Color& theXColor = Quantity_NOC_RED,
                                             const Quantity_Color& theYColor = Quantity_NOC_GREEN,
                                             const Quantity_Color& theZColor = Quantity_NOC_BLUE1,
                                             const Standard_Real theSizeRatio = 0.8,
                                             const Standard_Real theAxisDiametr = 0.05,
                                             const Standard_Integer theNbFacettes = 12);

  //! Display of the Triedron.
  //! Initialize position, color and length of Triedron axes.
  //! The scale is a percent of the window width.
  Standard_EXPORT void TriedronDisplay (const Aspect_TypeOfTriedronPosition thePosition = Aspect_TOTP_CENTER,
                                        const Quantity_Color& theColor = Quantity_NOC_WHITE,
                                        const Standard_Real theScale = 0.02,
                                        const V3d_TypeOfVisualization theMode = V3d_WIREFRAME);

  //! Erases the Triedron.
  Standard_EXPORT void TriedronErase();

  //! Returns data of a graduated trihedron.
  Standard_EXPORT const Graphic3d_GraduatedTrihedron& GetGraduatedTrihedron() const;

  //! Displays a graduated trihedron.
  Standard_EXPORT void GraduatedTrihedronDisplay (const Graphic3d_GraduatedTrihedron& theTrihedronData);

  //! Erases a graduated trihedron from the view.
  Standard_EXPORT void GraduatedTrihedronErase();

  //! modify the Projection of the view perpendicularly to
  //! the privileged plane of the viewer.
  Standard_EXPORT void SetFront();

  //! Rotates the eye about the coordinate system of
  //! reference of the screen
  //! for which the origin is the view point of the projection,
  //! with a relative angular value in RADIANS with respect to
  //! the initial position expressed by Start = Standard_True
  //! Warning! raises BadValue from V3d
  //! If the eye, the view point, or the high point are
  //! aligned or confused.
  Standard_EXPORT void Rotate (const Standard_Real Ax, const Standard_Real Ay, const Standard_Real Az, const Standard_Boolean Start = Standard_True);

  //! Rotates the eye about the coordinate system of
  //! reference of the screen
  //! for which the origin is Gravity point {X,Y,Z},
  //! with a relative angular value in RADIANS with respect to
  //! the initial position expressed by Start = Standard_True
  //! If the eye, the view point, or the high point are
  //! aligned or confused.
  Standard_EXPORT void Rotate (const Standard_Real Ax, const Standard_Real Ay, const Standard_Real Az,
                               const Standard_Real X,  const Standard_Real Y,  const Standard_Real Z,
                               const Standard_Boolean Start = Standard_True);

  //! Rotates the eye about one of the coordinate axes of
  //! of the view for which the origin is the Gravity point{X,Y,Z}
  //! with an relative angular value in RADIANS with
  //! respect to the initial position expressed by
  //! Start = Standard_True
  Standard_EXPORT void Rotate (const V3d_TypeOfAxe Axe,
                               const Standard_Real Angle,
                               const Standard_Real X, const Standard_Real Y, const Standard_Real Z,
                               const Standard_Boolean Start = Standard_True);

  //! Rotates the eye about one of the coordinate axes of
  //! of the view for which the origin is the view point of the
  //! projection with an relative angular value in RADIANS with
  //! respect to the initial position expressed by
  //! Start = Standard_True
  Standard_EXPORT void Rotate (const V3d_TypeOfAxe Axe, const Standard_Real Angle, const Standard_Boolean Start = Standard_True);

  //! Rotates the eye around the current axis a relative
  //! angular value in RADIANS with respect to the initial
  //! position expressed by Start = Standard_True
  Standard_EXPORT void Rotate (const Standard_Real Angle, const Standard_Boolean Start = Standard_True);

  //! Movement of the eye parallel to the coordinate system
  //! of reference of the screen a distance relative to the
  //! initial position expressed by Start = Standard_True.
  Standard_EXPORT void Move (const Standard_Real Dx, const Standard_Real Dy, const Standard_Real Dz, const Standard_Boolean Start = Standard_True);

  //! Movement of the eye parallel to one of the axes of the
  //! coordinate system of reference of the view a distance
  //! relative to the initial position expressed by
  //! Start = Standard_True.
  Standard_EXPORT void Move (const V3d_TypeOfAxe Axe, const Standard_Real Length, const Standard_Boolean Start = Standard_True);

  //! Movement of the eye parllel to the current axis
  //! a distance relative to the initial position
  //! expressed by Start = Standard_True
  Standard_EXPORT void Move (const Standard_Real Length, const Standard_Boolean Start = Standard_True);

  //! Movement of the ye and the view point parallel to the
  //! frame of reference of the screen a distance relative
  //! to the initial position expressed by
  //! Start = Standard_True
  Standard_EXPORT void Translate (const Standard_Real Dx, const Standard_Real Dy, const Standard_Real Dz, const Standard_Boolean Start = Standard_True);

  //! Movement of the eye and the view point parallel to one
  //! of the axes of the fame of reference of the view a
  //! distance relative to the initial position
  //! expressed by Start = Standard_True
  Standard_EXPORT void Translate (const V3d_TypeOfAxe Axe, const Standard_Real Length, const Standard_Boolean Start = Standard_True);

  //! Movement of the eye and view point parallel to
  //! the current axis a distance relative to the initial
  //! position expressed by Start = Standard_True
  Standard_EXPORT void Translate (const Standard_Real Length, const Standard_Boolean Start = Standard_True);

  //! places the point of the view corresponding
  //! at the pixel position x,y at the center of the window
  //! and updates the view.
  Standard_EXPORT void Place (const Standard_Integer theXp, const Standard_Integer theYp, const Standard_Real theZoomFactor = 1);

  //! Rotation of the view point around the frame of reference
  //! of the screen for which the origin is the eye of the
  //! projection with a relative angular value in RADIANS
  //! with respect to the initial position expressed by
  //! Start = Standard_True
  Standard_EXPORT void Turn (const Standard_Real Ax, const Standard_Real Ay, const Standard_Real Az, const Standard_Boolean Start = Standard_True);

  //! Rotation of the view point around one of the axes of the
  //! frame of reference of the view for which the origin is
  //! the eye of the projection with an angular value in
  //! RADIANS relative to the initial position expressed by
  //! Start = Standard_True
  Standard_EXPORT void Turn (const V3d_TypeOfAxe Axe, const Standard_Real Angle, const Standard_Boolean Start = Standard_True);

  //! Rotation of the view point around the current axis an
  //! angular value in RADIANS relative to the initial
  //! position expressed by Start = Standard_True
  Standard_EXPORT void Turn (const Standard_Real Angle, const Standard_Boolean Start = Standard_True);

  //! Defines the angular position of the high point of
  //! the reference frame of the view with respect to the
  //! Y screen axis with an absolute angular value in
  //! RADIANS.
  Standard_EXPORT void SetTwist (const Standard_Real Angle);

  //! Defines the position of the eye..
  Standard_EXPORT void SetEye (const Standard_Real X, const Standard_Real Y, const Standard_Real Z);

  //! Defines the Depth of the eye from the view point
  //! without update the projection .
  Standard_EXPORT void SetDepth (const Standard_Real Depth);

  //! Defines the orientation of the projection.
  Standard_EXPORT void SetProj (const Standard_Real Vx, const Standard_Real Vy, const Standard_Real Vz);

  //! Defines the orientation of the projection .
  //! @param theOrientation camera direction
  //! @param theIsYup       flag indicating Y-up (TRUE) or Z-up (FALSE) convention
  Standard_EXPORT void SetProj (const V3d_TypeOfOrientation theOrientation,
                                const Standard_Boolean theIsYup = Standard_False);

  //! Defines the position of the view point.
  Standard_EXPORT void SetAt (const Standard_Real X, const Standard_Real Y, const Standard_Real Z);

  //! Defines the orientation of the high point.
  Standard_EXPORT void SetUp (const Standard_Real Vx, const Standard_Real Vy, const Standard_Real Vz);

  //! Defines the orientation(SO) of the high point.
  Standard_EXPORT void SetUp (const V3d_TypeOfOrientation Orientation);

  //! Saves the current state of the orientation of the view
  //! which will be the return state at ResetViewOrientation.
  Standard_EXPORT void SetViewOrientationDefault();

  //! Resets the orientation of the view.
  //! Updates the view
  Standard_EXPORT void ResetViewOrientation();

  //! Translates the center of the view along "x" and "y" axes of
  //! view projection. Can be used to perform interactive panning operation.
  //! In that case the DXv, DXy parameters specify panning relative to the
  //! point where the operation is started.
  //! @param theDXv [in] the relative panning on "x" axis of view projection, in view space coordinates.
  //! @param theDYv [in] the relative panning on "y" axis of view projection, in view space coordinates.
  //! @param theZoomFactor [in] the zooming factor.
  //! @param theToStart [in] pass TRUE when starting panning to remember view
  //! state prior to panning for relative arguments. If panning is started,
  //! passing {0, 0} for {theDXv, theDYv} will return view to initial state.
  //! Performs update of view.
  Standard_EXPORT void Panning (const Standard_Real theDXv, const Standard_Real theDYv, const Standard_Real theZoomFactor = 1, const Standard_Boolean theToStart = Standard_True);

  //! Relocates center of screen to the point, determined by
  //! {Xp, Yp} pixel coordinates relative to the bottom-left corner of
  //! screen. To calculate pixel coordinates for any point from world
  //! coordinate space, it can be projected using "Project".
  //! @param theXp [in] the x coordinate.
  //! @param theYp [in] the y coordinate.
  Standard_EXPORT void SetCenter (const Standard_Integer theXp, const Standard_Integer theYp);

  //! Defines the view projection size in its maximum dimension,
  //! keeping the initial height/width ratio unchanged.
  Standard_EXPORT void SetSize (const Standard_Real theSize);

  //! Defines the Depth size of the view
  //! Front Plane will be set to Size/2.
  //! Back  Plane will be set to -Size/2.
  //! Any Object located Above the Front Plane or
  //! behind the Back Plane will be Clipped .
  //! NOTE than the XY Size of the View is NOT modified .
  Standard_EXPORT void SetZSize (const Standard_Real SetZSize);

  //! Zooms the view by a factor relative to the initial
  //! value expressed by Start = Standard_True
  //! Updates the view.
  Standard_EXPORT void SetZoom (const Standard_Real Coef, const Standard_Boolean Start = Standard_True);

  //! Zooms the view by a factor relative to the value
  //! initialised by SetViewMappingDefault().
  //! Updates the view.
  Standard_EXPORT void SetScale (const Standard_Real Coef);

  //! Sets  anisotropic (axial)  scale  factors  <Sx>, <Sy>, <Sz>  for  view <me>.
  //! Anisotropic  scaling  operation  is  performed  through  multiplying
  //! the current view  orientation  matrix  by  a  scaling  matrix:
  //! || Sx  0   0   0 ||
  //! || 0   Sy  0   0 ||
  //! || 0   0   Sz  0 ||
  //! || 0   0   0   1 ||
  //! Updates the view.
  Standard_EXPORT void SetAxialScale (const Standard_Real Sx, const Standard_Real Sy, const Standard_Real Sz);

  //! Adjust view parameters to fit the displayed scene, respecting height / width ratio.
  //! The Z clipping range (depth range) is fitted if AutoZFit flag is TRUE.
  //! Throws program error exception if margin coefficient is < 0 or >= 1.
  //! Updates the view.
  //! @param theMargin [in] the margin coefficient for view borders.
  //! @param theToUpdate [in] flag to perform view update.
  Standard_EXPORT void FitAll (const Standard_Real theMargin = 0.01, const Standard_Boolean theToUpdate = Standard_True);

  //! Adjust view parameters to fit the displayed scene, respecting height / width ratio
  //! according to the custom bounding box given.
  //! Throws program error exception if margin coefficient is < 0 or >= 1.
  //! Updates the view.
  //! @param theBox [in] the custom bounding box to fit.
  //! @param theMargin [in] the margin coefficient for view borders.
  //! @param theToUpdate [in] flag to perform view update.
  Standard_EXPORT void FitAll (const Bnd_Box& theBox, const Standard_Real theMargin = 0.01, const Standard_Boolean theToUpdate = Standard_True);

  //! Adjusts the viewing volume so as not to clip the displayed objects by front and back
  //! and back clipping planes. Also sets depth value automatically depending on the
  //! calculated Z size and Aspect parameter.
  //! NOTE than the original XY size of the view is NOT modified .
  Standard_EXPORT void DepthFitAll (const Standard_Real Aspect = 0.01, const Standard_Real Margin = 0.01);

  //! Centers the defined projection window so that it occupies
  //! the maximum space while respecting the initial
  //! height/width ratio.
  //! NOTE than the original Z size of the view is NOT modified .
  Standard_EXPORT void FitAll (const Standard_Real theMinXv, const Standard_Real theMinYv, const Standard_Real theMaxXv, const Standard_Real theMaxYv);

  //! Centers the defined PIXEL window so that it occupies
  //! the maximum space while respecting the initial height/width ratio.
  //! NOTE than the original Z size of the view is NOT modified.
  //! @param theMinXp [in] pixel coordinates of minimal corner on x screen axis.
  //! @param theMinYp [in] pixel coordinates of minimal corner on y screen axis.
  //! @param theMaxXp [in] pixel coordinates of maximal corner on x screen axis.
  //! @param theMaxYp [in] pixel coordinates of maximal corner on y screen axis.
  Standard_EXPORT void WindowFit (const Standard_Integer theMinXp, const Standard_Integer theMinYp, const Standard_Integer theMaxXp, const Standard_Integer theMaxYp);

  //! Saves the current view mapping. This will be the
  //! state returned from ResetViewmapping.
  Standard_EXPORT void SetViewMappingDefault();

  //! Resets the centering of the view.
  //! Updates the view
  Standard_EXPORT void ResetViewMapping();

  //! Resets the centering and the orientation of the view.
  Standard_EXPORT void Reset (const Standard_Boolean theToUpdate = Standard_True);

  //! Converts the PIXEL value
  //! to a value in the projection plane.
  Standard_EXPORT Standard_Real Convert (const Standard_Integer Vp) const;

  //! Converts the point PIXEL into a point projected
  //! in the reference frame of the projection plane.
  Standard_EXPORT void Convert (const Standard_Integer Xp, const Standard_Integer Yp,
                                Standard_Real& Xv, Standard_Real& Yv) const;

  //! Converts tha value of the projection plane into
  //! a PIXEL value.
  Standard_EXPORT Standard_Integer Convert (const Standard_Real Vv) const;

  //! Converts the point defined in the reference frame
  //! of the projection plane into a point PIXEL.
  Standard_EXPORT void Convert (const Standard_Real Xv, const Standard_Real Yv,
                                Standard_Integer& Xp, Standard_Integer& Yp) const;

  //! Converts the projected point into a point
  //! in the reference frame of the view corresponding
  //! to the intersection with the projection plane
  //! of the eye/view point vector.
  Standard_EXPORT void Convert (const Standard_Integer Xp, const Standard_Integer Yp,
                                Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const;

  //! Converts the projected point into a point
  //! in the reference frame of the view corresponding
  //! to the intersection with the projection plane
  //! of the eye/view point vector and returns the
  //! projection ray for further computations.
  Standard_EXPORT void ConvertWithProj (const Standard_Integer Xp, const Standard_Integer Yp,
                                        Standard_Real& X,  Standard_Real& Y,  Standard_Real& Z,
                                        Standard_Real& Vx, Standard_Real& Vy, Standard_Real& Vz) const;

  //! Converts the projected point into the nearest grid point
  //! in the reference frame of the view corresponding
  //! to the intersection with the projection plane
  //! of the eye/view point vector and display the grid marker.
  //! Warning: When the grid is not active the result is identical to the above Convert() method.
  //! How to use:
  //! 1) Enable the grid echo display
  //! myViewer->SetGridEcho(Standard_True);
  //! 2) When application receive a move event:
  //! 2.1) Check if any object is detected
  //! if( myInteractiveContext->MoveTo(x,y) == AIS_SOD_Nothing ) {
  //! 2.2) Check if the grid is active
  //! if( myViewer->Grid()->IsActive() ) {
  //! 2.3) Display the grid echo and gets the grid point
  //! myView->ConvertToGrid(x,y,X,Y,Z);
  //! myView->Viewer()->ShowGridEcho (myView, Graphic3d_Vertex (X,Y,Z));
  //! myView->RedrawImmediate();
  //! 2.4) Else this is the standard case
  //! } else myView->Convert(x,y,X,Y,Z);
  Standard_EXPORT void ConvertToGrid (const Standard_Integer Xp, const Standard_Integer Yp,
                                      Standard_Real& Xg, Standard_Real& Yg, Standard_Real& Zg) const;

  //! Converts the point into the nearest grid point
  //! and display the grid marker.
  Standard_EXPORT void ConvertToGrid (const Standard_Real X, const Standard_Real Y, const Standard_Real Z,
                                      Standard_Real& Xg, Standard_Real& Yg, Standard_Real& Zg) const;

  //! Projects the point defined in the reference frame of
  //! the view into the projected point in the associated window.
  Standard_EXPORT void Convert (const Standard_Real X, const Standard_Real Y, const Standard_Real Z,
                                Standard_Integer& Xp, Standard_Integer& Yp) const;

  //! Converts the point defined in the user space of
  //! the view to the projection plane at the depth
  //! relative to theZ.
  Standard_EXPORT void Project (const Standard_Real theX,
                                const Standard_Real theY,
                                const Standard_Real theZ,
                                Standard_Real& theXp,
                                Standard_Real& theYp) const;

  //! Converts the point defined in the user space of
  //! the view to the projection plane at the depth
  //! relative to theZ.
  Standard_EXPORT void Project (const Standard_Real theX,
                                const Standard_Real theY,
                                const Standard_Real theZ,
                                Standard_Real& theXp,
                                Standard_Real& theYp,
                                Standard_Real& theZp) const;

  //! Returns the Background color values of the view
  //! depending of the color Type.
  Standard_EXPORT void BackgroundColor (const Quantity_TypeOfColor Type, Standard_Real& V1, Standard_Real& V2, Standard_Real& V3) const;

  //! Returns the Background color object of the view.
  Standard_EXPORT Quantity_Color BackgroundColor() const;

  //! Returns the gradient background colors of the view.
  Standard_EXPORT void GradientBackgroundColors (Quantity_Color& theColor1, Quantity_Color& theColor2) const;

  //! Returns the gradient background of the view.
  Standard_EXPORT Aspect_GradientBackground GradientBackground() const;

  //! Returns the current value of the zoom expressed with
  //! respect to SetViewMappingDefault().
  Standard_EXPORT Standard_Real Scale() const;

  //! Returns the current values of the anisotropic (axial) scale factors.
  Standard_EXPORT void AxialScale (Standard_Real& Sx, Standard_Real& Sy, Standard_Real& Sz) const;

  //! Returns the height and width of the view.
  Standard_EXPORT void Size (Standard_Real& Width, Standard_Real& Height) const;

  //! Returns the Depth of the view .
  Standard_EXPORT Standard_Real ZSize() const;

  //! Returns the position of the eye.
  Standard_EXPORT void Eye (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const;

  //! Returns the position of point which emanating the projections.
  void FocalReferencePoint (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const { Eye (X,Y,Z); }

  //! Returns the coordinate of the point (Xpix,Ypix)
  //! in the view (XP,YP,ZP), and the projection vector of the
  //! view passing by the point (for PerspectiveView).
  Standard_EXPORT void ProjReferenceAxe (const Standard_Integer Xpix, const Standard_Integer Ypix,
                                         Standard_Real& XP, Standard_Real& YP, Standard_Real& ZP,
                                         Standard_Real& VX, Standard_Real& VY, Standard_Real& VZ) const;

  //! Returns the Distance between the Eye and View Point.
  Standard_EXPORT Standard_Real Depth() const;

  //! Returns the projection vector.
  Standard_EXPORT void Proj (Standard_Real& Vx, Standard_Real& Vy, Standard_Real& Vz) const;

  //! Returns the position of the view point.
  Standard_EXPORT void At (Standard_Real& X, Standard_Real& Y, Standard_Real& Z) const;

  //! Returns the vector giving the position of the high point.
  Standard_EXPORT void Up (Standard_Real& Vx, Standard_Real& Vy, Standard_Real& Vz) const;

  //! Returns in RADIANS the orientation of the view around
  //! the visual axis measured from the Y axis of the screen.
  Standard_EXPORT Standard_Real Twist() const;

  //! Returns the current shading model; Graphic3d_TypeOfShadingModel_Phong by default.
  Standard_EXPORT Graphic3d_TypeOfShadingModel ShadingModel() const;

  //! Defines the shading model for the visualization.
  Standard_EXPORT void SetShadingModel (const Graphic3d_TypeOfShadingModel theShadingModel);

  Standard_EXPORT Handle(Graphic3d_TextureEnv) TextureEnv() const;

  //! Returns the current visualisation mode.
  Standard_EXPORT V3d_TypeOfVisualization Visualization() const;

  //! Returns a list of active lights.
  const V3d_ListOfLight& ActiveLights() const { return myActiveLights; }

  //! Return iterator for defined lights.
  V3d_ListOfLightIterator ActiveLightIterator() const { return V3d_ListOfLightIterator (myActiveLights); }

  //! Returns the MAX number of light associated to the view.
  Standard_EXPORT Standard_Integer LightLimit() const;

  //! Returns the viewer in which the view has been created.
  Handle(V3d_Viewer) Viewer() const { return MyViewer; }

  //! Returns True if MyView is associated with a window .
  Standard_EXPORT Standard_Boolean IfWindow() const;

  //! Returns the Aspect Window associated with the view.
  const Handle(Aspect_Window)& Window() const { return MyWindow; }

  //! Returns the Type of the View
  Standard_EXPORT V3d_TypeOfView Type() const;

  //! Translates the center of the view along "x" and "y" axes of
  //! view projection. Can be used to perform interactive panning operation.
  //! In that case the DXp, DXp parameters specify panning relative to the
  //! point where the operation is started.
  //! @param theDXp [in] the relative panning on "x" axis of view projection, in pixels.
  //! @param theDYp [in] the relative panning on "y" axis of view projection, in pixels.
  //! @param theZoomFactor [in] the zooming factor.
  //! @param theToStart [in] pass TRUE when starting panning to remember view
  //! state prior to panning for relative arguments. Passing 0 for relative
  //! panning parameter should return view panning to initial state.
  //! Performs update of view.
  Standard_EXPORT void Pan (const Standard_Integer theDXp, const Standard_Integer theDYp, const Standard_Real theZoomFactor = 1, const Standard_Boolean theToStart = Standard_True);

  //! Zoom the view according to a zoom factor computed
  //! from the distance between the 2 mouse position.
  //! @param theXp1 [in] the x coordinate of first mouse position, in pixels.
  //! @param theYp1 [in] the y coordinate of first mouse position, in pixels.
  //! @param theXp2 [in] the x coordinate of second mouse position, in pixels.
  //! @param theYp2 [in] the y coordinate of second mouse position, in pixels.
  Standard_EXPORT void Zoom (const Standard_Integer theXp1, const Standard_Integer theYp1, const Standard_Integer theXp2, const Standard_Integer theYp2);

  //! Defines starting point for ZoomAtPoint view operation.
  //! @param theXp [in] the x mouse coordinate, in pixels.
  //! @param theYp [in] the y mouse coordinate, in pixels.
  Standard_EXPORT void StartZoomAtPoint (const Standard_Integer theXp, const Standard_Integer theYp);

  //! Zooms the model at a pixel defined by the method StartZoomAtPoint().
  Standard_EXPORT void ZoomAtPoint (const Standard_Integer theMouseStartX, const Standard_Integer theMouseStartY, const Standard_Integer theMouseEndX, const Standard_Integer theMouseEndY);

  //! Performs  anisotropic scaling  of  <me>  view  along  the  given  <Axis>.
  //! The  scale  factor  is  calculated on a basis of
  //! the mouse pointer displacement <Dx,Dy>.
  //! The  calculated  scale  factor  is  then  passed  to  SetAxialScale(Sx,  Sy,  Sz)  method.
  Standard_EXPORT void AxialScale (const Standard_Integer Dx, const Standard_Integer Dy, const V3d_TypeOfAxe Axis);

  //! Begin the rotation of the view around the screen axis
  //! according to the mouse position <X,Y>.
  //! Warning: Enable rotation around the Z screen axis when <zRotationThreshold>
  //! factor is > 0 soon the distance from the start point and the center
  //! of the view is > (medium viewSize * <zRotationThreshold> ).
  //! Generally a value of 0.4 is usable to rotate around XY screen axis
  //! inside the circular threshold area and to rotate around Z screen axis
  //! outside this area.
  Standard_EXPORT void StartRotation (const Standard_Integer X, const Standard_Integer Y, const Standard_Real zRotationThreshold = 0.0);

  //! Continues the rotation of the view
  //! with an angle computed from the last and new mouse position <X,Y>.
  Standard_EXPORT void Rotation (const Standard_Integer X, const Standard_Integer Y);

  //! Change View Plane Distance for Perspective Views
  //! Warning! raises TypeMismatch from Standard if the view
  //! is not a perspective view.
  Standard_EXPORT void SetFocale (const Standard_Real Focale);

  //! Returns the View Plane Distance for Perspective Views
  Standard_EXPORT Standard_Real Focale() const;

  //! Returns the associated Graphic3d view.
  const Handle(Graphic3d_CView)& View() const { return myView; }

  //! Switches computed HLR mode in the view.
  Standard_EXPORT void SetComputedMode (const Standard_Boolean theMode);

  //! Returns the computed HLR mode state.
  Standard_EXPORT Standard_Boolean ComputedMode() const;

  //! idem than WindowFit
  void WindowFitAll (const Standard_Integer Xmin, const Standard_Integer Ymin, const Standard_Integer Xmax, const Standard_Integer Ymax)
  {
    WindowFit (Xmin, Ymin, Xmax, Ymax);
  }

  //! Transform camera eye, center and scale to fit in the passed bounding box specified in WCS.
  //! @param theCamera [in] the camera
  //! @param theBox    [in] the bounding box
  //! @param theMargin [in] the margin coefficient for view borders
  //! @param theResolution [in] the minimum size of projection of bounding box in Xv or Yv direction when it considered to be a thin plane or point (without a volume);
  //!                           in this case only the center of camera is adjusted
  //! @param theToEnlargeIfLine [in] when TRUE - in cases when the whole bounding box projected into thin line going along Z-axis of screen,
  //!                                the view plane is enlarged such thatwe see the whole line on rotation, otherwise only the center of camera is adjusted.
  //! @return TRUE if the fit all operation can be done
  Standard_EXPORT Standard_Boolean FitMinMax (const Handle(Graphic3d_Camera)& theCamera,
                                              const Bnd_Box& theBox,
                                              const Standard_Real theMargin,
                                              const Standard_Real theResolution = 0.0,
                                              const Standard_Boolean theToEnlargeIfLine = Standard_True) const;

  //! Defines or Updates the definition of the
  //! grid in <me>
  Standard_EXPORT void SetGrid (const gp_Ax3& aPlane, const Handle(Aspect_Grid)& aGrid);

  //! Defines or Updates the activity of the
  //! grid in <me>
  Standard_EXPORT void SetGridActivity (const Standard_Boolean aFlag);

  //! Dumps the full contents of the View into the image file. This is an alias for ToPixMap() with Image_AlienPixMap.
  //! @param theFile destination image file (image format is determined by file extension like .png, .bmp, .jpg)
  //! @param theBufferType buffer to dump
  //! @return FALSE when the dump has failed
  Standard_EXPORT Standard_Boolean Dump (const Standard_CString theFile, const Graphic3d_BufferType& theBufferType = Graphic3d_BT_RGB);

  //! Dumps the full contents of the view to a pixmap with specified parameters.
  //! Internally this method calls Redraw() with an offscreen render buffer of requested target size (theWidth x theHeight),
  //! so that there is no need resizing a window control for making a dump of different size.
  Standard_EXPORT Standard_Boolean ToPixMap (Image_PixMap&               theImage,
                                             const V3d_ImageDumpOptions& theParams);

  //! Dumps the full contents of the view to a pixmap.
  //! Internally this method calls Redraw() with an offscreen render buffer of requested target size (theWidth x theHeight),
  //! so that there is no need resizing a window control for making a dump of different size.
  //! @param theImage          target image, will be re-allocated to match theWidth x theHeight
  //! @param theWidth          target image width
  //! @param theHeight         target image height
  //! @param theBufferType     type of the view buffer to dump (color / depth)
  //! @param theToAdjustAspect when true, active view aspect ratio will be overridden by (theWidth / theHeight)
  //! @param theStereoOptions  how to dump stereographic camera
  Standard_Boolean ToPixMap (Image_PixMap& theImage,
                             const Standard_Integer theWidth,
                             const Standard_Integer theHeight,
                             const Graphic3d_BufferType& theBufferType     = Graphic3d_BT_RGB,
                             const Standard_Boolean      theToAdjustAspect = Standard_True,
                             const V3d_StereoDumpOptions theStereoOptions  = V3d_SDO_MONO)
  {
    V3d_ImageDumpOptions aParams;
    aParams.Width  = theWidth;
    aParams.Height = theHeight;
    aParams.BufferType = theBufferType;
    aParams.StereoOptions  = theStereoOptions;
    aParams.ToAdjustAspect = theToAdjustAspect;
    return ToPixMap (theImage, aParams);
  }

  //! Manages display of the back faces
  Standard_EXPORT void SetBackFacingModel (const Graphic3d_TypeOfBackfacingModel theModel = Graphic3d_TypeOfBackfacingModel_Auto);

  //! Returns current state of the back faces display; Graphic3d_TypeOfBackfacingModel_Auto by default,
  //! which means that backface culling is defined by each presentation.
  Standard_EXPORT Graphic3d_TypeOfBackfacingModel BackFacingModel() const;

  //! Adds clip plane to the view. The composition of clip planes truncates the
  //! rendering space to convex volume. Number of supported clip planes can be consulted
  //! by PlaneLimit method of associated Graphic3d_GraphicDriver.
  //! Please be aware that the planes which exceed the limit are ignored during rendering.
  //! @param thePlane [in] the clip plane to be added to view.
  Standard_EXPORT virtual void AddClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane);

  //! Removes clip plane from the view.
  //! @param thePlane [in] the clip plane to be removed from view.
  Standard_EXPORT virtual void RemoveClipPlane (const Handle(Graphic3d_ClipPlane)& thePlane);

  //! Get clip planes.
  //! @return sequence clip planes that have been set for the view
  Standard_EXPORT const Handle(Graphic3d_SequenceOfHClipPlane)& ClipPlanes() const;

  //! Sets sequence of clip planes to the view. The planes that have been set
  //! before are removed from the view. The composition of clip planes
  //! truncates the rendering space to convex volume. Number of supported
  //! clip planes can be consulted by InquirePlaneLimit method of
  //! Graphic3d_GraphicDriver. Please be aware that the planes that
  //! exceed the limit are ignored during rendering.
  //! @param thePlanes [in] the clip planes to set.
  Standard_EXPORT void SetClipPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes);

  //! Returns the MAX number of clipping planes associated to the view.
  Standard_EXPORT Standard_Integer PlaneLimit() const;

  //! Change camera used by view.
  Standard_EXPORT void SetCamera (const Handle(Graphic3d_Camera)& theCamera);

  //! Returns camera object of the view.
  //! @return: handle to camera object, or NULL if 3D view does not use
  //! the camera approach.
  Standard_EXPORT const Handle(Graphic3d_Camera)& Camera() const;

  //! Return default camera.
  const Handle(Graphic3d_Camera)& DefaultCamera() const { return myDefaultCamera; }

  //! Returns current rendering parameters and effect settings.
  //! By default it returns default parameters of current viewer.
  //! To define view-specific settings use method V3d_View::ChangeRenderingParams().
  //! @sa V3d_Viewer::DefaultRenderingParams()
  Standard_EXPORT const Graphic3d_RenderingParams& RenderingParams() const;

  //! Returns reference to current rendering parameters and effect settings.
  Standard_EXPORT Graphic3d_RenderingParams& ChangeRenderingParams();

  //! @return flag value of objects culling mechanism
  Standard_Boolean IsCullingEnabled() const { return RenderingParams().FrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_On; }

  //! Turn on/off automatic culling of objects outside frustum (ON by default)
  void SetFrustumCulling (Standard_Boolean theMode) { ChangeRenderingParams().FrustumCullingState = theMode ? Graphic3d_RenderingParams::FrustumCulling_On : Graphic3d_RenderingParams::FrustumCulling_Off; }

  //! Fill in the dictionary with diagnostic info.
  //! Should be called within rendering thread.
  //!
  //! This API should be used only for user output or for creating automated reports.
  //! The format of returned information (e.g. key-value layout)
  //! is NOT part of this API and can be changed at any time.
  //! Thus application should not parse returned information to weed out specific parameters.
  //! @param theDict  destination map for information
  //! @param theFlags defines the information to be retrieved
  Standard_EXPORT void DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                              Graphic3d_DiagnosticInfo theFlags) const;

  //! Returns string with statistic performance info.
  Standard_EXPORT TCollection_AsciiString StatisticInformation() const;

  //! Fills in the dictionary with statistic performance info.
  Standard_EXPORT void StatisticInformation (TColStd_IndexedDataMapOfStringString& theDict) const;

  //! Returns the Objects number and the gravity center of ALL viewable points in the view
  Standard_EXPORT gp_Pnt GravityPoint() const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(V3d_View,Standard_Transient)

public: //! @name deprecated methods

  //! Returns True if One light more can be
  //! activated in this View.
  Standard_DEPRECATED ("Deprecated method - ActiveLights() should be used instead")
  Standard_EXPORT Standard_Boolean IfMoreLights() const;

  //! initializes an iteration on the active Lights.
  Standard_DEPRECATED ("Deprecated method - ActiveLights() should be used instead")
  void InitActiveLights() { myActiveLightsIterator.Initialize (myActiveLights); }

  //! returns true if there are more active Light(s) to return.
  Standard_DEPRECATED ("Deprecated method - ActiveLights() should be used instead")
  Standard_Boolean MoreActiveLights() const { return myActiveLightsIterator.More(); }

  //! Go to the next active Light (if there is not, ActiveLight will raise an exception)
  Standard_DEPRECATED ("Deprecated method - ActiveLights() should be used instead")
  void NextActiveLights() { myActiveLightsIterator.Next(); }

  Standard_DEPRECATED ("Deprecated method - ActiveLights() should be used instead")
  const Handle(V3d_Light)& ActiveLight() const { return myActiveLightsIterator.Value(); }

protected:

  Standard_EXPORT void ImmediateUpdate() const;

  //! Scales camera to fit the view frame of defined width and height
  //! keeping the aspect. For orthogonal camera the method changes scale,
  //! for perspective adjusts Eye location about the Center point.
  //! @param theSizeXv [in] size of viewport frame on "x" axis.
  //! @param theSizeYv [in] size of viewport frame on "y" axis.
  Standard_EXPORT void Scale (const Handle(Graphic3d_Camera)& theCamera, const Standard_Real theSizeXv, const Standard_Real theSizeYv) const;

  Standard_EXPORT void Translate (const Handle(Graphic3d_Camera)& theCamera, const Standard_Real theDXv, const Standard_Real theDYv) const;

private:

  //! Modifies the aspect ratio of the camera when
  //! the associated window is defined or resized.
  Standard_EXPORT void SetRatio();

  //! Determines the screen axes in the reference
  //! framework of the view.
  Standard_EXPORT static Standard_Boolean screenAxis (const gp_Dir& theVpn, const gp_Dir& theVup,
                                                      gp_Vec& theXaxe, gp_Vec& theYaxe, gp_Vec& theZaxe);
  
  //! Transforms the Vertex V according to the matrice Matrix .
  Standard_EXPORT static gp_XYZ TrsPoint (const Graphic3d_Vertex& V, const TColStd_Array2OfReal& Matrix);
  
  //! Returns the objects number and the projection window
  //! of the objects contained in the view.
  Standard_EXPORT Standard_Integer MinMax (Standard_Real& Umin, Standard_Real& Vmin, Standard_Real& Umax, Standard_Real& Vmax) const;
  
  //! Returns the objects number and the box encompassing
  //! the objects contained in the view
  Standard_EXPORT Standard_Integer MinMax (Standard_Real& Xmin, Standard_Real& Ymin, Standard_Real& Zmin, Standard_Real& Xmax, Standard_Real& Ymax, Standard_Real& Zmax) const;
  
  Standard_EXPORT void Init();
  
  //! Returns a new vertex when the grid is activated.
  Standard_EXPORT Graphic3d_Vertex Compute (const Graphic3d_Vertex& AVertex) const;

protected:

  Standard_Real myOldMouseX;
  Standard_Real myOldMouseY;
  gp_Dir myCamStartOpUp;
  gp_Dir myCamStartOpDir;
  gp_Pnt myCamStartOpEye;
  gp_Pnt myCamStartOpCenter;
  Handle(Graphic3d_Camera) myDefaultCamera;
  Handle(Graphic3d_CView) myView;
  Standard_Boolean myImmediateUpdate;
  mutable Standard_Boolean myIsInvalidatedImmediate;

private:

  V3d_Viewer* MyViewer;
  V3d_ListOfLight myActiveLights;
  gp_Dir myDefaultViewAxis;
  gp_Pnt myDefaultViewPoint;
  Handle(Aspect_Window) MyWindow;
  V3d_ListOfLight::Iterator myActiveLightsIterator;
  Standard_Integer sx;
  Standard_Integer sy;
  Standard_Real rx;
  Standard_Real ry;
  gp_Pnt myRotateGravity;
  Standard_Boolean myComputedMode;
  Standard_Boolean SwitchSetFront;
  Standard_Boolean myZRotation;
  Standard_Integer MyZoomAtPointX;
  Standard_Integer MyZoomAtPointY;
  Handle(V3d_Trihedron) myTrihedron;
  Handle(Aspect_Grid) MyGrid;
  gp_Ax3 MyPlane;
  TColStd_Array2OfReal MyTrsf;
  Handle(Graphic3d_Structure) MyGridEchoStructure;
  Handle(Graphic3d_Group) MyGridEchoGroup;
  gp_Vec myXscreenAxis;
  gp_Vec myYscreenAxis;
  gp_Vec myZscreenAxis;
  gp_Dir myViewAxis;
  Graphic3d_Vertex myGravityReferencePoint;
  Standard_Boolean myAutoZFitIsOn;
  Standard_Real myAutoZFitScaleFactor;

};

#endif // _V3d_View_HeaderFile
