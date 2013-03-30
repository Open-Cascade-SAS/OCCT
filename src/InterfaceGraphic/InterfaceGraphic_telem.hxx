// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef INTERFACEGRAPHIC_TELEM_H
#define INTERFACEGRAPHIC_TELEM_H

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <InterfaceGraphic_tgl_all.hxx>
#include <stdlib.h>

struct TEL_TEXTURE_COORD
{
  float xy[2];
  DEFINE_STANDARD_ALLOC
};

typedef TEL_TEXTURE_COORD* tel_texture_coord;

class TEL_POINT
{
 public:
  float xyz[3];
  DEFINE_STANDARD_ALLOC
};
typedef TEL_POINT* tel_point;

struct TEL_COLOUR
{
  /* OCC8854: san -- number of color components increased to include alpha value */
  float    rgb[4];
  DEFINE_STANDARD_ALLOC
};
typedef TEL_COLOUR* tel_colour;

typedef  enum
{
  TLightAmbient, 
  TLightDirectional, 
  TLightPositional, 
  TLightSpot
} TLightType;

typedef  enum
{
  TelCullNone, 
  TelCullFront, 
  TelCullBack
} TelCullMode;

/* Standard Hatch Styles */
#define  TEL_HS_SOLID              0
#define  TEL_HS_CROSS              1
#define  TEL_HS_CROSS_SPARSE       2
#define  TEL_HS_GRID               3
#define  TEL_HS_GRID_SPARSE        4
#define  TEL_HS_DIAG_45            5
#define  TEL_HS_DIAG_135           6
#define  TEL_HS_HORIZONTAL         7
#define  TEL_HS_VERTICAL           8
#define  TEL_HS_DIAG_45_SPARSE     9
#define  TEL_HS_DIAG_135_SPARSE    10
#define  TEL_HS_HORIZONTAL_SPARSE  11
#define  TEL_HS_VERTICAL_SPARSE    12

#define TEL_HS_USER_DEF_START      15

#define TEL_SM_FLAT    1
#define TEL_SM_GOURAUD 2

/* Standard Lighting Models */
#define  TEL_FRONT_BACK_LM  1

/* Standard Materials */
#define  TEL_FRONT_MATERIAL  1
#define  TEL_BACK_MATERIAL   2

struct  TEL_POFFSET_PARAM
{
  int   mode;
  float factor;
  float units;
  DEFINE_STANDARD_ALLOC
};
typedef TEL_POFFSET_PARAM* tel_poffset_param;

struct TEL_TRANSFORM_PERSISTENCE
{
  int       mode;
  float     pointX;
  float     pointY;
  float     pointZ;
  DEFINE_STANDARD_ALLOC
};
typedef TEL_TRANSFORM_PERSISTENCE* tel_transform_persistence;

#endif /* INTERFACEGRAPHIC_TELEM_H */
