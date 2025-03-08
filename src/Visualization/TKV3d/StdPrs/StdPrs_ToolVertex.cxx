// Created by: Laurent PAINNOT
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

#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <StdPrs_ToolVertex.hxx>
#include <TopoDS_Vertex.hxx>

void StdPrs_ToolVertex::Coord(const TopoDS_Vertex& aVertex,
                              Standard_Real&       X,
                              Standard_Real&       Y,
                              Standard_Real&       Z)
{
  gp_Pnt P = BRep_Tool::Pnt(aVertex);
  X        = P.X();
  Y        = P.Y();
  Z        = P.Z();
}
