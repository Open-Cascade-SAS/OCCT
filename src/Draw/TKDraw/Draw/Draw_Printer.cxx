// Created on: 2007-07-31
// Created by: OCC Team
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <Draw_Printer.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Draw_Printer, Message_Printer)

//=================================================================================================

Draw_Printer::Draw_Printer(Draw_Interpretor& theTcl)
    : myTcl(&theTcl)
{
}

//=================================================================================================

void Draw_Printer::send(const TCollection_AsciiString& theString,
                        const Message_Gravity          theGravity) const
{
  if (myTcl == NULL || theGravity < myTraceLevel)
  {
    return;
  }

  *myTcl << theString << "\n";
}
