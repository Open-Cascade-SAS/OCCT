// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
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

#ifndef OpenGl_View_HeaderFile
#define OpenGl_View_HeaderFile

#include <Graphic3d_CullingTool.hxx>
#include <Graphic3d_WorldViewProjState.hxx>
#include <math_BullardGenerator.hxx>

#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_FrameStatsPrs.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_LayerList.hxx>
#include <OpenGl_SceneGeometry.hxx>
#include <OpenGl_Structure.hxx>
#include <OpenGl_TileSampler.hxx>

#include <map>
#include <set>

class OpenGl_BackgroundArray;
class OpenGl_DepthPeeling;
class OpenGl_PBREnvironment;
struct OpenGl_RaytraceMaterial;
class OpenGl_ShadowMap;
class OpenGl_ShadowMapArray;
class OpenGl_ShaderObject;
class OpenGl_TextureBuffer;
class OpenGl_Workspace;

DEFINE_STANDARD_HANDLE(OpenGl_View,Graphic3d_CView)

//! Implementation of OpenGl view.
class OpenGl_View : public Graphic3d_CView
{

public:

  //! Constructor.
  Standard_EXPORT OpenGl_View (const Handle(Graphic3d_StructureManager)& theMgr,
                               const Handle(OpenGl_GraphicDriver)& theDriver,
                               const Handle(OpenGl_Caps)& theCaps,
                               OpenGl_StateCounter* theCounter);

  //! Default destructor.
  Standard_EXPORT virtual ~OpenGl_View();

  //! Release OpenGL resources.
  Standard_EXPORT virtual void ReleaseGlResources (const Handle(OpenGl_Context)& theCtx);

  //! Deletes and erases the view.
  Standard_EXPORT virtual void Remove() Standard_OVERRIDE;

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
  Standard_EXPORT Standard_Boolean SetImmediateModeDrawToFront (const Standard_Boolean theDrawToFrontBuffer) Standard_OVERRIDE;

  //! Creates and maps rendering window to the view.
  Standard_EXPORT virtual void SetWindow (const Handle(Graphic3d_CView)& theParentVIew,
                                          const Handle(Aspect_Window)& theWindow,
                                          const Aspect_RenderingContext theContext) Standard_OVERRIDE;

  //! Returns window associated with the view.
  Standard_EXPORT virtual Handle(Aspect_Window) Window() const Standard_OVERRIDE;

  //! Returns True if the window associated to the view is defined.
  virtual Standard_Boolean IsDefined() const Standard_OVERRIDE
  { return !myWindow.IsNull(); }

  //! Handle changing size of the rendering window.
  Standard_EXPORT virtual void Resized() Standard_OVERRIDE;

  //! Redraw content of the view.
  Standard_EXPORT virtual void Redraw() Standard_OVERRIDE;

  //! Redraw immediate content of the view.
  Standard_EXPORT virtual void RedrawImmediate() Standard_OVERRIDE;

  //! Marks BVH tree for given priority list as dirty and marks primitive set for rebuild.
  Standard_EXPORT virtual void Invalidate() Standard_OVERRIDE;

  //! Return true if view content cache has been invalidated.
  virtual Standard_Boolean IsInvalidated() Standard_OVERRIDE { return !myBackBufferRestored; }

  //! Dump active rendering buffer into specified memory buffer.
  //! In Ray-Tracing allow to get a raw HDR buffer using Graphic3d_BT_RGB_RayTraceHdrLeft buffer type,
  //! only Left view will be dumped ignoring stereoscopic parameter.
  Standard_EXPORT virtual Standard_Boolean BufferDump (Image_PixMap& theImage,
                                                       const Graphic3d_BufferType& theBufferType) Standard_OVERRIDE;

  //! Dumps the graphical contents of a shadowmap framebuffer into an image.
  //! @param theImage the image to store the shadow map.
  //! @param theLightName [in] name of the light used to generate the shadow map.
  Standard_EXPORT virtual Standard_Boolean ShadowMapDump (Image_PixMap& theImage,
	                                                      const TCollection_AsciiString& theLightName) Standard_OVERRIDE;

  //! Marks BVH tree and the set of BVH primitives of correspondent priority list with id theLayerId as outdated.
  Standard_EXPORT virtual void InvalidateBVHData (const Graphic3d_ZLayerId theLayerId) Standard_OVERRIDE;

  //! Add a layer to the view.
  //! @param theNewLayerId [in] id of new layer, should be > 0 (negative values are reserved for default layers).
  //! @param theSettings   [in] new layer settings
  //! @param theLayerAfter [in] id of layer to append new layer before
  Standard_EXPORT virtual void InsertLayerBefore (const Graphic3d_ZLayerId theLayerId,
                                                  const Graphic3d_ZLayerSettings& theSettings,
                                                  const Graphic3d_ZLayerId theLayerAfter) Standard_OVERRIDE;

  //! Add a layer to the view.
  //! @param theNewLayerId  [in] id of new layer, should be > 0 (negative values are reserved for default layers).
  //! @param theSettings    [in] new layer settings
  //! @param theLayerBefore [in] id of layer to append new layer after
  Standard_EXPORT virtual void InsertLayerAfter (const Graphic3d_ZLayerId theNewLayerId,
                                                 const Graphic3d_ZLayerSettings& theSettings,
                                                 const Graphic3d_ZLayerId theLayerBefore) Standard_OVERRIDE;

  //! Remove a z layer with the given ID.
  Standard_EXPORT virtual void RemoveZLayer (const Graphic3d_ZLayerId theLayerId) Standard_OVERRIDE;

  //! Sets the settings for a single Z layer of specified view.
  Standard_EXPORT virtual void SetZLayerSettings (const Graphic3d_ZLayerId theLayerId,
                                                  const Graphic3d_ZLayerSettings& theSettings) Standard_OVERRIDE;

  //! Returns the maximum Z layer ID.
  //! First layer ID is Graphic3d_ZLayerId_Default, last ID is ZLayerMax().
  Standard_EXPORT virtual Standard_Integer ZLayerMax() const Standard_OVERRIDE;

  //! Returns the list of layers.
  Standard_EXPORT virtual const NCollection_List<Handle(Graphic3d_Layer)>& Layers() const Standard_OVERRIDE;

  //! Returns layer with given ID or NULL if undefined.
  Standard_EXPORT virtual Handle(Graphic3d_Layer) Layer (const Graphic3d_ZLayerId theLayerId) const Standard_OVERRIDE;

  //! Returns the bounding box of all structures displayed in the view.
  //! If theToIncludeAuxiliary is TRUE, then the boundary box also includes minimum and maximum limits
  //! of graphical elements forming parts of infinite and other auxiliary structures.
  //! @param theToIncludeAuxiliary consider also auxiliary presentations (with infinite flag or with trihedron transformation persistence)
  //! @return computed bounding box
  Standard_EXPORT virtual Bnd_Box MinMaxValues (const Standard_Boolean theToIncludeAuxiliary) const Standard_OVERRIDE;

  //! Returns pointer to an assigned framebuffer object.
  Standard_EXPORT virtual Handle(Standard_Transient) FBO() const Standard_OVERRIDE;

  //! Sets framebuffer object for offscreen rendering.
  Standard_EXPORT virtual void SetFBO (const Handle(Standard_Transient)& theFbo) Standard_OVERRIDE;

  //! Generate offscreen FBO in the graphic library.
  //! If not supported on hardware returns NULL.
  Standard_EXPORT virtual Handle(Standard_Transient) FBOCreate (const Standard_Integer theWidth,
                                                                const Standard_Integer theHeight) Standard_OVERRIDE;

  //! Remove offscreen FBO from the graphic library
  Standard_EXPORT virtual void FBORelease (Handle(Standard_Transient)& theFbo) Standard_OVERRIDE;

  //! Read offscreen FBO configuration.
  Standard_EXPORT virtual void FBOGetDimensions (const Handle(Standard_Transient)& theFbo,
                                                 Standard_Integer& theWidth,
                                                 Standard_Integer& theHeight,
                                                 Standard_Integer& theWidthMax,
                                                 Standard_Integer& theHeightMax) Standard_OVERRIDE;

  //! Change offscreen FBO viewport.
  Standard_EXPORT virtual void FBOChangeViewport (const Handle(Standard_Transient)& theFbo,
                                                  const Standard_Integer theWidth,
                                                  const Standard_Integer theHeight) Standard_OVERRIDE;

  //! Returns additional buffers for depth peeling OIT.
  const Handle(OpenGl_DepthPeeling)& DepthPeelingFbos() const { return myDepthPeelingFbos; }

public:

  //! Returns gradient background fill colors.
  Standard_EXPORT virtual Aspect_GradientBackground GradientBackground() const Standard_OVERRIDE;

  //! Sets gradient background fill colors.
  Standard_EXPORT virtual void SetGradientBackground (const Aspect_GradientBackground& theBackground) Standard_OVERRIDE;

  //! Sets image texture or environment cubemap as background.
  //! @param theTextureMap [in] source to set a background;
  //!                           should be either Graphic3d_Texture2D or Graphic3d_CubeMap
  //! @param theToUpdatePBREnv [in] defines whether IBL maps will be generated or not
  //!                               (see GeneratePBREnvironment())
  Standard_EXPORT virtual void SetBackgroundImage (const Handle(Graphic3d_TextureMap)& theTextureMap,
                                                   Standard_Boolean theToUpdatePBREnv = Standard_True) Standard_OVERRIDE;

  //! Sets environment texture for the view.
  Standard_EXPORT virtual void SetTextureEnv (const Handle(Graphic3d_TextureEnv)& theTextureEnv) Standard_OVERRIDE;

  //! Returns background image fill style.
  Standard_EXPORT virtual Aspect_FillMethod BackgroundImageStyle() const Standard_OVERRIDE;

  //! Sets background image fill style.
  Standard_EXPORT virtual void SetBackgroundImageStyle (const Aspect_FillMethod theFillStyle) Standard_OVERRIDE;

  //! Enables or disables IBL (Image Based Lighting) from background cubemap.
  //! Has no effect if PBR is not used.
  //! @param[in] theToEnableIBL enable or disable IBL from background cubemap
  //! @param[in] theToUpdate redraw the view
  Standard_EXPORT virtual void SetImageBasedLighting (Standard_Boolean theToEnableIBL) Standard_OVERRIDE;

  //! Returns number of mipmap levels used in specular IBL map.
  //! 0 if PBR environment is not created.
  Standard_EXPORT unsigned int SpecIBLMapLevels() const;

  //! Returns local camera origin currently set for rendering, might be modified during rendering.
  const gp_XYZ& LocalOrigin() const { return myLocalOrigin; }

  //! Setup local camera origin currently set for rendering.
  Standard_EXPORT void SetLocalOrigin (const gp_XYZ& theOrigin);

  //! Returns list of lights of the view.
  virtual const Handle(Graphic3d_LightSet)& Lights() const Standard_OVERRIDE { return myLights; }

  //! Sets list of lights for the view.
  virtual void SetLights (const Handle(Graphic3d_LightSet)& theLights) Standard_OVERRIDE
  {
    myLights = theLights;
    myCurrLightSourceState = myStateCounter->Increment();
  }

  //! Returns list of clip planes set for the view.
  virtual const Handle(Graphic3d_SequenceOfHClipPlane)& ClipPlanes() const Standard_OVERRIDE { return myClipPlanes; }

  //! Sets list of clip planes for the view.
  virtual void SetClipPlanes (const Handle(Graphic3d_SequenceOfHClipPlane)& thePlanes) Standard_OVERRIDE { myClipPlanes = thePlanes; }

  //! Fill in the dictionary with diagnostic info.
  //! Should be called within rendering thread.
  //!
  //! This API should be used only for user output or for creating automated reports.
  //! The format of returned information (e.g. key-value layout)
  //! is NOT part of this API and can be changed at any time.
  //! Thus application should not parse returned information to weed out specific parameters.
  Standard_EXPORT virtual void DiagnosticInformation (TColStd_IndexedDataMapOfStringString& theDict,
                                                      Graphic3d_DiagnosticInfo theFlags) const Standard_OVERRIDE;

  //! Returns string with statistic performance info.
  Standard_EXPORT virtual TCollection_AsciiString StatisticInformation() const Standard_OVERRIDE;

  //! Fills in the dictionary with statistic performance info.
  Standard_EXPORT virtual void StatisticInformation (TColStd_IndexedDataMapOfStringString& theDict) const Standard_OVERRIDE;

public:

  //! Returns background color.
  const Quantity_ColorRGBA& BackgroundColor() const { return myBgColor; }

  //! Change graduated trihedron.
  OpenGl_GraduatedTrihedron& ChangeGraduatedTrihedron() { return myGraduatedTrihedron; }

  void SetTextureEnv (const Handle(OpenGl_Context)&       theCtx,
                      const Handle(Graphic3d_TextureEnv)& theTexture);

  void SetBackgroundTextureStyle (const Aspect_FillMethod FillStyle);

  void SetBackgroundGradient (const Quantity_Color& AColor1, const Quantity_Color& AColor2, const Aspect_GradientFillMethod AType);

  void SetBackgroundGradientType (const Aspect_GradientFillMethod AType);

  //! Returns list of OpenGL Z-layers.
  const OpenGl_LayerList& LayerList() const { return myZLayers; }

  //! Returns OpenGL window implementation.
  const Handle(OpenGl_Window)& GlWindow() const { return myWindow; }

  //! Returns OpenGL environment map.
  const Handle(OpenGl_TextureSet)& GlTextureEnv() const { return myTextureEnv; }

  //! Returns selector for BVH tree, providing a possibility to store information
  //! about current view volume and to detect which objects are overlapping it.
  const Graphic3d_CullingTool& BVHTreeSelector() const { return myBVHSelector; }

  //! Returns true if there are immediate structures to display
  bool HasImmediateStructures() const
  {
    return myZLayers.NbImmediateStructures() != 0;
  }

public: //! @name obsolete Graduated Trihedron functionality

  //! Displays Graduated Trihedron.
  Standard_EXPORT virtual void GraduatedTrihedronDisplay (const Graphic3d_GraduatedTrihedron& theTrihedronData) Standard_OVERRIDE;

  //! Erases Graduated Trihedron.
  Standard_EXPORT virtual void GraduatedTrihedronErase() Standard_OVERRIDE;

  //! Sets minimum and maximum points of scene bounding box for Graduated Trihedron stored in graphic view object.
  //! @param theMin [in] the minimum point of scene.
  //! @param theMax [in] the maximum point of scene.
  Standard_EXPORT virtual void GraduatedTrihedronMinMaxValues (const Graphic3d_Vec3 theMin, const Graphic3d_Vec3 theMax) Standard_OVERRIDE;

protected: //! @name Internal methods for managing GL resources

  //! Initializes OpenGl resource for environment texture.
  void initTextureEnv (const Handle(OpenGl_Context)& theContext);

protected: //! @name low-level redrawing sub-routines

  //! Prepare frame buffers for rendering.
  Standard_EXPORT virtual bool prepareFrameBuffers (Graphic3d_Camera::Projection& theProj);

  //! Redraws view for the given monographic camera projection, or left/right eye.
  Standard_EXPORT virtual void redraw (const Graphic3d_Camera::Projection theProjection,
                                       OpenGl_FrameBuffer*                theReadDrawFbo,
                                       OpenGl_FrameBuffer*                theOitAccumFbo);

  //! Redraws view for the given monographic camera projection, or left/right eye.
  //!
  //! Method will blit snapshot containing main scene (myMainSceneFbos or BackBuffer)
  //! into presentation buffer (myMainSceneFbos -> offscreen FBO or
  //! myMainSceneFbos -> BackBuffer or BackBuffer -> FrontBuffer),
  //! and redraw immediate structures on top.
  //!
  //! When scene caching is disabled (myTransientDrawToFront, no double buffer in window, etc.),
  //! the first step (blitting) will be skipped.
  //!
  //! @return false if immediate structures has been rendered directly into FrontBuffer
  //! and Buffer Swap should not be called.
  Standard_EXPORT virtual bool redrawImmediate (const Graphic3d_Camera::Projection theProjection,
                                                OpenGl_FrameBuffer* theReadFbo,
                                                OpenGl_FrameBuffer* theDrawFbo,
                                                OpenGl_FrameBuffer* theOitAccumFbo,
                                                const Standard_Boolean theIsPartialUpdate = Standard_False);

  //! Blit subviews into this view.
  Standard_EXPORT bool blitSubviews (const Graphic3d_Camera::Projection theProjection,
                                     OpenGl_FrameBuffer* theDrawFbo);

  //! Blit image from/to specified buffers.
  Standard_EXPORT bool blitBuffers (OpenGl_FrameBuffer*    theReadFbo,
                                    OpenGl_FrameBuffer*    theDrawFbo,
                                    const Standard_Boolean theToFlip = Standard_False);

  //! Setup default FBO.
  Standard_EXPORT void bindDefaultFbo (OpenGl_FrameBuffer* theCustomFbo = NULL);

protected: //! @name Rendering of GL graphics (with prepared drawing buffer).

  //! Renders the graphical contents of the view into the prepared shadowmap framebuffer.
  //! @param theShadowMap [in] the framebuffer for rendering shadowmap.
  Standard_EXPORT virtual void renderShadowMap (const Handle(OpenGl_ShadowMap)& theShadowMap);

  //! Renders the graphical contents of the view into the prepared window or framebuffer.
  //! @param theProjection [in] the projection that should be used for rendering.
  //! @param theReadDrawFbo [in] the framebuffer for rendering graphics.
  //! @param theOitAccumFbo [in] the framebuffer for accumulating color and coverage for OIT process.
  //! @param theToDrawImmediate [in] the flag indicates whether the rendering performs in immediate mode.
  Standard_EXPORT virtual void render (Graphic3d_Camera::Projection theProjection,
                                       OpenGl_FrameBuffer*          theReadDrawFbo,
                                       OpenGl_FrameBuffer*          theOitAccumFbo,
                                       const Standard_Boolean       theToDrawImmediate);

  //! Renders the graphical scene.
  //! @param theProjection [in] the projection that is used for rendering.
  //! @param theReadDrawFbo [in] the framebuffer for rendering graphics.
  //! @param theOitAccumFbo [in] the framebuffer for accumulating color and coverage for OIT process.
  //! @param theToDrawImmediate [in] the flag indicates whether the rendering performs in immediate mode.
  Standard_EXPORT virtual void renderScene (Graphic3d_Camera::Projection theProjection,
                                            OpenGl_FrameBuffer*    theReadDrawFbo,
                                            OpenGl_FrameBuffer*    theOitAccumFbo,
                                            const Standard_Boolean theToDrawImmediate);

  //! Draw background (gradient / image / cubemap)
  Standard_EXPORT virtual void drawBackground (const Handle(OpenGl_Workspace)& theWorkspace,
                                               Graphic3d_Camera::Projection theProjection);

  //! Render set of structures presented in the view.
  //! @param theProjection [in] the projection that is used for rendering.
  //! @param theReadDrawFbo [in] the framebuffer for rendering graphics.
  //! @param theOitAccumFbo [in] the framebuffer for accumulating color and coverage for OIT process.
  //! @param theToDrawImmediate [in] the flag indicates whether the rendering performs in immediate mode.
  Standard_EXPORT virtual void renderStructs (Graphic3d_Camera::Projection theProjection,
                                              OpenGl_FrameBuffer*    theReadDrawFbo,
                                              OpenGl_FrameBuffer*    theOitAccumFbo,
                                              const Standard_Boolean theToDrawImmediate);

  //! Renders trihedron.
  void renderTrihedron (const Handle(OpenGl_Workspace) &theWorkspace);

  //! Renders frame statistics.
  void renderFrameStats();

private:

  //! Adds the structure to display lists of the view.
  Standard_EXPORT virtual void displayStructure (const Handle(Graphic3d_CStructure)& theStructure,
                                                 const Graphic3d_DisplayPriority thePriority) Standard_OVERRIDE;

  //! Erases the structure from display lists of the view.
  Standard_EXPORT virtual void eraseStructure (const Handle(Graphic3d_CStructure)& theStructure) Standard_OVERRIDE;

  //! Change Z layer of a structure already presented in view.
  Standard_EXPORT virtual void changeZLayer (const Handle(Graphic3d_CStructure)& theCStructure,
                                             const Graphic3d_ZLayerId theNewLayerId) Standard_OVERRIDE;

  //! Changes the priority of a structure within its Z layer in the specified view.
  Standard_EXPORT virtual void changePriority (const Handle(Graphic3d_CStructure)& theCStructure,
                                               const Graphic3d_DisplayPriority theNewPriority) Standard_OVERRIDE;

private:

  //! Release sRGB resources (frame-buffers, textures, etc.).
  void releaseSrgbResources (const Handle(OpenGl_Context)& theCtx);

  //! Copy content of Back buffer to the Front buffer.
  bool copyBackToFront();

  //! Initialize blit quad.
  OpenGl_VertexBuffer* initBlitQuad (const Standard_Boolean theToFlip);

  //! Blend together views pair into stereo image.
  void drawStereoPair (OpenGl_FrameBuffer* theDrawFbo);

  //! Check and update OIT compatibility with current OpenGL context's state.
  bool checkOitCompatibility (const Handle(OpenGl_Context)& theGlContext,
                              const Standard_Boolean theMSAA);

protected:

  OpenGl_GraphicDriver*    myDriver;
  Handle(OpenGl_Window)    myWindow;
  Handle(OpenGl_Workspace) myWorkspace;
  Handle(OpenGl_Caps)      myCaps;
  Standard_Boolean         myWasRedrawnGL;

  Handle(Graphic3d_SequenceOfHClipPlane) myClipPlanes;
  gp_XYZ                          myLocalOrigin;
  Handle(OpenGl_FrameBuffer)      myFBO;
  Standard_Boolean                myToShowGradTrihedron;
  Graphic3d_GraduatedTrihedron    myGTrihedronData;

  Handle(Graphic3d_LightSet)      myNoShadingLight;
  Handle(Graphic3d_LightSet)      myLights;
  OpenGl_LayerList                myZLayers; //!< main list of displayed structure, sorted by layers

  Graphic3d_WorldViewProjState    myWorldViewProjState; //!< camera modification state
  OpenGl_StateCounter*            myStateCounter;
  Standard_Size                   myCurrLightSourceState;
  Standard_Size                   myLightsRevision;

  typedef std::pair<Standard_Size, Standard_Size> StateInfo;

  StateInfo myLastOrientationState;
  StateInfo myLastViewMappingState;
  StateInfo myLastLightSourceState;

  //! Is needed for selection of overlapping objects and storage of the current view volume
  Graphic3d_CullingTool myBVHSelector;

  OpenGl_GraduatedTrihedron myGraduatedTrihedron;
  OpenGl_FrameStatsPrs      myFrameStatsPrs;

  //! Framebuffers for OpenGL output.
  Handle(OpenGl_FrameBuffer) myOpenGlFBO;
  Handle(OpenGl_FrameBuffer) myOpenGlFBO2;

protected: //! @name Rendering properties

  //! Two framebuffers (left and right views) store cached main presentation
  //! of the view (without presentation of immediate layers).
  Standard_Integer           mySRgbState;             //!< track sRGB state
  GLint                      myFboColorFormat;        //!< sized format for color attachments
  GLint                      myFboDepthFormat;        //!< sized format for depth-stencil attachments
  OpenGl_ColorFormats        myFboOitColorConfig;     //!< selected color format configuration for OIT color attachments
  Handle(OpenGl_FrameBuffer) myMainSceneFbos[2];
  Handle(OpenGl_FrameBuffer) myMainSceneFbosOit[2];      //!< Additional buffers for transparent draw of main layer.
  Handle(OpenGl_FrameBuffer) myImmediateSceneFbos[2];    //!< Additional buffers for immediate layer in stereo mode.
  Handle(OpenGl_FrameBuffer) myImmediateSceneFbosOit[2]; //!< Additional buffers for transparency draw of immediate layer.
  Handle(OpenGl_FrameBuffer) myXrSceneFbo;               //!< additional FBO (without MSAA) for submitting to XR
  Handle(OpenGl_DepthPeeling)   myDepthPeelingFbos;   //!< additional buffers for depth peeling
  Handle(OpenGl_ShadowMapArray) myShadowMaps;         //!< additional FBOs for shadow map rendering
  OpenGl_VertexBuffer        myFullScreenQuad;        //!< Vertices for full-screen quad rendering.
  OpenGl_VertexBuffer        myFullScreenQuadFlip;
  Standard_Boolean           myToFlipOutput;          //!< Flag to draw result image upside-down
  unsigned int               myFrameCounter;          //!< redraw counter, for debugging
  Standard_Boolean           myHasFboBlit;            //!< disable FBOs on failure
  Standard_Boolean           myToDisableOIT;          //!< disable OIT on failure
  Standard_Boolean           myToDisableOITMSAA;      //!< disable OIT with MSAA on failure
  Standard_Boolean           myToDisableMSAA;         //!< disable MSAA after failure
  Standard_Boolean           myTransientDrawToFront; //!< optimization flag for immediate mode (to render directly to the front buffer)
  Standard_Boolean           myBackBufferRestored;
  Standard_Boolean           myIsImmediateDrawn;     //!< flag indicates that immediate mode buffer contains some data

protected: //! @name Background parameters

  OpenGl_Aspects*            myTextureParams;                     //!< Stores texture and its parameters for textured background
  OpenGl_Aspects*            myCubeMapParams;                     //!< Stores cubemap and its parameters for cubemap background
  OpenGl_Aspects*            myColoredQuadParams;                 //!< Stores parameters for gradient (corner mode) background
  OpenGl_BackgroundArray*    myBackgrounds[Graphic3d_TypeOfBackground_NB]; //!< Array of primitive arrays of different background types
  Handle(OpenGl_TextureSet)  myTextureEnv;
  Handle(OpenGl_Texture)     mySkydomeTexture;

protected: //! @name methods related to skydome background

  //! Generates skydome cubemap.
  Standard_EXPORT void updateSkydomeBg (const Handle(OpenGl_Context)& theCtx);

protected: //! @name methods related to PBR

  //! Checks whether PBR is available.
  Standard_EXPORT Standard_Boolean checkPBRAvailability() const;

  //! Generates IBL maps used in PBR pipeline.
  //! If background cubemap is not set clears all IBL maps.
  Standard_EXPORT void updatePBREnvironment (const Handle(OpenGl_Context)& theCtx);

protected: //! @name fields and types related to PBR

  //! State of PBR environment.
  enum PBREnvironmentState
  {
    OpenGl_PBREnvState_NONEXISTENT,
    OpenGl_PBREnvState_UNAVAILABLE, // indicates failed try to create PBR environment
    OpenGl_PBREnvState_CREATED
  };

  Handle(OpenGl_PBREnvironment) myPBREnvironment; //!< manager of IBL maps used in PBR pipeline
  PBREnvironmentState           myPBREnvState;    //!< state of PBR environment
  Standard_Boolean              myPBREnvRequest;  //!< update PBR environment

protected: //! @name data types related to ray-tracing

  //! Result of OpenGL shaders initialization.
  enum RaytraceInitStatus
  {
    OpenGl_RT_NONE,
    OpenGl_RT_INIT,
    OpenGl_RT_FAIL
  };

  //! Describes update mode (state).
  enum RaytraceUpdateMode
  {
    OpenGl_GUM_CHECK,   //!< check geometry state
    OpenGl_GUM_PREPARE, //!< collect unchanged objects
    OpenGl_GUM_REBUILD  //!< rebuild changed and new objects
  };

  //! Defines frequently used shader variables.
  enum ShaderVariableIndex
  {
    OpenGl_RT_aPosition,

    // camera position
    OpenGl_RT_uOriginLT,
    OpenGl_RT_uOriginLB,
    OpenGl_RT_uOriginRT,
    OpenGl_RT_uOriginRB,
    OpenGl_RT_uDirectLT,
    OpenGl_RT_uDirectLB,
    OpenGl_RT_uDirectRT,
    OpenGl_RT_uDirectRB,
    OpenGl_RT_uViewPrMat,
    OpenGl_RT_uUnviewMat,

    // 3D scene params
    OpenGl_RT_uSceneRad,
    OpenGl_RT_uSceneEps,
    OpenGl_RT_uLightAmbnt,
    OpenGl_RT_uLightCount,

    // background params
    OpenGl_RT_uBackColorTop,
    OpenGl_RT_uBackColorBot,

    // ray-tracing params
    OpenGl_RT_uShadowsEnabled,
    OpenGl_RT_uReflectEnabled,
    OpenGl_RT_uEnvMapEnabled,
    OpenGl_RT_uEnvMapForBack,
    OpenGl_RT_uTexSamplersArray,
    OpenGl_RT_uBlockedRngEnabled,

    // size of render window
    OpenGl_RT_uWinSizeX,
    OpenGl_RT_uWinSizeY,

    // sampled frame params
    OpenGl_RT_uAccumSamples,
    OpenGl_RT_uFrameRndSeed,

    // adaptive FSAA params
    OpenGl_RT_uFsaaOffset,
    OpenGl_RT_uSamples,

    // images used by ISS mode
    OpenGl_RT_uRenderImage,
    OpenGl_RT_uTilesImage,
    OpenGl_RT_uOffsetImage,
    OpenGl_RT_uTileSize,
    OpenGl_RT_uVarianceScaleFactor,

    // maximum radiance value
    OpenGl_RT_uMaxRadiance,

    OpenGl_RT_NbVariables // special field
  };

  //! Defines OpenGL image samplers.
  enum ShaderImageNames
  {
    OpenGl_RT_OutputImage = 0,
    OpenGl_RT_VisualErrorImage = 1,
    OpenGl_RT_TileOffsetsImage = 2,
    OpenGl_RT_TileSamplesImage = 3
  };

  //! Tool class for management of shader sources.
  class ShaderSource
  {
  public:

    //! Default shader prefix - empty string.
    static const TCollection_AsciiString EMPTY_PREFIX;

    //! Creates new uninitialized shader source.
    ShaderSource()
    {
      //
    }

  public:

    //! Returns error description in case of load fail.
    const TCollection_AsciiString& ErrorDescription() const
    {
      return myError;
    }

    //! Returns prefix to insert before the source.
    const TCollection_AsciiString& Prefix() const
    {
      return myPrefix;
    }

    //! Sets prefix to insert before the source.
    void SetPrefix (const TCollection_AsciiString& thePrefix)
    {
      myPrefix = thePrefix;
    }

    //! Returns shader source combined with prefix.
    TCollection_AsciiString Source (const Handle(OpenGl_Context)& theCtx,
                                    const GLenum theType) const;

    //! Loads shader source from specified files.
    Standard_Boolean LoadFromFiles (const TCollection_AsciiString* theFileNames, const TCollection_AsciiString& thePrefix = EMPTY_PREFIX);

    //! Loads shader source from specified strings.
    Standard_Boolean LoadFromStrings (const TCollection_AsciiString* theStrings, const TCollection_AsciiString& thePrefix = EMPTY_PREFIX);

  private:

    TCollection_AsciiString mySource; //!< Source string of the shader object
    TCollection_AsciiString myPrefix; //!< Prefix to insert before the source
    TCollection_AsciiString myError;  //!< error state

  };

  //! Default ray-tracing depth.
  static const Standard_Integer THE_DEFAULT_NB_BOUNCES = 3;

  //! Default size of traversal stack.
  static const Standard_Integer THE_DEFAULT_STACK_SIZE = 10;

  //! Compile-time ray-tracing parameters.
  struct RaytracingParams
  {
    //! Actual size of traversal stack in shader program.
    Standard_Integer StackSize;

    //! Actual ray-tracing depth (number of ray bounces).
    Standard_Integer NbBounces;

    //! Define depth computation
    Standard_Boolean IsZeroToOneDepth;

    //! Enables/disables light propagation through transparent media.
    Standard_Boolean TransparentShadows;

    //! Enables/disables global illumination (GI) effects.
    Standard_Boolean GlobalIllumination;

    //! Enables/disables the use of OpenGL bindless textures.
    Standard_Boolean UseBindlessTextures;

    //! Enables/disables two-sided BSDF models instead of one-sided.
    Standard_Boolean TwoSidedBsdfModels;

    //! Enables/disables adaptive screen sampling for path tracing.
    Standard_Boolean AdaptiveScreenSampling;

    //! Enables/disables 1-pass atomic mode for AdaptiveScreenSampling.
    Standard_Boolean AdaptiveScreenSamplingAtomic;

    //! Enables/disables environment map for background.
    Standard_Boolean UseEnvMapForBackground;

    //! Enables/disables normal map ignoring during path tracing.
    Standard_Boolean ToIgnoreNormalMap;

    //! Maximum radiance value used for clamping radiance estimation.
    Standard_ShortReal RadianceClampingValue;
    
    //! Enables/disables depth-of-field effect (path tracing, perspective camera).
    Standard_Boolean DepthOfField;

    //! Enables/disables cubemap background.
    Standard_Boolean CubemapForBack;

    //! Tone mapping method for path tracing.
    Graphic3d_ToneMappingMethod ToneMappingMethod;

    //! Creates default compile-time ray-tracing parameters.
    RaytracingParams()
    : StackSize              (THE_DEFAULT_STACK_SIZE),
      NbBounces              (THE_DEFAULT_NB_BOUNCES),
      IsZeroToOneDepth       (Standard_False),
      TransparentShadows     (Standard_False),
      GlobalIllumination     (Standard_False),
      UseBindlessTextures    (Standard_False),
      TwoSidedBsdfModels     (Standard_False),
      AdaptiveScreenSampling (Standard_False),
      AdaptiveScreenSamplingAtomic (Standard_False),
      UseEnvMapForBackground (Standard_False),
      ToIgnoreNormalMap      (Standard_False),
      RadianceClampingValue  (30.0),
      DepthOfField           (Standard_False),
      CubemapForBack         (Standard_False),
      ToneMappingMethod      (Graphic3d_ToneMappingMethod_Disabled) { }
  };

  //! Describes state of OpenGL structure.
  struct StructState
  {
    Standard_Size StructureState;
    Standard_Size InstancedState;

    //! Creates new structure state.
    StructState (const Standard_Size theStructureState = 0,
                 const Standard_Size theInstancedState = 0)
    : StructureState (theStructureState),
      InstancedState (theInstancedState)
    {
      //
    }

    //! Creates new structure state.
    StructState (const OpenGl_Structure* theStructure)
    {
      StructureState = theStructure->ModificationState();

      InstancedState = theStructure->InstancedStructure() != NULL ?
        theStructure->InstancedStructure()->ModificationState() : 0;
    }
  };

protected: //! @name methods related to ray-tracing

  //! Updates 3D scene geometry for ray-tracing.
  Standard_Boolean updateRaytraceGeometry (const RaytraceUpdateMode      theMode,
                                           const Standard_Integer        theViewId,
                                           const Handle(OpenGl_Context)& theGlContext);

  //! Updates 3D scene light sources for ray-tracing.
  Standard_Boolean updateRaytraceLightSources (const OpenGl_Mat4& theInvModelView, const Handle(OpenGl_Context)& theGlContext);

  //! Checks to see if the OpenGL structure is modified.
  Standard_Boolean toUpdateStructure (const OpenGl_Structure* theStructure);

  //! Adds OpenGL structure to ray-traced scene geometry.
  Standard_Boolean addRaytraceStructure (const OpenGl_Structure*       theStructure,
                                         const Handle(OpenGl_Context)& theGlContext);

  //! Adds OpenGL groups to ray-traced scene geometry.
  Standard_Boolean addRaytraceGroups (const OpenGl_Structure*        theStructure,
                                      const OpenGl_RaytraceMaterial& theStructMat,
                                      const Handle(TopLoc_Datum3D)&  theTrsf,
                                      const Handle(OpenGl_Context)&  theGlContext);

  //! Creates ray-tracing material properties.
  OpenGl_RaytraceMaterial convertMaterial (const OpenGl_Aspects* theAspect,
                                           const Handle(OpenGl_Context)& theGlContext);

  //! Adds OpenGL primitive array to ray-traced scene geometry.
  Handle(OpenGl_TriangleSet) addRaytracePrimitiveArray (const OpenGl_PrimitiveArray* theArray,
                                                        const Standard_Integer       theMatID,
                                                        const OpenGl_Mat4*           theTrans);

  //! Adds vertex indices from OpenGL primitive array to ray-traced scene geometry.
  Standard_Boolean addRaytraceVertexIndices (OpenGl_TriangleSet&                  theSet,
                                             const Standard_Integer               theMatID,
                                             const Standard_Integer               theCount,
                                             const Standard_Integer               theOffset,
                                             const OpenGl_PrimitiveArray&         theArray);

  //! Adds OpenGL triangle array to ray-traced scene geometry.
  Standard_Boolean addRaytraceTriangleArray (OpenGl_TriangleSet&                  theSet,
                                             const Standard_Integer               theMatID,
                                             const Standard_Integer               theCount,
                                             const Standard_Integer               theOffset,
                                             const Handle(Graphic3d_IndexBuffer)& theIndices);

  //! Adds OpenGL triangle fan array to ray-traced scene geometry.
  Standard_Boolean addRaytraceTriangleFanArray (OpenGl_TriangleSet&                  theSet,
                                                const Standard_Integer               theMatID,
                                                const Standard_Integer               theCount,
                                                const Standard_Integer               theOffset,
                                                const Handle(Graphic3d_IndexBuffer)& theIndices);

  //! Adds OpenGL triangle strip array to ray-traced scene geometry.
  Standard_Boolean addRaytraceTriangleStripArray (OpenGl_TriangleSet&                  theSet,
                                                  const Standard_Integer               theMatID,
                                                  const Standard_Integer               theCount,
                                                  const Standard_Integer               theOffset,
                                                  const Handle(Graphic3d_IndexBuffer)& theIndices);

  //! Adds OpenGL quadrangle array to ray-traced scene geometry.
  Standard_Boolean addRaytraceQuadrangleArray (OpenGl_TriangleSet&                  theSet,
                                               const Standard_Integer               theMatID,
                                               const Standard_Integer               theCount,
                                               const Standard_Integer               theOffset,
                                               const Handle(Graphic3d_IndexBuffer)& theIndices);

  //! Adds OpenGL quadrangle strip array to ray-traced scene geometry.
  Standard_Boolean addRaytraceQuadrangleStripArray (OpenGl_TriangleSet&                  theSet,
                                                    const Standard_Integer               theMatID,
                                                    const Standard_Integer               theCount,
                                                    const Standard_Integer               theOffset,
                                                    const Handle(Graphic3d_IndexBuffer)& theIndices);

  //! Adds OpenGL polygon array to ray-traced scene geometry.
  Standard_Boolean addRaytracePolygonArray (OpenGl_TriangleSet&                  theSet,
                                            const Standard_Integer               theMatID,
                                            const Standard_Integer               theCount,
                                            const Standard_Integer               theOffset,
                                            const Handle(Graphic3d_IndexBuffer)& theIndices);

  //! Uploads ray-trace data to the GPU.
  Standard_Boolean uploadRaytraceData (const Handle(OpenGl_Context)& theGlContext);

  //! Generates shader prefix based on current ray-tracing options.
  TCollection_AsciiString generateShaderPrefix (const Handle(OpenGl_Context)& theGlContext) const;

  //! Performs safe exit when shaders initialization fails.
  Standard_Boolean safeFailBack (const TCollection_ExtendedString& theMessage,
                                 const Handle(OpenGl_Context)&     theGlContext);

  //! Loads and compiles shader object from specified source.
  Handle(OpenGl_ShaderObject) initShader (const GLenum                  theType,
                                          const ShaderSource&           theSource,
                                          const Handle(OpenGl_Context)& theGlContext);

  //! Creates shader program from the given vertex and fragment shaders.
  Handle(OpenGl_ShaderProgram) initProgram (const Handle(OpenGl_Context)&      theGlContext,
                                            const Handle(OpenGl_ShaderObject)& theVertShader,
                                            const Handle(OpenGl_ShaderObject)& theFragShader,
                                            const TCollection_AsciiString& theName);

  //! Initializes OpenGL/GLSL shader programs.
  Standard_Boolean initRaytraceResources (const Standard_Integer theSizeX,
                                          const Standard_Integer theSizeY,
                                          const Handle(OpenGl_Context)& theGlContext);

  //! Releases OpenGL/GLSL shader programs.
  void releaseRaytraceResources (const Handle(OpenGl_Context)& theGlContext,
                                 const Standard_Boolean        theToRebuild = Standard_False);

  //! Updates auxiliary OpenGL frame buffers.
  Standard_Boolean updateRaytraceBuffers (const Standard_Integer        theSizeX,
                                          const Standard_Integer        theSizeY,
                                          const Handle(OpenGl_Context)& theGlContext);

  //! Generates viewing rays for corners of screen quad.
  //! (ray tracing; path tracing for orthographic camera)
  void updateCamera (const OpenGl_Mat4& theOrientation,
                     const OpenGl_Mat4& theViewMapping,
                     OpenGl_Vec3*       theOrigins,
                     OpenGl_Vec3*       theDirects,
                     OpenGl_Mat4&       theView,
                     OpenGl_Mat4&       theUnView);

  //! Generate viewing rays (path tracing, perspective camera).
  void updatePerspCameraPT(const OpenGl_Mat4&           theOrientation,
                           const OpenGl_Mat4&           theViewMapping,
                           Graphic3d_Camera::Projection theProjection,
                           OpenGl_Mat4&                 theViewPr,
                           OpenGl_Mat4&                 theUnview,
                           const int                    theWinSizeX,
                           const int                    theWinSizeY);

  //! Binds ray-trace textures to corresponding texture units.
  void bindRaytraceTextures (const Handle(OpenGl_Context)& theGlContext,
                             int theStereoView);

  //! Unbinds ray-trace textures from corresponding texture unit.
  void unbindRaytraceTextures (const Handle(OpenGl_Context)& theGlContext);

  //! Sets uniform state for the given ray-tracing shader program.
  Standard_Boolean setUniformState (const Standard_Integer        theProgramId,
                                    const Standard_Integer        theSizeX,
                                    const Standard_Integer        theSizeY,
                                    Graphic3d_Camera::Projection  theProjection,
                                    const Handle(OpenGl_Context)& theGlContext);

  //! Runs ray-tracing shader programs.
  Standard_Boolean runRaytraceShaders (const Standard_Integer        theSizeX,
                                       const Standard_Integer        theSizeY,
                                       Graphic3d_Camera::Projection  theProjection,
                                       OpenGl_FrameBuffer*           theReadDrawFbo,
                                       const Handle(OpenGl_Context)& theGlContext);

  //! Runs classical (Whitted-style) ray-tracing kernel.
  Standard_Boolean runRaytrace (const Standard_Integer        theSizeX,
                                const Standard_Integer        theSizeY,
                                Graphic3d_Camera::Projection  theProjection,
                                OpenGl_FrameBuffer*           theReadDrawFbo,
                                const Handle(OpenGl_Context)& theGlContext);

  //! Runs path tracing (global illumination) kernel.
  Standard_Boolean runPathtrace (const Standard_Integer        theSizeX,
                                 const Standard_Integer        theSizeY,
                                 Graphic3d_Camera::Projection  theProjection,
                                 const Handle(OpenGl_Context)& theGlContext);

  //! Runs path tracing (global illumination) kernel.
  Standard_Boolean runPathtraceOut (Graphic3d_Camera::Projection  theProjection,
                                    OpenGl_FrameBuffer*           theReadDrawFbo,
                                    const Handle(OpenGl_Context)& theGlContext);

  //! Redraws the window using OpenGL/GLSL ray-tracing or path tracing.
  Standard_Boolean raytrace (const Standard_Integer        theSizeX,
                             const Standard_Integer        theSizeY,
                             Graphic3d_Camera::Projection  theProjection,
                             OpenGl_FrameBuffer*           theReadDrawFbo,
                             const Handle(OpenGl_Context)& theGlContext);

protected: //! @name fields related to ray-tracing

  //! Result of RT/PT shaders initialization.
  RaytraceInitStatus myRaytraceInitStatus;

  //! Is ray-tracing geometry data valid?
  Standard_Boolean myIsRaytraceDataValid;

  //! True if warning about missing extension GL_ARB_bindless_texture has been displayed.
  Standard_Boolean myIsRaytraceWarnTextures;

  //! 3D scene geometry data for ray-tracing.
  OpenGl_RaytraceGeometry myRaytraceGeometry;

  //! Builder for triangle set.
  opencascade::handle<BVH_Builder<Standard_ShortReal, 3> > myRaytraceBVHBuilder;

  //! Compile-time ray-tracing parameters.
  RaytracingParams myRaytraceParameters;

  //! Radius of bounding sphere of the scene.
  Standard_ShortReal myRaytraceSceneRadius;
  //! Scene epsilon to prevent self-intersections.
  Standard_ShortReal myRaytraceSceneEpsilon;

  //! OpenGL/GLSL source of ray-tracing fragment shader.
  ShaderSource myRaytraceShaderSource;
  //! OpenGL/GLSL source of adaptive-AA fragment shader.
  ShaderSource myPostFSAAShaderSource;
  //! OpenGL/GLSL source of RT/PT display fragment shader.
  ShaderSource myOutImageShaderSource;

  //! OpenGL/GLSL ray-tracing fragment shader.
  Handle(OpenGl_ShaderObject) myRaytraceShader;
  //! OpenGL/GLSL adaptive-AA fragment shader.
  Handle(OpenGl_ShaderObject) myPostFSAAShader;
  //! OpenGL/GLSL ray-tracing display fragment shader.
  Handle(OpenGl_ShaderObject) myOutImageShader;

  //! OpenGL/GLSL ray-tracing shader program.
  Handle(OpenGl_ShaderProgram) myRaytraceProgram;
  //! OpenGL/GLSL adaptive-AA shader program.
  Handle(OpenGl_ShaderProgram) myPostFSAAProgram;
  //! OpenGL/GLSL program for displaying texture.
  Handle(OpenGl_ShaderProgram) myOutImageProgram;

  //! Texture buffer of data records of bottom-level BVH nodes.
  Handle(OpenGl_TextureBuffer) mySceneNodeInfoTexture;
  //! Texture buffer of minimum points of bottom-level BVH nodes.
  Handle(OpenGl_TextureBuffer) mySceneMinPointTexture;
  //! Texture buffer of maximum points of bottom-level BVH nodes.
  Handle(OpenGl_TextureBuffer) mySceneMaxPointTexture;
  //! Texture buffer of transformations of high-level BVH nodes.
  Handle(OpenGl_TextureBuffer) mySceneTransformTexture;

  //! Texture buffer of vertex coords.
  Handle(OpenGl_TextureBuffer) myGeometryVertexTexture;
  //! Texture buffer of vertex normals.
  Handle(OpenGl_TextureBuffer) myGeometryNormalTexture;
  //! Texture buffer of vertex UV coords.
  Handle(OpenGl_TextureBuffer) myGeometryTexCrdTexture;
  //! Texture buffer of triangle indices.
  Handle(OpenGl_TextureBuffer) myGeometryTriangTexture;

  //! Texture buffer of material properties.
  Handle(OpenGl_TextureBuffer) myRaytraceMaterialTexture;
  //! Texture buffer of light source properties.
  Handle(OpenGl_TextureBuffer) myRaytraceLightSrcTexture;

  //! 1st framebuffer (FBO) to perform adaptive FSAA.
  //! Used in compatibility mode (no adaptive sampling).
  Handle(OpenGl_FrameBuffer) myRaytraceFBO1[2];
  //! 2nd framebuffer (FBO) to perform adaptive FSAA.
  //! Used in compatibility mode (no adaptive sampling).
  Handle(OpenGl_FrameBuffer) myRaytraceFBO2[2];

  //! Output textures (2 textures are used in stereo mode).
  //! Used if adaptive screen sampling is activated.
  Handle(OpenGl_Texture) myRaytraceOutputTexture[2];

  //! Texture containing per-tile visual error estimation (2 textures are used in stereo mode).
  //! Used if adaptive screen sampling is activated.
  Handle(OpenGl_Texture) myRaytraceVisualErrorTexture[2];
  //! Texture containing offsets of sampled screen tiles (2 textures are used in stereo mode).
  //! Used if adaptive screen sampling is activated.
  Handle(OpenGl_Texture) myRaytraceTileOffsetsTexture[2];
  //! Texture containing amount of extra per-tile samples (2 textures are used in stereo mode).
  //! Used if adaptive screen sampling is activated.
  Handle(OpenGl_Texture) myRaytraceTileSamplesTexture[2];

  //! Vertex buffer (VBO) for drawing dummy quad.
  OpenGl_VertexBuffer myRaytraceScreenQuad;

  //! Cached locations of frequently used uniform variables.
  Standard_Integer myUniformLocations[2][OpenGl_RT_NbVariables];

  //! State of OpenGL structures reflected to ray-tracing.
  std::map<const OpenGl_Structure*, StructState> myStructureStates;

  //! PrimitiveArray to TriangleSet map for scene partial update.
  std::map<Standard_Size, OpenGl_TriangleSet*> myArrayToTrianglesMap;

  //! Set of IDs of non-raytracable elements (to detect updates).
  std::set<Standard_Integer> myNonRaytraceStructureIDs;

  //! Marks if environment map should be updated.
  Standard_Boolean myToUpdateEnvironmentMap;

  //! State of OpenGL layer list.
  Standard_Size myRaytraceLayerListState;

  //! Number of accumulated frames (for progressive rendering).
  Standard_Integer myAccumFrames;

  //! Stored ray origins used for detection of camera movements.
  OpenGl_Vec3 myPreviousOrigins[3];

  //! Bullard RNG to produce random sequence.
  math_BullardGenerator myRNG;

  //! Tool object for sampling screen tiles in PT mode.
  OpenGl_TileSampler myTileSampler;

  //! Camera position used for projective mode
  OpenGl_Vec3 myEyeOrig;

  //! Camera view direction used for projective mode
  OpenGl_Vec3 myEyeView;

  //! Camera's screen vertical direction used for projective mode
  OpenGl_Vec3 myEyeVert;

  //! Camera's screen horizontal direction used for projective mode
  OpenGl_Vec3 myEyeSide;

  //! Camera's screen size used for projective mode
  OpenGl_Vec2 myEyeSize;

  //! Aperture radius of camera on previous frame used for depth-of-field (path tracing)
  float myPrevCameraApertureRadius;

  //! Focal distance of camera on previous frame used for depth-of-field (path tracing)
  float myPrevCameraFocalPlaneDist;

public:

  DEFINE_STANDARD_ALLOC
  DEFINE_STANDARD_RTTIEXT(OpenGl_View,Graphic3d_CView) // Type definition

  friend class OpenGl_GraphicDriver;
  friend class OpenGl_Workspace;
  friend class OpenGl_LayerList;
  friend class OpenGl_FrameStats;
};

#endif // _OpenGl_View_Header
