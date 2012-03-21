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

/*============================================================================*/
/*==== Titre: Aspect_GraphicCallbackProc.hxx                                               */
/*==== Role : The header file of primitive type "GraphicCallbackProc" from package        */
/*==== "V3d"                                                           */
/*==== Implementation:  This is a primitive type implemented with typedef     */
/*============================================================================*/

#ifndef _Aspect_GraphicCallbackProc_HeaderFile
#define _Aspect_GraphicCallbackProc_HeaderFile
#include <Aspect_Display.hxx>
#include <Aspect_Drawable.hxx>
#include <Aspect_RenderingContext.hxx>

// The flags below provide additional information to define the moment when
// callback was invoked in redraw procedure. These flags are bitwise OR'ed
// with the "reason" value of callback:
// 1) OCC_PRE_REDRAW  - callback was invoked before redrawing underlayer
//                      ( at the beginning of redraw procedure );
// 2) OCC_PRE_OVERLAY - callback was invoked before redrawing overlayer;
// Otherwise, if no flags added to the "reason" value, the callback was
// invoked at the end of redraw ( after the overlayer is redrawn )
#define OCC_PRE_REDRAW  0x4000
#define OCC_PRE_OVERLAY 0x8000

// mask for all additional callbacks that invoked in process of redrawing
#define OCC_REDRAW_ADDITIONAL_CALLBACKS ( OCC_PRE_REDRAW | OCC_PRE_OVERLAY )

 typedef struct {
   int reason;
   int wsID;
   int viewID;
   Aspect_Display display;
   Aspect_Drawable window;
   Aspect_RenderingContext gcontext;
 } Aspect_GraphicCallbackStruct;

 typedef int (*Aspect_GraphicCallbackProc)(
    Aspect_Drawable   	/* Window ID */,
    void*           	/* user data */, 
    Aspect_GraphicCallbackStruct*  /* call data */ 
 );

#if defined(__cplusplus) || defined(c_plusplus)
/*==== Definition de Type ====================================================*/

#include <Standard_Macro.hxx>
class Handle(Standard_Type);
const Handle(Standard_Type)& STANDARD_TYPE(Aspect_GraphicCallbackProc);

/*============================================================================*/
#endif

#endif /* _Aspect_GraphicCallbackProc_HeaderFile */
