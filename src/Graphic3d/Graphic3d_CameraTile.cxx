// Created on: 2020-02-06
// Created by: Svetlana SHUTINA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Graphic3d_CameraTile.hxx>

#include <Standard_Dump.hxx>

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Graphic3d_CameraTile::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &TotalSize)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &TileSize)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &Offset)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, IsTopDown)
}
