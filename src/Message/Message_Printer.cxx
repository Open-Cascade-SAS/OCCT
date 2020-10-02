// Created on: 2001-01-06
// Created by: OCC Team
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <Message_Printer.hxx>

#include <Standard_Dump.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_Printer,Standard_Transient)

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
Message_Printer::Message_Printer()
: myTraceLevel (Message_Info)
{
}

//=======================================================================
//function : Send
//purpose  :
//=======================================================================
void Message_Printer::Send (const Standard_CString theString,
                            const Message_Gravity  theGravity) const
{
  if (theGravity >= myTraceLevel)
  {
    send (TCollection_AsciiString (theString), theGravity);
  }
}

//=======================================================================
//function : Send
//purpose  :
//=======================================================================
void Message_Printer::Send (const TCollection_ExtendedString& theString,
                            const Message_Gravity theGravity) const
{
  if (theGravity >= myTraceLevel)
  {
    send (TCollection_AsciiString (theString), theGravity);
  }
}

//=======================================================================
//function : Send
//purpose  :
//=======================================================================
void Message_Printer::Send (const TCollection_AsciiString& theString,
                            const Message_Gravity theGravity) const
{
  if (theGravity >= myTraceLevel)
  {
    send (theString, theGravity);
  }
}

//=======================================================================
//function : SendStringStream
//purpose  :
//=======================================================================
void Message_Printer::SendStringStream (const Standard_SStream& theStream,
                                        const Message_Gravity   theGravity) const
{
  if (theGravity >= myTraceLevel)
  {
    send (theStream.str().c_str(), theGravity);
  }
}

//=======================================================================
//function : SendObject
//purpose  :
//=======================================================================
void Message_Printer::SendObject (const Handle(Standard_Transient)& theObject,
                                  const Message_Gravity          theGravity) const
{
  if (!theObject.IsNull()
    && theGravity >= myTraceLevel)
  {
    send (TCollection_AsciiString (theObject->DynamicType()->Name())
        + ": " + Standard_Dump::GetPointerInfo (theObject), theGravity);
  }
}
