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

#ifndef InterfaceGraphic_AspectHeader
#define InterfaceGraphic_AspectHeader

#include <Aspect_Drawable.hxx>
#include <Aspect_RenderingContext.hxx>

/* WINDOW */

typedef struct {

  int IsDefined;

  Aspect_Drawable XWindow;
  Aspect_Drawable XParentWindow;

  int dx, dy;
  int left, top;

  struct {
    float r, g, b;
  } Background;

} CALL_DEF_WINDOW;


typedef struct {
  int listIndex;
  void*  layerData;
} CALL_DEF_PTRLAYER, *call_def_ptrLayer;


/* LAYER */

typedef struct {
  CALL_DEF_PTRLAYER* ptrLayer;
  int layerType;
  int attach;
  int sizeDependent;
  float ortho[4];
  float viewport[2];

} CALL_DEF_LAYER;

#endif /* InterfaceGraphic_AspectHeader */
