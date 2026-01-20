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

Message_Messenger::Message_Messenger(const occ::handle<Message_Printer>& thePrinter)
{
  AddPrinter(thePrinter);
}

//=================================================================================================

bool Message_Messenger::AddPrinter(const occ::handle<Message_Printer>& thePrinter)
{
  // check whether printer is already in the list
  for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const occ::handle<Message_Printer>& aPrinter = aPrinterIter.Value();
    if (aPrinter == thePrinter)
    {
      return false;
    }
  }

  myPrinters.Append(thePrinter);
  return true;
}

//=================================================================================================

bool Message_Messenger::RemovePrinter(const occ::handle<Message_Printer>& thePrinter)
{
  // find printer in the list
  for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const occ::handle<Message_Printer>& aPrinter = aPrinterIter.Value();
    if (aPrinter == thePrinter)
    {
      myPrinters.Remove(aPrinterIter);
      return true;
    }
  }
  return false;
}

//=================================================================================================

int Message_Messenger::RemovePrinters(const occ::handle<Standard_Type>& theType)
{
  // remove printers from the list
  int nb = 0;
  for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator aPrinterIter(myPrinters); aPrinterIter.More();)
  {
    const occ::handle<Message_Printer>& aPrinter = aPrinterIter.Value();
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

void Message_Messenger::Send(const char* const theString,
                             const Message_Gravity  theGravity) const
{
  for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const occ::handle<Message_Printer>& aPrinter = aPrinterIter.Value();
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
  for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const occ::handle<Message_Printer>& aPrinter = aPrinterIter.Value();
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
  for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const occ::handle<Message_Printer>& aPrinter = aPrinterIter.Value();
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
  for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const occ::handle<Message_Printer>& aPrinter = aPrinterIter.Value();
    if (!aPrinter.IsNull())
    {
      aPrinter->Send(theString, theGravity);
    }
  }
}

//=================================================================================================

void Message_Messenger::Send(const occ::handle<Standard_Transient>& theObject,
                             const Message_Gravity             theGravity) const
{
  for (NCollection_Sequence<occ::handle<Message_Printer>>::Iterator aPrinterIter(myPrinters); aPrinterIter.More();
       aPrinterIter.Next())
  {
    const occ::handle<Message_Printer>& aPrinter = aPrinterIter.Value();
    if (!aPrinter.IsNull())
    {
      aPrinter->SendObject(theObject, theGravity);
    }
  }
}

//=================================================================================================

void Message_Messenger::DumpJson(Standard_OStream& theOStream, int) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPrinters.Size())
}
