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

#include <Graphic3d_ShaderProgram_Handle.hxx>
#include <InterfaceGraphic_PrimitiveArray.hxx>
#include <Standard_Transient.hxx>

#ifdef THIS
  #undef THIS
#endif

#define CALL_DEF_STRUCTHIGHLIGHTED      1
#define CALL_DEF_STRUCTPICKABLE         2
#define CALL_DEF_STRUCTVISIBLE          3

#define CALL_DEF_STRUCTNOHIGHLIGHTED    11
#define CALL_DEF_STRUCTNOPICKABLE       12
#define CALL_DEF_STRUCTNOVISIBLE        13

/* LISTE D'ENTIERS */

typedef struct {

        int NbIntegers;

        int *Integers;

} CALL_DEF_LISTINTEGERS;


/* LISTE DE REELS */

typedef struct {

        int NbReals;

        float *Reals;

} CALL_DEF_LISTREALS;


/* COULEUR */

typedef struct {

        float r, g, b;

} CALL_DEF_COLOR;


/* ARETE */

typedef struct {

        int Index1, Index2;

        int Type;

} CALL_DEF_EDGE;


/* LISTE D'ARETES */

typedef struct {

        int NbEdges;

        CALL_DEF_EDGE *Edges;

} CALL_DEF_LISTEDGES;


/* NORMALE */

typedef struct {

        float dx, dy, dz;

} CALL_DEF_NORMAL;


/* TEXTURE COORD */

typedef struct {

        float tx, ty;

} CALL_DEF_TEXTURE_COORD;


/* POINT */

typedef struct {

        float x, y, z;

} CALL_DEF_POINT;


/* POINTC */

typedef struct {

        CALL_DEF_POINT Point;

        CALL_DEF_COLOR Color;

} CALL_DEF_POINTC;


/* POINTN */

typedef struct {

        CALL_DEF_POINT Point;

        CALL_DEF_NORMAL Normal;

} CALL_DEF_POINTN;


/* POINTNT */

typedef struct {

        CALL_DEF_POINT Point;

        CALL_DEF_NORMAL Normal;

        CALL_DEF_TEXTURE_COORD TextureCoord;

} CALL_DEF_POINTNT;


/* POINTNC */

typedef struct {

        CALL_DEF_POINT Point;

        CALL_DEF_NORMAL Normal;

        CALL_DEF_COLOR Color;

} CALL_DEF_POINTNC;


/* BOITE ENGLOBANTE */

typedef struct {

        CALL_DEF_COLOR Color;

        CALL_DEF_POINT Pmin;

        CALL_DEF_POINT Pmax;

} CALL_DEF_BOUNDBOX;


/* LISTE DE POINTS */

typedef union {

        CALL_DEF_POINT *Points;

        CALL_DEF_POINTN *PointsN;

        CALL_DEF_POINTC *PointsC;

        CALL_DEF_POINTNC *PointsNC;

        CALL_DEF_POINTNT *PointsNT;

} CALL_DEF_UPOINTS;


/* LISTE DE POINTS */

typedef struct {

        int NbPoints;

        int TypePoints;

        CALL_DEF_UPOINTS UPoints;

} CALL_DEF_LISTPOINTS;


/* MARKER */

typedef struct {

        float x, y, z;

} CALL_DEF_MARKER;


/* LISTE DE MARKERS */

typedef struct {

        int NbMarkers;

        CALL_DEF_MARKER *Markers;

} CALL_DEF_LISTMARKERS;


/* TEXTE */

typedef struct {

        unsigned short *string;

        CALL_DEF_POINT Position;

        float Height;

        float Angle;

        int Path;

        int HAlign;

        int VAlign;

        bool Zoomable;

} CALL_DEF_TEXT;


/* FACETTE */

typedef struct {

        int NormalIsDefined;

        CALL_DEF_NORMAL Normal;

        int ColorIsDefined;

        CALL_DEF_COLOR Color;

        int TypeFacet;

        int NbPoints;

        int TypePoints;

        CALL_DEF_UPOINTS UPoints;

} CALL_DEF_FACET;


/* LISTE DE FACETTES */

typedef struct {

        int NbFacets;

        CALL_DEF_FACET *LFacets;

} CALL_DEF_LISTFACETS;


/* QUADRILATERE */

typedef struct {

        int NbPoints;

        int TypePoints;

        int SizeRow;
        int SizeCol;

        CALL_DEF_UPOINTS UPoints;

} CALL_DEF_QUAD;


/* TRIANGLE */

typedef struct {

        int NbPoints;

        int TypePoints;

        CALL_DEF_UPOINTS UPoints;

} CALL_DEF_TRIKE;


/* PICK IDENTIFICATEUR */

typedef struct {

        int IsDef;

        int IsSet;

        int Value;

} CALL_DEF_PICKID;


/* CONTEXTE LIGNE */

typedef struct
{
  int IsDef;
  
  int IsSet;
  
  CALL_DEF_COLOR Color;
  
  int LineType;
  
  float Width;
  
  Handle(Graphic3d_ShaderProgram) ShaderProgram;

} CALL_DEF_CONTEXTLINE;


/* MATERIAL */

typedef struct {

        float Ambient;
        int IsAmbient;

        float Diffuse;
        int IsDiffuse;

        float Specular;
        int IsSpecular;

        float Emission;
        int IsEmission;

        float Transparency;
        float Shininess;

        float EnvReflexion;

        int IsPhysic;

        /* Attribut couleur eclairage */
        CALL_DEF_COLOR ColorAmb, ColorDif, ColorSpec, ColorEms, Color;


} CALL_DEF_MATERIAL;


/* CONTEXTE TEXT */

typedef struct
{
  int IsDef;
  
  int IsSet;
  
  const char* Font;
  
  float Space;
  
  float Expan;
  
  CALL_DEF_COLOR Color;
  
  int Style;
  
  int DisplayType;
  
  CALL_DEF_COLOR ColorSubTitle;

	int TextZoomable;

	float TextAngle;

	int TextFontAspect;

  Handle(Graphic3d_ShaderProgram) ShaderProgram;

} CALL_DEF_CONTEXTTEXT;

/* Transform persistence struct */
typedef struct
{
        int            IsSet;
	int            IsDef;
	int	       Flag;
        CALL_DEF_POINT Point;
} CALL_DEF_TRANSFORM_PERSISTENCE;

/* BOUNDING BOX */

typedef struct {

        float XMin;
        float YMin;
        float ZMin;

        float XMax;
        float YMax;
        float ZMax;

} CALL_DEF_BOUNDS;

/* USERDRAW DATA */

typedef struct {

        void            *Data;
        CALL_DEF_BOUNDS *Bounds;

} CALL_DEF_USERDRAW;

#endif /* InterfaceGraphic_Graphic3dHeader */
