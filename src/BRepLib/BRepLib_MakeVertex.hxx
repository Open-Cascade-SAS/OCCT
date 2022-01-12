// Created on: 1993-07-06
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepLib_MakeVertex_HeaderFile
#define _BRepLib_MakeVertex_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <BRepLib_MakeShape.hxx>
class gp_Pnt;
class TopoDS_Vertex;


//! Provides methods to build vertices.
class BRepLib_MakeVertex  : public BRepLib_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepLib_MakeVertex(const gp_Pnt& P);
  
  Standard_EXPORT const TopoDS_Vertex& Vertex();
  Standard_EXPORT operator TopoDS_Vertex();




protected:





private:





};







#endif // _BRepLib_MakeVertex_HeaderFile
