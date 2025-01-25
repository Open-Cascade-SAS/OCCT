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

#ifndef _Draw_Printer_HeaderFile
#define _Draw_Printer_HeaderFile

#include <Message_Printer.hxx>
#include <Draw_Interpretor.hxx>

class Draw_Printer;
DEFINE_STANDARD_HANDLE(Draw_Printer, Message_Printer)

//! Implementation of Printer class with output
//! (Message_Messenge) directed to Draw_Interpretor
class Draw_Printer : public Message_Printer
{
  DEFINE_STANDARD_RTTIEXT(Draw_Printer, Message_Printer)
public:
  //! Creates a printer connected to the interpretor.
  Standard_EXPORT Draw_Printer(Draw_Interpretor& theTcl);

protected:
  //! Send a string message with specified trace level.
  Standard_EXPORT virtual void send(const TCollection_AsciiString& theString,
                                    const Message_Gravity theGravity) const Standard_OVERRIDE;

private:
  Draw_Interpretor* myTcl;
};

#endif // _Draw_Printer_HeaderFile
