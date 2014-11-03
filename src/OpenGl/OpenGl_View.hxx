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
#include <Graphic3d_CGraduatedTrihedron.hxx>
#include <Graphic3d_SequenceOfHClipPlane.hxx>
#include <Graphic3d_ZLayerSettings.hxx>
#include <Visual3d_TypeOfSurfaceDetail.hxx>
#include <Visual3d_TypeOfModel.hxx>

#include <OpenGl_BVHTreeSelector.hxx>
#include <OpenGl_LayerList.hxx>
#include <OpenGl_Light.hxx>
#include <OpenGl_LineAttributes.hxx>

#include <Handle_OpenGl_Context.hxx>
#include <Handle_OpenGl_GraphicDriver.hxx>
#include <Handle_OpenGl_Workspace.hxx>
#include <Handle_OpenGl_View.hxx>
#include <Handle_OpenGl_Texture.hxx>

struct OPENGL_BG_TEXTURE
{
  Tuint             TexId;
  Tint              Width;
  Tint              Height;
  Aspect_FillMethod Style;
};

struct OPENGL_BG_GRADIENT
{
  TEL_COLOUR color1;
  TEL_COLOUR color2;
  Aspect_GradientFillMethod type;
};

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
class OpenGl_GraduatedTrihedron;
class OpenGl_Structure;
class OpenGl_Trihedron;
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

  void TriedronDisplay (const Handle(OpenGl_Context)&       theCtx,
                        const Aspect_TypeOfTriedronPosition thePosition,
                        const Quantity_NameOfColor          theColor,
                        const Standard_Real                 theScale,
                        const Standard_Boolean              theAsWireframe);
  void TriedronErase (const Handle(OpenGl_Context)& theCtx);

  void GraduatedTrihedronDisplay (const Handle(OpenGl_Context)&        theCtx,
                                  const Graphic3d_CGraduatedTrihedron& theCubic);
  void GraduatedTrihedronErase (const Handle(OpenGl_Context)& theCtx);

  Standard_Real Height () const { return myCamera->ViewDimensions().X(); }
  Standard_Real Width () const { return myCamera->ViewDimensions().Y(); }

  Standard_Integer Backfacing () const { return myBackfacing; }

  const TEL_TRANSFORM_PERSISTENCE * BeginTransformPersistence (const Handle(OpenGl_Context)& theCtx,
                                                               const TEL_TRANSFORM_PERSISTENCE *theTransPers);
  void EndTransformPersistence (const Handle(OpenGl_Context)& theCtx);

  //! Add structure to display list with specified priority.
  //! The structure will be added to associated with it z layer.
  //! If the z layer is not presented in the view, the structure will
  //! be displayed in default bottom-level z layer.
  void DisplayStructure (const OpenGl_Structure* theStructure,
                         const Standard_Integer  thePriority);

  //! Erase structure from display list.
  void EraseStructure (const OpenGl_Structure* theStructure);

  //! Add structure to the list of immediate structures.
  void DisplayImmediateStructure (const OpenGl_Structure* theStructure);

  //! Erase structure from display list.
  void EraseImmediateStructure (const OpenGl_Structure* theStructure);

  //! Insert a new top-level z layer with ID <theLayerId>
  void AddZLayer (const Standard_Integer theLayerId);

  //! Remove a z layer with ID <theLayerId>
  void RemoveZLayer (const Standard_Integer theLayerId);

  //! Display structure in z layer with ID <theNewLayerId>
  //! If the layer with ID <theNewLayerId> is not presented in the view,
  //! the structure will be displayed in default bottom-level layer.
  void ChangeZLayer (const OpenGl_Structure *theStructure,
                     const Standard_Integer  theNewLayerId);

  //! Sets the settings for a single Z layer of specified view.
  void SetZLayerSettings (const Standard_Integer theLayerId,
                          const Graphic3d_ZLayerSettings theSettings);

  //! Changes the priority of a structure within its ZLayer
  void ChangePriority (const OpenGl_Structure *theStructure,
                       const Standard_Integer theNewPriority);

  void CreateBackgroundTexture (const Standard_CString AFileName, const Aspect_FillMethod AFillStyle);
  void SetBackgroundTextureStyle (const Aspect_FillMethod FillStyle);
  void SetBackgroundGradient (const Quantity_Color& AColor1, const Quantity_Color& AColor2, const Aspect_GradientFillMethod AType);
  void SetBackgroundGradientType (const Aspect_GradientFillMethod AType);

  void Render (const Handle(OpenGl_PrinterContext)& thePrintContext,
               const Handle(OpenGl_Workspace)&      theWorkspace,
               const Graphic3d_CView&               theCView,
               const Aspect_CLayer2d&               theCUnderLayer,
               const Aspect_CLayer2d&               theCOverLayer);


  void DrawBackground (OpenGl_Workspace& theWorkspace);

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

  //! Returns view-mapping and orientation matrices.
  void GetMatrices (OpenGl_Mat4& theOrientation,
                    OpenGl_Mat4& theViewMapping) const;

  //! Returns list of immediate structures rendered on top of main presentation
  const OpenGl_SequenceOfStructure& ImmediateStructures() const
  {
    return myImmediateList;
  }

  //! Returns modification state for ray-tracing.
  Standard_Size ModificationState() const { return myModificationState; }

protected:

  void RenderStructs (const Handle(OpenGl_Workspace) &AWorkspace);
  void RedrawLayer2d (const Handle(OpenGl_PrinterContext)& thePrintContext,
                      const Handle(OpenGl_Workspace) &theWorkspace,
                      const Graphic3d_CView&               theCView,
                      const Aspect_CLayer2d&               theCLayer);
  void RedrawTrihedron (const Handle(OpenGl_Workspace) &theWorkspace);

  //! Redraw contents of model scene: clipping planes,
  //! lights, structures. The peculiar properties of "scene" is that
  //! it requires empty Z-Buffer and uses projection and orientation
  //! matrices supplied by 3d view.
  //! @param thePrintCtx [in] printer context which facilitates tiled printing.
  //! @param theWorkspace [in] rendering workspace.
  //! @param theCView [in] view data.
  //! @param theProjection [in] view projection matrix.
  //! @param theOrientation [in] view orientation matrix.
  void RedrawScene (const Handle(OpenGl_PrinterContext)& thePrintContext,
                    const Handle(OpenGl_Workspace)& theWorkspace);

  Handle(OpenGl_LineAttributes) myLineAttribs;
  Handle(OpenGl_Texture)        myTextureEnv;
  Visual3d_TypeOfSurfaceDetail  mySurfaceDetail;
  Standard_Integer              myBackfacing;

  OPENGL_BG_TEXTURE  myBgTexture;
  OPENGL_BG_GRADIENT myBgGradient;

  OPENGL_ZCLIP   myZClip;

  Graphic3d_SequenceOfHClipPlane myClipPlanes;
  
  Handle(Graphic3d_Camera) myCamera;

  OPENGL_FOG myFog;
  OpenGl_Trihedron*          myTrihedron;
  OpenGl_GraduatedTrihedron* myGraduatedTrihedron;

  //View_LABViewContext
  int myVisualization;
  Visual3d_TypeOfModel       myShadingModel;  //!< lighting shading model

  //View_LABLight
  OpenGl_ListOfLight myLights;

  //View_LABPlane
  //View_LABAliasing
  Standard_Boolean myAntiAliasing;

  //View_LABDepthCueing - fixed index used

  OpenGl_LayerList           myZLayers;       //!< main list of displayed structure, sorted by layers
  OpenGl_SequenceOfStructure myImmediateList; //!< list of immediate structures rendered on top of main presentation

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

  Standard_Size myModificationState;

public:

  DEFINE_STANDARD_ALLOC
  DEFINE_STANDARD_RTTI(OpenGl_View) // Type definition

  friend class OpenGl_Workspace;

};

#endif // _OpenGl_View_Header
