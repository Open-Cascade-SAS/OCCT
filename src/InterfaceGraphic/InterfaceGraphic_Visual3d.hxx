
#ifndef InterfaceGraphic_Visual3dHeader
#define InterfaceGraphic_Visual3dHeader

/*

                     Copyright (C) 1991,1992,1993 by

                      MATRA DATAVISION, FRANCE

This software is furnished in accordance with the terms and conditions
of the contract and with the inclusion of the above copyright notice.
This software or any other copy thereof may not be provided or otherwise
be made available to any other person. No title to an ownership of the
software is hereby transferred.

At the termination of the contract, the software and all copies of this
software must be deleted.

Facility : CAS-CADE V1

*/

	/* CAL 02/08/94
	 * #include <InterfaceGraphic_X11.hxx>
	 *   Retrait de InterfaceGraphic_X11.hxx et declaration de Window
	 * a la mode X11 dans les structures CALL_DEF_VIEW et CALL_DEF_PICK.
	 *   En effet, un #define Opposite 4 se trouve dans X11/X.h
	 * et rentre en conflit avec la methode Opposite de math_Matrix.
	 *
	 * PCT 05/08/97
	 * ajout support texture mapping
	 *
         * EUG 21/09/99 G003
         *  degenerated mode support
            Add fields IsDegenerates,IsDegeneratesPrev,Backfacing
	    in CALL_DEF_VIEW structure.
	 * VKH 25/01/00 G004
	 * Dump a view
	   Add CALL_DEF_BITMAP reference in CALL_DEF_VIEW
	 */

#ifndef RIC120302
#define RIC120302       /*GG Add NEW fields in CALL_DEF_VIEW structure
//                      to manage graphic context and call back
*/
#endif

#include <InterfaceGraphic_Aspect.hxx>
#ifdef RIC120302
#include <Aspect_RenderingContext.hxx>
#include <Aspect_GraphicCallbackProc.hxx>
#endif

typedef float CALL_DEF_MATRIX4X4[4][4];

/* SOMMET */

typedef struct {

	float x, y, z;

} CALL_DEF_VERTEX;


/* MODELE CLIPPING */

typedef struct {

	int WsId;

	int ViewId;

	int PlaneId;

	int Active;

	float CoefA;
	float CoefB;
	float CoefC;
	float CoefD;

} CALL_DEF_PLANE;


/* SOURCE LUMINEUSE */

typedef struct {

	int WsId;

	int ViewId;

	int LightId;

	int Active;

	int LightType;

	int Headlight;

	CALL_DEF_COLOR Color;

	CALL_DEF_VERTEX Position;

	CALL_DEF_VERTEX Direction;

	float Concentration;

	float Attenuation[2];

	float Angle;

} CALL_DEF_LIGHT;


/* ORIENTATION */

typedef struct {

  CALL_DEF_VERTEX ViewReferencePoint;
  CALL_DEF_VERTEX ViewReferencePlane;
  CALL_DEF_VERTEX ViewReferenceUp;
  float ViewScaleX;
  float ViewScaleY;
  float ViewScaleZ;

  int   IsCustomMatrix;
  float ModelViewMatrix[4][4];

} CALL_DEF_VIEWORIENTATION;


/* MAPPING */

typedef struct {

  int Projection;

  CALL_DEF_VERTEX ProjectionReferencePoint;

  float ViewPlaneDistance;
  float BackPlaneDistance;
  float FrontPlaneDistance;

  struct {
    float um, vm, uM, vM;
  } WindowLimit;

  int   IsCustomMatrix;
  float ProjectionMatrix[4][4];

} CALL_DEF_VIEWMAPPING;


/* CONTEXT */

 typedef struct {

	int Aliasing;

	int BackZClipping;
	int FrontZClipping;

	int DepthCueing;

	float ZClipFrontPlane;
	float ZClipBackPlane;

	float DepthFrontPlane;
	float DepthBackPlane;

	int ZBufferActivity;
	int Model;
	int Visualization;

	int NbActiveLight;
	CALL_DEF_LIGHT *ActiveLight;

	int NbActivePlane;
	CALL_DEF_PLANE *ActivePlane;

	int TexEnvId;
	int SurfaceDetail;

} CALL_DEF_VIEWCONTEXT;


/* VUE */

typedef struct {

	int WsId;
	int ViewId;
	void *ptrView;

	int IsDeleted;
	int IsOpen;

	int Active;

	CALL_DEF_VIEWORIENTATION Orientation;
	CALL_DEF_VIEWMAPPING Mapping;

	CALL_DEF_VIEWORIENTATION OrientationReset;
	CALL_DEF_VIEWMAPPING MappingReset;

	CALL_DEF_VIEWCONTEXT Context;

	CALL_DEF_WINDOW DefWindow;

	CALL_DEF_BITMAP DefBitmap;

	void *ptrUnderLayer;
	void *ptrOverLayer;

        int IsDegenerates;
        int IsDegeneratesPrev;
        int Backfacing;
#ifdef RIC120302
        Aspect_RenderingContext GContext;
        Aspect_GraphicCallbackProc GDisplayCB;
        void* GClientData;
#endif

  void *ptrFBO;

} CALL_DEF_VIEW;


/* REPERAGE */

typedef struct {

	int WsId;
	int ViewId;

	int x, y;

	CALL_DEF_WINDOW DefWindow;

	struct {
		float aperture;
		int order;
		int depth;
	} Context;

	struct {
		int depth;
		int *listid;
		int *listpickid;
		int *listelem;
	} Pick;

} CALL_DEF_PICK;

#endif /* InterfaceGraphic_Visual3dHeader */
