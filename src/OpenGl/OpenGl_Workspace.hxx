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

#ifdef HAVE_OPENCL
  #include <map>
  #include <set>

  #include <OpenGl_Cl.hxx>
#endif

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

#include <OpenGl_tsm.hxx>

#include <OpenGl_AspectFace.hxx>
#include <OpenGl_Display.hxx>
#include <OpenGl_Matrix.hxx>
#include <OpenGl_NamedStatus.hxx>
#include <OpenGl_PrinterContext.hxx>
#ifdef HAVE_OPENCL
  #include <OpenGl_SceneGeometry.hxx>
#endif
#include <OpenGl_TextParam.hxx>
#include <OpenGl_RenderFilter.hxx>
#include <OpenGl_Vec.hxx>

#include <Handle_OpenGl_View.hxx>
#include <Handle_OpenGl_Texture.hxx>

class OpenGl_AspectLine;
class OpenGl_AspectMarker;
class OpenGl_AspectText;
class OpenGl_FrameBuffer;
class OpenGl_Structure;
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

//! Represents window with GL context.
//! Provides methods to render primitives and maintain GL state.
class OpenGl_Workspace : public OpenGl_Window
{
public:

  //! Main constructor - prepare GL context for specified window.
  OpenGl_Workspace (const Handle(OpenGl_Display)& theDisplay,
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

  //! Deprecated. Simply calls Redraw().
  void Update (const Graphic3d_CView& theCView,
               const Aspect_CLayer2d& theCUnderLayer,
               const Aspect_CLayer2d& theCOverLayer)
  {
    Redraw (theCView, theCUnderLayer, theCOverLayer);
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

  Standard_Boolean SetImmediateModeDrawToFront (const Standard_Boolean theDrawToFrontBuffer);
  Standard_Boolean BeginAddMode();
  void EndAddMode();
  void ClearImmediatMode (const Graphic3d_CView& theCView,
                          const Standard_Boolean theToFlush);
  void RedrawImmediatMode();
  Standard_Boolean BeginImmediatMode (const Graphic3d_CView& theCView,
                                      const Standard_Boolean theUseDepthTest,
                                      const Standard_Boolean theRetainMode);
  void EndImmediatMode();
  void DrawStructure (const OpenGl_Structure* theStructure);

  Graphic3d_PtrFrameBuffer FBOCreate (const Standard_Integer theWidth, const Standard_Integer theHeight);
  void FBORelease (Graphic3d_PtrFrameBuffer theFBOPtr);
  Standard_Boolean BufferDump (OpenGl_FrameBuffer*         theFBOPtr,
                               Image_PixMap&               theImage,
                               const Graphic3d_BufferType& theBufferType);

  void UseTransparency (const Standard_Boolean theFlag);
  Standard_Boolean& UseZBuffer()   { return myUseZBuffer; }
  Standard_Boolean& UseDepthTest() { return myUseDepthTest; }
  Standard_Boolean& UseGLLight()   { return myUseGLLight; }

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

protected:

  void CopyBuffers (const Standard_Boolean theFrontToBack);

  virtual Standard_Boolean Activate();

  // TEMPORARY!!!
  void Redraw1 (const Graphic3d_CView& theCView,
                const Aspect_CLayer2d& theCUnderLayer,
                const Aspect_CLayer2d& theCOverLayer,
                const int theToSwap);

  void updateMaterial (const int theFlag);

  void setTextureParams (Handle(OpenGl_Texture)&                theTexture,
                         const Handle(Graphic3d_TextureParams)& theParams);

#ifdef HAVE_OPENCL

public:

  //! Returns information about OpenCL device used for computations.
  Standard_Boolean GetOpenClDeviceInfo (
    NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>& theInfo) const;

protected:

  //! Describes result of OpenCL initializing.
  enum OpenClInitStatus
  {
    OpenGl_CLIS_NONE,
    OpenGl_CLIS_INIT,
    OpenGl_CLIS_FAIL
  };

protected: //! @name methods related to ray-tracing

  //! Updates 3D scene geometry for ray-tracing.
  Standard_Boolean UpdateRaytraceGeometry (Standard_Boolean theCheck);

  //! Checks to see if the structure is modified.
  Standard_Boolean CheckRaytraceStructure (const OpenGl_Structure* theStructure);

  //! Updates 3D scene light sources for ray-tracing.
  Standard_Boolean UpdateRaytraceLightSources (const GLdouble theInvModelView[16]);

  //! Updates environment map for ray-tracing.
  Standard_Boolean UpdateRaytraceEnvironmentMap();

  //! Adds OpenGL structure to ray-traced scene geometry.
  Standard_Boolean AddRaytraceStructure (const OpenGl_Structure* theStruct,
                       const float* theTrans, std::set<const OpenGl_Structure*>& theElements);

  //! Adds OpenGL primitive array to ray-traced scene geometry.
  Standard_Boolean AddRaytracePrimitiveArray (
                       const CALL_DEF_PARRAY* theArray, int theMatID, const float* theTrans);

  //! Adds vertex indices from OpenGL primitive array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceVertexIndices (const CALL_DEF_PARRAY* theArray,
   int theFirstVert, int theVertOffset, int theVertNum, int theMatID);

  //! Adds OpenGL triangle array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceTriangleArray (const CALL_DEF_PARRAY* theArray,
                              int theFirstVert, int theVertOffset, int theVertNum, int theMatID);

  //! Adds OpenGL triangle fan array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceTriangleFanArray (const CALL_DEF_PARRAY* theArray,
                              int theFirstVert, int theVertOffset, int theVertNum, int theMatID);

  //! Adds OpenGL triangle fan array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceTriangleStripArray (const CALL_DEF_PARRAY* theArray,
                              int theFirstVert, int theVertOffset, int theVertNum, int theMatID);

  //! Adds OpenGL quadrangle array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceQuadrangleArray (const CALL_DEF_PARRAY* theArray,
                              int theFirstVert, int theVertOffset, int theVertNum, int theMatID);

  //! Adds OpenGL quadrangle strip array to ray-traced scene geometry.
  Standard_Boolean AddRaytraceQuadrangleStripArray (const CALL_DEF_PARRAY* theArray,
                              int theFirstVert, int theVertOffset, int theVertNum, int theMatID);

  //! Adds OpenGL polygon array to ray-traced scene geometry.
  Standard_Boolean AddRaytracePolygonArray (const CALL_DEF_PARRAY* theArray,
                              int theFirstVert, int theVertOffset, int theVertNum, int theMatID);

  //! Initializes OpenCL resources.
  Standard_Boolean InitOpenCL();
  
  //! Releases OpenCL resources.
  void ReleaseOpenCL();

  //! Resizes OpenCL output image.
  Standard_Boolean ResizeRaytraceOutputBuffer (const cl_int theSizeX, const cl_int theSizeY);

  //! Writes scene geometry to OpenCl device.
  Standard_Boolean WriteRaytraceSceneToDevice();

  //! Runs OpenCL ray-tracing kernels.
  Standard_Boolean RunRaytraceOpenCLKernels (const Graphic3d_CView& theCView,
                                             const GLfloat theOrigins[16],
                                             const GLfloat theDirects[16],
                                             const int theSizeX,
                                             const int theSizeY);

  //! Redraws the window using OpenCL ray tracing.
  Standard_Boolean Raytrace (const Graphic3d_CView& theCView,
              const int theSizeX, int theSizeY, const Tint theToSwap);

protected: //! @name fields related to ray-tracing

  //! Result of OpenCL initialization.
  OpenClInitStatus myComputeInitStatus;
  //! Is ATI/AMD OpenCL platform used?
  Standard_Boolean myIsAmdComputePlatform;

  //! Is geometry data valid?
  Standard_Boolean myIsRaytraceDataValid;
  //! Is geometry data musty be updated?
  Standard_Boolean myToUpdateRaytraceData;
  //! 3D scene geometry data for ray-tracing.
  OpenGl_RaytraceScene myRaytraceSceneData;

  //! Radius of bounding sphere of the scene.
  float myRaytraceSceneRadius;
  //! Scene epsilon to prevent self-intersections.
  float myRaytraceSceneEpsilon;

  //! Binned SAH-based BVH builder.
  OpenGl_BinnedBVHBuilder myBVHBuilder;

  //! OpenCL context.
  cl_context myComputeContext;
  //! OpenCL command queue.
  cl_command_queue myRaytraceQueue;
  //! OpenCL computing program.
  cl_program myRaytraceProgram;
  //! OpenCL ray-tracing render kernel.
  cl_kernel myRaytraceRenderKernel;
  //! OpenCL adaptive anti-aliasing kernel.
  cl_kernel myRaytraceSmoothKernel;

  //! OpenCL image to store environment map.
  cl_mem myRaytraceEnvironment;
  //! OpenCL image to store rendering result.
  cl_mem myRaytraceOutputImage;
  //! OpenCL image to store anti-aliasing result.
  cl_mem myRaytraceOutputImageSmooth;

  //! OpenGL output texture handle.
  GLuint myRaytraceOutputTexture[2];

  //! OpenCL buffer of vertex normals.
  cl_mem myRaytraceNormalBuffer;
  //! OpenCL buffer of vertex coordinates.
  cl_mem myRaytraceVertexBuffer;
  //! OpenCL buffer of indices of triangle vertices.
  cl_mem myRaytraceTriangleBuffer;

  //! OpenCL buffer of minimum points of BVH nodes.
  cl_mem myRaytraceNodeMinPointBuffer;
  //! OpenCL buffer of maximum points of BVH nodes.
  cl_mem myRaytraceNodeMaxPointBuffer;
  //! OpenCL buffer of data records of BVH nodes.
  cl_mem myRaytraceNodeDataRcrdBuffer;

  //! OpenCL buffer of material properties.
  cl_mem myRaytraceMaterialBuffer;
  
  //! OpenCL buffer of light source properties.
  cl_mem myRaytraceLightSourceBuffer;

  //! State of OpenGL view.
  Standard_Size myViewModificationStatus;

  //! State of OpenGL layer list.
  Standard_Size myLayersModificationStatus;

  //! State of OpenGL elements reflected to ray-tracing.
  std::map<const OpenGl_Structure*, Standard_Size> myStructureStates;

#endif // HAVE_OPENCL

protected: //! @name protected fields

  Handle(OpenGl_PrinterContext) myPrintContext;
  Handle(OpenGl_View)    myView;            // WSViews - now just one view is supported
  Standard_Boolean       myIsTransientOpen; // transientOpen
  Standard_Boolean       myRetainMode;
  Standard_Boolean       myTransientDrawToFront; //!< optimization flag for immediate mode (to render directly to the front buffer)

  NCollection_Sequence<const OpenGl_Structure*> myTransientList;

  Standard_Boolean       myUseTransparency;
  Standard_Boolean       myUseZBuffer;
  Standard_Boolean       myUseDepthTest;
  Standard_Boolean       myUseGLLight;
  Standard_Boolean       myBackBufferRestored;

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

  OpenGl_Material myMatFront; //!< current front material state (cached to reduce GL context updates)
  OpenGl_Material myMatBack;  //!< current back  material state
  OpenGl_Material myMatTmp;   //!< temporary variable

  OpenGl_Matrix myModelViewMatrix; //!< Model matrix with applied structure transformations

  TEL_POFFSET_PARAM PolygonOffset_applied; //!< Currently applied polygon offset.

  OpenGl_AspectFace myAspectFaceHl; //!< Hiddenline aspect

public: //! @name type definition

  DEFINE_STANDARD_RTTI(OpenGl_Workspace)
  DEFINE_STANDARD_ALLOC

};

#endif // _OpenGl_Workspace_Header
