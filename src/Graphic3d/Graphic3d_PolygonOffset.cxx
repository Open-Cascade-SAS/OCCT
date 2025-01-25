// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Graphic3d_PolygonOffset.hxx>

#include <Standard_Dump.hxx>

//=================================================================================================

void Graphic3d_PolygonOffset::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, Graphic3d_PolygonOffset)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Mode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Factor)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, Units)
}
