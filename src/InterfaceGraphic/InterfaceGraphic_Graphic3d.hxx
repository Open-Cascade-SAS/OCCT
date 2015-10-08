// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef InterfaceGraphic_Graphic3dHeader
#define InterfaceGraphic_Graphic3dHeader

#include <InterfaceGraphic_telem.hxx>
#include <Graphic3d_BndBox4f.hxx>
#include <Graphic3d_BSDF.hxx>
#include <Standard_Transient.hxx>

/* COULEUR */

typedef struct {

  Standard_ShortReal r;
  Standard_ShortReal g;
  Standard_ShortReal b;

} CALL_DEF_COLOR;

/* POINT */

typedef struct {

  Standard_ShortReal x;
  Standard_ShortReal y;
  Standard_ShortReal z;

} CALL_DEF_POINT;

/* MATERIAL */

typedef struct {

  Standard_ShortReal Ambient;
  Standard_Integer   IsAmbient;

  Standard_ShortReal Diffuse;
  Standard_Integer   IsDiffuse;

  Standard_ShortReal Specular;
  Standard_Integer   IsSpecular;

  Standard_ShortReal Emission;
  Standard_Integer   IsEmission;

  Graphic3d_BSDF     BSDF;

  Standard_ShortReal Shininess;
  Standard_ShortReal Transparency;
  Standard_ShortReal RefractionIndex;

  Standard_ShortReal EnvReflexion;

  Standard_Integer   IsPhysic;

  /* Color attributes */
  CALL_DEF_COLOR     ColorAmb;
  CALL_DEF_COLOR     ColorDif;
  CALL_DEF_COLOR     ColorSpec;
  CALL_DEF_COLOR     ColorEms;
  CALL_DEF_COLOR     Color;

} CALL_DEF_MATERIAL;

/* Transform persistence struct */
typedef struct
{
  Standard_Integer IsSet;
  Standard_Integer IsDef;
  Standard_Integer Flag;
  CALL_DEF_POINT   Point;
} CALL_DEF_TRANSFORM_PERSISTENCE;

#endif /* InterfaceGraphic_Graphic3dHeader */
