// Created by: NW,JPB,CAL
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

#ifndef _Aspect_TypeOfStyleText_HeaderFile
#define _Aspect_TypeOfStyleText_HeaderFile

//! Define the style of the text.
//!
//! TOST_NORMAL         Default text. The text is displayed like any other graphic object.
//! This text can be hidden by another object that is nearest from the
//! point of view.
//! TOST_ANNOTATION     The text is always visible. The texte is displayed
//! over the other object according to the priority.
enum Aspect_TypeOfStyleText
{
  Aspect_TOST_NORMAL,
  Aspect_TOST_ANNOTATION
};

#endif // _Aspect_TypeOfStyleText_HeaderFile
