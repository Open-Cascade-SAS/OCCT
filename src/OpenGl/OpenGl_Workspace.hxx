// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#ifndef _OpenGl_Workspace_Header
#define _OpenGl_Workspace_Header

#include <map>
#include <set>

#include <Handle_OpenGl_Workspace.hxx>
#include <OpenGl_Window.hxx>

#include <TColStd_Array2OfReal.hxx>
#include <Quantity_Color.hxx>
#include <Graphic3d_CView.hxx>
#include <Graphic3d_TypeOfComposition.hxx>
#include <Graphic3d_TypeOfTexture.hxx>
#include <Graphic3d_PtrFrameBuffer.hxx>
#include <Graphic3d_BufferType.hxx>
#include <Handle_Graphic3d_TextureParams.hxx>

#include <Aspect_CLayer2d.hxx>
#include <Aspect_Handle.hxx>
#include <Aspect_PrintAlgo.hxx>
#include <Aspect_PolygonOffsetMode.hxx>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <NCollection_Sequence.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_NamedStatus.hxx>
#include <OpenGl_PrinterContext.hxx>
#include <OpenGl_SceneGeometry.hxx>
#include <OpenGl_TextParam.hxx>
#include <OpenGl_RenderFilter.hxx>
#include <OpenGl_Vec.hxx>
#include <OpenGl_LineAttributes.hxx>

#include <Handle_OpenGl_View.hxx>
#include <Handle_OpenGl_Texture.hxx>

#include <OpenGl_ShaderObject.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_TextureBufferArb.hxx>

class OpenGl_AspectLine;
class OpenGl_AspectMarker;
class OpenGl_AspectText;
class OpenGl_FrameBuffer;
class OpenGl_Structure;
class OpenGl_TriangleSet;
class OpenGl_Element;
class Image_PixMap;

//! OpenGL material definition
struct OpenGl_Material
{

  OpenGl_Vec4 Ambient;  //!< ambient reflection coefficient
  OpenGl_Vec4 Diffuse;  //!< diffuse reflection coefficient
  OpenGl_Vec4 Specular; //!< glossy  reflection coefficient
  OpenGl_Vec4 Emission; //!< material emission
  OpenGl_Vec4 Params;   //!< extra packed parameters

  Standard_ShortReal  Shine()              const { return Params.x(); }
  Standard_ShortReal& ChangeShine()              { return Params.x(); }

  Standard_ShortReal  Transparency()       const { return Params.y(); }
  Standard_ShortReal& ChangeTransparency()       { return Params.y(); }

  //! Initialize material
  void Init (const OPENGL_SURF_PROP& theProps);

  //! Returns packed (serialized) representation of material properties
  const OpenGl_Vec4* Packed() const { return reinterpret_cast<const OpenGl_Vec4*> (this); }
  static Standard_Integer NbOfVec4() { return 5; }

};

DEFINE_STANDARD_HANDLE (OpenGl_RaytraceFilter, OpenGl_RenderFilter)

//! Graphical raytracing filter.
//! Filters out all raytracable structures.
class OpenGl_RaytraceFilter : public OpenGl_RenderFilter
{
public:

  //! Default constructor.
  OpenGl_RaytraceFilter() {}

  //! Remembers the previously set filter.
  inline void SetPrevRenderFilter (const Handle(OpenGl_RenderFilter)& theFilter)
  {
    myPrevRenderFilter = theFilter;
  }

  //! Checks whether the element can be rendered or not.
  //! @param theElement [in] the element to check.
  //! @return True if element can be rendered.
  virtual Standard_Boolean CanRender (const OpenGl_Element* theElement);

private:

  Handle(OpenGl_RenderFilter) myPrevRenderFilter;

public:

  DEFINE_STANDARD_RTTI(OpenGl_RaytraceFilter)
};

//! Represents window with GL context.
//! Provides methods to render primitives and maintain GL state.
class OpenGl_Workspace : public OpenGl_Window
{
public:

  //! Main constructor - prepare GL context for specified window.
  OpenGl_Workspace (const Handle(OpenGl_GraphicDriver)& theDriver,
                    const CALL_DEF_WINDOW&        theCWindow,
                    Aspect_RenderingContext       theGContext,
                    const Handle(OpenGl_Caps)&    theCaps,
                    const Handle(OpenGl_Context)& theShareCtx);

  //! Destructor
  virtual ~OpenGl_Workspace();

  void SetActiveView (const Handle(OpenGl_View)& theView) { myView = theView; }
  const Handle(OpenGl_View)& ActiveView () const { return myView; }

  //! Redraw the window.
  void Redraw (const Graphic3d_CView& theCView,
               const Aspect_CLayer2d& theCUnderLayer,
               const Aspect_CLayer2d& theCOverLayer);

  Standard_Boolean SetImmediateModeDrawToFront (const Standard_Boolean theDrawToFrontBuffer);
  void RedrawImmediate (const Graphic3d_CView& theCView,
                        const Aspect_CLayer2d& theCUnderLayer,
                        const Aspect_CLayer2d& theCOverLayer,
                        const Standard_Boolean theToForce = Standard_False);

  void Invalidate (const Graphic3d_CView& /*theCView*/)
  {
    myBackBufferRestored = Standard_False;
  }

  //! Special method to perform printing.
  //! System-specific and currently only Win platform implemented.
  Standard_Boolean Print (const Handle(OpenGl_PrinterContext)& thePrintContext,
                          const Graphic3d_CView& theCView,
                          const Aspect_CLayer2d& theCUnderLayer,
                          const Aspect_CLayer2d& theCOverLayer,
                          const Aspect_Handle    theHPrintDC,
                          const Standard_Boolean theToShowBackground,
                          const Standard_CString theFileName,
                          const Aspect_PrintAlgo thePrintAlgorithm,
                          const Standard_Real    theScaleFactor);

  const Handle(OpenGl_PrinterContext)& PrinterContext() const
  {
    return myPrintContext;
  }

  void DisplayCallback (const Graphic3d_CView& theCView, int theReason);

  Graphic3d_PtrFrameBuffer FBOCreate (const Standard_Integer theWidth, const Standard_Integer theHeight);
  void FBORelease (Graphic3d_PtrFrameBuffer theFBOPtr);
  Standard_Boolean BufferDump (OpenGl_FrameBuffer*         theFBOPtr,
                               Image_PixMap&               theImage,
                               const Graphic3d_BufferType& theBufferType);

  void UseTransparency (const Standard_Boolean theFlag);
  Standard_Boolean& UseZBuffer()   { return myUseZBuffer; }
  Standard_Boolean& UseDepthTest() { return myUseDepthTest; }
  Standard_Boolean& UseGLLight()   { return myUseGLLight; }

  Standard_Integer AntiAliasingMode() const { return myAntiAliasingMode; }

  //// RELATED TO STATUS ////

  Standard_Integer   NamedStatus;

  const TEL_COLOUR* HighlightColor;

  const OpenGl_Matrix* SetViewMatrix (const OpenGl_Matrix* );
  const OpenGl_Matrix* SetStructureMatrix (const OpenGl_Matrix*, bool aRevert = false);

  //! Updates current model-view matrix
  //! replacing it with StructureMatrixT*ViewMatrix from the workspace.
  const void UpdateModelViewMatrix();

  const OpenGl_AspectLine*   SetAspectLine   (const OpenGl_AspectLine*   theAspect);
  const OpenGl_AspectFace*   SetAspectFace   (const OpenGl_AspectFace*   theAspect);
  const OpenGl_AspectMarker* SetAspectMarker (const OpenGl_AspectMarker* theAspect);
  const OpenGl_AspectText*   SetAspectText   (const OpenGl_AspectText*   theAspect);

  void SetTextParam (const OpenGl_TextParam* theParam) { TextParam_set = theParam; }

  //// THESE METHODS ARE EXPORTED AS THEY PROVIDE STATE INFO TO USERDRAW
  Standard_EXPORT const OpenGl_AspectLine*   AspectLine   (const Standard_Boolean theWithApply);
  Standard_EXPORT const OpenGl_AspectFace*   AspectFace   (const Standard_Boolean theWithApply);
  Standard_EXPORT const OpenGl_AspectMarker* AspectMarker (const Standard_Boolean theWithApply);
  Standard_EXPORT const OpenGl_AspectText*   AspectText   (const Standard_Boolean theWithApply);
  inline const OpenGl_TextParam* AspectTextParams() const
  {
    return TextParam_applied;
  }

  //! Clear the applied aspect state.
  void ResetAppliedAspect();

  Standard_EXPORT Handle(OpenGl_Texture) DisableTexture();
  Standard_EXPORT Handle(OpenGl_Texture) EnableTexture (const Handle(OpenGl_Texture)&          theTexture,
                                                        const Handle(Graphic3d_TextureParams)& theParams = NULL);
  const Handle(OpenGl_Texture)& ActiveTexture() const { return myTextureBound; }

  //! Set filter for restricting rendering of particular elements.
  //! Filter can be applied for rendering passes used by recursive
  //! rendering algorithms for rendering elements of groups.
  //! @param theFilter [in] the filter instance.
  inline void SetRenderFilter (const Handle(OpenGl_RenderFilter)& theFilter)
  {
    myRenderFilter = theFilter;
  }

  //! Get rendering filter.
  //! @return filter instance.
  inline const Handle(OpenGl_RenderFilter)& GetRenderFilter() const
  {
    return myRenderFilter;
  }

  //! @return applied view matrix.
  inline const OpenGl_Matrix* ViewMatrix() const { return ViewMatrix_applied; }

  //! @return applied model structure matrix.
  inline const OpenGl_Matrix* ModelMatrix() const { return StructureMatrix_applied; }

  //! Sets and applies current polygon offset.
  void SetPolygonOffset (int theMode, Standard_ShortReal theFactor, Standard_ShortReal theUnits);

  //! Returns currently applied polygon offset params.
  const TEL_POFFSET_PARAM& AppliedPolygonOffset() { return PolygonOffset_applied; }

  //! @return true if clipping algorithm enabled
  inline Standard_Boolean IsCullingEnabled() const { return myIsCullingEnabled; }

  //! Returns a flag whether to redraw the scene using OpenGL rasterization
  Standard_Boolean ToRedrawGL() const { return myToRedrawGL; }

protected:

  //! Copy content of Back buffer to the Front buffer
  void copyBackToFront();

  virtual Standard_Boolean Activate();

  void redraw1 (const Graphic3d_CView& theCView,
                const Aspect_CLayer2d& theCUnderLayer,
                const Aspect_CLayer2d& theCOverLayer,
                const int              theToSwap);

  void updateMaterial (const int theFlag);

  void setTextureParams (Handle(OpenGl_Texture)&                theTexture,
                         const Handle(Graphic3d_TextureParams)& theParams);

protected:

  //! Result of OpenGL shaders initialization.
  enum RaytraceInitStatus
  {
    OpenGl_RT_NONE,
    OpenGl_RT_INIT,
    OpenGl_RT_FAIL
  };

  //! Describes update mode (state).
  enum GeomUpdateMode
  {
    OpenGl_GUM_CHECK,   //!< check if geometry update is necessary
    OpenGl_GUM_PREPARE, //!< collect unchanged objects
    OpenGl_GUM_UPDATE   //!< update raytracing data, rebuild changed objects
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
    OpenGl_RT_uInvModelProj,
    
    OpenGl_RT_uSceneRad,
    OpenGl_RT_uSceneEps,

    OpenGl_RT_uLightAmbnt,
    OpenGl_RT_uLightCount,

    OpenGl_RT_uShadEnabled,
    OpenGl_RT_uReflEnabled,
    
    OpenGl_RT_uInputTexture,

    OpenGl_RT_uOffsetX,
    OpenGl_RT_uOffsetY,
    OpenGl_RT_uSamples,

    OpenGl_RT_uEnvironmentEnable,

    OpenGl_RT_NbVariables // special field
  };

  //! Defines texture samplers.
  enum ShaderSamplerNames
  {
    OpenGl_RT_SceneNodeInfoTexture = 0,
    OpenGl_RT_SceneMinPointTexture = 1,
    OpenGl_RT_SceneMaxPointTexture = 2,

    OpenGl_RT_ObjectNodeInfoTexture = 3,
    OpenGl_RT_ObjectMinPointTexture = 4,
    OpenGl_RT_ObjectMaxPointTexture = 5,

    OpenGl_RT_GeometryVertexTexture = 6,
    OpenGl_RT_GeometryNormalTexture = 7,
    OpenGl_RT_GeometryTriangTexture = 8,

    OpenGl_RT_EnvironmentMapTexture = 9,

    OpenGl_RT_RaytraceMaterialTexture = 10,
    OpenGl_RT_RaytraceLightSrcTexture = 11,

    OpenGl_RT_FSAAInputTexture = 12,

    OpenGl_RT_SceneTransformTexture = 13,

    OpenGl_RT_OpenGlColorTexture = 14,
    OpenGl_RT_OpenGlDepthTexture = 15
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
  static const Standard_Integer THE_DEFAULT_RAY_DEPTH = 3;

  //! Default size of traversal stack.
  static const Standard_Integer THE_DEFAULT_STACK_SIZE = 24;

  //! Compile-time ray-tracing parameters.
  struct RaytracingParams
  {
    //! Actual size of traversal stack in shader program.
    Standard_Integer StackSize;

    //! Actual ray-tracing depth (number of ray bounces).
    Standard_Integer TraceDepth;

    //! Sets light propagation through transparent media.
    Standard_Boolean TransparentShadows;

    //! Creates default compile-time ray-tracing parameters.
    RaytracingParams()
    : StackSize (THE_DEFAULT_STACK_SIZE),
      TraceDepth (THE_DEFAULT_RAY_DEPTH),
      TransparentShadows (Standard_False)
    {
      //
    }
  };

protected: //! @name methods related to ray-tracing

  //! Updates 3D scene geometry for ray-tracing.
  Standard_Boolean UpdateRaytraceGeometry (GeomUpdateMode theMode);

  //! Checks to see if the structure is modified.
  Standard_Boolean CheckRaytraceStructure (const OpenGl_Structure* theStructure);

  //! Updates 3D scene light sources for ray-tracing.
  Standard_Boolean UpdateRaytraceLightSources (const GLdouble theInvModelView[16]);

  //! Updates environment map for ray-tracing.
  Standard_Boolean UpdateRaytraceEnvironmentMap();

  //! Adds OpenGL structure to ray-traced scene geometry.
  Standard_Boolean AddRaytraceStructure (const OpenGl_Structure* theStructure, std::set<const OpenGl_Structure*>& theElements);

  //! Adds OpenGL groups to ray-traced scene geometry.
  Standard_Boolean AddRaytraceGroups (const OpenGl_Structure*   theStructure,
                                      const Standard_Integer    theStructMatId,
                                      const Standard_ShortReal* theTransform);

  //! Adds OpenGL primitive array to ray-traced scene geometry.
  OpenGl_TriangleSet* AddRaytracePrimitiveArray (
    const OpenGl_PrimitiveArray* theArray, int theMatID, const Standard_ShortReal* theTrans);

  //! Adds vertex indices from OpenGL primitive array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceVertexIndices (OpenGl_TriangleSet&          theSet,
                                             const OpenGl_PrimitiveArray& theArray,
                                             Standard_Integer             theOffset,
                                             Standard_Integer             theCount,
                                             Standard_Integer             theMatID);

  //! Adds OpenGL triangle array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceTriangleArray (OpenGl_TriangleSet&                  theSet,
                                             const Handle(Graphic3d_IndexBuffer)& theIndices,
                                             Standard_Integer                     theOffset,
                                             Standard_Integer                     theCount,
                                             Standard_Integer                     theMatID);

  //! Adds OpenGL triangle fan array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceTriangleFanArray (OpenGl_TriangleSet&                  theSet,
                                                const Handle(Graphic3d_IndexBuffer)& theIndices,
                                                Standard_Integer                     theOffset,
                                                Standard_Integer                     theCount,
                                                Standard_Integer                     theMatID);

  //! Adds OpenGL triangle strip array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceTriangleStripArray (OpenGl_TriangleSet&                  theSet,
                                                  const Handle(Graphic3d_IndexBuffer)& theIndices,
                                                  Standard_Integer                     theOffset,
                                                  Standard_Integer                     theCount,
                                                  Standard_Integer                     theMatID);

  //! Adds OpenGL quadrangle array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceQuadrangleArray (OpenGl_TriangleSet&                  theSet,
                                               const Handle(Graphic3d_IndexBuffer)& theIndices,
                                               Standard_Integer                     theOffset,
                                               Standard_Integer                     theCount,
                                               Standard_Integer                     theMatID);

  //! Adds OpenGL quadrangle strip array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceQuadrangleStripArray (OpenGl_TriangleSet&                  theSet,
                                                    const Handle(Graphic3d_IndexBuffer)& theIndices,
                                                    Standard_Integer                     theOffset,
                                                    Standard_Integer                     theCount,
                                                    Standard_Integer                     theMatID);

  //! Adds OpenGL polygon array to ray-traced scene geometry.
  Standard_Boolean AddRaytracePolygonArray (OpenGl_TriangleSet&                  theSet,
                                            const Handle(Graphic3d_IndexBuffer)& theIndices,
                                            Standard_Integer                     theOffset,
                                            Standard_Integer                     theCount,
                                            Standard_Integer                     theMatID);

  //! Loads and compiles shader object from specified source.
  Handle(OpenGl_ShaderObject) LoadShader (const ShaderSource& theSource, GLenum theType);

  //! Performs safe exit when shaders initialization fails.
  Standard_Boolean SafeFailBack (const TCollection_ExtendedString& theMessage);

  //! Initializes OpenGL/GLSL shader programs.
  Standard_Boolean InitRaytraceResources (const Graphic3d_CView& theCView);

  //! Releases OpenGL/GLSL shader programs.
  void ReleaseRaytraceResources();

  //! Uploads ray-trace data to the GPU.
  Standard_Boolean UploadRaytraceData();

  //! Resizes OpenGL frame buffers.
  Standard_Boolean ResizeRaytraceBuffers (const Standard_Integer theSizeX,
                                          const Standard_Integer theSizeY);

  //! Generates viewing rays for corners of screen quad.
  void UpdateCamera (const NCollection_Mat4<GLdouble>& theOrientation,
                     const NCollection_Mat4<GLdouble>& theViewMapping,
                     OpenGl_Vec3                       theOrigins[4],
                     OpenGl_Vec3                       theDirects[4],
                     NCollection_Mat4<GLdouble>&       theInvModelProj);

  //! Runs ray-tracing shader programs.
  Standard_Boolean RunRaytraceShaders (const Graphic3d_CView& theCView,
                                       const Standard_Integer theSizeX,
                                       const Standard_Integer theSizeY,
                                       const OpenGl_Vec3      theOrigins[4],
                                       const OpenGl_Vec3      theDirects[4],
                                       const OpenGl_Matrix&   theInvModelProj,
                                       OpenGl_FrameBuffer*    theFrameBuffer);

  //! Redraws the window using OpenGL/GLSL ray-tracing.
  Standard_Boolean Raytrace (const Graphic3d_CView& theCView,
                             const Standard_Integer theSizeX,
                             const Standard_Integer theSizeY,
                             const Standard_Boolean theToSwap,
                             const Aspect_CLayer2d& theCOverLayer,
                             const Aspect_CLayer2d& theCUnderLayer,
                             OpenGl_FrameBuffer*    theFrameBuffer);

protected: //! @name fields related to ray-tracing

  //! Result of shaders initialization.
  RaytraceInitStatus myComputeInitStatus;

  //! Is geometry data valid?
  Standard_Boolean myIsRaytraceDataValid;

  //! 3D scene geometry data for ray-tracing.
  OpenGl_RaytraceGeometry myRaytraceGeometry;

  //! Radius of bounding sphere of the scene.
  Standard_ShortReal myRaytraceSceneRadius;
  //! Scene epsilon to prevent self-intersections.
  Standard_ShortReal myRaytraceSceneEpsilon;

  //! Compile-time ray-tracing parameters.
  RaytracingParams myRaytraceParameters;

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

  //! Texture buffer of data records of high-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) mySceneNodeInfoTexture;
  //! Texture buffer of minimum points of high-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) mySceneMinPointTexture;
  //! Texture buffer of maximum points of high-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) mySceneMaxPointTexture;
  //! Texture buffer of transformations of high-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) mySceneTransformTexture;

  //! Texture buffer of data records of bottom-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) myObjectNodeInfoTexture;
  //! Texture buffer of minimum points of bottom-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) myObjectMinPointTexture;
  //! Texture buffer of maximum points of bottom-level BVH nodes.
  Handle(OpenGl_TextureBufferArb) myObjectMaxPointTexture;

  //! Texture buffer of vertex coords.
  Handle(OpenGl_TextureBufferArb) myGeometryVertexTexture;
  //! Texture buffer of vertex normals.
  Handle(OpenGl_TextureBufferArb) myGeometryNormalTexture;
  //! Texture buffer of triangle indices.
  Handle(OpenGl_TextureBufferArb) myGeometryTriangTexture;
  
  //! Texture buffer of material properties.
  Handle(OpenGl_TextureBufferArb) myRaytraceMaterialTexture;
  //! Texture buffer of light source properties.
  Handle(OpenGl_TextureBufferArb) myRaytraceLightSrcTexture;

  //! Vertex buffer (VBO) for drawing dummy quad.
  OpenGl_VertexBuffer myRaytraceScreenQuad;

  //! Framebuffer (FBO) to perform adaptive FSAA.
  Handle(OpenGl_FrameBuffer) myRaytraceFBO1;
  //! Framebuffer (FBO) to perform adaptive FSAA.
  Handle(OpenGl_FrameBuffer) myRaytraceFBO2;
  //! Framebuffer (FBO) for pre-raytrace rendering by OpenGL.
  Handle(OpenGl_FrameBuffer) myOpenGlFBO;

  //! State of OpenGL view.
  Standard_Size myViewModificationStatus;
  //! State of OpenGL layer list.
  Standard_Size myLayersModificationStatus;

  //! State of OpenGL structures reflected to ray-tracing.
  std::map<const OpenGl_Structure*, Standard_Size> myStructureStates;

  //! PrimitiveArray to TriangleSet map for scene partial update.
  std::map<Standard_Size, OpenGl_TriangleSet*> myArrayToTrianglesMap;

  //! Cached locations of frequently used uniform variables.
  Standard_Integer myUniformLocations[2][OpenGl_RT_NbVariables];

  //! Graphical raytracing filter to filter out all raytracable structures.
  Handle(OpenGl_RaytraceFilter) myRaytraceFilter;

  //! Redraw the scene using OpenGL rasterization or raytracing?
  Standard_Boolean myToRedrawGL;

protected: //! @name protected fields

  Handle(OpenGl_PrinterContext) myPrintContext;
  Handle(OpenGl_View)           myView;
  Handle(OpenGl_LineAttributes) myLineAttribs;
  Standard_Integer       myAntiAliasingMode;
  Standard_Boolean       myTransientDrawToFront; //!< optimization flag for immediate mode (to render directly to the front buffer)
  Standard_Boolean       myBackBufferRestored;
  Standard_Boolean       myIsImmediateDrawn;     //!< flag indicates that immediate mode buffer contains some data
  Standard_Boolean       myUseTransparency;
  Standard_Boolean       myUseZBuffer;
  Standard_Boolean       myUseDepthTest;
  Standard_Boolean       myUseGLLight;
  Standard_Boolean       myIsCullingEnabled;     //!< frustum culling flag

  unsigned int           myFrameCounter;         //!< redraw counter, for debugging

protected: //! @name fields related to status

  Handle(OpenGl_RenderFilter) myRenderFilter;
  Handle(OpenGl_Texture) myTextureBound;    //!< currently bound texture (managed by OpenGl_AspectFace and OpenGl_View environment texture)
  const OpenGl_AspectLine *AspectLine_set, *AspectLine_applied;
  const OpenGl_AspectFace *AspectFace_set, *AspectFace_applied;
  const OpenGl_AspectMarker *AspectMarker_set, *AspectMarker_applied;
  const OpenGl_AspectText *AspectText_set, *AspectText_applied;

  const OpenGl_TextParam *TextParam_set, *TextParam_applied;

  const OpenGl_Matrix* ViewMatrix_applied;
  const OpenGl_Matrix* StructureMatrix_applied;

  OpenGl_Material myMatFront;    //!< current front material state (cached to reduce GL context updates)
  OpenGl_Material myMatBack;     //!< current back  material state
  OpenGl_Material myMatTmp;      //!< temporary variable
  TelCullMode     myCullingMode; //!< back face culling mode, applied from face aspect

  OpenGl_Matrix myModelViewMatrix; //!< Model matrix with applied structure transformations

  TEL_POFFSET_PARAM PolygonOffset_applied; //!< Currently applied polygon offset.

  OpenGl_AspectFace myAspectFaceHl; //!< Hiddenline aspect

public: //! @name type definition

  DEFINE_STANDARD_RTTI(OpenGl_Workspace)
  DEFINE_STANDARD_ALLOC

};

#endif // _OpenGl_Workspace_Header
