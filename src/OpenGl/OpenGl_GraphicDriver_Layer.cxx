// Created on: 2011-10-20
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

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_GraphicDriver.hxx>

#include <Font_FontAspect.hxx>

#include <OpenGl_AspectText.hxx>
#include <OpenGl_Text.hxx>
#include <OpenGl_TextParam.hxx>
#include <OpenGl_LineAttributes.hxx>

/*----------------------------------------------------------------------*/

struct OpenGl_LAYER_PROP
{
  int        ListId;

  TEL_COLOUR Color;
  int        NbPoints;
  int        LineType;
  float      LineWidth;
  OpenGl_AspectText AspectText;
  OpenGl_TextParam TextParam;
};

/*----------------------------------------------------------------------*/

static const TEL_COLOUR myDefaultColor = {{ 1.F, 1.F, 1.F, 1.F }};
static const CALL_DEF_CONTEXTTEXT myDefaultContextText =
{
  1, //IsDef
  1, //IsSet
  "Courier", //Font
  0.3F, //Space
  1.F, //Expan
  { 1.F, 1.F, 1.F }, //Color
  (int)Aspect_TOST_NORMAL, //Style
  (int)Aspect_TODT_NORMAL, //DisplayType
  { 1.F, 1.F, 1.F }, //ColorSubTitle
  0, //TextZoomable
  0.F, //TextAngle
  (int)Font_FA_Regular //TextFontAspect
};

static Standard_Boolean TheLayerIsOpen = Standard_False;
static OpenGl_LAYER_PROP TheLayerProp;

/*----------------------------------------------------------------------*/

void InitLayerProp (const int theListId)
{
  TheLayerProp.ListId = theListId;

  if (theListId)
  {
    TheLayerProp.Color = myDefaultColor;
    TheLayerProp.NbPoints = 0;
    TheLayerProp.LineType = -1;
    TheLayerProp.LineWidth = -1.F;

    TheLayerProp.AspectText.SetAspect (myDefaultContextText);

    TheLayerProp.TextParam.HAlign = Graphic3d_HTA_LEFT;
    TheLayerProp.TextParam.VAlign = Graphic3d_VTA_BOTTOM;
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_GraphicDriver::Layer (Aspect_CLayer2d& ACLayer)
{
  ACLayer.ptrLayer = new CALL_DEF_PTRLAYER();
#if !defined(GL_ES_VERSION_2_0)
  ACLayer.ptrLayer->listIndex = glGenLists(1);
#endif
}

/*----------------------------------------------------------------------*/

void OpenGl_GraphicDriver::RemoveLayer (const Aspect_CLayer2d& ACLayer)
{
  if (!ACLayer.ptrLayer) return;
  if (!ACLayer.ptrLayer->listIndex) return;
  if (TheLayerProp.ListId == ACLayer.ptrLayer->listIndex)
    EndLayer();

#if !defined(GL_ES_VERSION_2_0)
  glDeleteLists (ACLayer.ptrLayer->listIndex, 1);
#endif
  ACLayer.ptrLayer->listIndex = 0;
  //szvgl: memory leak here?
  //free ( ACLayer.ptrLayer );
  //ACLayer.ptrLayer = NULL;
}

/*----------------------------------------------------------------------*/

void OpenGl_GraphicDriver::BeginLayer (const Aspect_CLayer2d& ACLayer)
{
  call_def_ptrLayer ptrLayer = (call_def_ptrLayer) ACLayer.ptrLayer;
  if (!ptrLayer) return;

  InitLayerProp (ptrLayer->listIndex);
  if (!TheLayerProp.ListId) return;

#if !defined(GL_ES_VERSION_2_0)
  glNewList (TheLayerProp.ListId, GL_COMPILE);
#endif
  TheLayerIsOpen = Standard_True;
}

void OpenGl_GraphicDriver::BeginPolygon2d ()
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.NbPoints = 0;
#if !defined(GL_ES_VERSION_2_0)
  glBegin (GL_POLYGON);
#endif
}

void OpenGl_GraphicDriver::BeginPolyline2d ()
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.NbPoints = 0;
#if !defined(GL_ES_VERSION_2_0)
  glBegin (GL_LINE_STRIP);
#endif
}

void OpenGl_GraphicDriver::ClearLayer (const Aspect_CLayer2d& ACLayer)
{
  if (!ACLayer.ptrLayer) return;

  InitLayerProp (ACLayer.ptrLayer->listIndex);
  if (!TheLayerProp.ListId) return;

#if !defined(GL_ES_VERSION_2_0)
  glNewList (TheLayerProp.ListId, GL_COMPILE);
  glEndList ();
#endif
}

void OpenGl_GraphicDriver::Draw (const Standard_ShortReal X, const Standard_ShortReal Y)
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.NbPoints++;
#if !defined(GL_ES_VERSION_2_0)
  glVertex3f (X, Y, 0.F);
#endif
}

void OpenGl_GraphicDriver::Edge (const Standard_ShortReal X, const Standard_ShortReal Y)
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.NbPoints++;
#if !defined(GL_ES_VERSION_2_0)
  glVertex3f (X, Y, 0.F);
#endif
}

void OpenGl_GraphicDriver::EndLayer ()
{
  if (!TheLayerProp.ListId) return;
  if (TheLayerIsOpen)
  {
  #if !defined(GL_ES_VERSION_2_0)
    glEndList();
  #endif
    TheLayerIsOpen = Standard_False;
  }
  TheLayerProp.ListId = 0;
}

void OpenGl_GraphicDriver::EndPolygon2d ()
{
  if (!TheLayerProp.ListId) return;
#if !defined(GL_ES_VERSION_2_0)
  glEnd ();
#endif
}

void OpenGl_GraphicDriver::EndPolyline2d ()
{
  if (!TheLayerProp.ListId) return;
#if !defined(GL_ES_VERSION_2_0)
  glEnd ();
#endif
}

void OpenGl_GraphicDriver::Move (const Standard_ShortReal X, const Standard_ShortReal Y)
{
  if (!TheLayerProp.ListId) return;
  if (TheLayerProp.NbPoints)
  {
  #if !defined(GL_ES_VERSION_2_0)
    glEnd ();
    glBegin (GL_LINE_STRIP);
  #endif
    TheLayerProp.NbPoints = 0;
  }
  TheLayerProp.NbPoints++;
#if !defined(GL_ES_VERSION_2_0)
  glVertex3f (X, Y, 0.F);
#endif
}

void OpenGl_GraphicDriver::Rectangle (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Width, const Standard_ShortReal Height)
{
  if (!TheLayerProp.ListId) return;
#if !defined(GL_ES_VERSION_2_0)
  glRectf (X, Y, X + Width, Y + Height);
#endif
}

void OpenGl_GraphicDriver::SetColor (const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B)
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.Color.rgb[0] = R;
  TheLayerProp.Color.rgb[1] = G;
  TheLayerProp.Color.rgb[2] = B;
#if !defined(GL_ES_VERSION_2_0)
  glColor3fv (TheLayerProp.Color.rgb);
#endif
}

void OpenGl_GraphicDriver::SetTransparency (const Standard_ShortReal ATransparency)
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.Color.rgb[3] = ATransparency;
#if !defined(GL_ES_VERSION_2_0)
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4fv (TheLayerProp.Color.rgb);
#endif
}

void OpenGl_GraphicDriver::UnsetTransparency ()
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.Color.rgb[3] = 1.F;
#if !defined(GL_ES_VERSION_2_0)
  glDisable (GL_BLEND);
#endif
}

void OpenGl_GraphicDriver::SetLineAttributes (const Standard_Integer   theType,
                                              const Standard_ShortReal theWidth)
{
  const Handle(OpenGl_Context)& aCtx = GetSharedContext();
  if (!TheLayerProp.ListId
   || aCtx.IsNull())
  {
    return;
  }

  if (TheLayerProp.LineType != theType)
  {
    Handle(OpenGl_LineAttributes) aLineAttribs;
    if (aCtx->GetResource ("OpenGl_LineAttributes", aLineAttribs))
    {
      TheLayerProp.LineType = theType;
      aLineAttribs->SetTypeOfLine ((Aspect_TypeOfLine )theType);
    }
  }
  if (TheLayerProp.LineWidth != theWidth)
  {
    TheLayerProp.LineWidth = theWidth;
    glLineWidth ((GLfloat )theWidth);
  }
}

void OpenGl_GraphicDriver::SetTextAttributes (const Standard_CString   theFont,
                                              const Standard_Integer   theType,
                                              const Standard_ShortReal theR,
                                              const Standard_ShortReal theG,
                                              const Standard_ShortReal theB)
{
  if (!TheLayerProp.ListId)
  {
    return;
  }

  TheLayerProp.AspectText.ChangeFontName() = theFont;
  TheLayerProp.AspectText.SetDisplayType ((Aspect_TypeOfDisplayText )theType);
  TEL_COLOUR& aSubColor = TheLayerProp.AspectText.ChangeSubtitleColor();
  aSubColor.rgb[0] = (float )theR;
  aSubColor.rgb[1] = (float )theG;
  aSubColor.rgb[2] = (float )theB;
}

void OpenGl_GraphicDriver::Text (const Standard_CString   theText,
                                 const Standard_ShortReal theX,
                                 const Standard_ShortReal theY,
                                 const Standard_ShortReal theHeight)
{
  const Handle(OpenGl_Context)& aCtx = GetSharedContext();
  if (!TheLayerProp.ListId || aCtx.IsNull())
  {
    return;
  }

  const Standard_ShortReal aHeight = (theHeight < 2.0f) ? DefaultTextHeight() : theHeight;
  TheLayerProp.TextParam.Height = (int )aHeight;
  TheLayerProp.AspectText.ChangeColor() = TheLayerProp.Color;

  myTempText->Init (aCtx, TCollection_ExtendedString (theText), OpenGl_Vec2 (theX, theY), TheLayerProp.TextParam);
  myTempText->Render (myPrintContext, aCtx, TheLayerProp.AspectText);
  //myTempText->Release (aCtx);
}

void OpenGl_GraphicDriver::TextSize (const Standard_CString   theText,
                                     const Standard_ShortReal theHeight,
                                     Standard_ShortReal&      theWidth,
                                     Standard_ShortReal&      theAscent,
                                     Standard_ShortReal&      theDescent) const
{
  const Handle(OpenGl_Context)& aCtx = GetSharedContext();
  if (!TheLayerProp.ListId || aCtx.IsNull())
  {
    return;
  }

  const Standard_ShortReal aHeight = (theHeight < 2.0f) ? DefaultTextHeight() : theHeight;
  TheLayerProp.TextParam.Height = (int )aHeight;

  TCollection_ExtendedString anExtText = theText;
  NCollection_String aText = (Standard_Utf16Char* )anExtText.ToExtString();
  OpenGl_Text::StringSize (aCtx, aText, TheLayerProp.AspectText, TheLayerProp.TextParam, theWidth, theAscent, theDescent);
}
