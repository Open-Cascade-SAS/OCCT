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

#ifndef InterfaceGraphic_Visual3dHeader
#define InterfaceGraphic_Visual3dHeader

#include <InterfaceGraphic_Aspect.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <Aspect_RenderingContext.hxx>
#include <Aspect_GraphicCallbackProc.hxx>
#include <Standard_Transient.hxx>

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
