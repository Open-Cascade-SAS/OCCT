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

#ifndef _Aspect_GraphicCallbackProc_HeaderFile
#define _Aspect_GraphicCallbackProc_HeaderFile

// To manage client post display

#include <Aspect_Display.hxx>
#include <Aspect_Drawable.hxx>
#include <Aspect_RenderingContext.hxx>
#include <Standard_Transient.hxx>

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

typedef struct
{
  int reason;
  int wsID;
  int viewID;
  bool IsCoreProfile;
  Handle(Standard_Transient) glContext;
} Aspect_GraphicCallbackStruct;

// Prototype for callback function to perform custom drawing within the same window and GL context.
typedef int (*Aspect_GraphicCallbackProc) (Aspect_Drawable               theWindowID,
                                           void*                         theUserData,
                                           Aspect_GraphicCallbackStruct* theCallData);

#endif /* _Aspect_GraphicCallbackProc_HeaderFile */
