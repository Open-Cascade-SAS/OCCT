// Created on: 1995-10-12
// Created by: Bruno DUMORTIER
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _BRepOffset_Error_HeaderFile
#define _BRepOffset_Error_HeaderFile


enum BRepOffset_Error
{
  BRepOffset_NoError,
  BRepOffset_UnknownError,
  BRepOffset_BadNormalsOnGeometry,
  BRepOffset_C0Geometry,
  BRepOffset_NullOffset,
  BRepOffset_NotConnectedShell,
  BRepOffset_CannotTrimEdges,        //!< exception while trim edges
  BRepOffset_CannotFuseVertices,     //!< exception while fuse vertices
  BRepOffset_CannotExtentEdge,       //!< exception while extent edges
  BRepOffset_UserBreak               //!< user break
};

#endif // _BRepOffset_Error_HeaderFile
