// Created on: 2000-05-30
// Created by: Sergey MOZOKHIN
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _VrmlAPI_RepresentationOfShape_HeaderFile
#define _VrmlAPI_RepresentationOfShape_HeaderFile

//! Identifies the representation of the shape written
//! to a VRML file. The available options are :
//! -      VrmlAPI_ShadedRepresentation :
//! the shape is translated with a shaded representation.
//! -      VrmlAPI_WireFrameRepresentation :
//! the shape is translated with a wireframe representation.
//! -      VrmlAPI_BothRepresentation : the shape is translated
//! to VRML format with both representations : shaded and
//! wireframe. This is the default option.
enum VrmlAPI_RepresentationOfShape
{
  VrmlAPI_ShadedRepresentation,
  VrmlAPI_WireFrameRepresentation,
  VrmlAPI_BothRepresentation
};

#endif // _VrmlAPI_RepresentationOfShape_HeaderFile
