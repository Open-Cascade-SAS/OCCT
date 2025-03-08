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

#include <Message_Messenger.hxx>

#include <Message_Printer.hxx>
#include <Message_PrinterOStream.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_Messenger, Standard_Transient)

//=================================================================================================

Message_Messenger::Message_Messenger()
{
  AddPrinter(new Message_PrinterOStream);
}

//=================================================================================================

Message_Messenger::Message_Messenger(const Handle(Message_Printer)& thePrinter)
{
  AddPrinter(thePrinter);
}

//=================================================================================================

Standard_Boolean Message_Messenger::AddPrinter(const Handle(Message_Printer)& thePrinter)
{
  // check whether printer is already in the list
  for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const Handle(Message_Printer)& aPrinter = aPrinterIter.Value();
    if (aPrinter == thePrinter)
    {
      return Standard_False;
    }
  }

  myPrinters.Append(thePrinter);
  return Standard_True;
}

//=================================================================================================

Standard_Boolean Message_Messenger::RemovePrinter(const Handle(Message_Printer)& thePrinter)
{
  // find printer in the list
  for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const Handle(Message_Printer)& aPrinter = aPrinterIter.Value();
    if (aPrinter == thePrinter)
    {
      myPrinters.Remove(aPrinterIter);
      return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Standard_Integer Message_Messenger::RemovePrinters(const Handle(Standard_Type)& theType)
{
  // remove printers from the list
  Standard_Integer nb = 0;
  for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More();)
  {
    const Handle(Message_Printer)& aPrinter = aPrinterIter.Value();
    if (!aPrinter.IsNull() && aPrinter->IsKind(theType))
    {
      myPrinters.Remove(aPrinterIter);
      nb++;
    }
    else
    {
      aPrinterIter.Next();
    }
  }
  return nb;
}

//=================================================================================================

void Message_Messenger::Send(const Standard_CString theString,
                             const Message_Gravity  theGravity) const
{
  for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const Handle(Message_Printer)& aPrinter = aPrinterIter.Value();
    if (!aPrinter.IsNull())
    {
      aPrinter->Send(theString, theGravity);
    }
  }
}

//=================================================================================================

void Message_Messenger::Send(const Standard_SStream& theStream,
                             const Message_Gravity   theGravity) const
{
  for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const Handle(Message_Printer)& aPrinter = aPrinterIter.Value();
    if (!aPrinter.IsNull())
    {
      aPrinter->SendStringStream(theStream, theGravity);
    }
  }
}

//=================================================================================================

void Message_Messenger::Send(const TCollection_AsciiString& theString,
                             const Message_Gravity          theGravity) const
{
  for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const Handle(Message_Printer)& aPrinter = aPrinterIter.Value();
    if (!aPrinter.IsNull())
    {
      aPrinter->Send(theString, theGravity);
    }
  }
}

//=================================================================================================

void Message_Messenger::Send(const TCollection_ExtendedString& theString,
                             const Message_Gravity             theGravity) const
{
  for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const Handle(Message_Printer)& aPrinter = aPrinterIter.Value();
    if (!aPrinter.IsNull())
    {
      aPrinter->Send(theString, theGravity);
    }
  }
}

//=================================================================================================

void Message_Messenger::Send(const Handle(Standard_Transient)& theObject,
                             const Message_Gravity             theGravity) const
{
  for (Message_SequenceOfPrinters::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const Handle(Message_Printer)& aPrinter = aPrinterIter.Value();
    if (!aPrinter.IsNull())
    {
      aPrinter->SendObject(theObject, theGravity);
    }
  }
}

//=================================================================================================

void Message_Messenger::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPrinters.Size())
}
