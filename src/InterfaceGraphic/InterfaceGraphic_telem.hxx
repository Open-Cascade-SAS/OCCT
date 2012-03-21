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

/*
File: InterfaceGraphic_telem.h

16/06/2000 : ATS : G005 : Copied from OpenGl_telem.h to support required 
for InterfaceGraphic_Parray.hxx definitions

22/03/2004 : SAN : OCC4895 High-level interface for controlling polygon offsets

20/05/2005 : SAN : OCC8854 Number of color components increased to include alpha value

*/

#ifndef  INTERFACEGRAPHIC_TELEM_H
#define  INTERFACEGRAPHIC_TELEM_H

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

struct TEL_POINT
{
  float  xyz[3];
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

struct TEL_POINT_DATA
{
  int      num;
  TEL_POINT* data;
  DEFINE_STANDARD_ALLOC
};
typedef TEL_POINT_DATA* tel_point_data;

struct TEL_TINT_DATA
{
  int   num;
  int*  data;
  DEFINE_STANDARD_ALLOC
};
typedef TEL_TINT_DATA* tel_tint_data;

struct TEL_MATRIX3_DATA
{
  TComposeType  mode;
  Tmatrix3      mat;
  DEFINE_STANDARD_ALLOC
};
typedef TEL_MATRIX3_DATA* tel_matrix3_data;

struct TEL_ALIGN_DATA
{
  Tint Hmode;
  Tint Vmode;
  DEFINE_STANDARD_ALLOC
};
typedef TEL_ALIGN_DATA* tel_align_data;

typedef  enum
{
  TLightAmbient, 
  TLightDirectional, 
  TLightPositional, 
  TLightSpot
} TLightType;

typedef  enum
{
  TelHLColour, 
  TelHLForcedColour
} THighlightType;

typedef  enum
{
  TelDCSuppressed, 
  TelDCAllowed
} TDepthCueType;

/* Proprietes materiels */
struct  TEL_SURF_PROP
{
  float     amb, diff, spec, emsv; 
  float     trans, shine;
  float     env_reflexion;
  int       isamb, isdiff, isspec, isemsv;
  int       isphysic; 
  TEL_COLOUR speccol, difcol, ambcol, emscol, matcol;
  DEFINE_STANDARD_ALLOC
};
typedef TEL_SURF_PROP* tel_surf_prop;

typedef  enum
{
  TelCullNone, 
  TelCullFront, 
  TelCullBack
} TelCullMode;

/* Interior Styles */
#define TSM_SOLID  1
#define TSM_HOLLOW 2
#define TSM_EMPTY  3
#define TSM_HATCH  4
#define TSM_POINT  5
#define TSM_HIDDENLINE  6

/* Standard Line Styles */
#define TEL_LS_SOLID           0
#define TEL_LS_DOT             2
#define TEL_LS_DASH_DOT        3
#define TEL_LS_DASH            1
#define TEL_LS_DOUBLE_DOT_DASH 4

#define TEL_LS_USER_DEF_START  10

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

#define  TEL_SHAPE_UNKNOWN         1
#define  TEL_SHAPE_COMPLEX         2
#define  TEL_SHAPE_CONCAVE         3
#define  TEL_SHAPE_CONVEX          4

/* Key ids for area type primitives */
#define NUM_FACETS_ID         1   /* key.data.ldata contains no. of facets */
#define FNORMALS_ID           2   /* key.data.pdata is array of TEL_POINT */
#define FACET_COLOUR_VALS_ID  3   /* key.data.pdata is array of TSM_COLOUR */
#define VERTICES_ID           4   /* key.data.pdata is array of TEL_POINT */
#define VERTEX_COLOUR_VALS_ID 5   /* key.data.pdata is array of TSM_COLOUR */
#define VNORMALS_ID           6   /* key.data.pdata is array of TEL_POINT */
#define NUM_VERTICES_ID       7   /* key.data.ldata is num of vertices */
#define SHAPE_FLAG_ID         8   /* key.data.ldata is shape flag */
#define EDGE_DATA_ID          9   /* key.data.pdata is edge data (Tint*) */
#define CONNECTIVITY_ID       10  /* key.data.pdata is conn data  (Tint*) */
#define BOUNDS_DATA_ID        11  /* key.data.pdata is bounds data  (Tint*) */
#define NUM_LINES_ID          12  /* key.data.ldata is number of lines */
#define NUM_ROWS_ID           13  /* key.data.ldata is number of rows */
#define NUM_COLUMNS_ID        14  /* key.data.ldata is number of columns */
#define VTEXTURECOORD_ID      15  /* key.data.pdata is array of TEL_TEXTURE_COORD */

#define PARRAY_ID       16  /* key.data.pdata is a primitive array */

/* Key ids for text primitives */
#define TEXT_ATTACH_PT_ID     1   /* key.data.pdata contains tel_point */
#define TEXT_STRING_ID        2   /* key.data.pdata contains Tchar *   */


/* Key ids for curve primitives */
#define CURVE_TYPE_ID         1   /* key.data.ldata is curve type */
/* TelCurveType :
* Bezier
* Cardinal
* BSpline
*/
#define CURVE_NUM_POINTS_ID   2   /* key.data.ldata is num of points */
#define CURVE_VERTICES_ID     3   /* key.data.pdata is array of TEL_POINT */


/* Polymarker Types */
#define TEL_PM_PLUS        0
#define TEL_PM_STAR        1
#define TEL_PM_CROSS       2
#define TEL_PM_CIRC        3
#define TEL_PM_DOT         4
#define TEL_PM_USERDEFINED 5

#define TEL_PM_USER_DEF_START 7

#define TEL_SD_SHADING 0
#define TEL_SD_TEXTURE 1

#define TEL_SM_FLAT    1
#define TEL_SM_GOURAUD 2

#define TEL_POLYMARKER_FONT (short)1

/* Standard Lighting Models */
#define  TEL_FRONT_BACK_LM  1

/* Standard Materials */
#define  TEL_FRONT_MATERIAL  1
#define  TEL_BACK_MATERIAL   2

/* facet flags */
#define TEL_FAFLAG_NONE         1
#define TEL_FAFLAG_NORMAL       2
#define TEL_FAFLAG_COLOUR       3
#define TEL_FAFLAG_COLOURNORMAL 4

/* internal use */
#define TEL_FA_NONE   TEL_FAFLAG_NONE
#define TEL_FA_NORMAL TEL_FAFLAG_NORMAL

/* vertex flags */
#define TEL_VTFLAG_NONE         1
#define TEL_VTFLAG_NORMAL       2
#define TEL_VTFLAG_COLOUR       3
#define TEL_VTFLAG_COLOURNORMAL 4

/* internal use */
#define TEL_VT_NONE   TEL_VTFLAG_NONE
#define TEL_VT_NORMAL TEL_VTFLAG_NORMAL

/* Defbasis identifier */
#define TEL_BEZIER   1
#define TEL_CARDINAL 2
#define TEL_BSPLINE  3

typedef  enum
{
  TelBezierCurve   = TEL_BEZIER,
  TelCardinalCurve = TEL_CARDINAL,
  TelBSplineCurve  = TEL_BSPLINE
} TelCurveType;

typedef enum
{
  TelHLHSRNone, 
  TelHLHSRZBuff

} TelHLHSRId;

/* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
struct  TEL_POFFSET_PARAM
{
  int   mode;
  float factor;
  float units;
  DEFINE_STANDARD_ALLOC
};
typedef TEL_POFFSET_PARAM* tel_poffset_param;
/* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
struct TEL_TRANSFORM_PERSISTENCE
{
  int       mode;
  float     pointX;
  float     pointY;
  float     pointZ;
  DEFINE_STANDARD_ALLOC
};
typedef TEL_TRANSFORM_PERSISTENCE* tel_transform_persistence;
/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */

#endif /* INTERFACEGRAPHIC_TELEM_H */
