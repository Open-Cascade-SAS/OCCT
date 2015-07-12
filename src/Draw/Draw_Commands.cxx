// Created on: 1995-03-01
// Created by: Remi LEQUETTE
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


#include <Draw.hxx>
#include <Draw_Drawable3D.hxx>
#include <Draw_Printer.hxx>
#include <Draw_ProgressIndicator.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterOStream.hxx>
#include <TCollection_AsciiString.hxx>

void Draw::Commands (Draw_Interpretor& theCommands)
{
  static Standard_Boolean isFirstTime = Standard_True;
  if (isFirstTime)
  {
    // override default std::cout printer by draw interpretor printer
    const Handle(Message_Messenger)& aMsgMgr = Message::DefaultMessenger();
    if (!aMsgMgr.IsNull())
    {
      aMsgMgr->RemovePrinters (STANDARD_TYPE (Message_PrinterOStream));
      aMsgMgr->RemovePrinters (STANDARD_TYPE (Draw_Printer));
      aMsgMgr->AddPrinter (new Draw_Printer (theCommands));
    }
    isFirstTime = Standard_False;
  }

  Draw::BasicCommands(theCommands);
  Draw::VariableCommands(theCommands);
  Draw::GraphicCommands(theCommands);
  Draw::PloadCommands(theCommands);
  Draw::UnitCommands(theCommands);
}
