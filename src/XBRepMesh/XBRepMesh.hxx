// Created on: 2008-04-11
// Created by: Peter KURNEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _XBRepMesh_HeaderFile
#define _XBRepMesh_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <BRepMesh_DiscretRoot.hxx>

class TopoDS_Shape;

class XBRepMesh
{
public:

  DEFINE_STANDARD_ALLOC
  
  Standard_EXPORT static Standard_Integer Discret(
    const TopoDS_Shape&    theShape,
    const Standard_Real    theDeflection,
    const Standard_Real    theAngle,
    BRepMesh_DiscretRoot* &theAlgo);
};

#endif
