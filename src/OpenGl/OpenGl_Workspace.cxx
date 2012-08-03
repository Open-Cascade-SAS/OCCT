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


#include <OpenGl_GlCore11.hxx>

#include <InterfaceGraphic.hxx>

#include <OpenGl_Workspace.hxx>

#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>

#include <OpenGl_TextureBox.hxx>

IMPLEMENT_STANDARD_HANDLE(OpenGl_Workspace,OpenGl_Window)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_Workspace,OpenGl_Window)

namespace
{
  static const TEL_COLOUR myDefaultHighlightColor = { { 1.F, 1.F, 1.F, 1.F } };

  static const OpenGl_AspectLine myDefaultAspectLine;
  static const OpenGl_AspectFace myDefaultAspectFace;
  static const OpenGl_AspectMarker myDefaultAspectMarker;
  static const OpenGl_AspectText myDefaultAspectText;

  static const OpenGl_TextParam myDefaultTextParam =
  {
    16, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM
  };

  static const OpenGl_Matrix myDefaultMatrix =
  {
    {{ 1.0F, 0.0F, 0.0F, 0.0F },
     { 0.0F, 1.0F, 0.0F, 0.0F },
     { 0.0F, 0.0F, 1.0F, 0.0F },
     { 0.0F, 0.0F, 0.0F, 1.0F }}
  };
};

// =======================================================================
// function : OpenGl_Workspace
// purpose  :
// =======================================================================
OpenGl_Workspace::OpenGl_Workspace (const Handle(OpenGl_Display)& theDisplay,
                                    const CALL_DEF_WINDOW&        theCWindow,
                                    Aspect_RenderingContext       theGContext,
                                    const Handle(OpenGl_Context)& theShareCtx)
: OpenGl_Window (theDisplay, theCWindow, theGContext, theShareCtx),
  myTransientList (0),
  myIsTransientOpen (Standard_False),
  myRetainMode (Standard_False),
  myUseTransparency (Standard_False),
  myUseZBuffer (Standard_False),
  myUseDepthTest (Standard_True),
  myUseGLLight (Standard_True),
  myBackBufferRestored (Standard_False),
  //
  NamedStatus (0),
  DegenerateModel (0),
  SkipRatio (0.F),
  HighlightColor (&myDefaultHighlightColor),
  AspectLine_set (&myDefaultAspectLine),
  AspectLine_applied (NULL),
  AspectFace_set (&myDefaultAspectFace),
  AspectFace_applied (NULL),
  AspectMarker_set (&myDefaultAspectMarker),
  AspectMarker_applied (NULL),
  AspectText_set (&myDefaultAspectText),
  AspectText_applied (NULL),
  TextParam_set (&myDefaultTextParam),
  TextParam_applied (NULL),
  ViewMatrix_applied (&myDefaultMatrix),
  StructureMatrix_applied (&myDefaultMatrix),
  PolygonOffset_applied (NULL)
{
  theDisplay->InitAttributes();

  // General initialization of the context

  // Eviter d'avoir les faces mal orientees en noir.
  // Pourrait etre utiliser pour detecter les problemes d'orientation
  glLightModeli ((GLenum )GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); 

  // Optimisation pour le Fog et l'antialiasing
  glHint (GL_FOG_HINT,            GL_FASTEST);
  glHint (GL_POINT_SMOOTH_HINT,   GL_FASTEST);
  glHint (GL_LINE_SMOOTH_HINT,    GL_FASTEST);
  glHint (GL_POLYGON_SMOOTH_HINT, GL_FASTEST);

  // Polygon Offset
  EnablePolygonOffset();
}

// =======================================================================
// function : ~OpenGl_Workspace
// purpose  :
// =======================================================================
OpenGl_Workspace::~OpenGl_Workspace()
{
}

// =======================================================================
// function : Activate
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_Workspace::Activate()
{
  if (!OpenGl_Window::Activate())
    return Standard_False;

  DegenerateModel         = 0;
  SkipRatio               = 0.0f;
  ViewMatrix_applied      = &myDefaultMatrix;
  StructureMatrix_applied = &myDefaultMatrix;

  ResetAppliedAspect();

  return Standard_True;
}

// =======================================================================
// function : UseTransparency
// purpose  : call_togl_transparency
// =======================================================================
void OpenGl_Workspace::UseTransparency (const Standard_Boolean theFlag)
{
  if ((myUseTransparency ? 1 : 0) != (theFlag ? 1 : 0))
  {
    myUseTransparency = theFlag;
    EraseAnimation();
  }
}

//=======================================================================
//function : ResetAppliedAspect
//purpose  : Sets default values of GL parameters in accordance with default aspects
//=======================================================================
void OpenGl_Workspace::ResetAppliedAspect()
{
  NamedStatus           = IsTextureEnabled() ? OPENGL_NS_TEXTURE : 0;
  HighlightColor        = &myDefaultHighlightColor;
  AspectLine_set        = &myDefaultAspectLine;
  AspectLine_applied    = NULL;
  AspectFace_set        = &myDefaultAspectFace;
  AspectFace_applied    = NULL;
  AspectMarker_set      = &myDefaultAspectMarker;
  AspectMarker_applied  = NULL;
  AspectText_set        = &myDefaultAspectText;
  AspectText_applied    = NULL;
  TextParam_set         = &myDefaultTextParam;
  TextParam_applied     = NULL;
  PolygonOffset_applied = NULL;

  AspectLine(Standard_True);
  AspectFace(Standard_True);
  AspectMarker(Standard_True);
  AspectText(Standard_True);
}
