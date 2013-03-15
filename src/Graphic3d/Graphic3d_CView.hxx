// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#ifndef _Graphic3d_CView_HeaderFile
#define _Graphic3d_CView_HeaderFile

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>
#include <Handle_Graphic3d_TextureEnv.hxx>
#include <Standard_Type.hxx>

class CALL_DEF_VIEWCONTEXT
{

public:

  CALL_DEF_VIEWCONTEXT()
  : Aliasing (0),
    BackZClipping  (0),
    FrontZClipping (0),
    DepthCueing (0),
    ZClipFrontPlane (0.0f),
    ZClipBackPlane  (0.0f),
    DepthFrontPlane (0.0f),
    DepthBackPlane  (0.0f),
    ZBufferActivity (0),
    Model (0),
    Visualization (0),
    NbActiveLight (0),
    ActiveLight (NULL),
    NbActivePlane (0),
    ActivePlane (NULL),
    SurfaceDetail (0)
  {
    //
  }

public:

  int   Aliasing;

  int   BackZClipping;
  int   FrontZClipping;

  int   DepthCueing;

  float ZClipFrontPlane;
  float ZClipBackPlane;

  float DepthFrontPlane;
  float DepthBackPlane;

  int   ZBufferActivity;
  int   Model;
  int   Visualization;

  int   NbActiveLight;
  CALL_DEF_LIGHT* ActiveLight;

  int   NbActivePlane;
  CALL_DEF_PLANE* ActivePlane;

  Handle(Graphic3d_TextureEnv) TextureEnv;
  int   SurfaceDetail;

};

class Graphic3d_CView
{

public:

  Graphic3d_CView()
  : WsId (-1),
    ViewId (0),
    ptrView (NULL),
    IsDeleted (0),
    IsOpen (0),
    Active (0),
    ptrUnderLayer (NULL),
    ptrOverLayer  (NULL),
    Backfacing  (0),
    GDisplayCB  (NULL),
    GClientData (NULL),
    ptrFBO (NULL)
  {
    //
  }

public:

  int   WsId;
  int   ViewId;
  void* ptrView;

  int   IsDeleted;
  int   IsOpen;

  int   Active;

  CALL_DEF_VIEWORIENTATION Orientation;
  CALL_DEF_VIEWMAPPING     Mapping;

  CALL_DEF_VIEWORIENTATION OrientationReset;
  CALL_DEF_VIEWMAPPING     MappingReset;

  CALL_DEF_VIEWCONTEXT     Context;

  CALL_DEF_WINDOW          DefWindow;

  void* ptrUnderLayer;
  void* ptrOverLayer;

  int   Backfacing;

  Aspect_RenderingContext    GContext;
  Aspect_GraphicCallbackProc GDisplayCB;
  void* GClientData;

  void* ptrFBO;

};

const Handle(Standard_Type)& TYPE(Graphic3d_CView);

#endif // Graphic3d_CView_HeaderFile
