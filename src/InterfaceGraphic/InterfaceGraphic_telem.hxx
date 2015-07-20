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

typedef enum
{
  TelCullUndefined = -1,
  TelCullNone = 0,
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

#endif /* INTERFACEGRAPHIC_TELEM_H */
