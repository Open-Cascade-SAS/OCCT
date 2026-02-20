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
#include <TCollection_AsciiString.hxx>

#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>

class OpenGl_BackgroundArray;
class OpenGl_DepthPeeling;
class OpenGl_PBREnvironment;
struct OpenGl_RaytraceMaterial;
class OpenGl_ShadowMap;
class OpenGl_ShadowMapArray;
class OpenGl_ShaderObject;
class OpenGl_TextureBuffer;
class OpenGl_Workspace;

//! Implementation of OpenGl view.
class OpenGl_View : public Graphic3d_CView
{

public:
  //! Constructor.
  Standard_EXPORT OpenGl_View(const occ::handle<Graphic3d_StructureManager>& theMgr,
                              const occ::handle<OpenGl_GraphicDriver>&       theDriver,
                              const occ::handle<OpenGl_Caps>&                theCaps,
                              OpenGl_StateCounter*                           theCounter);

  //! Default destructor.
  Standard_EXPORT ~OpenGl_View() override;

  //! Release OpenGL resources.
  Standard_EXPORT virtual void ReleaseGlResources(const occ::handle<OpenGl_Context>& theCtx);

  //! Deletes and erases the view.
  Standard_EXPORT void Remove() override;

  //! @param theDrawToFrontBuffer Advanced option to modify rendering mode:
  //! 1. TRUE.  Drawing immediate mode structures directly to the front buffer over the scene image.
  //! Fast, so preferred for interactive work (used by default).
  //! However these extra drawings will be missed in image dump since it is performed from back
  //! buffer. Notice that since no pre-buffering used the V-Sync will be ignored and rendering could
  //! be seen in run-time (in case of slow hardware) and/or tearing may appear. So this is strongly
  //! recommended to draw only simple (fast) structures.
  //! 2. FALSE. Drawing immediate mode structures to the back buffer.
  //! The complete scene is redrawn first, so this mode is slower if scene contains complex data
  //! and/or V-Sync is turned on. But it works in any case and is especially useful for view dump
  //! because the dump image is read from the back buffer.
  //! @return previous mode.
  Standard_EXPORT bool SetImmediateModeDrawToFront(const bool theDrawToFrontBuffer) override;

  //! Creates and maps rendering window to the view.
  Standard_EXPORT void SetWindow(const occ::handle<Graphic3d_CView>& theParentVIew,
                                 const occ::handle<Aspect_Window>&   theWindow,
                                 const Aspect_RenderingContext       theContext) override;

  //! Returns window associated with the view.
  Standard_EXPORT occ::handle<Aspect_Window> Window() const override;

  //! Returns True if the window associated to the view is defined.
  bool IsDefined() const override { return !myWindow.IsNull(); }

  //! Handle changing size of the rendering window.
  Standard_EXPORT void Resized() override;

  //! Redraw content of the view.
  Standard_EXPORT void Redraw() override;

  //! Redraw immediate content of the view.
  Standard_EXPORT void RedrawImmediate() override;

  //! Marks BVH tree for given priority list as dirty and marks primitive set for rebuild.
  Standard_EXPORT void Invalidate() override;

  //! Return true if view content cache has been invalidated.
  bool IsInvalidated() override { return !myBackBufferRestored; }

  //! Dump active rendering buffer into specified memory buffer.
  //! In Ray-Tracing allow to get a raw HDR buffer using Graphic3d_BT_RGB_RayTraceHdrLeft buffer
  //! type, only Left view will be dumped ignoring stereoscopic parameter.
  Standard_EXPORT bool BufferDump(Image_PixMap&               theImage,
                                  const Graphic3d_BufferType& theBufferType) override;

  //! Dumps the graphical contents of a shadowmap framebuffer into an image.
  //! @param theImage the image to store the shadow map.
  //! @param[in] theLightName  name of the light used to generate the shadow map.
  Standard_EXPORT bool ShadowMapDump(Image_PixMap&                  theImage,
                                     const TCollection_AsciiString& theLightName) override;

  //! Marks BVH tree and the set of BVH primitives of correspondent priority list with id theLayerId
  //! as outdated.
  Standard_EXPORT void InvalidateBVHData(const Graphic3d_ZLayerId theLayerId) override;

  //! Add a layer to the view.
  //! @param[in] theNewLayerId  id of new layer, should be > 0 (negative values are reserved for
  //! default layers).
  //! @param[in] theSettings    new layer settings
  //! @param[in] theLayerAfter  id of layer to append new layer before
  Standard_EXPORT void InsertLayerBefore(const Graphic3d_ZLayerId        theLayerId,
                                         const Graphic3d_ZLayerSettings& theSettings,
                                         const Graphic3d_ZLayerId        theLayerAfter) override;

  //! Add a layer to the view.
  //! @param[in] theNewLayerId   id of new layer, should be > 0 (negative values are reserved for
  //! default layers).
  //! @param[in] theSettings     new layer settings
  //! @param[in] theLayerBefore  id of layer to append new layer after
  Standard_EXPORT void InsertLayerAfter(const Graphic3d_ZLayerId        theNewLayerId,
                                        const Graphic3d_ZLayerSettings& theSettings,
                                        const Graphic3d_ZLayerId        theLayerBefore) override;

  //! Remove a z layer with the given ID.
  Standard_EXPORT void RemoveZLayer(const Graphic3d_ZLayerId theLayerId) override;

  //! Sets the settings for a single Z layer of specified view.
  Standard_EXPORT void SetZLayerSettings(const Graphic3d_ZLayerId        theLayerId,
                                         const Graphic3d_ZLayerSettings& theSettings) override;

  //! Returns the maximum Z layer ID.
  //! First layer ID is Graphic3d_ZLayerId_Default, last ID is ZLayerMax().
  Standard_EXPORT int ZLayerMax() const override;

  //! Returns the list of layers.
  Standard_EXPORT const NCollection_List<occ::handle<Graphic3d_Layer>>& Layers() const override;

  //! Returns layer with given ID or NULL if undefined.
  Standard_EXPORT occ::handle<Graphic3d_Layer> Layer(
    const Graphic3d_ZLayerId theLayerId) const override;

  //! Returns the bounding box of all structures displayed in the view.
  //! If theToIncludeAuxiliary is TRUE, then the boundary box also includes minimum and maximum
  //! limits of graphical elements forming parts of infinite and other auxiliary structures.
  //! @param theToIncludeAuxiliary consider also auxiliary presentations (with infinite flag or with
  //! trihedron transformation persistence)
  //! @return computed bounding box
  Standard_EXPORT Bnd_Box MinMaxValues(const bool theToIncludeAuxiliary) const override;

  //! Returns pointer to an assigned framebuffer object.
  Standard_EXPORT occ::handle<Standard_Transient> FBO() const override;

  //! Sets framebuffer object for offscreen rendering.
  Standard_EXPORT void SetFBO(const occ::handle<Standard_Transient>& theFbo) override;

  //! Generate offscreen FBO in the graphic library.
  //! If not supported on hardware returns NULL.
  Standard_EXPORT occ::handle<Standard_Transient> FBOCreate(const int theWidth,
                                                            const int theHeight) override;

  //! Remove offscreen FBO from the graphic library
  Standard_EXPORT void FBORelease(occ::handle<Standard_Transient>& theFbo) override;

  //! Read offscreen FBO configuration.
  Standard_EXPORT void FBOGetDimensions(const occ::handle<Standard_Transient>& theFbo,
                                        int&                                   theWidth,
                                        int&                                   theHeight,
                                        int&                                   theWidthMax,
                                        int& theHeightMax) override;

  //! Change offscreen FBO viewport.
  Standard_EXPORT void FBOChangeViewport(const occ::handle<Standard_Transient>& theFbo,
                                         const int                              theWidth,
                                         const int                              theHeight) override;

  //! Returns additional buffers for depth peeling OIT.
  const occ::handle<OpenGl_DepthPeeling>& DepthPeelingFbos() const { return myDepthPeelingFbos; }

public:
  //! Returns gradient background fill colors.
  Standard_EXPORT Aspect_GradientBackground GradientBackground() const override;

  //! Sets gradient background fill colors.
  Standard_EXPORT void SetGradientBackground(
    const Aspect_GradientBackground& theBackground) override;

  //! Sets image texture or environment cubemap as background.
  //! @param[in] theTextureMap  source to set a background;
  //!                           should be either Graphic3d_Texture2D or Graphic3d_CubeMap
  //! @param[in] theToUpdatePBREnv  defines whether IBL maps will be generated or not
  //!                               (see GeneratePBREnvironment())
  Standard_EXPORT void SetBackgroundImage(const occ::handle<Graphic3d_TextureMap>& theTextureMap,
                                          bool theToUpdatePBREnv = true) override;

  //! Sets environment texture for the view.
  Standard_EXPORT void SetTextureEnv(
    const occ::handle<Graphic3d_TextureEnv>& theTextureEnv) override;

  //! Returns background image fill style.
  Standard_EXPORT Aspect_FillMethod BackgroundImageStyle() const override;

  //! Sets background image fill style.
  Standard_EXPORT void SetBackgroundImageStyle(const Aspect_FillMethod theFillStyle) override;

  //! Enables or disables IBL (Image Based Lighting) from background cubemap.
  //! Has no effect if PBR is not used.
  //! @param[in] theToEnableIBL enable or disable IBL from background cubemap
  //! @param[in] theToUpdate redraw the view
  Standard_EXPORT void SetImageBasedLighting(bool theToEnableIBL) override;

  //! Returns number of mipmap levels used in specular IBL map.
  //! 0 if PBR environment is not created.
  Standard_EXPORT unsigned int SpecIBLMapLevels() const;

  //! Returns local camera origin currently set for rendering, might be modified during rendering.
  const gp_XYZ& LocalOrigin() const { return myLocalOrigin; }

  //! Setup local camera origin currently set for rendering.
  Standard_EXPORT void SetLocalOrigin(const gp_XYZ& theOrigin);

  //! Returns list of lights of the view.
  const occ::handle<Graphic3d_LightSet>& Lights() const override { return myLights; }

  //! Sets list of lights for the view.
  void SetLights(const occ::handle<Graphic3d_LightSet>& theLights) override
  {
    myLights               = theLights;
    myCurrLightSourceState = myStateCounter->Increment();
  }

  //! Returns list of clip planes set for the view.
  const occ::handle<Graphic3d_SequenceOfHClipPlane>& ClipPlanes() const override
  {
    return myClipPlanes;
  }

  //! Sets list of clip planes for the view.
  void SetClipPlanes(const occ::handle<Graphic3d_SequenceOfHClipPlane>& thePlanes) override
  {
    myClipPlanes = thePlanes;
  }

  //! Fill in the dictionary with diagnostic info.
  //! Should be called within rendering thread.
  //!
  //! This API should be used only for user output or for creating automated reports.
  //! The format of returned information (e.g. key-value layout)
  //! is NOT part of this API and can be changed at any time.
  //! Thus application should not parse returned information to weed out specific parameters.
  Standard_EXPORT void DiagnosticInformation(
    NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theDict,
    Graphic3d_DiagnosticInfo theFlags) const override;

  //! Returns string with statistic performance info.
  Standard_EXPORT TCollection_AsciiString StatisticInformation() const override;

  //! Fills in the dictionary with statistic performance info.
  Standard_EXPORT void StatisticInformation(
    NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theDict)
    const override;

public:
  //! Returns background color.
  const Quantity_ColorRGBA& BackgroundColor() const { return myBgColor; }

  //! Change graduated trihedron.
  OpenGl_GraduatedTrihedron& ChangeGraduatedTrihedron() { return myGraduatedTrihedron; }

  void SetTextureEnv(const occ::handle<OpenGl_Context>&       theCtx,
                     const occ::handle<Graphic3d_TextureEnv>& theTexture);

  void SetBackgroundTextureStyle(const Aspect_FillMethod FillStyle);

  void SetBackgroundGradient(const Quantity_Color&           AColor1,
                             const Quantity_Color&           AColor2,
                             const Aspect_GradientFillMethod AType);

  void SetBackgroundGradientType(const Aspect_GradientFillMethod AType);

  //! Returns list of OpenGL Z-layers.
  const OpenGl_LayerList& LayerList() const { return myZLayers; }

  //! Returns OpenGL window implementation.
  const occ::handle<OpenGl_Window>& GlWindow() const { return myWindow; }

  //! Returns OpenGL environment map.
  const occ::handle<OpenGl_TextureSet>& GlTextureEnv() const { return myTextureEnv; }

  //! Returns selector for BVH tree, providing a possibility to store information
  //! about current view volume and to detect which objects are overlapping it.
  const Graphic3d_CullingTool& BVHTreeSelector() const { return myBVHSelector; }

  //! Returns true if there are immediate structures to display
  bool HasImmediateStructures() const { return myZLayers.NbImmediateStructures() != 0; }

public: //! @name obsolete Graduated Trihedron functionality
  //! Displays Graduated Trihedron.
  Standard_EXPORT void GraduatedTrihedronDisplay(
    const Graphic3d_GraduatedTrihedron& theTrihedronData) override;

  //! Erases Graduated Trihedron.
  Standard_EXPORT void GraduatedTrihedronErase() override;

  //! Sets minimum and maximum points of scene bounding box for Graduated Trihedron stored in
  //! graphic view object.
  //! @param[in] theMin  the minimum point of scene.
  //! @param[in] theMax  the maximum point of scene.
  Standard_EXPORT void GraduatedTrihedronMinMaxValues(
    const NCollection_Vec3<float> theMin,
    const NCollection_Vec3<float> theMax) override;

  //! Returns necessity to flip OY in projection matrix
  bool ToFlipOutput() const override { return myToFlipOutput; }

  //! Sets state of flip OY necessity in projection matrix
  void SetToFlipOutput(const bool theFlip) override { myToFlipOutput = theFlip; }

protected: //! @name Internal methods for managing GL resources
  //! Initializes OpenGl resource for environment texture.
  void initTextureEnv(const occ::handle<OpenGl_Context>& theContext);

protected: //! @name low-level redrawing sub-routines
  //! Prepare frame buffers for rendering.
  Standard_EXPORT virtual bool prepareFrameBuffers(Graphic3d_Camera::Projection& theProj);

  //! Redraws view for the given monographic camera projection, or left/right eye.
  Standard_EXPORT virtual void redraw(const Graphic3d_Camera::Projection theProjection,
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
  Standard_EXPORT virtual bool redrawImmediate(const Graphic3d_Camera::Projection theProjection,
                                               OpenGl_FrameBuffer*                theReadFbo,
                                               OpenGl_FrameBuffer*                theDrawFbo,
                                               OpenGl_FrameBuffer*                theOitAccumFbo,
                                               const bool theIsPartialUpdate = false);

  //! Blit subviews into this view.
  Standard_EXPORT bool blitSubviews(const Graphic3d_Camera::Projection theProjection,
                                    OpenGl_FrameBuffer*                theDrawFbo);

  //! Blit image from/to specified buffers.
  Standard_EXPORT bool blitBuffers(OpenGl_FrameBuffer* theReadFbo,
                                   OpenGl_FrameBuffer* theDrawFbo,
                                   const bool          theToFlip = false);

  //! Setup default FBO.
  Standard_EXPORT void bindDefaultFbo(OpenGl_FrameBuffer* theCustomFbo = nullptr);

protected: //! @name Rendering of GL graphics (with prepared drawing buffer).
  //! Renders the graphical contents of the view into the prepared shadowmap framebuffer.
  //! @param[in] theShadowMap  the framebuffer for rendering shadowmap.
  Standard_EXPORT virtual void renderShadowMap(const occ::handle<OpenGl_ShadowMap>& theShadowMap);

  //! Renders the graphical contents of the view into the prepared window or framebuffer.
  //! @param[in] theProjection  the projection that should be used for rendering.
  //! @param[in] theReadDrawFbo  the framebuffer for rendering graphics.
  //! @param[in] theOitAccumFbo  the framebuffer for accumulating color and coverage for OIT
  //! process.
  //! @param[in] theToDrawImmediate  the flag indicates whether the rendering performs in immediate
  //! mode.
  Standard_EXPORT virtual void render(Graphic3d_Camera::Projection theProjection,
                                      OpenGl_FrameBuffer*          theReadDrawFbo,
                                      OpenGl_FrameBuffer*          theOitAccumFbo,
                                      const bool                   theToDrawImmediate);

  //! Renders the graphical scene.
  //! @param[in] theProjection  the projection that is used for rendering.
  //! @param[in] theReadDrawFbo  the framebuffer for rendering graphics.
  //! @param[in] theOitAccumFbo  the framebuffer for accumulating color and coverage for OIT
  //! process.
  //! @param[in] theToDrawImmediate  the flag indicates whether the rendering performs in immediate
  //! mode.
  Standard_EXPORT virtual void renderScene(Graphic3d_Camera::Projection theProjection,
                                           OpenGl_FrameBuffer*          theReadDrawFbo,
                                           OpenGl_FrameBuffer*          theOitAccumFbo,
                                           const bool                   theToDrawImmediate);

  //! Draw background (gradient / image / cubemap)
  Standard_EXPORT virtual void drawBackground(const occ::handle<OpenGl_Workspace>& theWorkspace,
                                              Graphic3d_Camera::Projection         theProjection);

  //! Render set of structures presented in the view.
  //! @param[in] theProjection  the projection that is used for rendering.
  //! @param[in] theReadDrawFbo  the framebuffer for rendering graphics.
  //! @param[in] theOitAccumFbo  the framebuffer for accumulating color and coverage for OIT
  //! process.
  //! @param[in] theToDrawImmediate  the flag indicates whether the rendering performs in immediate
  //! mode.
  Standard_EXPORT virtual void renderStructs(Graphic3d_Camera::Projection theProjection,
                                             OpenGl_FrameBuffer*          theReadDrawFbo,
                                             OpenGl_FrameBuffer*          theOitAccumFbo,
                                             const bool                   theToDrawImmediate);

  //! Renders trihedron.
  void renderTrihedron(const occ::handle<OpenGl_Workspace>& theWorkspace);

  //! Renders frame statistics.
  void renderFrameStats();

private:
  //! Adds the structure to display lists of the view.
  Standard_EXPORT void displayStructure(const occ::handle<Graphic3d_CStructure>& theStructure,
                                        const Graphic3d_DisplayPriority thePriority) override;

  //! Erases the structure from display lists of the view.
  Standard_EXPORT void eraseStructure(
    const occ::handle<Graphic3d_CStructure>& theStructure) override;

  //! Change Z layer of a structure already presented in view.
  Standard_EXPORT void changeZLayer(const occ::handle<Graphic3d_CStructure>& theCStructure,
                                    const Graphic3d_ZLayerId theNewLayerId) override;

  //! Changes the priority of a structure within its Z layer in the specified view.
  Standard_EXPORT void changePriority(const occ::handle<Graphic3d_CStructure>& theCStructure,
                                      const Graphic3d_DisplayPriority theNewPriority) override;

private:
  //! Release sRGB resources (frame-buffers, textures, etc.).
  void releaseSrgbResources(const occ::handle<OpenGl_Context>& theCtx);

  //! Copy content of Back buffer to the Front buffer.
  bool copyBackToFront();

  //! Initialize blit quad.
  OpenGl_VertexBuffer* initBlitQuad(const bool theToFlip);

  //! Blend together views pair into stereo image.
  void drawStereoPair(OpenGl_FrameBuffer* theDrawFbo);

  //! Check and update OIT compatibility with current OpenGL context's state.
  bool checkOitCompatibility(const occ::handle<OpenGl_Context>& theGlContext, const bool theMSAA);

protected:
  OpenGl_GraphicDriver*         myDriver;
  occ::handle<OpenGl_Window>    myWindow;
  occ::handle<OpenGl_Workspace> myWorkspace;
  occ::handle<OpenGl_Caps>      myCaps;
  bool                          myWasRedrawnGL;

  occ::handle<Graphic3d_SequenceOfHClipPlane> myClipPlanes;
  gp_XYZ                                      myLocalOrigin;
  occ::handle<OpenGl_FrameBuffer>             myFBO;
  bool                                        myToShowGradTrihedron;
  Graphic3d_GraduatedTrihedron                myGTrihedronData;

  occ::handle<Graphic3d_LightSet> myNoShadingLight;
  occ::handle<Graphic3d_LightSet> myLights;
  // clang-format off
  OpenGl_LayerList                myZLayers; //!< main list of displayed structure, sorted by layers
  // clang-format on

  Graphic3d_WorldViewProjState myWorldViewProjState; //!< camera modification state
  OpenGl_StateCounter*         myStateCounter;
  size_t                       myCurrLightSourceState;
  size_t                       myLightsRevision;

  typedef std::pair<size_t, size_t> StateInfo;

  StateInfo myLastOrientationState;
  StateInfo myLastViewMappingState;
  StateInfo myLastLightSourceState;

  //! Is needed for selection of overlapping objects and storage of the current view volume
  Graphic3d_CullingTool myBVHSelector;

  OpenGl_GraduatedTrihedron myGraduatedTrihedron;
  OpenGl_FrameStatsPrs      myFrameStatsPrs;

  //! Framebuffers for OpenGL output.
  occ::handle<OpenGl_FrameBuffer> myOpenGlFBO;
  occ::handle<OpenGl_FrameBuffer> myOpenGlFBO2;

protected: //! @name Rendering properties
  //! Two framebuffers (left and right views) store cached main presentation
  //! of the view (without presentation of immediate layers).
  int   mySRgbState;                              //!< track sRGB state
  GLint myFboColorFormat;                         //!< sized format for color attachments
                                                  // clang-format off
  GLint                      myFboDepthFormat;        //!< sized format for depth-stencil attachments
  NCollection_Vector<int>        myFboOitColorConfig;     //!< selected color format configuration for OIT color attachments
  occ::handle<OpenGl_FrameBuffer> myMainSceneFbos[2];
  occ::handle<OpenGl_FrameBuffer> myMainSceneFbosOit[2];      //!< Additional buffers for transparent draw of main layer.
  occ::handle<OpenGl_FrameBuffer> myImmediateSceneFbos[2];    //!< Additional buffers for immediate layer in stereo mode.
  occ::handle<OpenGl_FrameBuffer> myImmediateSceneFbosOit[2]; //!< Additional buffers for transparency draw of immediate layer.
  occ::handle<OpenGl_FrameBuffer> myXrSceneFbo;               //!< additional FBO (without MSAA) for submitting to XR
  occ::handle<OpenGl_DepthPeeling>   myDepthPeelingFbos;   //!< additional buffers for depth peeling
  occ::handle<OpenGl_ShadowMapArray> myShadowMaps;         //!< additional FBOs for shadow map rendering
  OpenGl_VertexBuffer        myFullScreenQuad;        //!< Vertices for full-screen quad rendering.
  OpenGl_VertexBuffer        myFullScreenQuadFlip;
  bool           myToFlipOutput;          //!< Flag to draw result image upside-down
  unsigned int               myFrameCounter;          //!< redraw counter, for debugging
  bool           myHasFboBlit;            //!< disable FBOs on failure
  bool           myToDisableOIT;          //!< disable OIT on failure
  bool           myToDisableOITMSAA;      //!< disable OIT with MSAA on failure
  bool           myToDisableMSAA;         //!< disable MSAA after failure
  bool           myTransientDrawToFront; //!< optimization flag for immediate mode (to render directly to the front buffer)
  bool           myBackBufferRestored;
  bool           myIsImmediateDrawn;     //!< flag indicates that immediate mode buffer contains some data

protected: //! @name Background parameters

  OpenGl_Aspects*            myTextureParams;                     //!< Stores texture and its parameters for textured background
  OpenGl_Aspects*            myCubeMapParams;                     //!< Stores cubemap and its parameters for cubemap background
  OpenGl_Aspects*            myColoredQuadParams;                 //!< Stores parameters for gradient (corner mode) background
  OpenGl_BackgroundArray*    myBackgrounds[Graphic3d_TypeOfBackground_NB]; //!< Array of primitive arrays of different background types
                                                  // clang-format on
  occ::handle<OpenGl_TextureSet> myTextureEnv;
  occ::handle<OpenGl_Texture>    mySkydomeTexture;

protected: //! @name methods related to skydome background
  //! Generates skydome cubemap.
  Standard_EXPORT void updateSkydomeBg(const occ::handle<OpenGl_Context>& theCtx);

protected: //! @name methods related to PBR
  //! Checks whether PBR is available.
  Standard_EXPORT bool checkPBRAvailability() const;

  //! Generates IBL maps used in PBR pipeline.
  //! If background cubemap is not set clears all IBL maps.
  Standard_EXPORT void updatePBREnvironment(const occ::handle<OpenGl_Context>& theCtx);

protected: //! @name fields and types related to PBR
  //! State of PBR environment.
  enum PBREnvironmentState
  {
    OpenGl_PBREnvState_NONEXISTENT,
    OpenGl_PBREnvState_UNAVAILABLE, // indicates failed try to create PBR environment
    OpenGl_PBREnvState_CREATED
  };

  occ::handle<OpenGl_PBREnvironment> myPBREnvironment; //!< manager of IBL maps used in PBR pipeline
  PBREnvironmentState                myPBREnvState;    //!< state of PBR environment
  bool                               myPBREnvRequest;  //!< update PBR environment

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
    OpenGl_RT_OutputImage      = 0,
    OpenGl_RT_VisualErrorImage = 1,
    OpenGl_RT_TileOffsetsImage = 2,
    OpenGl_RT_TileSamplesImage = 3
  };

  //! Tool class for management of shader sources.
  class ShaderSource
  {
  public:
    //! Creates new uninitialized shader source.
    ShaderSource()
    {
      //
    }

  public:
    //! Returns error description in case of load fail.
    const TCollection_AsciiString& ErrorDescription() const { return myError; }

    //! Returns prefix to insert before the source.
    const TCollection_AsciiString& Prefix() const { return myPrefix; }

    //! Sets prefix to insert before the source.
    void SetPrefix(const TCollection_AsciiString& thePrefix) { myPrefix = thePrefix; }

    //! Returns shader source combined with prefix.
    TCollection_AsciiString Source(const occ::handle<OpenGl_Context>& theCtx,
                                   const GLenum                       theType) const;

    //! Loads shader source from specified files.
    bool LoadFromFiles(
      const TCollection_AsciiString* theFileNames,
      const TCollection_AsciiString& thePrefix = TCollection_AsciiString::EmptyString());

    //! Loads shader source from specified strings.
    bool LoadFromStrings(
      const TCollection_AsciiString* theStrings,
      const TCollection_AsciiString& thePrefix = TCollection_AsciiString::EmptyString());

  private:
    TCollection_AsciiString mySource; //!< Source string of the shader object
    TCollection_AsciiString myPrefix; //!< Prefix to insert before the source
    TCollection_AsciiString myError;  //!< error state
  };

  //! Default ray-tracing depth.
  static constexpr int THE_DEFAULT_NB_BOUNCES = 3;

  //! Default size of traversal stack.
  static constexpr int THE_DEFAULT_STACK_SIZE = 10;

  //! Compile-time ray-tracing parameters.
  struct RaytracingParams
  {
    //! Actual size of traversal stack in shader program.
    int StackSize;

    //! Actual ray-tracing depth (number of ray bounces).
    int NbBounces;

    //! Define depth computation
    bool IsZeroToOneDepth;

    //! Enables/disables light propagation through transparent media.
    bool TransparentShadows;

    //! Enables/disables global illumination (GI) effects.
    bool GlobalIllumination;

    //! Enables/disables the use of OpenGL bindless textures.
    bool UseBindlessTextures;

    //! Enables/disables two-sided BSDF models instead of one-sided.
    bool TwoSidedBsdfModels;

    //! Enables/disables adaptive screen sampling for path tracing.
    bool AdaptiveScreenSampling;

    //! Enables/disables 1-pass atomic mode for AdaptiveScreenSampling.
    bool AdaptiveScreenSamplingAtomic;

    //! Enables/disables environment map for background.
    bool UseEnvMapForBackground;

    //! Enables/disables normal map ignoring during path tracing.
    bool ToIgnoreNormalMap;

    //! Maximum radiance value used for clamping radiance estimation.
    float RadianceClampingValue;

    //! Enables/disables depth-of-field effect (path tracing, perspective camera).
    bool DepthOfField;

    //! Enables/disables cubemap background.
    bool CubemapForBack;

    //! Tone mapping method for path tracing.
    Graphic3d_ToneMappingMethod ToneMappingMethod;

    //! Creates default compile-time ray-tracing parameters.
    RaytracingParams()
        : StackSize(THE_DEFAULT_STACK_SIZE),
          NbBounces(THE_DEFAULT_NB_BOUNCES),
          IsZeroToOneDepth(false),
          TransparentShadows(false),
          GlobalIllumination(false),
          UseBindlessTextures(false),
          TwoSidedBsdfModels(false),
          AdaptiveScreenSampling(false),
          AdaptiveScreenSamplingAtomic(false),
          UseEnvMapForBackground(false),
          ToIgnoreNormalMap(false),
          RadianceClampingValue(30.0),
          DepthOfField(false),
          CubemapForBack(false),
          ToneMappingMethod(Graphic3d_ToneMappingMethod_Disabled)
    {
    }
  };

  //! Describes state of OpenGL structure.
  struct StructState
  {
    size_t StructureState;
    size_t InstancedState;

    //! Creates new structure state.
    StructState(const size_t theStructureState = 0, const size_t theInstancedState = 0)
        : StructureState(theStructureState),
          InstancedState(theInstancedState)
    {
      //
    }

    //! Creates new structure state.
    StructState(const OpenGl_Structure* theStructure)
    {
      StructureState = theStructure->ModificationState();

      InstancedState = theStructure->InstancedStructure() != nullptr
                         ? theStructure->InstancedStructure()->ModificationState()
                         : 0;
    }
  };

protected: //! @name methods related to ray-tracing
  //! Updates 3D scene geometry for ray-tracing.
  bool updateRaytraceGeometry(const RaytraceUpdateMode           theMode,
                              const int                          theViewId,
                              const occ::handle<OpenGl_Context>& theGlContext);

  //! Updates 3D scene light sources for ray-tracing.
  bool updateRaytraceLightSources(const NCollection_Mat4<float>&     theInvModelView,
                                  const occ::handle<OpenGl_Context>& theGlContext);

  //! Checks to see if the OpenGL structure is modified.
  bool toUpdateStructure(const OpenGl_Structure* theStructure);

  //! Adds OpenGL structure to ray-traced scene geometry.
  bool addRaytraceStructure(const OpenGl_Structure*            theStructure,
                            const occ::handle<OpenGl_Context>& theGlContext);

  //! Adds OpenGL groups to ray-traced scene geometry.
  bool addRaytraceGroups(const OpenGl_Structure*            theStructure,
                         const OpenGl_RaytraceMaterial&     theStructMat,
                         const occ::handle<TopLoc_Datum3D>& theTrsf,
                         const occ::handle<OpenGl_Context>& theGlContext);

  //! Creates ray-tracing material properties.
  OpenGl_RaytraceMaterial convertMaterial(const OpenGl_Aspects*              theAspect,
                                          const occ::handle<OpenGl_Context>& theGlContext);

  //! Adds OpenGL primitive array to ray-traced scene geometry.
  occ::handle<OpenGl_TriangleSet> addRaytracePrimitiveArray(
    const OpenGl_PrimitiveArray*   theArray,
    const int                      theMatID,
    const NCollection_Mat4<float>* theTrans);

  //! Adds vertex indices from OpenGL primitive array to ray-traced scene geometry.
  bool addRaytraceVertexIndices(OpenGl_TriangleSet&          theSet,
                                const int                    theMatID,
                                const int                    theCount,
                                const int                    theOffset,
                                const OpenGl_PrimitiveArray& theArray);

  //! Adds OpenGL triangle array to ray-traced scene geometry.
  bool addRaytraceTriangleArray(OpenGl_TriangleSet&                       theSet,
                                const int                                 theMatID,
                                const int                                 theCount,
                                const int                                 theOffset,
                                const occ::handle<Graphic3d_IndexBuffer>& theIndices);

  //! Adds OpenGL triangle fan array to ray-traced scene geometry.
  bool addRaytraceTriangleFanArray(OpenGl_TriangleSet&                       theSet,
                                   const int                                 theMatID,
                                   const int                                 theCount,
                                   const int                                 theOffset,
                                   const occ::handle<Graphic3d_IndexBuffer>& theIndices);

  //! Adds OpenGL triangle strip array to ray-traced scene geometry.
  bool addRaytraceTriangleStripArray(OpenGl_TriangleSet&                       theSet,
                                     const int                                 theMatID,
                                     const int                                 theCount,
                                     const int                                 theOffset,
                                     const occ::handle<Graphic3d_IndexBuffer>& theIndices);

  //! Adds OpenGL quadrangle array to ray-traced scene geometry.
  bool addRaytraceQuadrangleArray(OpenGl_TriangleSet&                       theSet,
                                  const int                                 theMatID,
                                  const int                                 theCount,
                                  const int                                 theOffset,
                                  const occ::handle<Graphic3d_IndexBuffer>& theIndices);

  //! Adds OpenGL quadrangle strip array to ray-traced scene geometry.
  bool addRaytraceQuadrangleStripArray(OpenGl_TriangleSet&                       theSet,
                                       const int                                 theMatID,
                                       const int                                 theCount,
                                       const int                                 theOffset,
                                       const occ::handle<Graphic3d_IndexBuffer>& theIndices);

  //! Adds OpenGL polygon array to ray-traced scene geometry.
  bool addRaytracePolygonArray(OpenGl_TriangleSet&                       theSet,
                               const int                                 theMatID,
                               const int                                 theCount,
                               const int                                 theOffset,
                               const occ::handle<Graphic3d_IndexBuffer>& theIndices);

  //! Uploads ray-trace data to the GPU.
  bool uploadRaytraceData(const occ::handle<OpenGl_Context>& theGlContext);

  //! Generates shader prefix based on current ray-tracing options.
  TCollection_AsciiString generateShaderPrefix(
    const occ::handle<OpenGl_Context>& theGlContext) const;

  //! Performs safe exit when shaders initialization fails.
  bool safeFailBack(const TCollection_ExtendedString&  theMessage,
                    const occ::handle<OpenGl_Context>& theGlContext);

  //! Loads and compiles shader object from specified source.
  occ::handle<OpenGl_ShaderObject> initShader(const GLenum                       theType,
                                              const ShaderSource&                theSource,
                                              const occ::handle<OpenGl_Context>& theGlContext);

  //! Creates shader program from the given vertex and fragment shaders.
  occ::handle<OpenGl_ShaderProgram> initProgram(
    const occ::handle<OpenGl_Context>&      theGlContext,
    const occ::handle<OpenGl_ShaderObject>& theVertShader,
    const occ::handle<OpenGl_ShaderObject>& theFragShader,
    const TCollection_AsciiString&          theName);

  //! Initializes OpenGL/GLSL shader programs.
  bool initRaytraceResources(const int                          theSizeX,
                             const int                          theSizeY,
                             const occ::handle<OpenGl_Context>& theGlContext);

  //! Releases OpenGL/GLSL shader programs.
  void releaseRaytraceResources(const occ::handle<OpenGl_Context>& theGlContext,
                                const bool                         theToRebuild = false);

  //! Updates auxiliary OpenGL frame buffers.
  bool updateRaytraceBuffers(const int                          theSizeX,
                             const int                          theSizeY,
                             const occ::handle<OpenGl_Context>& theGlContext);

  //! Generates viewing rays for corners of screen quad.
  //! (ray tracing; path tracing for orthographic camera)
  void updateCamera(const NCollection_Mat4<float>& theOrientation,
                    const NCollection_Mat4<float>& theViewMapping,
                    NCollection_Vec3<float>*       theOrigins,
                    NCollection_Vec3<float>*       theDirects,
                    NCollection_Mat4<float>&       theView,
                    NCollection_Mat4<float>&       theUnView);

  //! Generate viewing rays (path tracing, perspective camera).
  void updatePerspCameraPT(const NCollection_Mat4<float>& theOrientation,
                           const NCollection_Mat4<float>& theViewMapping,
                           Graphic3d_Camera::Projection   theProjection,
                           NCollection_Mat4<float>&       theViewPr,
                           NCollection_Mat4<float>&       theUnview,
                           const int                      theWinSizeX,
                           const int                      theWinSizeY);

  //! Binds ray-trace textures to corresponding texture units.
  void bindRaytraceTextures(const occ::handle<OpenGl_Context>& theGlContext, int theStereoView);

  //! Unbinds ray-trace textures from corresponding texture unit.
  void unbindRaytraceTextures(const occ::handle<OpenGl_Context>& theGlContext);

  //! Sets uniform state for the given ray-tracing shader program.
  bool setUniformState(const int                          theProgramId,
                       const int                          theSizeX,
                       const int                          theSizeY,
                       Graphic3d_Camera::Projection       theProjection,
                       const occ::handle<OpenGl_Context>& theGlContext);

  //! Runs ray-tracing shader programs.
  bool runRaytraceShaders(const int                          theSizeX,
                          const int                          theSizeY,
                          Graphic3d_Camera::Projection       theProjection,
                          OpenGl_FrameBuffer*                theReadDrawFbo,
                          const occ::handle<OpenGl_Context>& theGlContext);

  //! Runs classical (Whitted-style) ray-tracing kernel.
  bool runRaytrace(const int                          theSizeX,
                   const int                          theSizeY,
                   Graphic3d_Camera::Projection       theProjection,
                   OpenGl_FrameBuffer*                theReadDrawFbo,
                   const occ::handle<OpenGl_Context>& theGlContext);

  //! Runs path tracing (global illumination) kernel.
  bool runPathtrace(const int                          theSizeX,
                    const int                          theSizeY,
                    Graphic3d_Camera::Projection       theProjection,
                    const occ::handle<OpenGl_Context>& theGlContext);

  //! Runs path tracing (global illumination) kernel.
  bool runPathtraceOut(Graphic3d_Camera::Projection       theProjection,
                       OpenGl_FrameBuffer*                theReadDrawFbo,
                       const occ::handle<OpenGl_Context>& theGlContext);

  //! Redraws the window using OpenGL/GLSL ray-tracing or path tracing.
  bool raytrace(const int                          theSizeX,
                const int                          theSizeY,
                Graphic3d_Camera::Projection       theProjection,
                OpenGl_FrameBuffer*                theReadDrawFbo,
                const occ::handle<OpenGl_Context>& theGlContext);

protected: //! @name fields related to ray-tracing
  //! Result of RT/PT shaders initialization.
  RaytraceInitStatus myRaytraceInitStatus;

  //! Is ray-tracing geometry data valid?
  bool myIsRaytraceDataValid;

  //! True if warning about missing extension GL_ARB_bindless_texture has been displayed.
  bool myIsRaytraceWarnTextures;

  //! 3D scene geometry data for ray-tracing.
  OpenGl_RaytraceGeometry myRaytraceGeometry;

  //! Builder for triangle set.
  opencascade::handle<BVH_Builder<float, 3>> myRaytraceBVHBuilder;

  //! Compile-time ray-tracing parameters.
  RaytracingParams myRaytraceParameters;

  //! Radius of bounding sphere of the scene.
  float myRaytraceSceneRadius;
  //! Scene epsilon to prevent self-intersections.
  float myRaytraceSceneEpsilon;

  //! OpenGL/GLSL source of ray-tracing fragment shader.
  ShaderSource myRaytraceShaderSource;
  //! OpenGL/GLSL source of adaptive-AA fragment shader.
  ShaderSource myPostFSAAShaderSource;
  //! OpenGL/GLSL source of RT/PT display fragment shader.
  ShaderSource myOutImageShaderSource;

  //! OpenGL/GLSL ray-tracing fragment shader.
  occ::handle<OpenGl_ShaderObject> myRaytraceShader;
  //! OpenGL/GLSL adaptive-AA fragment shader.
  occ::handle<OpenGl_ShaderObject> myPostFSAAShader;
  //! OpenGL/GLSL ray-tracing display fragment shader.
  occ::handle<OpenGl_ShaderObject> myOutImageShader;

  //! OpenGL/GLSL ray-tracing shader program.
  occ::handle<OpenGl_ShaderProgram> myRaytraceProgram;
  //! OpenGL/GLSL adaptive-AA shader program.
  occ::handle<OpenGl_ShaderProgram> myPostFSAAProgram;
  //! OpenGL/GLSL program for displaying texture.
  occ::handle<OpenGl_ShaderProgram> myOutImageProgram;

  //! Texture buffer of data records of bottom-level BVH nodes.
  occ::handle<OpenGl_TextureBuffer> mySceneNodeInfoTexture;
  //! Texture buffer of minimum points of bottom-level BVH nodes.
  occ::handle<OpenGl_TextureBuffer> mySceneMinPointTexture;
  //! Texture buffer of maximum points of bottom-level BVH nodes.
  occ::handle<OpenGl_TextureBuffer> mySceneMaxPointTexture;
  //! Texture buffer of transformations of high-level BVH nodes.
  occ::handle<OpenGl_TextureBuffer> mySceneTransformTexture;

  //! Texture buffer of vertex coords.
  occ::handle<OpenGl_TextureBuffer> myGeometryVertexTexture;
  //! Texture buffer of vertex normals.
  occ::handle<OpenGl_TextureBuffer> myGeometryNormalTexture;
  //! Texture buffer of vertex UV coords.
  occ::handle<OpenGl_TextureBuffer> myGeometryTexCrdTexture;
  //! Texture buffer of triangle indices.
  occ::handle<OpenGl_TextureBuffer> myGeometryTriangTexture;

  //! Texture buffer of material properties.
  occ::handle<OpenGl_TextureBuffer> myRaytraceMaterialTexture;
  //! Texture buffer of light source properties.
  occ::handle<OpenGl_TextureBuffer> myRaytraceLightSrcTexture;

  //! 1st framebuffer (FBO) to perform adaptive FSAA.
  //! Used in compatibility mode (no adaptive sampling).
  occ::handle<OpenGl_FrameBuffer> myRaytraceFBO1[2];
  //! 2nd framebuffer (FBO) to perform adaptive FSAA.
  //! Used in compatibility mode (no adaptive sampling).
  occ::handle<OpenGl_FrameBuffer> myRaytraceFBO2[2];

  //! Output textures (2 textures are used in stereo mode).
  //! Used if adaptive screen sampling is activated.
  occ::handle<OpenGl_Texture> myRaytraceOutputTexture[2];

  //! Texture containing per-tile visual error estimation (2 textures are used in stereo mode).
  //! Used if adaptive screen sampling is activated.
  occ::handle<OpenGl_Texture> myRaytraceVisualErrorTexture[2];
  //! Texture containing offsets of sampled screen tiles (2 textures are used in stereo mode).
  //! Used if adaptive screen sampling is activated.
  occ::handle<OpenGl_Texture> myRaytraceTileOffsetsTexture[2];
  //! Texture containing amount of extra per-tile samples (2 textures are used in stereo mode).
  //! Used if adaptive screen sampling is activated.
  occ::handle<OpenGl_Texture> myRaytraceTileSamplesTexture[2];

  //! Vertex buffer (VBO) for drawing dummy quad.
  OpenGl_VertexBuffer myRaytraceScreenQuad;

  //! Cached locations of frequently used uniform variables.
  int myUniformLocations[2][OpenGl_RT_NbVariables];

  //! State of OpenGL structures reflected to ray-tracing.
  NCollection_DataMap<const OpenGl_Structure*, StructState> myStructureStates;

  //! PrimitiveArray to TriangleSet map for scene partial update.
  NCollection_DataMap<size_t, OpenGl_TriangleSet*> myArrayToTrianglesMap;

  //! Set of IDs of non-raytracable elements (to detect updates).
  NCollection_Map<int> myNonRaytraceStructureIDs;

  //! Marks if environment map should be updated.
  bool myToUpdateEnvironmentMap;

  //! State of OpenGL layer list.
  size_t myRaytraceLayerListState;

  //! Number of accumulated frames (for progressive rendering).
  int myAccumFrames;

  //! Stored ray origins used for detection of camera movements.
  NCollection_Vec3<float> myPreviousOrigins[3];

  //! Bullard RNG to produce random sequence.
  math_BullardGenerator myRNG;

  //! Tool object for sampling screen tiles in PT mode.
  OpenGl_TileSampler myTileSampler;

  //! Camera position used for projective mode
  NCollection_Vec3<float> myEyeOrig;

  //! Camera view direction used for projective mode
  NCollection_Vec3<float> myEyeView;

  //! Camera's screen vertical direction used for projective mode
  NCollection_Vec3<float> myEyeVert;

  //! Camera's screen horizontal direction used for projective mode
  NCollection_Vec3<float> myEyeSide;

  //! Camera's screen size used for projective mode
  NCollection_Vec2<float> myEyeSize;

  //! Aperture radius of camera on previous frame used for depth-of-field (path tracing)
  float myPrevCameraApertureRadius;

  //! Focal distance of camera on previous frame used for depth-of-field (path tracing)
  float myPrevCameraFocalPlaneDist;

public:
  DEFINE_STANDARD_ALLOC
  DEFINE_STANDARD_RTTIEXT(OpenGl_View, Graphic3d_CView) // Type definition

  friend class OpenGl_GraphicDriver;
  friend class OpenGl_Workspace;
  friend class OpenGl_LayerList;
  friend class OpenGl_FrameStats;
};

#endif // _OpenGl_View_Header
