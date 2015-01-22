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

/* ORIENTATION */

typedef struct {

  CALL_DEF_VERTEX ViewReferencePoint;
  CALL_DEF_VERTEX ViewReferencePlane;
  CALL_DEF_VERTEX ViewReferenceUp;
  float ViewScaleX;
  float ViewScaleY;
  float ViewScaleZ;

  float ModelViewMatrix[4][4];

  unsigned IsCustomMatrix;

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

  float ProjectionMatrix[4][4];

  unsigned IsCustomMatrix;

} CALL_DEF_VIEWMAPPING;

#endif /* InterfaceGraphic_Visual3dHeader */
