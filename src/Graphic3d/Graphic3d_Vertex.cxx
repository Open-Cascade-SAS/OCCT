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

#include <Graphic3d_Vertex.hxx>

#include <gp_XYZ.hxx>
#include <Standard_Dump.hxx>

#include <cmath>

Standard_ShortReal Graphic3d_Vertex::Distance(const Graphic3d_Vertex& AOther) const
{
  return std::sqrt((X() - AOther.X()) * (X() - AOther.X()) + (Y() - AOther.Y()) * (Y() - AOther.Y())
                   + (Z() - AOther.Z()) * (Z() - AOther.Z()));
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Graphic3d_Vertex::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  gp_XYZ aCoord(xyz[0], xyz[1], xyz[2]);
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &aCoord)
}
