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
 * modified 27/08/97 ; PCT : ajout texture mapping
 * modified 08/04/98 ; FGU : ajout parametres d emission (CALL_DEF_MATERIAL)
 * modified 30/11/98 ; FMN : ajout parametres pour les textes visibles
 * modified 24/01/00 ; EUG : G003 add DegenerationMode flag and SkipRatio value in
 *                         CALL_DEF_STRUCTURE.
 * modified 22/03/04 ; SAN : OCC4895 High-level interface for controlling polygon offsets
 *
 */


#ifndef InterfaceGraphic_Graphic3dHeader
#define InterfaceGraphic_Graphic3dHeader

#include <InterfaceGraphic_PrimitiveArray.hxx>

#define G003    /*EUG 26-01-00 Degeneration management
*/

#define OCC1174 /*SAV 08/01/03 CONTEXTFILLAREA extended with back face interior color*/

#define OCC2934 /* SAN 22/01/04 Texture mapped fonts on WNT */

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

typedef struct {

        int IsDef;

        int IsSet;

        CALL_DEF_COLOR Color;

        int LineType;

        float Width;

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


/* TEXTURE */

typedef struct
{
  int doModulate;
  int doRepeat;
  int Mode;
  int doLinear;
  float sx, sy;
  float tx, ty;
  float angle;

  float sparams[4];
  float tparams[4];

} CALL_DEF_INIT_TEXTURE;


typedef struct
{
  int TexId;
  int doTextureMap;

} CALL_DEF_TEXTURE;



/* CONTEXTE POLYGONE */

typedef struct {

        int IsDef;

        int IsSet;

        int Style;

        CALL_DEF_COLOR IntColor;

#ifdef OCC1174
        CALL_DEF_COLOR BackIntColor;
#endif

        CALL_DEF_COLOR EdgeColor;

        int LineType;

        float Width;

        int Hatch;

        int Distinguish;
        int BackFace;

        int Edge;

        CALL_DEF_MATERIAL Front;
        CALL_DEF_MATERIAL Back;

        CALL_DEF_TEXTURE Texture;

#ifdef G003
        int   DegenerationMode;
        float SkipRatio;
#endif
        /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
        int   PolygonOffsetMode;
        float PolygonOffsetFactor;
        float PolygonOffsetUnits;
        /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */

} CALL_DEF_CONTEXTFILLAREA;


/* CONTEXTE MARKER */

typedef struct {

        int IsDef;

        int IsSet;

        CALL_DEF_COLOR Color;

        int MarkerType;

        float Scale;

} CALL_DEF_CONTEXTMARKER;


/* CONTEXTE TEXT */

typedef struct {

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
	
	
} CALL_DEF_CONTEXTTEXT;

/* Transform persistence struct */
typedef struct
{
        int            IsSet;
	int            IsDef;
	int	       Flag;
        CALL_DEF_POINT Point;
} CALL_DEF_TRANSFORM_PERSISTENCE;

/* STRUCTURE */

typedef struct {

        int Id;
        void *ptrStructure;

        int Priority;
        int PreviousPriority;
        int GroupBegin;
        int GroupEnd;

        CALL_DEF_CONTEXTLINE ContextLine;
        CALL_DEF_CONTEXTFILLAREA ContextFillArea;
        CALL_DEF_CONTEXTMARKER ContextMarker;
        CALL_DEF_CONTEXTTEXT ContextText;

        CALL_DEF_BOUNDBOX BoundBox;

        float Transformation[4][4];
        int Composition;

        int ContainsFacet;

        unsigned IsDeleted      :1;
        unsigned IsOpen         :1;
        unsigned IsInfinite     :1;
        unsigned stick          :1;
        unsigned highlight      :1;
        unsigned visible        :1;
        unsigned pick           :1;
        unsigned HLRValidation  :1;
        /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
	/*int	       TransformPersistenceFlag;
        CALL_DEF_POINT TransformPersistencePoint;
	*/
        CALL_DEF_TRANSFORM_PERSISTENCE TransformPersistence;
	/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
} CALL_DEF_STRUCTURE;


/* GROUPE */

typedef struct {

        int LabelBegin;
        int LabelEnd;
        void *ptrGroup;

        int StructureEnd;

        CALL_DEF_CONTEXTLINE ContextLine;
        CALL_DEF_CONTEXTFILLAREA ContextFillArea;
        CALL_DEF_CONTEXTMARKER ContextMarker;
        CALL_DEF_CONTEXTTEXT ContextText;

        CALL_DEF_STRUCTURE *Struct;

        CALL_DEF_PICKID PickId;

        unsigned IsDeleted      :1;
        unsigned IsOpen         :1;
        /*int	 TransformPersistenceFlag;*/

} CALL_DEF_GROUP;

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
