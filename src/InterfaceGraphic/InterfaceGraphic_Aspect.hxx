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
