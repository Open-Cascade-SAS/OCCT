// File:      OpenGl_GraphicDriver_Layer.cxx
// Created:   20 October 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#include <OpenGl_GraphicDriver.hxx>

#include <OSD_FontAspect.hxx>

#include <OpenGl_tgl_all.hxx>

#include <OpenGl_Display.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_TextParam.hxx>

/*----------------------------------------------------------------------*/

struct OpenGl_LAYER_PROP
{
  int        ListId;

  TEL_COLOUR Color;
  int        NbPoints;
  int        LineType;
  float      LineWidth;
  int        FontCurrent;

  Standard_Boolean FontChanged;

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
  (int)OSD_FA_Regular //TextFontAspect
};

static Standard_Boolean TheLayerIsOpen = Standard_False;
static OpenGl_LAYER_PROP TheLayerProp;

/*----------------------------------------------------------------------*/

void InitLayerProp (const int AListId)
{
  TheLayerProp.ListId = AListId;

  if (AListId)
  {
    TheLayerProp.Color = myDefaultColor;
    TheLayerProp.NbPoints = 0;
    TheLayerProp.LineType = -1;
    TheLayerProp.LineWidth = -1.F;
    TheLayerProp.FontCurrent = 0;

    TheLayerProp.FontChanged = Standard_False;

    TheLayerProp.AspectText.SetContext(myDefaultContextText);

    TheLayerProp.TextParam.Height = -1;
    TheLayerProp.TextParam.HAlign = Graphic3d_HTA_LEFT;
    TheLayerProp.TextParam.VAlign = Graphic3d_VTA_BOTTOM;
  }
}

/*----------------------------------------------------------------------*/

void OpenGl_GraphicDriver::Layer (Aspect_CLayer2d& ACLayer)
{
  ACLayer.ptrLayer = (call_def_ptrLayer) malloc (sizeof (CALL_DEF_PTRLAYER));
  ACLayer.ptrLayer->listIndex = glGenLists(1);
}

/*----------------------------------------------------------------------*/

void OpenGl_GraphicDriver::RemoveLayer (const Aspect_CLayer2d& ACLayer)
{
  if (!ACLayer.ptrLayer) return;
  if (!ACLayer.ptrLayer->listIndex) return;
  if (TheLayerProp.ListId == ACLayer.ptrLayer->listIndex)
    EndLayer();
  glDeleteLists (ACLayer.ptrLayer->listIndex, 1);
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

  InitLayerProp(ptrLayer->listIndex);
  if (!TheLayerProp.ListId) return;

  glEnable(GL_TEXTURE_2D);
  //GLboolean stat = glIsEnabled( GL_TEXTURE_2D );

  glNewList (TheLayerProp.ListId, GL_COMPILE);
  TheLayerIsOpen = Standard_True;
}

void OpenGl_GraphicDriver::BeginPolygon2d ()
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.NbPoints = 0;
  glBegin (GL_POLYGON);
}

void OpenGl_GraphicDriver::BeginPolyline2d ()
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.NbPoints = 0;
  glBegin (GL_LINE_STRIP);
}

void OpenGl_GraphicDriver::ClearLayer (const Aspect_CLayer2d& ACLayer)
{
  if (!ACLayer.ptrLayer) return;

  InitLayerProp(ACLayer.ptrLayer->listIndex);
  if (!TheLayerProp.ListId) return;

  glNewList (TheLayerProp.ListId, GL_COMPILE);
  glEndList ();
}

void OpenGl_GraphicDriver::Draw (const Standard_ShortReal X, const Standard_ShortReal Y)
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.NbPoints++;
  glVertex3f (X, Y, 0.F);
}

void OpenGl_GraphicDriver::Edge (const Standard_ShortReal X, const Standard_ShortReal Y)
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.NbPoints++;
  glVertex3f (X, Y, 0.F);
}

void OpenGl_GraphicDriver::EndLayer ()
{
  if (!TheLayerProp.ListId) return;
  if (TheLayerIsOpen)
  {
    glEndList();
    TheLayerIsOpen = Standard_False;
  }
  TheLayerProp.ListId = 0;
}

void OpenGl_GraphicDriver::EndPolygon2d ()
{
  if (!TheLayerProp.ListId) return;
  glEnd ();
}

void OpenGl_GraphicDriver::EndPolyline2d ()
{
  if (!TheLayerProp.ListId) return;
  glEnd ();
}

void OpenGl_GraphicDriver::Move (const Standard_ShortReal X, const Standard_ShortReal Y)
{
  if (!TheLayerProp.ListId) return;
  if (TheLayerProp.NbPoints)
  {
    glEnd ();
    TheLayerProp.NbPoints = 0;
    glBegin (GL_LINE_STRIP);
  }
  TheLayerProp.NbPoints++;
  glVertex3f (X, Y, 0.F);
}

void OpenGl_GraphicDriver::Rectangle (const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal Width, const Standard_ShortReal Height)
{
  if (!TheLayerProp.ListId) return;
  glRectf (X, Y, X + Width, Y + Height);
}

void OpenGl_GraphicDriver::SetColor (const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B)
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.Color.rgb[0] = R;
  TheLayerProp.Color.rgb[1] = G;
  TheLayerProp.Color.rgb[2] = B;
  glColor3fv (TheLayerProp.Color.rgb);
}

void OpenGl_GraphicDriver::SetTransparency (const Standard_ShortReal ATransparency)
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.Color.rgb[3] = ATransparency;
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColor4fv (TheLayerProp.Color.rgb);
}

void OpenGl_GraphicDriver::UnsetTransparency ()
{
  if (!TheLayerProp.ListId) return;
  TheLayerProp.Color.rgb[3] = 1.F;
  glDisable (GL_BLEND);
}

void OpenGl_GraphicDriver::SetLineAttributes (const Standard_Integer Type, const Standard_ShortReal Width)
{
  if (!TheLayerProp.ListId || openglDisplay.IsNull()) return;

  if (TheLayerProp.LineType != Type)
  {
    TheLayerProp.LineType = Type;
    openglDisplay->SetTypeOfLine((Aspect_TypeOfLine) Type);
  }
  if (TheLayerProp.LineWidth != Width)
  {
    TheLayerProp.LineWidth = Width;
    glLineWidth ((GLfloat) Width);
  }
}

void OpenGl_GraphicDriver::SetTextAttributes (const Standard_CString Font, const Standard_Integer AType, const Standard_ShortReal R, const Standard_ShortReal G, const Standard_ShortReal B)
{
  if (!TheLayerProp.ListId || openglDisplay.IsNull()) return;

  if ( strcmp ( TheLayerProp.AspectText.Font(), Font ) != 0 ||
	   (int)TheLayerProp.AspectText.DisplayType() != AType )
  {
    CALL_DEF_CONTEXTTEXT aContextText = myDefaultContextText;

    aContextText.Font = Font;
    aContextText.DisplayType = AType;
    aContextText.Color.r = R;
    aContextText.Color.g = G;
    aContextText.Color.b = B;
    TheLayerProp.AspectText.SetContext(aContextText);

    TheLayerProp.FontCurrent = openglDisplay->FindFont(TheLayerProp.AspectText.Font(), TheLayerProp.AspectText.FontAspect(), TheLayerProp.TextParam.Height);

    TheLayerProp.FontChanged = Standard_True;
  }
}

void OpenGl_GraphicDriver::Text (const Standard_CString AText, const Standard_ShortReal X, const Standard_ShortReal Y, const Standard_ShortReal AHeight)
{
  if (!TheLayerProp.ListId || openglDisplay.IsNull()) return;

  const Standard_ShortReal height = (AHeight < 0)? DefaultTextHeight() : AHeight;

  if ( TheLayerProp.TextParam.Height != height || TheLayerProp.FontChanged || TheLayerProp.FontCurrent == 0 )
  {
    TheLayerProp.TextParam.Height = (int )height;
    TheLayerProp.FontCurrent = openglDisplay->FindFont(TheLayerProp.AspectText.Font(), TheLayerProp.AspectText.FontAspect(), (int )height);
	TheLayerProp.FontChanged = Standard_False;
  }

  TCollection_ExtendedString estr(AText);
  const Techar *s = (const Techar *)estr.ToExtString();

  //szv: conversion of Techar to wchar_t
  wchar_t *s1 = (wchar_t*)s;
  if (sizeof(Techar) != sizeof(wchar_t))
  {
    Tint i = 0; while (s[i++]);
    s1 = new wchar_t[i];
    i = 0; while (s1[i++] = (wchar_t)(*s++));
  }

  openglDisplay->RenderText(s1, 1, (float)X, (float)Y, 0.F, &TheLayerProp.AspectText, &TheLayerProp.TextParam);

  //szv: delete temporary wide string
  if (sizeof(Techar) != sizeof(wchar_t))
    delete[] s1;
}

void OpenGl_GraphicDriver::TextSize (const Standard_CString AText, const Standard_ShortReal AHeight, Standard_ShortReal& AWidth, Standard_ShortReal& AnAscent, Standard_ShortReal& ADescent) const
{
  if (!TheLayerProp.ListId || openglDisplay.IsNull()) return;

  const Standard_ShortReal height = (AHeight < 0)? DefaultTextHeight() : AHeight;

  if ( TheLayerProp.TextParam.Height != height || TheLayerProp.FontChanged || TheLayerProp.FontCurrent == 0 )
  {
    TheLayerProp.TextParam.Height = (int )height;
    TheLayerProp.FontCurrent = openglDisplay->FindFont(TheLayerProp.AspectText.Font(), TheLayerProp.AspectText.FontAspect(), (int )height);
	TheLayerProp.FontChanged = Standard_False;
  }

  TCollection_ExtendedString estr(AText);
  const Techar *s = (const Techar *)estr.ToExtString();

  //szv: conversion of Techar to wchar_t
  wchar_t *s1 = (wchar_t*)s;
  if (sizeof(Techar) != sizeof(wchar_t))
  {
    Tint i = 0; while (s[i++]);
    s1 = new wchar_t[i];
    i = 0; while (s1[i++] = (wchar_t)(*s++));
  }

  int aWidth = 0, anAscent = 0, aDescent = 0;
  openglDisplay->StringSize(s1, aWidth, anAscent, aDescent);

  //szv: delete temporary wide string
  if (sizeof(Techar) != sizeof(wchar_t))
    delete[] s1;

  AWidth = (Standard_ShortReal) aWidth;
  AnAscent = (Standard_ShortReal) anAscent;
  ADescent = (Standard_ShortReal) aDescent;
}
