// Created on: 2018-06-10
// Created by: Natalia Ermolaeva
// Copyright (c) 2018-2020 OPEN CASCADE SAS
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

#include <TopoDS_AlertAttribute.hxx>

#include <Message_PrinterToReport.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopoDS_AlertAttribute, Message_Attribute)

//=======================================================================
//function : TopoDS_AlertAttribute
//purpose  :
//=======================================================================
TopoDS_AlertAttribute::TopoDS_AlertAttribute (const TopoDS_Shape& theShape,
                                              const TCollection_AsciiString& theName)
: Message_AttributeStream (Standard_SStream(), theName), myShape (theShape)
{
  Standard_SStream aStream;
  theShape.DumpJson (aStream);

  SetStream (aStream);
}

//=======================================================================
//function : Send
//purpose  :
//=======================================================================
void TopoDS_AlertAttribute::Send (const Handle(Message_Messenger)& theMessenger,
                                  const TopoDS_Shape& theShape)
{
  for (Message_SequenceOfPrinters::Iterator aPrinterIter (theMessenger->Printers()); aPrinterIter.More(); aPrinterIter.Next())
  {
    const Handle(Message_Printer)& aPrinter = aPrinterIter.Value();
    if (!aPrinter->IsKind (STANDARD_TYPE (Message_PrinterToReport)))
    {
      continue;
    }

    Handle (Message_PrinterToReport) aPrinterToReport = Handle(Message_PrinterToReport)::DownCast (aPrinter);
    const Handle(Message_Report)& aReport = aPrinterToReport->Report();

    Message_AlertExtended::AddAlert (aReport, new TopoDS_AlertAttribute (theShape), Message_Info);
  }
}

//=======================================================================
//function : DumpJson
//purpose  :
//=======================================================================
void TopoDS_AlertAttribute::DumpJson (Standard_OStream& theOStream,
                                      Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, Message_Attribute)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myShape)
}
