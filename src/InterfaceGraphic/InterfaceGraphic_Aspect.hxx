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

#ifndef IMP100701
#define IMP100701 /* GG Add depth field in the bitmap structure */
#endif

#ifndef RIC120302
#define RIC120302       /*GG Add NEW field in CALL_DEF_WINDOW structure
//      to registred parent window ID 
//      Change Aspect_Handle to Aspect_Drawable for
//      a more correct understanding.
*/
#endif

#include <Aspect_Drawable.hxx>
#include <Aspect_RenderingContext.hxx>

typedef struct {
  int mapped;
} EXT_WINDOW ;

/* WINDOW */

typedef struct {

  int IsDefined;

  Aspect_Drawable XWindow;

#ifdef RIC120302
  Aspect_Drawable XParentWindow;
#endif

  EXT_WINDOW *ext_data;

  struct {
    float xm, ym, xM, yM;
  } Position;

  float dx, dy;

  char *Title;

  char *Icon;

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
