// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _Graphic3d_CView_HeaderFile
#define _Graphic3d_CView_HeaderFile

#include <Aspect_RenderingContext.hxx>
#include <Aspect_SkydomeBackground.hxx>
#include <Aspect_Window.hxx>
#include <Graphic3d_BufferType.hxx>
#include <Graphic3d_CubeMap.hxx>
#include <Graphic3d_DataStructureManager.hxx>
#include <Graphic3d_DiagnosticInfo.hxx>
#include <Graphic3d_GraduatedTrihedron.hxx>
#include <Graphic3d_NMapOfTransient.hxx>
#include <Graphic3d_RenderingParams.hxx>
#include <Graphic3d_SequenceOfStructure.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_TypeOfAnswer.hxx>
#include <Graphic3d_TypeOfBackfacingModel.hxx>
#include <Graphic3d_TypeOfBackground.hxx>
#include <Graphic3d_TypeOfShadingModel.hxx>
#include <Graphic3d_TypeOfVisualization.hxx>
#include <Graphic3d_Vec3.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <Image_PixMap.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_IndexedDataMapOfStringString.hxx>

class Aspect_NeutralWindow;
class Aspect_XRSession;
class Graphic3d_CView;
class Graphic3d_Layer;
class Graphic3d_StructureManager;

DEFINE_STANDARD_HANDLE (Graphic3d_CView, Graphic3d_DataStructureManager)

//! Base class of a graphical view that carries out rendering process for a concrete
//! implementation of graphical driver. Provides virtual interfaces for redrawing its
//! contents, management of displayed structures and render settings. The source code 
//! of the class itself implements functionality related to management of
//! computed (HLR or "view-dependent") structures.
class Graphic3d_CView : public Graphic3d_DataStructureManager
{
  friend class Graphic3d_StructureManager;
  DEFINE_STANDARD_RTTIEXT(Graphic3d_CView, Graphic3d_DataStructureManager)
public:

  //! Constructor.
  Standard_EXPORT Graphic3d_CView (const Handle(Graphic3d_StructureManager)& theMgr);

  //! Destructor.
  Standard_EXPORT virtual ~Graphic3d_CView();

  //! Returns the identification number of the view.
  Standard_Integer Identification() const { return myId; }

  //! Activates the view. Maps presentations defined within structure manager onto this view.
  Standard_EXPORT virtual void Activate();

  //! Deactivates the view. Unmaps presentations defined within structure manager.
  //! The view in deactivated state will ignore actions on structures such as Display().
  Standard_EXPORT virtual void Deactivate();

  //! Returns the activity flag of the view.
  Standard_Boolean IsActive() const { return myIsActive; }

  //! Erases the view and removes from graphic driver.
  //! No more graphic operations are allowed in this view after the call.
  Standard_EXPORT virtual void Remove();

  //! Returns true if the view was removed.
  Standard_Boolean IsRemoved() const { return myIsRemoved; }

  //! Returns camera object of the view.
  virtual const Handle(Graphic3d_Camera)& Camera() const Standard_OVERRIDE { return myCamera; }

  //! Sets camera used by the view.
  virtual void SetCamera (const Handle(Graphic3d_Camera)& theCamera) { myCamera = theCamera; }

public:

  //! Returns default Shading Model of the view; Graphic3d_TypeOfShadingModel_Phong by default.
  Graphic3d_TypeOfShadingModel ShadingModel() const { return myRenderParams.ShadingModel; }

  //! Sets default Shading Model of the view.
  //! Will throw an exception on attempt to set Graphic3d_TypeOfShadingModel_DEFAULT.
  Standard_EXPORT void SetShadingModel (Graphic3d_TypeOfShadingModel theModel);

  //! Return backfacing model used for the view; Graphic3d_TypeOfBackfacingModel_Auto by default,
  //! which means that backface culling is defined by each presentation.
  Graphic3d_TypeOfBackfacingModel BackfacingModel() const { return myBackfacing; }

  //! Sets backfacing model for the view.
  void SetBackfacingModel (const Graphic3d_TypeOfBackfacingModel theModel) { myBackfacing = theModel; }

  //! Returns visualization type of the view.
  Graphic3d_TypeOfVisualization VisualizationType() const { return myVisualization; }

  //! Sets visualization type of the view.
  void SetVisualizationType (const Graphic3d_TypeOfVisualization theType) { myVisualization = theType; }

  //! Switches computed HLR mode in the view
  Standard_EXPORT void SetComputedMode (const Standard_Boolean theMode);

  //! Returns the computed HLR mode state
  Standard_Boolean ComputedMode() const { return myIsInComputedMode; }

  //! Computes the new presentation of the structure  displayed in this view with the type Graphic3d_TOS_COMPUTED.
  Standard_EXPORT void ReCompute (const Handle(Graphic3d_Structure)& theStructure);

  //! Invalidates bounding box of specified ZLayerId.
  Standard_EXPORT void Update (const Graphic3d_ZLayerId theLayerId = Graphic3d_ZLayerId_UNKNOWN);

  //! Computes the new presentation of the structures displayed in this view with the type Graphic3d_TOS_COMPUTED.
  Standard_EXPORT void Compute();

  //! Returns the set of structures displayed in this view.
  Standard_EXPORT void DisplayedStructures (Graphic3d_MapOfStructure& theStructures) const;

  //! Returns number of displayed structures in the view.
  virtual Standard_Integer NumberOfDisplayedStructures() const { return myStructsDisplayed.Extent(); }

  //! Returns Standard_True in case if the structure with the given <theStructId> is
  //! in list of structures to be computed and stores computed struct to <theComputedStruct>.
  Standard_EXPORT Standard_Boolean IsComputed (const Standard_Integer theStructId,
                                               Handle(Graphic3d_Structure)& theComputedStruct) const;

  //! Returns the bounding box of all structures displayed in the view.
  //! If theToIncludeAuxiliary is TRUE, then the boundary box also includes minimum and maximum limits
  //! of graphical elements forming parts of infinite and other auxiliary structures.
  //! @param theToIncludeAuxiliary consider also auxiliary presentations (with infinite flag or with trihedron transformation persistence)
  //! @return computed bounding box
  Standard_EXPORT virtual Bnd_Box MinMaxValues (const Standard_Boolean theToIncludeAuxiliary = Standard_False) const;

  //! Returns the coordinates of the boundary box of all structures in the set <theSet>.
  //! If <theToIgnoreInfiniteFlag> is TRUE, then the boundary box
  //! also includes minimum and maximum limits of graphical elements
  //! forming parts of infinite structures.
  Standard_EXPORT Bnd_Box MinMaxValues (const Graphic3d_MapOfStructure& theSet,
                                        const Standard_Boolean theToIncludeAuxiliary = Standard_False) const;

  //! Returns the structure manager handle which manage structures associated with this view.
  const Handle(Graphic3d_StructureManager)& StructureManager() const { return myStructureManager; }

private:

  //! Is it possible to display the structure in the view?
  Standard_EXPORT Graphic3d_TypeOfAnswer acceptDisplay (const Graphic3d_TypeOfStructure theStructType) const;

  //! Clears the structure in this view.
  Standard_EXPORT void Clear (Graphic3d_Structure* theStructure,
                              const Standard_Boolean theWithDestruction);

  //! Connects the structures.
  Standard_EXPORT void Connect (const Graphic3d_Structure* theMother,
                                const Graphic3d_Structure* theDaughter);

  //! Disconnects the structures.
  Standard_EXPORT void Disconnect (const Graphic3d_Structure* theMother,
                                   const Graphic3d_Structure* theDaughter);

  //! Displays the structure in the view.
  Standard_EXPORT void Display (const Handle(Graphic3d_Structure)& theStructure);

  //! Erases the structure from the view.
  Standard_EXPORT void Erase (const Handle(Graphic3d_Structure)& theStructure);

  //! Highlights the structure in the view.
  Standard_EXPORT void Highlight (const Handle(Graphic3d_Structure)& theStructure);

  //! Transforms the structure in the view.
  Standard_EXPORT void SetTransform (const Handle(Graphic3d_Structure)& theStructure,
                                     const Handle(TopLoc_Datum3D)& theTrsf);

  //! Suppress the highlighting on the structure <AStructure>
  //! in the view <me>.
  Standard_EXPORT void UnHighlight (const Handle(Graphic3d_Structure)& theStructure);

  //! Returns an index != 0 if the structure have another structure computed for the view <me>.
  Standard_EXPORT Standard_Integer IsComputed (const Graphic3d_Structure* theStructure) const;

  Standard_Integer IsComputed (const Handle(Graphic3d_Structure)& theStructure) const { return IsComputed (theStructure.get()); }

  //! Returns true if the structure is displayed in the view.
  Standard_EXPORT Standard_Boolean IsDisplayed (const Handle(Graphic3d_Structure)& theStructure) const;

  //! Changes the display priority of the structure.
  Standard_EXPORT void ChangePriority (const Handle(Graphic3d_Structure)& theStructure,
                                       const Graphic3d_DisplayPriority theOldPriority,
                                       const Graphic3d_DisplayPriority theNewPriority);

  //! Change Z layer of already displayed structure in the view.
  Standard_EXPORT void ChangeZLayer (const Handle(Graphic3d_Structure)& theStructure,
                                     const Graphic3d_ZLayerId theLayerId);

  //! Returns an index != 0 if the structure have the same owner than another structure
  //! in the sequence of the computed structures.
  Standard_EXPORT Standard_Integer HaveTheSameOwner (const Handle(Graphic3d_Structure)& theStructure) const;

public:

  //! Redraw content of the view.
  virtual void Redraw() = 0;

  //! Redraw immediate content of the view.
  virtual void RedrawImmediate() = 0;

  //! Invalidates content of the view but does not redraw it.
  virtual void Invalidate() = 0;

  //! Return true if view content cache has been invalidated.
  virtual Standard_Boolean IsInvalidated() = 0;

  //! Handle changing size of the rendering window.
  Standard_EXPORT virtual void Resized() = 0;

  //! @param theDrawToFrontBuffer Advanced option to modify rendering mode:
  //! 1. TRUE.  Drawing immediate mode structures directly to the front buffer over the scene image.
  //! Fast, so preferred for interactive work (used by default).
  //! However these extra drawings will be missed in image dump since it is performed from back buffer.
  //! Notice that since no pre-buffering used the V-Sync will be ignored and rendering could be seen
  //! in run-time (in case of slow hardware) and/or tearing may appear.
  //! So this is strongly recommended to draw only simple (fast) structures.
  //! 2. FALSE. Drawing immediate mode structures to the back buffer.
  //! The complete scene is redrawn first, so this mode is slower if scene contains complex data and/or V-Sync
  //! is turned on. But it works in any case and is especially useful for view dump because the dump image is read
  //! from the back buffer.
  //! @return previous mode.
  virtual Standard_Boolean SetImmediateModeDrawToFront (const Standard_Boolean theDrawToFrontBuffer) = 0;

  //! Creates and maps rendering window to the view.
  //! @param[in] theParentVIew parent view or NULL
  //! @param[in] theWindow the window
  //! @param[in] theContext the rendering context; if NULL the context will be created internally
  virtual void SetWindow (const Handle(Graphic3d_CView)& theParentVIew,
                          const Handle(Aspect_Window)& theWindow,
                          const Aspect_RenderingContext theContext) = 0;

  //! Returns the window associated to the view.
  virtual Handle(Aspect_Window) Window() const = 0;

  //! Returns True if the window associated to the view is defined.
  virtual Standard_Boolean IsDefined() const = 0;

  //! Dump active rendering buffer into specified memory buffer.
  virtual Standard_Boolean BufferDump (Image_PixMap& theImage, const Graphic3d_BufferType& theBufferType) = 0;

  //! Marks BVH tree and the set of BVH primitives of correspondent priority list with id theLayerId as outdated.
  virtual void InvalidateBVHData (const Graphic3d_ZLayerId theLayerId) = 0;

  //! Add a layer to the view.
  //! @param theNewLayerId [in] id of new layer, should be > 0 (negative values are reserved for default layers).
  //! @param theSettings   [in] new layer settings
  //! @param theLayerAfter [in] id of layer to append new layer before
  virtual void InsertLayerBefore (const Graphic3d_ZLayerId theNewLayerId,
                                  const Graphic3d_ZLayerSettings& theSettings,
                                  const Graphic3d_ZLayerId theLayerAfter) = 0;

  //! Add a layer to the view.
  //! @param theNewLayerId  [in] id of new layer, should be > 0 (negative values are reserved for default layers).
  //! @param theSettings    [in] new layer settings
  //! @param theLayerBefore [in] id of layer to append new layer after
  virtual void InsertLayerAfter (const Graphic3d_ZLayerId theNewLayerId,
                                 const Graphic3d_ZLayerSettings& theSettings,
                                 const Graphic3d_ZLayerId theLayerBefore) = 0;

  //! Returns the maximum Z layer ID.
  //! First layer ID is Graphic3d_ZLayerId_Default, last ID is ZLayerMax().
  virtual Standard_Integer ZLayerMax() const = 0;

  //! Returns the list of layers.
  virtual const NCollection_List<Handle(Graphic3d_Layer)>& Layers() const = 0;

  //! Returns layer with given ID or NULL if undefined.
  virtual Handle(Graphic3d_Layer) Layer (const Graphic3d_ZLayerId theLayerId) const = 0;

  //! Returns the bounding box of all structures displayed in the Z layer.
  Standard_EXPORT virtual void InvalidateZLayerBoundingBox (const Graphic3d_ZLayerId theLayerId);

  //! Remove Z layer from the specified view. All structures
  //! displayed at the moment in layer will be displayed in default layer
  //! ( the bottom-level z layer ). To unset layer ID from associated
  //! structures use method UnsetZLayer (...).
  virtual void RemoveZLayer (const Graphic3d_ZLayerId theLayerId) = 0;

  //! Sets the settings for a single Z layer of specified view.
  virtual void SetZLayerSettings (const Graphic3d_ZLayerId theLayerId,
                                  const Graphic3d_ZLayerSettings& theSettings) = 0;

  //! Returns zoom-scale factor.
  Standard_EXPORT Standard_Real ConsiderZoomPersistenceObjects();

  //! Returns pointer to an assigned framebuffer object.
  virtual Handle(Standard_Transient) FBO() const = 0;

  //! Sets framebuffer object for offscreen rendering.
  virtual void SetFBO (const Handle(Standard_Transient)& theFbo) = 0;

  //! Generate offscreen FBO in the graphic library.
  //! If not supported on hardware returns NULL.
  virtual Handle(Standard_Transient) FBOCreate (const Standard_Integer theWidth,
                                                const Standard_Integer theHeight) = 0;

  //! Remove offscreen FBO from the graphic library
  virtual void FBORelease (Handle(Standard_Transient)& theFbo) = 0;

  //! Read offscreen FBO configuration.
  virtual void FBOGetDimensions (const Handle(Standard_Transient)& theFbo,
                                 Standard_Integer& theWidth,
                                 Standard_Integer& theHeight,
                                 Standard_Integer& theWidthMax,
                                 Standard_Integer& theHeightMax) = 0;

  //! Change offscreen FBO viewport.
  virtual void FBOChangeViewport (const Handle(Standard_Transient)& theFbo,
                                  const Standard_Integer theWidth,
                                  const Standard_Integer theHeight) = 0;

public:

  //! Copy visualization settings from another view.
  //! Method is used for cloning views in viewer when its required to create view
  //! with same view properties.
  Standard_EXPORT virtual void CopySettings (const Handle(Graphic3d_CView)& theOther);

  //! Returns current rendering parameters and effect settings.
  const Graphic3d_RenderingParams& RenderingParams() const { return myRenderParams; }

  //! Returns reference to current rendering parameters and effect settings.
  Graphic3d_RenderingParams& ChangeRenderingParams() { return myRenderParams; }

public:

  //! Returns background  fill color.
  virtual Aspect_Background Background() const { return Aspect_Background (myBgColor.GetRGB()); }

  //! Sets background fill color.
  virtual void SetBackground (const Aspect_Background& theBackground) { myBgColor.SetRGB (theBackground.Color()); }

  //! Returns gradient background fill colors.
  virtual Aspect_GradientBackground GradientBackground() const = 0;

  //! Sets gradient background fill colors.
  virtual void SetGradientBackground (const Aspect_GradientBackground& theBackground) = 0;

  //! Returns background image texture map.
  const Handle(Graphic3d_TextureMap)& BackgroundImage() { return myBackgroundImage; }

  //! Returns cubemap being set last time on background.
  const Handle(Graphic3d_CubeMap)& BackgroundCubeMap() const { return myCubeMapBackground; }

  //! Returns cubemap being set last time on background.
  const Handle(Graphic3d_CubeMap)& IBLCubeMap() const { return myCubeMapIBL; }

  //! Sets image texture or environment cubemap as background.
  //! @param theTextureMap [in] source to set a background;
  //!                           should be either Graphic3d_Texture2D or Graphic3d_CubeMap
  //! @param theToUpdatePBREnv [in] defines whether IBL maps will be generated or not
  //!                               (see GeneratePBREnvironment())
  virtual void SetBackgroundImage (const Handle(Graphic3d_TextureMap)& theTextureMap,
                                   Standard_Boolean theToUpdatePBREnv = Standard_True) = 0;

  //! Returns background image fill style.
  virtual Aspect_FillMethod BackgroundImageStyle() const = 0;

  //! Sets background image fill style.
  virtual void SetBackgroundImageStyle (const Aspect_FillMethod theFillStyle) = 0;

  //! Returns background type.
  Graphic3d_TypeOfBackground BackgroundType() const { return myBackgroundType; }

  //! Sets background type.
  void SetBackgroundType (Graphic3d_TypeOfBackground theType) { myBackgroundType = theType; }

  //! Returns skydome aspect;
  const Aspect_SkydomeBackground& BackgroundSkydome() const { return mySkydomeAspect; }

  //! Sets skydome aspect
  Standard_EXPORT void SetBackgroundSkydome (const Aspect_SkydomeBackground& theAspect,
                                             Standard_Boolean theToUpdatePBREnv = Standard_True);

  //! Enables or disables IBL (Image Based Lighting) from background cubemap.
  //! Has no effect if PBR is not used.
  //! @param[in] theToEnableIBL enable or disable IBL from background cubemap
  virtual void SetImageBasedLighting (Standard_Boolean theToEnableIBL) = 0;

  //! Returns environment texture set for the view.
  const Handle(Graphic3d_TextureEnv)& TextureEnv() const { return myTextureEnvData; }

  //! Sets environment texture for the view.
  virtual void SetTextureEnv (const Handle(Graphic3d_TextureEnv)& theTextureEnv) = 0;

public:

  //! Returns list of lights of the view.
  virtual const Handle(Graphic3d_LightSet)& Lights() const = 0;

  //! Sets list of lights for the view.
  virtual void SetLights (const Handle(Graphic3d_LightSet)& theLights) = 0;

  //! Returns list of clip planes set for the view.
  virtual const Handle(Graphic3d_SequenceOfHClipPlane)& ClipPlanes() const = 0;

  //! Sets list of clip planes for the view.
  virtual void SetClipPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes) = 0;

  //! Fill in the dictionary with diagnostic info.
  //! Should be called within rendering thread.
  //!
  //! This API should be used only for user output or for creating automated reports.
  //! The format of returned information (e.g. key-value layout)
  //! is NOT part of this API and can be changed at any time.
  //! Thus application should not parse returned information to weed out specific parameters.
  Standard_EXPORT virtual void DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                                      Graphic3d_DiagnosticInfo theFlags) const = 0;

  //! Returns string with statistic performance info.
  virtual TCollection_AsciiString StatisticInformation() const = 0;

  //! Fills in the dictionary with statistic performance info.
  virtual void StatisticInformation (TColStd_IndexedDataMapOfStringString& theDict) const = 0;

public:

  //! Return unit scale factor defined as scale factor for m (meters); 1.0 by default.
  //! Normally, view definition is unitless, however some operations like VR input requires proper units mapping.
  Standard_Real UnitFactor() const { return myUnitFactor; }

  //! Set unit scale factor.
  Standard_EXPORT void SetUnitFactor (Standard_Real theFactor);

  //! Return XR session.
  const Handle(Aspect_XRSession)& XRSession() const { return myXRSession; }

  //! Set XR session.
  void SetXRSession (const Handle(Aspect_XRSession)& theSession) { myXRSession = theSession; }

  //! Return TRUE if there is active XR session.
  Standard_EXPORT bool IsActiveXR() const;

  //! Initialize XR session.
  Standard_EXPORT virtual bool InitXR();

  //! Release XR session.
  Standard_EXPORT virtual void ReleaseXR();

  //! Process input.
  Standard_EXPORT virtual void ProcessXRInput();

  //! Compute PosedXRCamera() based on current XR head pose and make it active.
  Standard_EXPORT void SetupXRPosedCamera();

  //! Set current camera back to BaseXRCamera() and copy temporary modifications of PosedXRCamera().
  //! Calls SynchronizeXRPosedToBaseCamera() beforehand.
  Standard_EXPORT void UnsetXRPosedCamera();

  //! Returns transient XR camera position with tracked head orientation applied.
  const Handle(Graphic3d_Camera)& PosedXRCamera() const { return myPosedXRCamera; }

  //! Sets transient XR camera position with tracked head orientation applied.
  void SetPosedXRCamera (const Handle(Graphic3d_Camera)& theCamera) { myPosedXRCamera = theCamera; }

  //! Returns anchor camera definition (without tracked head orientation).
  const Handle(Graphic3d_Camera)& BaseXRCamera() const { return myBaseXRCamera; }

  //! Sets anchor camera definition.
  void SetBaseXRCamera (const Handle(Graphic3d_Camera)& theCamera) { myBaseXRCamera = theCamera; }

  //! Convert XR pose to world space.
  //! @param thePoseXR [in] transformation defined in VR local coordinate system,
  //!                       oriented as Y-up, X-right and -Z-forward
  //! @return transformation defining orientation of XR pose in world space
  gp_Trsf PoseXRToWorld (const gp_Trsf& thePoseXR) const
  {
    const Handle(Graphic3d_Camera)& anOrigin = myBaseXRCamera;
    const gp_Ax3 anAxVr    (gp::Origin(),  gp::DZ(), gp::DX());
    const gp_Ax3 aCameraCS (anOrigin->Eye().XYZ(), -anOrigin->Direction(), -anOrigin->SideRight());
    gp_Trsf aTrsfCS;
    aTrsfCS.SetTransformation (aCameraCS, anAxVr);
    return aTrsfCS * thePoseXR;
  }

  //! Returns view direction in the world space based on XR pose.
  //! @param thePoseXR [in] transformation defined in VR local coordinate system,
  //!                       oriented as Y-up, X-right and -Z-forward
  gp_Ax1 ViewAxisInWorld (const gp_Trsf& thePoseXR) const
  {
    return gp_Ax1 (gp::Origin(), -gp::DZ()).Transformed (PoseXRToWorld (thePoseXR));
  }

  //! Recomputes PosedXRCamera() based on BaseXRCamera() and head orientation.
  Standard_EXPORT void SynchronizeXRBaseToPosedCamera();

  //! Checks if PosedXRCamera() has been modified since SetupXRPosedCamera()
  //! and copies these modifications to BaseXRCamera().
  Standard_EXPORT void SynchronizeXRPosedToBaseCamera();

  //! Compute camera position based on XR pose.
  Standard_EXPORT void ComputeXRPosedCameraFromBase (Graphic3d_Camera& theCam,
                                                     const gp_Trsf& theXRTrsf) const;

  //! Update based camera from posed camera by applying reversed transformation.
  Standard_EXPORT void ComputeXRBaseCameraFromPosed (const Graphic3d_Camera& theCamPosed,
                                                     const gp_Trsf& thePoseTrsf);

  //! Turn XR camera direction using current (head) eye position as anchor.
  Standard_EXPORT void TurnViewXRCamera (const gp_Trsf& theTrsfTurn);

public: //! @name obsolete Graduated Trihedron functionality

  //! Returns data of a graduated trihedron
  virtual const Graphic3d_GraduatedTrihedron& GetGraduatedTrihedron() { return myGTrihedronData; }

  //! Displays Graduated Trihedron.
  virtual void GraduatedTrihedronDisplay (const Graphic3d_GraduatedTrihedron& theTrihedronData) { (void )theTrihedronData; }

  //! Erases Graduated Trihedron.
  virtual void GraduatedTrihedronErase() {}

  //! Sets minimum and maximum points of scene bounding box for Graduated Trihedron stored in graphic view object.
  //! @param theMin [in] the minimum point of scene.
  //! @param theMax [in] the maximum point of scene.
  virtual void GraduatedTrihedronMinMaxValues (const Graphic3d_Vec3 theMin, const Graphic3d_Vec3 theMax)
  {
    (void )theMin;
    (void )theMax;
  }
  
  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

public: //! @name subview properties

  //! Return TRUE if this is a subview of another view.
  bool IsSubview() const { return myParentView != nullptr; }

  //! Return parent View or NULL if this is not a subview.
  Graphic3d_CView* ParentView() { return myParentView; }

  //! Return TRUE if this is view performs rendering of subviews and nothing else; FALSE by default.
  //! By default, view with subviews will render main scene and blit subviews on top of it.
  //! Rendering of main scene might become redundant in case if subviews cover entire window of parent view.
  //! This flag allows to disable rendering of the main scene in such scenarios
  //! without creation of a dedicated V3d_Viewer instance just for composing subviews.
  bool IsSubviewComposer() const { return myIsSubviewComposer; }

  //! Set if this view should perform composing of subviews and nothing else.
  void SetSubviewComposer (bool theIsComposer) { myIsSubviewComposer = theIsComposer; }

  //! Return subview list.
  const NCollection_Sequence<Handle(Graphic3d_CView)>& Subviews() const { return mySubviews; }

  //! Add subview to the list.
  Standard_EXPORT void AddSubview (const Handle(Graphic3d_CView)& theView);

  //! Remove subview from the list.
  Standard_EXPORT bool RemoveSubview (const Graphic3d_CView* theView);

  //! Return subview position within parent view; Aspect_TOTP_LEFT_UPPER by default.
  Aspect_TypeOfTriedronPosition SubviewCorner() const { return mySubviewCorner; }

  //! Set subview position within parent view.
  void SetSubviewCorner (Aspect_TypeOfTriedronPosition thePos) { mySubviewCorner = thePos; }

  //! Return subview top-left position relative to parent view in pixels.
  const Graphic3d_Vec2i& SubviewTopLeft() const { return mySubviewTopLeft; }

  //! Return TRUE if subview size is set as proportions relative to parent view.
  bool IsSubViewRelativeSize() const { return mySubviewSize.x() <= 1.0 && mySubviewSize.y() <= 1.0; }

  //! Return subview dimensions; (1.0, 1.0) by default.
  //! Values >= 2   define size in pixels;
  //! Values <= 1.0 define size as fraction of parent view.
  const Graphic3d_Vec2d& SubviewSize() const { return mySubviewSize; }

  //! Set subview size relative to parent view.
  void SetSubviewSize (const Graphic3d_Vec2d& theSize) { mySubviewSize = theSize; }

  //! Return corner offset within parent view; (0.0,0.0) by default.
  //! Values >= 2   define offset in pixels;
  //! Values <= 1.0 define offset as fraction of parent view dimensions.
  const Graphic3d_Vec2d& SubviewOffset() const { return mySubviewOffset; }

  //! Set corner offset within parent view.
  void SetSubviewOffset (const Graphic3d_Vec2d& theOffset) { mySubviewOffset = theOffset; }

  //! Return subview margins in pixels; (0,0) by default
  const Graphic3d_Vec2i& SubviewMargins() const { return mySubviewMargins; }

  //! Set subview margins in pixels.
  void SetSubviewMargins (const Graphic3d_Vec2i& theMargins) { mySubviewMargins = theMargins; }

  //! Update subview position and dimensions.
  Standard_EXPORT void SubviewResized (const Handle(Aspect_NeutralWindow)& theWindow);

private:

  //! Adds the structure to display lists of the view.
  virtual void displayStructure (const Handle(Graphic3d_CStructure)& theStructure,
                                 const Graphic3d_DisplayPriority thePriority) = 0;

  //! Erases the structure from display lists of the view.
  virtual void eraseStructure (const Handle(Graphic3d_CStructure)& theStructure) = 0;

  //! Change Z layer of a structure already presented in view.
  virtual void changeZLayer (const Handle(Graphic3d_CStructure)& theCStructure,
                             const Graphic3d_ZLayerId theNewLayerId) = 0;

  //! Changes the priority of a structure within its Z layer in the specified view.
  virtual void changePriority (const Handle(Graphic3d_CStructure)& theCStructure,
                               const Graphic3d_DisplayPriority theNewPriority) = 0;

protected:

  Standard_Integer myId;
  Graphic3d_RenderingParams myRenderParams;

  NCollection_Sequence<Handle(Graphic3d_CView)> mySubviews; //!< list of child views
  Graphic3d_CView*              myParentView;               //!< back-pointer to the parent view
  Standard_Boolean              myIsSubviewComposer;        //!< flag to skip rendering of viewer contents
  Aspect_TypeOfTriedronPosition mySubviewCorner;            //!< position within parent view
  Graphic3d_Vec2i               mySubviewTopLeft;           //!< subview top-left position relative to parent view
  Graphic3d_Vec2i               mySubviewMargins;           //!< subview margins in pixels
  Graphic3d_Vec2d               mySubviewSize;              //!< subview size
  Graphic3d_Vec2d               mySubviewOffset;            //!< subview corner offset within parent view

  Handle(Graphic3d_StructureManager) myStructureManager;
  Handle(Graphic3d_Camera)  myCamera;
  Graphic3d_SequenceOfStructure myStructsToCompute;
  Graphic3d_SequenceOfStructure myStructsComputed;
  Graphic3d_MapOfStructure myStructsDisplayed;
  Standard_Boolean myIsInComputedMode;
  Standard_Boolean myIsActive;
  Standard_Boolean myIsRemoved;
  Graphic3d_TypeOfBackfacingModel myBackfacing;
  Graphic3d_TypeOfVisualization myVisualization;

  Quantity_ColorRGBA           myBgColor;
  Handle(Graphic3d_TextureMap) myBackgroundImage;
  Handle(Graphic3d_CubeMap)    myCubeMapBackground;  //!< Cubemap displayed at background
  Handle(Graphic3d_CubeMap)    myCubeMapIBL;         //!< Cubemap used for environment lighting
  Handle(Graphic3d_TextureEnv) myTextureEnvData;
  Graphic3d_GraduatedTrihedron myGTrihedronData;
  Graphic3d_TypeOfBackground   myBackgroundType;     //!< Current type of background
  Aspect_SkydomeBackground     mySkydomeAspect;
  Standard_Boolean             myToUpdateSkydome;

  Handle(Aspect_XRSession) myXRSession;
  Handle(Graphic3d_Camera) myBackXRCamera;       //!< camera projection parameters to restore after closing XR session (FOV, aspect and similar)
  Handle(Graphic3d_Camera) myBaseXRCamera;       //!< neutral camera orientation defining coordinate system in which head tracking is defined
  Handle(Graphic3d_Camera) myPosedXRCamera;      //!< transient XR camera orientation with tracked head orientation applied (based on myBaseXRCamera)
  Handle(Graphic3d_Camera) myPosedXRCameraCopy;  //!< neutral camera orientation copy at the beginning of processing input
  Standard_Real            myUnitFactor;         //!< unit scale factor defined as scale factor for m (meters)

};

#endif // _Graphic3d_CView_HeaderFile
