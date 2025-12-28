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

#include <Graphic3d_ClipPlane.hxx>
#include <Graphic3d_Texture2D.hxx>
#include <Graphic3d_TypeOfShadingModel.hxx>
#include <Image_PixMap.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <V3d_ImageDumpOptions.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_Trihedron.hxx>
#include <V3d_TypeOfAxe.hxx>
#include <V3d_TypeOfOrientation.hxx>
#include <V3d_TypeOfView.hxx>
#include <V3d_TypeOfVisualization.hxx>

class Aspect_Grid;
class Aspect_Window;
class Graphic3d_Group;
class Graphic3d_Structure;
class Graphic3d_TextureEnv;

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
  DEFINE_STANDARD_RTTIEXT(V3d_View, Standard_Transient)
public:
  //! Initializes the view.
  Standard_EXPORT V3d_View(const occ::handle<V3d_Viewer>& theViewer,
                           const V3d_TypeOfView           theType = V3d_ORTHOGRAPHIC);

  //! Initializes the view by copying.
  Standard_EXPORT V3d_View(const occ::handle<V3d_Viewer>& theViewer,
                           const occ::handle<V3d_View>&   theView);

  //! Default destructor.
  Standard_EXPORT virtual ~V3d_View();

  //! Activates the view in the specified Window
  //! If <aContext> is not NULL the graphic context is used
  //! to draw something in this view.
  //! Otherwise an internal graphic context is created.
  //! Warning: The view is centered and resized to preserve
  //! the height/width ratio of the window.
  Standard_EXPORT void SetWindow(const occ::handle<Aspect_Window>& theWindow,
                                 const Aspect_RenderingContext     theContext = NULL);

  //! Activates the view as subview of another view.
  //! @param[in] theParentView parent view to put subview into
  //! @param[in] theSize subview dimensions;
  //!                    values >= 2   define size in pixels,
  //!                    values <= 1.0 define size as a fraction of parent view
  //! @param[in] theCorner corner within parent view
  //! @param[in] theOffset offset from the corner;
  //!                      values >= 1   define offset in pixels,
  //!                      values <  1.0 define offset as a fraction of parent view
  //! @param[in] theMargins subview margins in pixels
  //!
  //! Example: to split parent view horizontally into 2 subview,
  //! define one subview with Size=(0.5,1.0),Offset=(0.0,0.0), and 2nd with
  //! Size=(0.5,1.0),Offset=(5.0,0.0);
  Standard_EXPORT void SetWindow(
    const occ::handle<V3d_View>&    theParentView,
    const NCollection_Vec2<double>& theSize,
    Aspect_TypeOfTriedronPosition   theCorner  = Aspect_TOTP_LEFT_UPPER,
    const NCollection_Vec2<double>& theOffset  = NCollection_Vec2<double>(),
    const NCollection_Vec2<int>&    theMargins = NCollection_Vec2<int>());

  Standard_EXPORT void SetMagnify(const occ::handle<Aspect_Window>& theWindow,
                                  const occ::handle<V3d_View>&      thePreviousView,
                                  const int                         theX1,
                                  const int                         theY1,
                                  const int                         theX2,
                                  const int                         theY2);

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
  Standard_EXPORT bool IsInvalidated() const;

  //! Returns true if immediate layer content has been invalidated.
  bool IsInvalidatedImmediate() const { return myIsInvalidatedImmediate; }

  //! Invalidates view content within immediate layer but does not redraw it.
  void InvalidateImmediate() { myIsInvalidatedImmediate = true; }

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
  Standard_EXPORT bool IsEmpty() const;

  //! Updates the lights of the view.
  Standard_EXPORT void UpdateLights() const;

  //! Sets the automatic z-fit mode and its parameters.
  //! The auto z-fit has extra parameters which can controlled from application level
  //! to ensure that the size of viewing volume will be sufficiently large to cover
  //! the depth of unmanaged objects, for example, transformation persistent ones.
  //! @param[in] theScaleFactor  the scale factor for Z-range.
  //! The range between Z-min, Z-max projection volume planes
  //! evaluated by z fitting method will be scaled using this coefficient.
  //! Program error exception is thrown if negative or zero value
  //! is passed.
  Standard_EXPORT void SetAutoZFitMode(const bool theIsOn, const double theScaleFactor = 1.0);

  //! returns TRUE if automatic z-fit mode is turned on.
  bool AutoZFitMode() const { return myAutoZFitIsOn; }

  //! returns scale factor parameter of automatic z-fit mode.
  double AutoZFitScaleFactor() const { return myAutoZFitScaleFactor; }

  //! If automatic z-range fitting is turned on, adjusts Z-min and Z-max
  //! projection volume planes with call to ZFitAll.
  Standard_EXPORT void AutoZFit() const;

  //! Change Z-min and Z-max planes of projection volume to match the
  //! displayed objects.
  Standard_EXPORT void ZFitAll(const double theScaleFactor = 1.0) const;

public:
  //! Defines the background color of the view by the color definition type and the three
  //! corresponding values.
  Standard_EXPORT void SetBackgroundColor(const Quantity_TypeOfColor theType,
                                          const double               theV1,
                                          const double               theV2,
                                          const double               theV3);

  //! Defines the background color of the view.
  Standard_EXPORT void SetBackgroundColor(const Quantity_Color& theColor);

  //! Defines the gradient background colors of the view by supplying the colors
  //! and the fill method (horizontal by default).
  Standard_EXPORT void SetBgGradientColors(
    const Quantity_Color&           theColor1,
    const Quantity_Color&           theColor2,
    const Aspect_GradientFillMethod theFillStyle = Aspect_GradientFillMethod_Horizontal,
    const bool                      theToUpdate  = false);

  //! Defines the gradient background fill method of the view.
  Standard_EXPORT void SetBgGradientStyle(
    const Aspect_GradientFillMethod theMethod   = Aspect_GradientFillMethod_Horizontal,
    const bool                      theToUpdate = false);

  //! Defines the background texture of the view by supplying the texture image file name
  //! and fill method (centered by default).
  Standard_EXPORT void SetBackgroundImage(const char*             theFileName,
                                          const Aspect_FillMethod theFillStyle = Aspect_FM_CENTERED,
                                          const bool              theToUpdate  = false);

  //! Defines the background texture of the view by supplying the texture and fill method (centered
  //! by default)
  Standard_EXPORT void SetBackgroundImage(const occ::handle<Graphic3d_Texture2D>& theTexture,
                                          const Aspect_FillMethod theFillStyle = Aspect_FM_CENTERED,
                                          const bool              theToUpdate  = false);

  //! Defines the textured background fill method of the view.
  Standard_EXPORT void SetBgImageStyle(const Aspect_FillMethod theFillStyle,
                                       const bool              theToUpdate = false);

  //! Sets environment cubemap as background.
  //! @param theCubeMap cubemap source to be set as background
  //! @param theToUpdatePBREnv defines whether IBL maps will be generated or not (see
  //! 'GeneratePBREnvironment')
  Standard_EXPORT void SetBackgroundCubeMap(const occ::handle<Graphic3d_CubeMap>& theCubeMap,
                                            bool theToUpdatePBREnv = true,
                                            bool theToUpdate       = false);

  //! Returns skydome aspect;
  const Aspect_SkydomeBackground& BackgroundSkydome() const { return myView->BackgroundSkydome(); }

  //! Sets skydome aspect
  //! @param theAspect cubemap generation parameters
  //! @param theToUpdatePBREnv defines whether IBL maps will be generated or not
  Standard_EXPORT void SetBackgroundSkydome(const Aspect_SkydomeBackground& theAspect,
                                            bool theToUpdatePBREnv = true);

  //! Returns TRUE if IBL (Image Based Lighting) from background cubemap is enabled.
  Standard_EXPORT bool IsImageBasedLighting() const;

  //! Enables or disables IBL (Image Based Lighting) from background cubemap.
  //! Has no effect if PBR is not used.
  //! @param[in] theToEnableIBL enable or disable IBL from background cubemap
  //! @param[in] theToUpdate redraw the view
  Standard_EXPORT void SetImageBasedLighting(bool theToEnableIBL, bool theToUpdate = false);

  //! Activates IBL from background cubemap.
  void GeneratePBREnvironment(bool theToUpdate = false)
  {
    SetImageBasedLighting(true, theToUpdate);
  }

  //! Disables IBL from background cubemap; fills PBR specular probe and irradiance map with white
  //! color.
  void ClearPBREnvironment(bool theToUpdate = false) { SetImageBasedLighting(true, theToUpdate); }

  //! Sets the environment texture to use. No environment texture by default.
  Standard_EXPORT void SetTextureEnv(const occ::handle<Graphic3d_TextureEnv>& theTexture);

  //! Definition of an axis from its origin and
  //! its orientation .
  //! This will be the current axis for rotations and movements.
  //! Warning! raises BadValue from V3d if the vector normal is NULL. .
  Standard_EXPORT void SetAxis(const double X,
                               const double Y,
                               const double Z,
                               const double Vx,
                               const double Vy,
                               const double Vz);

public:
  //! Defines the visualization type in the view.
  Standard_EXPORT void SetVisualization(const V3d_TypeOfVisualization theType);

  //! Activates theLight in the view.
  Standard_EXPORT void SetLightOn(const occ::handle<V3d_Light>& theLight);

  //! Activates all the lights defined in this view.
  Standard_EXPORT void SetLightOn();

  //! Deactivate theLight in this view.
  Standard_EXPORT void SetLightOff(const occ::handle<V3d_Light>& theLight);

  //! Deactivate all the Lights defined in this view.
  Standard_EXPORT void SetLightOff();

  //! Returns TRUE when the light is active in this view.
  Standard_EXPORT bool IsActiveLight(const occ::handle<V3d_Light>& theLight) const;

  //! sets the immediate update mode and returns the previous one.
  Standard_EXPORT bool SetImmediateUpdate(const bool theImmediateUpdate);

  //! Returns trihedron object.
  const occ::handle<V3d_Trihedron>& Trihedron(bool theToCreate = true)
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
  Standard_EXPORT void ZBufferTriedronSetup(const Quantity_Color& theXColor    = Quantity_NOC_RED,
                                            const Quantity_Color& theYColor    = Quantity_NOC_GREEN,
                                            const Quantity_Color& theZColor    = Quantity_NOC_BLUE1,
                                            const double          theSizeRatio = 0.8,
                                            const double          theAxisDiametr = 0.05,
                                            const int             theNbFacettes  = 12);

  //! Display of the Triedron.
  //! Initialize position, color and length of Triedron axes.
  //! The scale is a percent of the window width.
  Standard_EXPORT void TriedronDisplay(
    const Aspect_TypeOfTriedronPosition thePosition = Aspect_TOTP_CENTER,
    const Quantity_Color&               theColor    = Quantity_NOC_WHITE,
    const double                        theScale    = 0.02,
    const V3d_TypeOfVisualization       theMode     = V3d_WIREFRAME);

  //! Erases the Triedron.
  Standard_EXPORT void TriedronErase();

  //! Returns data of a graduated trihedron.
  Standard_EXPORT const Graphic3d_GraduatedTrihedron& GetGraduatedTrihedron() const;

  //! Displays a graduated trihedron.
  Standard_EXPORT void GraduatedTrihedronDisplay(
    const Graphic3d_GraduatedTrihedron& theTrihedronData);

  //! Erases a graduated trihedron from the view.
  Standard_EXPORT void GraduatedTrihedronErase();

  //! modify the Projection of the view perpendicularly to
  //! the privileged plane of the viewer.
  Standard_EXPORT void SetFront();

  //! Rotates the eye about the coordinate system of
  //! reference of the screen
  //! for which the origin is the view point of the projection,
  //! with a relative angular value in RADIANS with respect to
  //! the initial position expressed by Start = true
  //! Warning! raises BadValue from V3d
  //! If the eye, the view point, or the high point are
  //! aligned or confused.
  Standard_EXPORT void Rotate(const double Ax,
                              const double Ay,
                              const double Az,
                              const bool   Start = true);

  //! Rotates the eye about the coordinate system of
  //! reference of the screen
  //! for which the origin is Gravity point {X,Y,Z},
  //! with a relative angular value in RADIANS with respect to
  //! the initial position expressed by Start = true
  //! If the eye, the view point, or the high point are
  //! aligned or confused.
  Standard_EXPORT void Rotate(const double Ax,
                              const double Ay,
                              const double Az,
                              const double X,
                              const double Y,
                              const double Z,
                              const bool   Start = true);

  //! Rotates the eye about one of the coordinate axes of
  //! of the view for which the origin is the Gravity point{X,Y,Z}
  //! with an relative angular value in RADIANS with
  //! respect to the initial position expressed by
  //! Start = true
  Standard_EXPORT void Rotate(const V3d_TypeOfAxe Axe,
                              const double        Angle,
                              const double        X,
                              const double        Y,
                              const double        Z,
                              const bool          Start = true);

  //! Rotates the eye about one of the coordinate axes of
  //! of the view for which the origin is the view point of the
  //! projection with an relative angular value in RADIANS with
  //! respect to the initial position expressed by
  //! Start = true
  Standard_EXPORT void Rotate(const V3d_TypeOfAxe Axe, const double Angle, const bool Start = true);

  //! Rotates the eye around the current axis a relative
  //! angular value in RADIANS with respect to the initial
  //! position expressed by Start = true
  Standard_EXPORT void Rotate(const double Angle, const bool Start = true);

  //! Movement of the eye parallel to the coordinate system
  //! of reference of the screen a distance relative to the
  //! initial position expressed by Start = true.
  Standard_EXPORT void Move(const double Dx,
                            const double Dy,
                            const double Dz,
                            const bool   Start = true);

  //! Movement of the eye parallel to one of the axes of the
  //! coordinate system of reference of the view a distance
  //! relative to the initial position expressed by
  //! Start = true.
  Standard_EXPORT void Move(const V3d_TypeOfAxe Axe, const double Length, const bool Start = true);

  //! Movement of the eye parllel to the current axis
  //! a distance relative to the initial position
  //! expressed by Start = true
  Standard_EXPORT void Move(const double Length, const bool Start = true);

  //! Movement of the ye and the view point parallel to the
  //! frame of reference of the screen a distance relative
  //! to the initial position expressed by
  //! Start = true
  Standard_EXPORT void Translate(const double Dx,
                                 const double Dy,
                                 const double Dz,
                                 const bool   Start = true);

  //! Movement of the eye and the view point parallel to one
  //! of the axes of the fame of reference of the view a
  //! distance relative to the initial position
  //! expressed by Start = true
  Standard_EXPORT void Translate(const V3d_TypeOfAxe Axe,
                                 const double        Length,
                                 const bool          Start = true);

  //! Movement of the eye and view point parallel to
  //! the current axis a distance relative to the initial
  //! position expressed by Start = true
  Standard_EXPORT void Translate(const double Length, const bool Start = true);

  //! places the point of the view corresponding
  //! at the pixel position x,y at the center of the window
  //! and updates the view.
  Standard_EXPORT void Place(const int theXp, const int theYp, const double theZoomFactor = 1);

  //! Rotation of the view point around the frame of reference
  //! of the screen for which the origin is the eye of the
  //! projection with a relative angular value in RADIANS
  //! with respect to the initial position expressed by
  //! Start = true
  Standard_EXPORT void Turn(const double Ax,
                            const double Ay,
                            const double Az,
                            const bool   Start = true);

  //! Rotation of the view point around one of the axes of the
  //! frame of reference of the view for which the origin is
  //! the eye of the projection with an angular value in
  //! RADIANS relative to the initial position expressed by
  //! Start = true
  Standard_EXPORT void Turn(const V3d_TypeOfAxe Axe, const double Angle, const bool Start = true);

  //! Rotation of the view point around the current axis an
  //! angular value in RADIANS relative to the initial
  //! position expressed by Start = true
  Standard_EXPORT void Turn(const double Angle, const bool Start = true);

  //! Defines the angular position of the high point of
  //! the reference frame of the view with respect to the
  //! Y screen axis with an absolute angular value in
  //! RADIANS.
  Standard_EXPORT void SetTwist(const double Angle);

  //! Defines the position of the eye..
  Standard_EXPORT void SetEye(const double X, const double Y, const double Z);

  //! Defines the Depth of the eye from the view point
  //! without update the projection .
  Standard_EXPORT void SetDepth(const double Depth);

  //! Defines the orientation of the projection.
  Standard_EXPORT void SetProj(const double Vx, const double Vy, const double Vz);

  //! Defines the orientation of the projection .
  //! @param theOrientation camera direction
  //! @param theIsYup       flag indicating Y-up (TRUE) or Z-up (FALSE) convention
  Standard_EXPORT void SetProj(const V3d_TypeOfOrientation theOrientation,
                               const bool                  theIsYup = false);

  //! Defines the position of the view point.
  Standard_EXPORT void SetAt(const double X, const double Y, const double Z);

  //! Defines the orientation of the high point.
  Standard_EXPORT void SetUp(const double Vx, const double Vy, const double Vz);

  //! Defines the orientation(SO) of the high point.
  Standard_EXPORT void SetUp(const V3d_TypeOfOrientation Orientation);

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
  //! @param[in] theDXv  the relative panning on "x" axis of view projection, in view space
  //! coordinates.
  //! @param[in] theDYv  the relative panning on "y" axis of view projection, in view space
  //! coordinates.
  //! @param[in] theZoomFactor  the zooming factor.
  //! @param[in] theToStart  pass TRUE when starting panning to remember view
  //! state prior to panning for relative arguments. If panning is started,
  //! passing {0, 0} for {theDXv, theDYv} will return view to initial state.
  //! Performs update of view.
  Standard_EXPORT void Panning(const double theDXv,
                               const double theDYv,
                               const double theZoomFactor = 1,
                               const bool   theToStart    = true);

  //! Relocates center of screen to the point, determined by
  //! {Xp, Yp} pixel coordinates relative to the bottom-left corner of
  //! screen. To calculate pixel coordinates for any point from world
  //! coordinate space, it can be projected using "Project".
  //! @param[in] theXp  the x coordinate.
  //! @param[in] theYp  the y coordinate.
  Standard_EXPORT void SetCenter(const int theXp, const int theYp);

  //! Defines the view projection size in its maximum dimension,
  //! keeping the initial height/width ratio unchanged.
  Standard_EXPORT void SetSize(const double theSize);

  //! Defines the Depth size of the view
  //! Front Plane will be set to Size/2.
  //! Back Plane will be set to -Size/2.
  //! Any Object located Above the Front Plane or
  //! behind the Back Plane will be Clipped .
  //! NOTE than the XY Size of the View is NOT modified .
  Standard_EXPORT void SetZSize(const double SetZSize);

  //! Zooms the view by a factor relative to the initial
  //! value expressed by Start = true
  //! Updates the view.
  Standard_EXPORT void SetZoom(const double Coef, const bool Start = true);

  //! Zooms the view by a factor relative to the value
  //! initialised by SetViewMappingDefault().
  //! Updates the view.
  Standard_EXPORT void SetScale(const double Coef);

  //! Sets anisotropic (axial) scale factors <Sx>, <Sy>, <Sz> for view <me>.
  //! Anisotropic scaling operation is performed through multiplying
  //! the current view orientation matrix by a scaling matrix:
  //! || Sx  0   0   0 ||
  //! || 0   Sy  0   0 ||
  //! || 0   0   Sz  0 ||
  //! || 0   0   0   1 ||
  //! Updates the view.
  Standard_EXPORT void SetAxialScale(const double Sx, const double Sy, const double Sz);

  //! Adjust view parameters to fit the displayed scene, respecting height / width ratio.
  //! The Z clipping range (depth range) is fitted if AutoZFit flag is TRUE.
  //! Throws program error exception if margin coefficient is < 0 or >= 1.
  //! Updates the view.
  //! @param[in] theMargin  the margin coefficient for view borders.
  //! @param[in] theToUpdate  flag to perform view update.
  Standard_EXPORT void FitAll(const double theMargin = 0.01, const bool theToUpdate = true);

  //! Adjust view parameters to fit the displayed scene, respecting height / width ratio
  //! according to the custom bounding box given.
  //! Throws program error exception if margin coefficient is < 0 or >= 1.
  //! Updates the view.
  //! @param[in] theBox  the custom bounding box to fit.
  //! @param[in] theMargin  the margin coefficient for view borders.
  //! @param[in] theToUpdate  flag to perform view update.
  Standard_EXPORT void FitAll(const Bnd_Box& theBox,
                              const double   theMargin   = 0.01,
                              const bool     theToUpdate = true);

  //! Adjusts the viewing volume so as not to clip the displayed objects by front and back
  //! and back clipping planes. Also sets depth value automatically depending on the
  //! calculated Z size and Aspect parameter.
  //! NOTE than the original XY size of the view is NOT modified .
  Standard_EXPORT void DepthFitAll(const double Aspect = 0.01, const double Margin = 0.01);

  //! Centers the defined projection window so that it occupies
  //! the maximum space while respecting the initial
  //! height/width ratio.
  //! NOTE than the original Z size of the view is NOT modified .
  Standard_EXPORT void FitAll(const double theMinXv,
                              const double theMinYv,
                              const double theMaxXv,
                              const double theMaxYv);

  //! Centers the defined PIXEL window so that it occupies
  //! the maximum space while respecting the initial height/width ratio.
  //! NOTE than the original Z size of the view is NOT modified.
  //! @param[in] theMinXp  pixel coordinates of minimal corner on x screen axis.
  //! @param[in] theMinYp  pixel coordinates of minimal corner on y screen axis.
  //! @param[in] theMaxXp  pixel coordinates of maximal corner on x screen axis.
  //! @param[in] theMaxYp  pixel coordinates of maximal corner on y screen axis.
  Standard_EXPORT void WindowFit(const int theMinXp,
                                 const int theMinYp,
                                 const int theMaxXp,
                                 const int theMaxYp);

  //! Saves the current view mapping. This will be the
  //! state returned from ResetViewmapping.
  Standard_EXPORT void SetViewMappingDefault();

  //! Resets the centering of the view.
  //! Updates the view
  Standard_EXPORT void ResetViewMapping();

  //! Resets the centering and the orientation of the view.
  Standard_EXPORT void Reset(const bool theToUpdate = true);

  //! Converts the PIXEL value
  //! to a value in the projection plane.
  Standard_EXPORT double Convert(const int Vp) const;

  //! Converts the point PIXEL into a point projected
  //! in the reference frame of the projection plane.
  Standard_EXPORT void Convert(const int Xp, const int Yp, double& Xv, double& Yv) const;

  //! Converts tha value of the projection plane into
  //! a PIXEL value.
  Standard_EXPORT int Convert(const double Vv) const;

  //! Converts the point defined in the reference frame
  //! of the projection plane into a point PIXEL.
  Standard_EXPORT void Convert(const double Xv, const double Yv, int& Xp, int& Yp) const;

  //! Converts the projected point into a point
  //! in the reference frame of the view corresponding
  //! to the intersection with the projection plane
  //! of the eye/view point vector.
  Standard_EXPORT void Convert(const int Xp, const int Yp, double& X, double& Y, double& Z) const;

  //! Converts the projected point into a point
  //! in the reference frame of the view corresponding
  //! to the intersection with the projection plane
  //! of the eye/view point vector and returns the
  //! projection ray for further computations.
  Standard_EXPORT void ConvertWithProj(const int Xp,
                                       const int Yp,
                                       double&   X,
                                       double&   Y,
                                       double&   Z,
                                       double&   Vx,
                                       double&   Vy,
                                       double&   Vz) const;

  //! Converts the projected point into the nearest grid point
  //! in the reference frame of the view corresponding
  //! to the intersection with the projection plane
  //! of the eye/view point vector and display the grid marker.
  //! Warning: When the grid is not active the result is identical to the above Convert() method.
  //! How to use:
  //! 1) Enable the grid echo display
  //! myViewer->SetGridEcho(true);
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
  Standard_EXPORT void ConvertToGrid(const int Xp,
                                     const int Yp,
                                     double&   Xg,
                                     double&   Yg,
                                     double&   Zg) const;

  //! Converts the point into the nearest grid point
  //! and display the grid marker.
  Standard_EXPORT void ConvertToGrid(const double X,
                                     const double Y,
                                     const double Z,
                                     double&      Xg,
                                     double&      Yg,
                                     double&      Zg) const;

  //! Projects the point defined in the reference frame of
  //! the view into the projected point in the associated window.
  Standard_EXPORT void Convert(const double X,
                               const double Y,
                               const double Z,
                               int&         Xp,
                               int&         Yp) const;

  //! Converts the point defined in the user space of
  //! the view to the projection plane at the depth
  //! relative to theZ.
  Standard_EXPORT void Project(const double theX,
                               const double theY,
                               const double theZ,
                               double&      theXp,
                               double&      theYp) const;

  //! Converts the point defined in the user space of
  //! the view to the projection plane at the depth
  //! relative to theZ.
  Standard_EXPORT void Project(const double theX,
                               const double theY,
                               const double theZ,
                               double&      theXp,
                               double&      theYp,
                               double&      theZp) const;

  //! Returns the Background color values of the view
  //! depending of the color Type.
  Standard_EXPORT void BackgroundColor(const Quantity_TypeOfColor Type,
                                       double&                    V1,
                                       double&                    V2,
                                       double&                    V3) const;

  //! Returns the Background color object of the view.
  Standard_EXPORT Quantity_Color BackgroundColor() const;

  //! Returns the gradient background colors of the view.
  Standard_EXPORT void GradientBackgroundColors(Quantity_Color& theColor1,
                                                Quantity_Color& theColor2) const;

  //! Returns the gradient background of the view.
  Standard_EXPORT Aspect_GradientBackground GradientBackground() const;

  //! Returns the current value of the zoom expressed with
  //! respect to SetViewMappingDefault().
  Standard_EXPORT double Scale() const;

  //! Returns the current values of the anisotropic (axial) scale factors.
  Standard_EXPORT void AxialScale(double& Sx, double& Sy, double& Sz) const;

  //! Returns the height and width of the view.
  Standard_EXPORT void Size(double& Width, double& Height) const;

  //! Returns the Depth of the view .
  Standard_EXPORT double ZSize() const;

  //! Returns the position of the eye.
  Standard_EXPORT void Eye(double& X, double& Y, double& Z) const;

  //! Returns the position of point which emanating the projections.
  void FocalReferencePoint(double& X, double& Y, double& Z) const { Eye(X, Y, Z); }

  //! Returns the coordinate of the point (Xpix,Ypix)
  //! in the view (XP,YP,ZP), and the projection vector of the
  //! view passing by the point (for PerspectiveView).
  Standard_EXPORT void ProjReferenceAxe(const int Xpix,
                                        const int Ypix,
                                        double&   XP,
                                        double&   YP,
                                        double&   ZP,
                                        double&   VX,
                                        double&   VY,
                                        double&   VZ) const;

  //! Returns the Distance between the Eye and View Point.
  Standard_EXPORT double Depth() const;

  //! Returns the projection vector.
  Standard_EXPORT void Proj(double& Vx, double& Vy, double& Vz) const;

  //! Returns the position of the view point.
  Standard_EXPORT void At(double& X, double& Y, double& Z) const;

  //! Returns the vector giving the position of the high point.
  Standard_EXPORT void Up(double& Vx, double& Vy, double& Vz) const;

  //! Returns in RADIANS the orientation of the view around
  //! the visual axis measured from the Y axis of the screen.
  Standard_EXPORT double Twist() const;

  //! Returns the current shading model; Graphic3d_TypeOfShadingModel_Phong by default.
  Standard_EXPORT Graphic3d_TypeOfShadingModel ShadingModel() const;

  //! Defines the shading model for the visualization.
  Standard_EXPORT void SetShadingModel(const Graphic3d_TypeOfShadingModel theShadingModel);

  Standard_EXPORT occ::handle<Graphic3d_TextureEnv> TextureEnv() const;

  //! Returns the current visualisation mode.
  Standard_EXPORT V3d_TypeOfVisualization Visualization() const;

  //! Returns a list of active lights.
  const NCollection_List<occ::handle<Graphic3d_CLight>>& ActiveLights() const
  {
    return myActiveLights;
  }

  //! Return iterator for defined lights.
  NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator ActiveLightIterator() const
  {
    return NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator(myActiveLights);
  }

  //! Returns the MAX number of light associated to the view.
  Standard_EXPORT int LightLimit() const;

  //! Returns the viewer in which the view has been created.
  occ::handle<V3d_Viewer> Viewer() const { return MyViewer; }

  //! Returns True if MyView is associated with a window .
  Standard_EXPORT bool IfWindow() const;

  //! Returns the Aspect Window associated with the view.
  const occ::handle<Aspect_Window>& Window() const { return MyWindow; }

  //! Returns the Type of the View
  Standard_EXPORT V3d_TypeOfView Type() const;

  //! Translates the center of the view along "x" and "y" axes of
  //! view projection. Can be used to perform interactive panning operation.
  //! In that case the DXp, DXp parameters specify panning relative to the
  //! point where the operation is started.
  //! @param[in] theDXp  the relative panning on "x" axis of view projection, in pixels.
  //! @param[in] theDYp  the relative panning on "y" axis of view projection, in pixels.
  //! @param[in] theZoomFactor  the zooming factor.
  //! @param[in] theToStart  pass TRUE when starting panning to remember view
  //! state prior to panning for relative arguments. Passing 0 for relative
  //! panning parameter should return view panning to initial state.
  //! Performs update of view.
  Standard_EXPORT void Pan(const int    theDXp,
                           const int    theDYp,
                           const double theZoomFactor = 1,
                           const bool   theToStart    = true);

  //! Zoom the view according to a zoom factor computed
  //! from the distance between the 2 mouse position.
  //! @param[in] theXp1  the x coordinate of first mouse position, in pixels.
  //! @param[in] theYp1  the y coordinate of first mouse position, in pixels.
  //! @param[in] theXp2  the x coordinate of second mouse position, in pixels.
  //! @param[in] theYp2  the y coordinate of second mouse position, in pixels.
  Standard_EXPORT void Zoom(const int theXp1, const int theYp1, const int theXp2, const int theYp2);

  //! Defines starting point for ZoomAtPoint view operation.
  //! @param[in] theXp  the x mouse coordinate, in pixels.
  //! @param[in] theYp  the y mouse coordinate, in pixels.
  Standard_EXPORT void StartZoomAtPoint(const int theXp, const int theYp);

  //! Zooms the model at a pixel defined by the method StartZoomAtPoint().
  Standard_EXPORT void ZoomAtPoint(const int theMouseStartX,
                                   const int theMouseStartY,
                                   const int theMouseEndX,
                                   const int theMouseEndY);

  //! Performs anisotropic scaling of <me> view along the given <Axis>.
  //! The scale factor is calculated on a basis of
  //! the mouse pointer displacement <Dx,Dy>.
  //! The calculated scale factor is then passed to SetAxialScale(Sx, Sy, Sz) method.
  Standard_EXPORT void AxialScale(const int Dx, const int Dy, const V3d_TypeOfAxe Axis);

  //! Begin the rotation of the view around the screen axis
  //! according to the mouse position <X,Y>.
  //! Warning: Enable rotation around the Z screen axis when <zRotationThreshold>
  //! factor is > 0 soon the distance from the start point and the center
  //! of the view is > (medium viewSize * <zRotationThreshold> ).
  //! Generally a value of 0.4 is usable to rotate around XY screen axis
  //! inside the circular threshold area and to rotate around Z screen axis
  //! outside this area.
  Standard_EXPORT void StartRotation(const int    X,
                                     const int    Y,
                                     const double zRotationThreshold = 0.0);

  //! Continues the rotation of the view
  //! with an angle computed from the last and new mouse position <X,Y>.
  Standard_EXPORT void Rotation(const int X, const int Y);

  //! Change View Plane Distance for Perspective Views
  //! Warning! raises TypeMismatch from Standard if the view
  //! is not a perspective view.
  Standard_EXPORT void SetFocale(const double Focale);

  //! Returns the View Plane Distance for Perspective Views
  Standard_EXPORT double Focale() const;

  //! Returns the associated Graphic3d view.
  const occ::handle<Graphic3d_CView>& View() const { return myView; }

  //! Switches computed HLR mode in the view.
  Standard_EXPORT void SetComputedMode(const bool theMode);

  //! Returns the computed HLR mode state.
  Standard_EXPORT bool ComputedMode() const;

  //! idem than WindowFit
  void WindowFitAll(const int Xmin, const int Ymin, const int Xmax, const int Ymax)
  {
    WindowFit(Xmin, Ymin, Xmax, Ymax);
  }

  //! Transform camera eye, center and scale to fit in the passed bounding box specified in WCS.
  //! @param[in] theCamera  the camera
  //! @param[in] theBox     the bounding box
  //! @param[in] theMargin  the margin coefficient for view borders
  //! @param[in] theResolution  the minimum size of projection of bounding box in Xv or Yv direction
  //! when it considered to be a thin plane or point (without a volume);
  //!                           in this case only the center of camera is adjusted
  //! @param[in] theToEnlargeIfLine  when TRUE - in cases when the whole bounding box projected into
  //! thin line going along Z-axis of screen,
  //!                                the view plane is enlarged such thatwe see the whole line on
  //!                                rotation, otherwise only the center of camera is adjusted.
  //! @return TRUE if the fit all operation can be done
  Standard_EXPORT bool FitMinMax(const occ::handle<Graphic3d_Camera>& theCamera,
                                 const Bnd_Box&                       theBox,
                                 const double                         theMargin,
                                 const double                         theResolution = 0.0,
                                 const bool theToEnlargeIfLine                      = true) const;

  //! Defines or Updates the definition of the
  //! grid in <me>
  Standard_EXPORT void SetGrid(const gp_Ax3& aPlane, const occ::handle<Aspect_Grid>& aGrid);

  //! Defines or Updates the activity of the
  //! grid in <me>
  Standard_EXPORT void SetGridActivity(const bool aFlag);

  //! Dumps the full contents of the View into the image file. This is an alias for ToPixMap() with
  //! Image_AlienPixMap.
  //! @param theFile destination image file (image format is determined by file extension like .png,
  //! .bmp, .jpg)
  //! @param theBufferType buffer to dump
  //! @return FALSE when the dump has failed
  Standard_EXPORT bool Dump(const char*                 theFile,
                            const Graphic3d_BufferType& theBufferType = Graphic3d_BT_RGB);

  //! Dumps the full contents of the view to a pixmap with specified parameters.
  //! Internally this method calls Redraw() with an offscreen render buffer of requested target size
  //! (theWidth x theHeight), so that there is no need resizing a window control for making a dump
  //! of different size.
  Standard_EXPORT bool ToPixMap(Image_PixMap& theImage, const V3d_ImageDumpOptions& theParams);

  //! Dumps the full contents of the view to a pixmap.
  //! Internally this method calls Redraw() with an offscreen render buffer of requested target size
  //! (theWidth x theHeight), so that there is no need resizing a window control for making a dump
  //! of different size.
  //! @param theImage          target image, will be re-allocated to match theWidth x theHeight
  //! @param theWidth          target image width
  //! @param theHeight         target image height
  //! @param theBufferType     type of the view buffer to dump (color / depth)
  //! @param theToAdjustAspect when true, active view aspect ratio will be overridden by (theWidth /
  //! theHeight)
  //! @param theStereoOptions  how to dump stereographic camera
  bool ToPixMap(Image_PixMap&               theImage,
                const int                   theWidth,
                const int                   theHeight,
                const Graphic3d_BufferType& theBufferType     = Graphic3d_BT_RGB,
                const bool                  theToAdjustAspect = true,
                const Graphic3d_ZLayerId    theTargetZLayerId = Graphic3d_ZLayerId_BotOSD,
                const int                   theIsSingleLayer  = false,
                const V3d_StereoDumpOptions theStereoOptions  = V3d_SDO_MONO,
                const char*                 theLightName      = "")
  {
    V3d_ImageDumpOptions aParams;
    aParams.Width          = theWidth;
    aParams.Height         = theHeight;
    aParams.BufferType     = theBufferType;
    aParams.StereoOptions  = theStereoOptions;
    aParams.ToAdjustAspect = theToAdjustAspect;
    aParams.TargetZLayerId = theTargetZLayerId;
    aParams.IsSingleLayer  = theIsSingleLayer;
    aParams.LightName      = theLightName;
    return ToPixMap(theImage, aParams);
  }

  //! Manages display of the back faces
  Standard_EXPORT void SetBackFacingModel(
    const Graphic3d_TypeOfBackfacingModel theModel = Graphic3d_TypeOfBackfacingModel_Auto);

  //! Returns current state of the back faces display; Graphic3d_TypeOfBackfacingModel_Auto by
  //! default, which means that backface culling is defined by each presentation.
  Standard_EXPORT Graphic3d_TypeOfBackfacingModel BackFacingModel() const;

  //! Adds clip plane to the view. The composition of clip planes truncates the
  //! rendering space to convex volume. Number of supported clip planes can be consulted
  //! by PlaneLimit method of associated Graphic3d_GraphicDriver.
  //! Please be aware that the planes which exceed the limit are ignored during rendering.
  //! @param[in] thePlane  the clip plane to be added to view.
  Standard_EXPORT virtual void AddClipPlane(const occ::handle<Graphic3d_ClipPlane>& thePlane);

  //! Removes clip plane from the view.
  //! @param[in] thePlane  the clip plane to be removed from view.
  Standard_EXPORT virtual void RemoveClipPlane(const occ::handle<Graphic3d_ClipPlane>& thePlane);

  //! Get clip planes.
  //! @return sequence clip planes that have been set for the view
  Standard_EXPORT const occ::handle<Graphic3d_SequenceOfHClipPlane>& ClipPlanes() const;

  //! Sets sequence of clip planes to the view. The planes that have been set
  //! before are removed from the view. The composition of clip planes
  //! truncates the rendering space to convex volume. Number of supported
  //! clip planes can be consulted by InquirePlaneLimit method of
  //! Graphic3d_GraphicDriver. Please be aware that the planes that
  //! exceed the limit are ignored during rendering.
  //! @param[in] thePlanes  the clip planes to set.
  Standard_EXPORT void SetClipPlanes(const occ::handle<Graphic3d_SequenceOfHClipPlane>& thePlanes);

  //! Returns the MAX number of clipping planes associated to the view.
  Standard_EXPORT int PlaneLimit() const;

  //! Change camera used by view.
  Standard_EXPORT void SetCamera(const occ::handle<Graphic3d_Camera>& theCamera);

  //! Returns camera object of the view.
  //! @return: handle to camera object, or NULL if 3D view does not use
  //! the camera approach.
  Standard_EXPORT const occ::handle<Graphic3d_Camera>& Camera() const;

  //! Return default camera.
  const occ::handle<Graphic3d_Camera>& DefaultCamera() const { return myDefaultCamera; }

  //! Returns current rendering parameters and effect settings.
  //! By default it returns default parameters of current viewer.
  //! To define view-specific settings use method V3d_View::ChangeRenderingParams().
  //! @sa V3d_Viewer::DefaultRenderingParams()
  Standard_EXPORT const Graphic3d_RenderingParams& RenderingParams() const;

  //! Returns reference to current rendering parameters and effect settings.
  Standard_EXPORT Graphic3d_RenderingParams& ChangeRenderingParams();

  //! @return flag value of objects culling mechanism
  bool IsCullingEnabled() const
  {
    return RenderingParams().FrustumCullingState == Graphic3d_RenderingParams::FrustumCulling_On;
  }

  //! Turn on/off automatic culling of objects outside frustum (ON by default)
  void SetFrustumCulling(bool theMode)
  {
    ChangeRenderingParams().FrustumCullingState = theMode
                                                    ? Graphic3d_RenderingParams::FrustumCulling_On
                                                    : Graphic3d_RenderingParams::FrustumCulling_Off;
  }

  //! Fill in the dictionary with diagnostic info.
  //! Should be called within rendering thread.
  //!
  //! This API should be used only for user output or for creating automated reports.
  //! The format of returned information (e.g. key-value layout)
  //! is NOT part of this API and can be changed at any time.
  //! Thus application should not parse returned information to weed out specific parameters.
  //! @param theDict  destination map for information
  //! @param theFlags defines the information to be retrieved
  Standard_EXPORT void DiagnosticInformation(
    NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theDict,
    Graphic3d_DiagnosticInfo                                                      theFlags) const;

  //! Returns string with statistic performance info.
  Standard_EXPORT TCollection_AsciiString StatisticInformation() const;

  //! Fills in the dictionary with statistic performance info.
  Standard_EXPORT void StatisticInformation(
    NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theDict) const;

  //! Returns the Objects number and the gravity center of ALL viewable points in the view
  Standard_EXPORT gp_Pnt GravityPoint() const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

public: //! @name subvew management
  //! Return TRUE if this is a subview of another view.
  bool IsSubview() const { return myParentView != nullptr; }

  //! Return parent View or NULL if this is not a subview.
  V3d_View* ParentView() { return myParentView; }

  //! Return subview list.
  const NCollection_Sequence<occ::handle<V3d_View>>& Subviews() const { return mySubviews; }

  //! Pick subview from the given 2D point.
  Standard_EXPORT occ::handle<V3d_View> PickSubview(const NCollection_Vec2<int>& thePnt) const;

  //! Add subview to the list.
  Standard_EXPORT void AddSubview(const occ::handle<V3d_View>& theView);

  //! Remove subview from the list.
  Standard_EXPORT bool RemoveSubview(const V3d_View* theView);

public: //! @name deprecated methods
  //! Returns True if One light more can be
  //! activated in this View.
  Standard_DEPRECATED("Deprecated method - ActiveLights() should be used instead")
  Standard_EXPORT bool IfMoreLights() const;

  //! initializes an iteration on the active Lights.
  Standard_DEPRECATED("Deprecated method - ActiveLights() should be used instead")
  void InitActiveLights() { myActiveLightsIterator.Initialize(myActiveLights); }

  //! returns true if there are more active Light(s) to return.
  Standard_DEPRECATED("Deprecated method - ActiveLights() should be used instead")
  bool MoreActiveLights() const { return myActiveLightsIterator.More(); }

  //! Go to the next active Light (if there is not, ActiveLight will raise an exception)
  Standard_DEPRECATED("Deprecated method - ActiveLights() should be used instead")
  void NextActiveLights() { myActiveLightsIterator.Next(); }

  Standard_DEPRECATED("Deprecated method - ActiveLights() should be used instead")
  const occ::handle<V3d_Light>& ActiveLight() const { return myActiveLightsIterator.Value(); }

protected:
  Standard_EXPORT void ImmediateUpdate() const;

  //! Scales camera to fit the view frame of defined width and height
  //! keeping the aspect. For orthogonal camera the method changes scale,
  //! for perspective adjusts Eye location about the Center point.
  //! @param[in] theSizeXv  size of viewport frame on "x" axis.
  //! @param[in] theSizeYv  size of viewport frame on "y" axis.
  Standard_EXPORT void Scale(const occ::handle<Graphic3d_Camera>& theCamera,
                             const double                         theSizeXv,
                             const double                         theSizeYv) const;

  Standard_EXPORT void Translate(const occ::handle<Graphic3d_Camera>& theCamera,
                                 const double                         theDXv,
                                 const double                         theDYv) const;

private:
  //! Modifies the aspect ratio of the camera when
  //! the associated window is defined or resized.
  Standard_EXPORT void SetRatio();

  //! Determines the screen axes in the reference
  //! framework of the view.
  Standard_EXPORT static bool screenAxis(const gp_Dir& theVpn,
                                         const gp_Dir& theVup,
                                         gp_Vec&       theXaxe,
                                         gp_Vec&       theYaxe,
                                         gp_Vec&       theZaxe);

  //! Transforms the Vertex V according to the matrice Matrix .
  Standard_EXPORT static gp_XYZ TrsPoint(const Graphic3d_Vertex&           V,
                                         const NCollection_Array2<double>& Matrix);

  //! Returns the objects number and the projection window
  //! of the objects contained in the view.
  Standard_EXPORT int MinMax(double& Umin, double& Vmin, double& Umax, double& Vmax) const;

  //! Returns the objects number and the box encompassing
  //! the objects contained in the view
  Standard_EXPORT int MinMax(double& Xmin,
                             double& Ymin,
                             double& Zmin,
                             double& Xmax,
                             double& Ymax,
                             double& Zmax) const;

  Standard_EXPORT void Init();

  //! Returns a new vertex when the grid is activated.
  Standard_EXPORT Graphic3d_Vertex Compute(const Graphic3d_Vertex& AVertex) const;

protected:
  double                        myOldMouseX;
  double                        myOldMouseY;
  gp_Dir                        myCamStartOpUp;
  gp_Dir                        myCamStartOpDir;
  gp_Pnt                        myCamStartOpEye;
  gp_Pnt                        myCamStartOpCenter;
  occ::handle<Graphic3d_Camera> myDefaultCamera;
  occ::handle<Graphic3d_CView>  myView;
  bool                          myImmediateUpdate;
  mutable bool                  myIsInvalidatedImmediate;

private:
  V3d_Viewer* MyViewer;

  NCollection_Sequence<occ::handle<V3d_View>> mySubviews;
  V3d_View*                                   myParentView;

  NCollection_List<occ::handle<Graphic3d_CLight>>           myActiveLights;
  gp_Dir                                                    myDefaultViewAxis;
  gp_Pnt                                                    myDefaultViewPoint;
  occ::handle<Aspect_Window>                                MyWindow;
  NCollection_List<occ::handle<Graphic3d_CLight>>::Iterator myActiveLightsIterator;
  int                                                       sx;
  int                                                       sy;
  double                                                    rx;
  double                                                    ry;
  gp_Pnt                                                    myRotateGravity;
  bool                                                      myComputedMode;
  bool                                                      SwitchSetFront;
  bool                                                      myZRotation;
  int                                                       MyZoomAtPointX;
  int                                                       MyZoomAtPointY;
  occ::handle<V3d_Trihedron>                                myTrihedron;
  occ::handle<Aspect_Grid>                                  MyGrid;
  gp_Ax3                                                    MyPlane;
  NCollection_Array2<double>                                MyTrsf;
  occ::handle<Graphic3d_Structure>                          MyGridEchoStructure;
  occ::handle<Graphic3d_Group>                              MyGridEchoGroup;
  gp_Vec                                                    myXscreenAxis;
  gp_Vec                                                    myYscreenAxis;
  gp_Vec                                                    myZscreenAxis;
  gp_Dir                                                    myViewAxis;
  Graphic3d_Vertex                                          myGravityReferencePoint;
  bool                                                      myAutoZFitIsOn;
  double                                                    myAutoZFitScaleFactor;
};

#endif // _V3d_View_HeaderFile
