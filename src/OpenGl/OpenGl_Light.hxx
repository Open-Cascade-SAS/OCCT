// File:      OpenGl_Light.hxx
// Created:   13 July 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE 2011

#ifndef OpenGl_Light_Header
#define OpenGl_Light_Header

#include <NCollection_List.hxx>

#include <InterfaceGraphic_telem.hxx>

#define  OpenGLMaxLights    8

struct OpenGl_Light
{
  TLightType  type;
  int   HeadLight;
  TEL_COLOUR  col;
  Tfloat      pos[3];
  Tfloat      dir[3];
  Tfloat      shine;
  Tfloat      atten[2];
  Tfloat      angle;
  DEFINE_STANDARD_ALLOC
};

typedef NCollection_List<OpenGl_Light> OpenGl_ListOfLight;

#endif //OpenGl_Light_Header
