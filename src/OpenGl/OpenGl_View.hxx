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

#ifndef _OpenGl_View_Header
#define _OpenGl_View_Header

#include <MMgt_TShared.hxx>
#include <Standard_DefineHandle.hxx>

#include <TColStd_Array2OfReal.hxx>
#include <NCollection_List.hxx>

#include <Quantity_NameOfColor.hxx>
#include <Aspect_FillMethod.hxx>
#include <Aspect_GradientFillMethod.hxx>
#include <Aspect_TypeOfTriedronPosition.hxx>
#include <Aspect_CLayer2d.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <Graphic3d_CView.hxx>
#include <Graphic3d_GraduatedTrihedron.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <Visual3d_TypeOfSurfaceDetail.hxx>
#include <Visual3d_TypeOfModel.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_BackgroundArray.hxx>
#include <OpenGl_BVHTreeSelector.hxx>
#include <OpenGl_LayerList.hxx>
#include <OpenGl_Light.hxx>
#include <OpenGl_LineAttributes.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>

#include <Handle_OpenGl_Context.hxx>
#include <Handle_OpenGl_GraphicDriver.hxx>
#include <Handle_OpenGl_Workspace.hxx>
#include <Handle_OpenGl_View.hxx>
#include <Handle_OpenGl_Texture.hxx>

struct OPENGL_ZCLIP
{
  struct {
    Standard_Boolean   IsOn;
    Standard_ShortReal Limit; /* in the range [0., 1.] */
  } Back;
  struct {
    Standard_Boolean   IsOn;
    Standard_ShortReal Limit; /* in the range [0., 1.] */
  } Front;
};

struct OPENGL_FOG
{
  Standard_Boolean   IsOn;
  Standard_ShortReal Front; /* in the range [0., 1.] */
  Standard_ShortReal Back; /* in the range [0., 1.] */
  TEL_COLOUR         Color;
};

struct OpenGl_Matrix;
class OpenGl_Structure;
class Handle(OpenGl_PrinterContext);
class OpenGl_StateCounter;

class OpenGl_View : public MMgt_TShared
{
 public:
  OpenGl_View (const CALL_DEF_VIEWCONTEXT &AContext, OpenGl_StateCounter* theCounter);
  virtual ~OpenGl_View ();

  void ReleaseGlResources (const Handle(OpenGl_Context)& theCtx);

  void SetTextureEnv (const Handle(OpenGl_Context)&       theCtx,
                      const Handle(Graphic3d_TextureEnv)& theTexture);
  void SetSurfaceDetail (const Visual3d_TypeOfSurfaceDetail AMode);
  void SetBackfacing (const Standard_Integer AMode);
  void SetLights (const CALL_DEF_VIEWCONTEXT &AContext);
  void SetAntiAliasing (const Standard_Boolean AMode) { myAntiAliasing = AMode; }
  void SetClipPlanes (const Graphic3d_SequenceOfHClipPlane &thePlanes) { myClipPlanes = thePlanes; }
  void SetVisualisation (const CALL_DEF_VIEWCONTEXT &AContext);

  const Handle(Graphic3d_Camera)& Camera() { return myCamera; }
  void SetCamera (const Handle(Graphic3d_Camera)& theCamera) { myCamera = theCamera; }

  void SetClipLimit (const Graphic3d_CView& theCView);

  void SetFog (const Graphic3d_CView& theCView, const Standard_Boolean theFlag);

  OpenGl_Trihedron& ChangeTrihedron() { return myTrihedron; }
  void TriedronDisplay (const Aspect_TypeOfTriedronPosition thePosition,
                        const Quantity_NameOfColor          theColor,
                        const Standard_Real                 theScale,
                        const Standard_Boolean              theAsWireframe);
  void TriedronErase (const Handle(OpenGl_Context)& theCtx);

  OpenGl_GraduatedTrihedron& ChangeGraduatedTrihedron() { return myGraduatedTrihedron; }
  void GraduatedTrihedronDisplay (const Handle(OpenGl_Context)&        theCtx,
                                  const Graphic3d_GraduatedTrihedron& theCubic);
  void GraduatedTrihedronErase (const Handle(OpenGl_Context)& theCtx);

  Standard_Real Height () const { return myCamera->ViewDimensions().X(); }
  Standard_Real Width () const { return myCamera->ViewDimensions().Y(); }

  Standard_Integer Backfacing () const { return myBackfacing; }

  const TEL_TRANSFORM_PERSISTENCE * BeginTransformPersistence (const Handle(OpenGl_Context)& theCtx,
                                                               const TEL_TRANSFORM_PERSISTENCE *theTransPers,
                                                               Standard_Integer theWidth,
                                                               Standard_Integer theHeight);
  void EndTransformPersistence (const Handle(OpenGl_Context)& theCtx);

  //! Add structure to display list with specified priority.
  //! The structure will be added to associated with it z layer.
  //! If the z layer is not presented in the view, the structure will
  //! be displayed in default bottom-level z layer.
  void DisplayStructure (const Handle(Graphic3d_Structure)& theStructure,
                         const Standard_Integer             thePriority);

  //! Erase structure from display list.
  void EraseStructure (const Handle(Graphic3d_Structure)& theStructure);

  //! Add structure to the list of immediate structures.
  void DisplayImmediateStructure (const Handle(Graphic3d_Structure)& theStructure);

  //! Erase structure from display list.
  void EraseImmediateStructure (const OpenGl_Structure* theStructure);

  //! Insert a new top-level z layer with ID <theLayerId>
  void AddZLayer (const Graphic3d_ZLayerId theLayerId);

  //! Remove a z layer with ID <theLayerId>
  void RemoveZLayer (const Graphic3d_ZLayerId theLayerId);

  //! Display structure in z layer with ID <theNewLayerId>
  //! If the layer with ID <theNewLayerId> is not presented in the view,
  //! the structure will be displayed in default bottom-level layer.
  void ChangeZLayer (const OpenGl_Structure*  theStructure,
                     const Graphic3d_ZLayerId theNewLayerId);

  //! Sets the settings for a single Z layer of specified view.
  void SetZLayerSettings (const Graphic3d_ZLayerId        theLayerId,
                          const Graphic3d_ZLayerSettings& theSettings);

  //! Changes the priority of a structure within its ZLayer
  void ChangePriority (const OpenGl_Structure* theStructure,
                       const Standard_Integer  theNewPriority);

  void CreateBackgroundTexture (const Standard_CString AFileName,
                                const Aspect_FillMethod AFillStyle);

  void SetBackgroundTextureStyle (const Aspect_FillMethod FillStyle);
  void SetBackgroundGradient (const Quantity_Color& AColor1, const Quantity_Color& AColor2, const Aspect_GradientFillMethod AType);
  void SetBackgroundGradientType (const Aspect_GradientFillMethod AType);

  void Render (const Handle(OpenGl_PrinterContext)& thePrintContext,
               const Handle(OpenGl_Workspace)&      theWorkspace,
               OpenGl_FrameBuffer*                  theReadDrawFbo,
               Graphic3d_Camera::Projection         theProjection,
               const Graphic3d_CView&               theCView,
               const Aspect_CLayer2d&               theCUnderLayer,
               const Aspect_CLayer2d&               theCOverLayer,
               const Standard_Boolean               theToDrawImmediate);


  void DrawBackground (const Handle(OpenGl_Workspace)& theWorkspace);

  //! Returns list of OpenGL Z-layers.
  const OpenGl_LayerList& LayerList() const { return myZLayers; }

  //! Returns list of openGL light sources.
  const OpenGl_ListOfLight& LightList() const { return myLights; }

  //! Returns OpenGL environment map.
  const Handle(OpenGl_Texture)& TextureEnv() const { return myTextureEnv; }

  //! Returns visualization mode for objects in the view.
  Visual3d_TypeOfSurfaceDetail SurfaceDetail() const { return mySurfaceDetail; }

  //! Returns selector for BVH tree, providing a possibility to store information
  //! about current view volume and to detect which objects are overlapping it.
  OpenGl_BVHTreeSelector& BVHTreeSelector() { return myBVHSelector; }

  //! Marks BVH tree for given priority list as dirty and
  //! marks primitive set for rebuild.
  void InvalidateBVHData (const Standard_Integer theLayerId);

  //! Returns list of immediate structures rendered on top of main presentation
  const OpenGl_IndexedMapOfStructure& ImmediateStructures() const
  {
    return myImmediateList;
  }

  //! Returns true if there are immediate structures to display
  bool HasImmediateStructures() const
  {
    return !myImmediateList.IsEmpty()
         || myZLayers.NbImmediateStructures() != 0;
  }

protected:

  void RenderStructs (const Handle(OpenGl_Workspace)& theWorkspace,
                      OpenGl_FrameBuffer*             theReadDrawFbo,
                      const Graphic3d_CView&          theCView,
                      const Standard_Boolean          theToDrawImmediate);

  void RedrawLayer2d (const Handle(OpenGl_PrinterContext)& thePrintContext,
                      const Handle(OpenGl_Workspace)&      theWorkspace,
                      const Graphic3d_CView&               theCView,
                      const Aspect_CLayer2d&               theCLayer);

  void RedrawTrihedron (const Handle(OpenGl_Workspace) &theWorkspace);

  //! Redraw contents of model scene: clipping planes,
  //! lights, structures. The peculiar properties of "scene" is that
  //! it requires empty Z-Buffer and uses projection and orientation
  //! matrices supplied by 3d view.
  void RedrawScene (const Handle(OpenGl_PrinterContext)& thePrintContext,
                    const Handle(OpenGl_Workspace)&      theWorkspace,
                    OpenGl_FrameBuffer*                  theReadDrawFbo,
                    const Graphic3d_CView&               theCView,
                    const Standard_Boolean               theToDrawImmediate);

  Handle(OpenGl_LineAttributes) myLineAttribs;
  Handle(OpenGl_Texture)        myTextureEnv;
  Visual3d_TypeOfSurfaceDetail  mySurfaceDetail;
  Standard_Integer              myBackfacing;

  OPENGL_ZCLIP   myZClip;

  Graphic3d_SequenceOfHClipPlane myClipPlanes;
  
  Handle(Graphic3d_Camera) myCamera;

  OPENGL_FOG myFog;
  OpenGl_Trihedron           myTrihedron;
  OpenGl_GraduatedTrihedron  myGraduatedTrihedron;
  bool                       myToShowTrihedron;
  bool                       myToShowGradTrihedron;

  //View_LABViewContext
  int myVisualization;
  Visual3d_TypeOfModel       myShadingModel;  //!< lighting shading model

  //View_LABLight
  OpenGl_ListOfLight myLights;

  //View_LABPlane
  //View_LABAliasing
  Standard_Boolean myAntiAliasing;

  //View_LABDepthCueing - fixed index used

  OpenGl_LayerList             myZLayers;       //!< main list of displayed structure, sorted by layers
  OpenGl_IndexedMapOfStructure myImmediateList; //!< list of immediate structures rendered on top of main presentation

  const TEL_TRANSFORM_PERSISTENCE *myTransPers;
  Standard_Boolean myIsTransPers;

  //! Modification state
  Standard_Size myProjectionState;
  Standard_Size myModelViewState;
  OpenGl_StateCounter* myStateCounter;

  Standard_Size myCurrLightSourceState;

  typedef std::pair<Standard_Size, Standard_Size> StateInfo;

  StateInfo myLastOrientationState;
  StateInfo myLastViewMappingState;
  StateInfo myLastLightSourceState;

  //! Is needed for selection of overlapping objects and storage of the current view volume
  OpenGl_BVHTreeSelector myBVHSelector;

protected: //! @name Background parameters

  OpenGl_AspectFace*      myTextureParams;   //!< Stores texture and its parameters for textured background
  OpenGl_BackgroundArray* myBgGradientArray; //!< Primitive array for gradient background
  OpenGl_BackgroundArray* myBgTextureArray;  //!< Primitive array for texture  background

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

    OpenGl_RT_uOriginLT,
    OpenGl_RT_uOriginLB,
    OpenGl_RT_uOriginRT,
    OpenGl_RT_uOriginRB,
    OpenGl_RT_uDirectLT,
    OpenGl_RT_uDirectLB,
    OpenGl_RT_uDirectRT,
    OpenGl_RT_uDirectRB,
    OpenGl_RT_uUnviewMat,

    OpenGl_RT_uSceneRad,
    OpenGl_RT_uSceneEps,
    OpenGl_RT_uLightAmbnt,
    OpenGl_RT_uLightCount,

    OpenGl_RT_uShadEnabled,
    OpenGl_RT_uReflEnabled,
    OpenGl_RT_uEnvMapEnable,

    OpenGl_RT_uOffsetX,
    OpenGl_RT_uOffsetY,
    OpenGl_RT_uSamples,
    OpenGl_RT_uWinSizeX,
    OpenGl_RT_uWinSizeY,

    OpenGl_RT_uTextures,

    OpenGl_RT_NbVariables // special field
  };

  //! Defines texture samplers.
  enum ShaderSamplerNames
  {
    OpenGl_RT_SceneNodeInfoTexture  = 0,
    OpenGl_RT_SceneMinPointTexture  = 1,
    OpenGl_RT_SceneMaxPointTexture  = 2,
    OpenGl_RT_SceneTransformTexture = 3,

    OpenGl_RT_GeometryVertexTexture = 4,
    OpenGl_RT_GeometryNormalTexture = 5,
    OpenGl_RT_GeometryTexCrdTexture = 6,
    OpenGl_RT_GeometryTriangTexture = 7,

    OpenGl_RT_EnvironmentMapTexture = 8,

    OpenGl_RT_RaytraceMaterialTexture = 9,
    OpenGl_RT_RaytraceLightSrcTexture = 10,

    OpenGl_RT_FSAAInputTexture = 11,

    OpenGl_RT_OpenGlColorTexture = 12,
    OpenGl_RT_OpenGlDepthTexture = 13
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

    //! Creates new shader source from specified file.
    ShaderSource (const TCollection_AsciiString& theFileName)
    {
      Load (&theFileName, 1);
    }

  public:

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
    TCollection_AsciiString Source() const;

    //! Loads shader source from specified files.
    void Load (const TCollection_AsciiString* theFileNames, const Standard_Integer theCount);

  private:

    TCollection_AsciiString mySource; //!< Source string of the shader object
    TCollection_AsciiString myPrefix; //!< Prefix to insert before the source

  };

  //! Default ray-tracing depth.
  static const Standard_Integer THE_DEFAULT_NB_BOUNCES = 3;

  //! Default size of traversal stack.
  static const Standard_Integer THE_DEFAULT_STACK_SIZE = 24;

  //! Compile-time ray-tracing parameters.
  struct RaytracingParams
  {
    //! Actual size of traversal stack in shader program.
    Standard_Integer StackSize;

    //! Actual ray-tracing depth (number of ray bounces).
    Standard_Integer NbBounces;

    //! Enables/disables light propagation through transparent media.
    Standard_Boolean TransparentShadows;

    //! Enables/disables the use of OpenGL bindless textures.
    Standard_Boolean UseBindlessTextures;

    //! Creates default compile-time ray-tracing parameters.
    RaytracingParams()
    : StackSize (THE_DEFAULT_STACK_SIZE),
      NbBounces (THE_DEFAULT_NB_BOUNCES),
      TransparentShadows (Standard_False),
      UseBindlessTextures (Standard_False)
    {
      //
    }
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

  //! Updates environment map for ray-tracing.
  Standard_Boolean updateRaytraceEnvironmentMap (const Handle(OpenGl_Context)& theGlContext);

  //! Checks to see if the OpenGL structure is modified.
  Standard_Boolean toUpdateStructure (const OpenGl_Structure* theStructure);

  //! Adds OpenGL structure to ray-traced scene geometry.
  Standard_Boolean addRaytraceStructure (const OpenGl_Structure*       theStructure,
                                         const Handle(OpenGl_Context)& theGlContext);

  //! Adds OpenGL groups to ray-traced scene geometry.
  Standard_Boolean addRaytraceGroups (const OpenGl_Structure*       theStructure,
                                      const Standard_Integer        theStructMat,
                                      const Standard_ShortReal*     theTransform,
                                      const Handle(OpenGl_Context)& theGlContext);

  //! Creates ray-tracing material properties.
  OpenGl_RaytraceMaterial convertMaterial (const OpenGl_AspectFace*      theAspect,
                                           const Handle(OpenGl_Context)& theGlContext);

  //! Adds OpenGL primitive array to ray-traced scene geometry.
  OpenGl_TriangleSet* addRaytracePrimitiveArray (const OpenGl_PrimitiveArray* theArray,
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

  //! Initializes OpenGL/GLSL shader programs.
  Standard_Boolean initRaytraceResources (const Graphic3d_CView&        theCView,
                                          const Handle(OpenGl_Context)& theGlContext);

  //! Releases OpenGL/GLSL shader programs.
  void releaseRaytraceResources (const Handle(OpenGl_Context)& theGlContext);

  //! Resizes OpenGL frame buffers.
  Standard_Boolean resizeRaytraceBuffers (const Standard_Integer        theSizeX,
                                          const Standard_Integer        theSizeY,
                                          const Handle(OpenGl_Context)& theGlContext);

  //! Generates viewing rays for corners of screen quad.
  void updateCamera (const OpenGl_Mat4& theOrientation,
                     const OpenGl_Mat4& theViewMapping,
                     OpenGl_Vec3*       theOrigins,
                     OpenGl_Vec3*       theDirects,
                     OpenGl_Mat4&       theUnView);

  //! Binds ray-trace textures to corresponding texture units.
  void bindRaytraceTextures (const Handle(OpenGl_Context)& theGlContext);

  //! Unbinds ray-trace textures from corresponding texture unit.
  void unbindRaytraceTextures (const Handle(OpenGl_Context)& theGlContext);

  //! Sets uniform state for the given ray-tracing shader program.
  Standard_Boolean setUniformState (const Graphic3d_CView&        theCView,
                                    const OpenGl_Vec3*            theOrigins,
                                    const OpenGl_Vec3*            theDirects,
                                    const OpenGl_Mat4&            theUnviewMat,
                                    const Standard_Integer        theProgramId,
                                    const Handle(OpenGl_Context)& theGlContext);

  //! Runs ray-tracing shader programs.
  Standard_Boolean runRaytraceShaders (const Graphic3d_CView&        theCView,
                                       const Standard_Integer        theSizeX,
                                       const Standard_Integer        theSizeY,
                                       const OpenGl_Vec3*            theOrigins,
                                       const OpenGl_Vec3*            theDirects,
                                       const OpenGl_Mat4&            theUnviewMat,
                                       OpenGl_FrameBuffer*           theReadDrawFbo,
                                       const Handle(OpenGl_Context)& theGlContext);

  //! Redraws the window using OpenGL/GLSL ray-tracing.
  Standard_Boolean raytrace (const Graphic3d_CView&        theCView,
                             const Standard_Integer        theSizeX,
                             const Standard_Integer        theSizeY,
                             OpenGl_FrameBuffer*           theReadDrawFbo,
                             const Handle(OpenGl_Context)& theGlContext);

protected: //! @name fields related to ray-tracing

  //! Result of shaders initialization.
  RaytraceInitStatus myRaytraceInitStatus;

  //! Is geometry data valid?
  Standard_Boolean myIsRaytraceDataValid;

  //! Warning about missing extension GL_ARB_bindless_texture has been displayed?
  Standard_Boolean myIsRaytraceWarnTextures;

  //! 3D scene geometry data for ray-tracing.
  OpenGl_RaytraceGeometry myRaytraceGeometry;

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

  //! OpenGL/GLSL ray-tracing fragment shader.
  Handle(OpenGl_ShaderObject) myRaytraceShader;
  //! OpenGL/GLSL adaptive-AA fragment shader.
  Handle(OpenGl_ShaderObject) myPostFSAAShader;

  //! OpenGL/GLSL ray-tracing shader program.
  Handle(OpenGl_ShaderProgram) myRaytraceProgram;
  //! OpenGL/GLSL adaptive-AA shader program.
  Handle(OpenGl_ShaderProgram) myPostFSAAProgram;

  //! Texture buffer of data records of bottom-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) mySceneNodeInfoTexture;
  //! Texture buffer of minimum points of bottom-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) mySceneMinPointTexture;
  //! Texture buffer of maximum points of bottom-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) mySceneMaxPointTexture;
  //! Texture buffer of transformations of high-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) mySceneTransformTexture;

  //! Texture buffer of vertex coords.
  Handle(OpenGl_TextureBufferArb) myGeometryVertexTexture;
  //! Texture buffer of vertex normals.
  Handle(OpenGl_TextureBufferArb) myGeometryNormalTexture;
  //! Texture buffer of vertex UV coords.
  Handle(OpenGl_TextureBufferArb) myGeometryTexCrdTexture;
  //! Texture buffer of triangle indices.
  Handle(OpenGl_TextureBufferArb) myGeometryTriangTexture;

  //! Texture buffer of material properties.
  Handle(OpenGl_TextureBufferArb) myRaytraceMaterialTexture;
  //! Texture buffer of light source properties.
  Handle(OpenGl_TextureBufferArb) myRaytraceLightSrcTexture;

  //! 1st framebuffer (FBO) to perform adaptive FSAA.
  Handle(OpenGl_FrameBuffer) myRaytraceFBO1;
  //! 2nd framebuffer (FBO) to perform adaptive FSAA.
  Handle(OpenGl_FrameBuffer) myRaytraceFBO2;
  //! Framebuffer (FBO) for preliminary OpenGL output.
  Handle(OpenGl_FrameBuffer) myOpenGlFBO;

  //! Vertex buffer (VBO) for drawing dummy quad.
  OpenGl_VertexBuffer myRaytraceScreenQuad;

  //! Cached locations of frequently used uniform variables.
  Standard_Integer myUniformLocations[2][OpenGl_RT_NbVariables];

  //! State of OpenGL structures reflected to ray-tracing.
  std::map<const OpenGl_Structure*, StructState> myStructureStates;

  //! PrimitiveArray to TriangleSet map for scene partial update.
  std::map<Standard_Size, OpenGl_TriangleSet*> myArrayToTrianglesMap;

  //! Graphical ray-tracing filter to filter out all raytracable structures.
  Handle(OpenGl_RaytraceFilter) myRaytraceFilter;

  //! Marks if environment map should be updated.
  Standard_Boolean myToUpdateEnvironmentMap;

  //! State of OpenGL layer list.
  Standard_Size myLayersModificationStatus;

public:

  DEFINE_STANDARD_ALLOC
  DEFINE_STANDARD_RTTI(OpenGl_View) // Type definition

  friend class OpenGl_Workspace;

};

#endif // _OpenGl_View_Header
