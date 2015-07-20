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

#include <NCollection_Mat4.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_GlCore11.hxx>
#include <OpenGl_GraduatedTrihedron.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Texture.hxx>
#include <OpenGl_Trihedron.hxx>
#include <OpenGl_View.hxx>
#include <OpenGl_Workspace.hxx>

#include <Graphic3d_TextureEnv.hxx>
#include <Graphic3d_Mat4d.hxx>


/*----------------------------------------------------------------------*/

namespace
{
  static const OPENGL_ZCLIP myDefaultZClip = { { Standard_True, 0.F }, { Standard_True, 1.F } };
  static const OPENGL_FOG myDefaultFog = { Standard_False, 0.F, 1.F, { { 0.F, 0.F, 0.F, 1.F } } };
}

/*----------------------------------------------------------------------*/

OpenGl_View::OpenGl_View (const CALL_DEF_VIEWCONTEXT &AContext,
                          OpenGl_StateCounter*       theCounter)
: mySurfaceDetail(Visual3d_TOD_ALL),
  myBackfacing(0),
  //shield_indicator = TOn,
  //shield_colour = { { 0.F, 0.F, 0.F, 1.F } },
  //border_indicator = TOff,
  //border_colour = { { 0.F, 0.F, 0.F, 1.F } },
  //active_status = TOn,
  myZClip(myDefaultZClip),
  myCamera(AContext.Camera),
  myFog(myDefaultFog),
  myToShowTrihedron (false),
  myToShowGradTrihedron (false),
  myVisualization(AContext.Visualization),
  myShadingModel ((Visual3d_TypeOfModel )AContext.Model),
  myAntiAliasing(Standard_False),
  myWorldViewProjState(),
  myStateCounter (theCounter),
  myLastLightSourceState (0, 0),
  myTextureParams   (new OpenGl_AspectFace()),
  myBgGradientArray (new OpenGl_BackgroundArray (Graphic3d_TOB_GRADIENT)),
  myBgTextureArray  (new OpenGl_BackgroundArray (Graphic3d_TOB_TEXTURE)),
  // ray-tracing fields initialization
  myRaytraceInitStatus (OpenGl_RT_NONE),
  myIsRaytraceDataValid (Standard_False),
  myIsRaytraceWarnTextures (Standard_False),
  myToUpdateEnvironmentMap (Standard_False),
  myLayerListState (0)
{
  myCurrLightSourceState = myStateCounter->Increment();
}

/*----------------------------------------------------------------------*/

OpenGl_View::~OpenGl_View()
{
  ReleaseGlResources (NULL); // ensure ReleaseGlResources() was called within valid context
  OpenGl_Element::Destroy (NULL, myBgGradientArray);
  OpenGl_Element::Destroy (NULL, myBgTextureArray);
  OpenGl_Element::Destroy (NULL, myTextureParams);
}

void OpenGl_View::ReleaseGlResources (const Handle(OpenGl_Context)& theCtx)
{
  myTrihedron         .Release (theCtx.operator->());
  myGraduatedTrihedron.Release (theCtx.operator->());

  if (!myTextureEnv.IsNull())
  {
    theCtx->DelayedRelease (myTextureEnv);
    myTextureEnv.Nullify();
  }

  if (myTextureParams != NULL)
  {
    myTextureParams->Release (theCtx.operator->());
  }
  if (myBgGradientArray != NULL)
  {
    myBgGradientArray->Release (theCtx.operator->());
  }
  if (myBgTextureArray != NULL)
  {
    myBgTextureArray->Release (theCtx.operator->());
  }

  releaseRaytraceResources (theCtx);
}

void OpenGl_View::SetTextureEnv (const Handle(OpenGl_Context)&       theCtx,
                                 const Handle(Graphic3d_TextureEnv)& theTexture)
{
  if (!myTextureEnv.IsNull())
  {
    theCtx->DelayedRelease (myTextureEnv);
    myTextureEnv.Nullify();
  }

  if (theTexture.IsNull())
  {
    return;
  }

  myTextureEnv = new OpenGl_Texture (theTexture->GetParams());
  Handle(Image_PixMap) anImage = theTexture->GetImage();
  if (!anImage.IsNull())
    myTextureEnv->Init (theCtx, *anImage.operator->(), theTexture->Type());

  myToUpdateEnvironmentMap = Standard_True;
}

void OpenGl_View::SetSurfaceDetail (const Visual3d_TypeOfSurfaceDetail theMode)
{
  mySurfaceDetail = theMode;

  myToUpdateEnvironmentMap = Standard_True;
}

// =======================================================================
// function : SetBackfacing
// purpose  :
// =======================================================================
void OpenGl_View::SetBackfacing (const Standard_Integer theMode)
{
  myBackfacing = theMode;
}

// =======================================================================
// function : SetLights
// purpose  :
// =======================================================================
void OpenGl_View::SetLights (const CALL_DEF_VIEWCONTEXT& theViewCtx)
{
  myLights.Clear();
  for (Standard_Integer aLightIt = 0; aLightIt < theViewCtx.NbActiveLight; ++aLightIt)
  {
    myLights.Append (theViewCtx.ActiveLight[aLightIt]);
  }
  myCurrLightSourceState = myStateCounter->Increment();
}

/*----------------------------------------------------------------------*/

//call_togl_setvisualisation
void OpenGl_View::SetVisualisation (const CALL_DEF_VIEWCONTEXT &AContext)
{
  myVisualization = AContext.Visualization;
  myShadingModel  = (Visual3d_TypeOfModel )AContext.Model;
}

/*----------------------------------------------------------------------*/

//call_togl_cliplimit
void OpenGl_View::SetClipLimit (const Graphic3d_CView& theCView)
{
  myZClip.Back.Limit = theCView.Context.ZClipBackPlane;
  myZClip.Front.Limit = theCView.Context.ZClipFrontPlane;

  myZClip.Back.IsOn  = (theCView.Context.BackZClipping  != 0);
  myZClip.Front.IsOn = (theCView.Context.FrontZClipping != 0);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::SetFog (const Graphic3d_CView& theCView,
                          const Standard_Boolean theFlag)
{
  if (!theFlag)
  {
    myFog.IsOn = Standard_False;
  }
  else
  {
    myFog.IsOn = Standard_True;

    myFog.Front = theCView.Context.DepthFrontPlane;
    myFog.Back = theCView.Context.DepthBackPlane;

    myFog.Color.rgb[0] = theCView.DefWindow.Background.r;
    myFog.Color.rgb[1] = theCView.DefWindow.Background.g;
    myFog.Color.rgb[2] = theCView.DefWindow.Background.b;
    myFog.Color.rgb[3] = 1.0f;
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_View::TriedronDisplay (const Aspect_TypeOfTriedronPosition thePosition,
                                   const Quantity_NameOfColor          theColor,
                                   const Standard_Real                 theScale,
                                   const Standard_Boolean              theAsWireframe)
{
  myToShowTrihedron = true;
  myTrihedron.SetWireframe   (theAsWireframe);
  myTrihedron.SetPosition    (thePosition);
  myTrihedron.SetScale       (theScale);
  myTrihedron.SetLabelsColor (theColor);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::TriedronErase (const Handle(OpenGl_Context)& theCtx)
{
  myToShowTrihedron = false;
  myTrihedron.Release (theCtx.operator->());
}

/*----------------------------------------------------------------------*/

void OpenGl_View::GraduatedTrihedronDisplay (const Handle(OpenGl_Context)&       theCtx,
                                             const Graphic3d_GraduatedTrihedron& theData)
{
  myToShowGradTrihedron = true;
  myGraduatedTrihedron.SetValues (theCtx, theData);
}

/*----------------------------------------------------------------------*/

void OpenGl_View::GraduatedTrihedronErase (const Handle(OpenGl_Context)& theCtx)
{
  myToShowGradTrihedron = false;
  myGraduatedTrihedron.Release (theCtx.operator->());
}
