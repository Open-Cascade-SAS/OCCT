
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

/* 
25-01-00 VKH : Add CALL_DEF_BITMAP structure

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

typedef struct {
  int     width;
  int     height;
#ifdef IMP100701
  int     depth;
#endif
  Aspect_Drawable bitmap;
  Aspect_RenderingContext bitmapContext;
} CALL_DEF_BITMAP;

#endif /* InterfaceGraphic_AspectHeader */
