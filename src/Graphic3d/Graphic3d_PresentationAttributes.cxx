// Created on: 2016-09-23
// Created by: Varvara POSKONINA
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

#include <Graphic3d_PresentationAttributes.hxx>

#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_PresentationAttributes, Standard_Transient)

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Graphic3d_PresentationAttributes::DumpJson(Standard_OStream& theOStream,
                                                Standard_Integer  theDepth) const
{
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myBasicFillAreaAspect.get())

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myBasicColor)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myHiMethod)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myZLayer)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDispMode)
}
