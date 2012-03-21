// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
#include <Visual3d_TypeOfSurfaceDetail.hxx>

#include <OpenGl_telem_view.hxx>
#include <OpenGl_LayerList.hxx>
#include <OpenGl_Light.hxx>

#include <Handle_OpenGl_Trihedron.hxx>
#include <Handle_OpenGl_GraduatedTrihedron.hxx>
#include <Handle_OpenGl_Workspace.hxx>
#include <Handle_OpenGl_View.hxx>

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

struct OPENGL_EXTRA_REP
{
  Tfloat  vrp[3];
  Tfloat  vpn[3];
  Tfloat  vup[3];
  TEL_VIEW_MAPPING map;
  Tfloat  scaleFactors[3];
};

struct OPENGL_CLIP_REP
{
  Standard_Real equation[4];
  DEFINE_STANDARD_ALLOC
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

class OpenGl_Structure;

class OpenGl_View : public MMgt_TShared
{
 public:
  OpenGl_View (const CALL_DEF_VIEWCONTEXT &AContext);
  virtual ~OpenGl_View ();

  void SetTextureEnv (const Standard_Integer AId) { myTextureEnv = AId; }
  void SetSurfaceDetail (const Visual3d_TypeOfSurfaceDetail AMode) { mySurfaceDetail = AMode; }
  void SetBackfacing (const Standard_Integer AMode);
  void SetLights (const CALL_DEF_VIEWCONTEXT &AContext);
  void SetAntiAliasing (const Standard_Boolean AMode) { myAntiAliasing = AMode; }
  void SetClippingPlanes (const CALL_DEF_VIEWCONTEXT &AContext);
  void SetVisualisation (const CALL_DEF_VIEWCONTEXT &AContext);

  void SetClipLimit (const CALL_DEF_VIEW &ACView);
  void SetMapping (const CALL_DEF_VIEW &ACView);
  void SetOrientation (const CALL_DEF_VIEW &ACView);

  void SetFog (const CALL_DEF_VIEW &ACView, const Standard_Boolean AFlag);

  void TriedronDisplay (const Aspect_TypeOfTriedronPosition APosition, const Quantity_NameOfColor AColor, const Standard_Real AScale, const Standard_Boolean AsWireframe);
  void TriedronErase ();

  void GraduatedTrihedronDisplay (const Graphic3d_CGraduatedTrihedron &ACubic);
  void GraduatedTrihedronErase ();

  Standard_Boolean ProjectObjectToRaster (const Standard_Integer w, const Standard_Integer h,
                                          const Standard_ShortReal x, const Standard_ShortReal y, const Standard_ShortReal z,
                                          Standard_ShortReal &xr, Standard_ShortReal &yr);
  Standard_Boolean ProjectRasterToObject (const Standard_Integer w, const Standard_Integer h,
                                          const Standard_Integer xr, const Standard_Integer yr,
                                          Standard_ShortReal &x, Standard_ShortReal &y, Standard_ShortReal &z);
  Standard_Boolean ProjectRasterToObjectWithRay (const Standard_Integer w, const Standard_Integer h,
                                                 const Standard_Integer xr, const Standard_Integer yr,
                                                 Standard_ShortReal &x, Standard_ShortReal &y, Standard_ShortReal &z,
                                                 Standard_ShortReal &dx, Standard_ShortReal &dy, Standard_ShortReal &dz);
  void GetMatrices (TColStd_Array2OfReal&  theMatOrient,
                    TColStd_Array2OfReal&  theMatMapping,
                    const Standard_Boolean theIsCustom) const;

  Standard_Real Height () const { return (myExtra.map.window.xmax - myExtra.map.window.xmin); }
  Standard_Real Width () const { return (myExtra.map.window.ymax - myExtra.map.window.ymin); }

  Standard_Integer Backfacing () const { return myBackfacing; }

  const TEL_TRANSFORM_PERSISTENCE * BeginTransformPersistence ( const TEL_TRANSFORM_PERSISTENCE *ATransPers );
  void EndTransformPersistence ();

  //! Add structure to display list with specified priority.
  //! The structure will be added to associated with it z layer.
  //! If the z layer is not presented in the view, the structure will
  //! be displayed in default bottom-level z layer.
  void DisplayStructure (const OpenGl_Structure *theStructure,
                         const Standard_Integer  thePriority);

  //! Erase structure from display list.
  void EraseStructure (const OpenGl_Structure *theStructure);

  //! Insert a new top-level z layer with ID <theLayerId>
  void AddZLayer (const Standard_Integer theLayerId);

  //! Remove a z layer with ID <theLayerId>
  void RemoveZLayer (const Standard_Integer theLayerId);

  //! Display structure in z layer with ID <theNewLayerId>
  //! If the layer with ID <theNewLayerId> is not presented in the view,
  //! the structure will be displayed in default bottom-level layer.
  void ChangeZLayer (const OpenGl_Structure *theStructure,
                     const Standard_Integer  theNewLayerId);

  void CreateBackgroundTexture (const Standard_CString AFileName, const Aspect_FillMethod AFillStyle);
  void SetBackgroundTextureStyle (const Aspect_FillMethod FillStyle);
  void SetBackgroundGradient (const Quantity_Color& AColor1, const Quantity_Color& AColor2, const Aspect_GradientFillMethod AType);
  void SetBackgroundGradientType (const Aspect_GradientFillMethod AType);

  void Render (const Handle(OpenGl_Workspace) &AWorkspace,
               const Graphic3d_CView& ACView,
               const Aspect_CLayer2d& ACUnderLayer,
               const Aspect_CLayer2d& ACOverLayer);

  // Type definition
  //
  DEFINE_STANDARD_RTTI(OpenGl_View)

 protected:

  void RenderStructs (const Handle(OpenGl_Workspace) &AWorkspace);
  void RedrawLayer2d (const Handle(OpenGl_Workspace) &AWorkspace, const Graphic3d_CView& ACView, const Aspect_CLayer2d& ACLayer);

  Standard_Integer myTextureEnv; //WSTextureEnv
  Visual3d_TypeOfSurfaceDetail mySurfaceDetail; //WSSurfaceDetail
  Standard_Integer myBackfacing; //WSBackfacing

  OPENGL_BG_TEXTURE myBgTexture; //WSBgTexture
  OPENGL_BG_GRADIENT myBgGradient; //WSBgGradient

  //{ myViewRep
  Tmatrix3    myOrientationMatrix;
  Tmatrix3    myMappingMatrix;

  //Tint        shield_indicator;
  //TEL_COLOUR  shield_colour;
  //Tint        border_indicator;
  //TEL_COLOUR  border_colour;
  //Tint        active_status;

  OPENGL_ZCLIP   myZClip;
  NCollection_List<OPENGL_CLIP_REP> myClippingPlanes;

  OPENGL_EXTRA_REP myExtra;
  //}

  OPENGL_FOG myFog;
  Handle(OpenGl_Trihedron) myTrihedron;
  Handle(OpenGl_GraduatedTrihedron) myGraduatedTrihedron;

  //View_LABViewContext
  int myVisualization;
  int myIntShadingMethod;

  //View_LABLight
  OpenGl_ListOfLight myLights;

  //View_LABPlane
  //View_LABAliasing
  Standard_Boolean myAntiAliasing;

  //View_LABDepthCueing - fixed index used

  OpenGl_LayerList myZLayers;

  int myAnimationListIndex;
  Standard_Boolean myAnimationListReady;

  const TEL_TRANSFORM_PERSISTENCE *myTransPers;
  Standard_Boolean myIsTransPers;

  Standard_Boolean myResetFLIST;

 public:
  DEFINE_STANDARD_ALLOC
};

#endif //_OpenGl_View_Header
