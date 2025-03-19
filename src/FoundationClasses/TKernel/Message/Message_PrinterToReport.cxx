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

#include <Message_PrinterToReport.hxx>

#include <Message.hxx>
#include <Message_AttributeMeter.hxx>
#include <Message_AttributeObject.hxx>
#include <Message_AttributeStream.hxx>
#include <Message_Report.hxx>

#include <Standard_Dump.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Message_PrinterToReport, Message_Printer)

//=================================================================================================

const Handle(Message_Report)& Message_PrinterToReport::Report() const
{
  if (!myReport.IsNull())
  {
    return myReport;
  }

  return Message::DefaultReport(Standard_True);
}

//=================================================================================================

void Message_PrinterToReport::SendStringStream(const Standard_SStream& theStream,
                                               const Message_Gravity   theGravity) const
{
  const Handle(Message_Report)& aReport = Report();
  if (!aReport->ActiveMetrics().IsEmpty())
  {
    sendMetricAlert(theStream.str().c_str(), theGravity);
    return;
  }
  if (Standard_Dump::HasChildKey(Standard_Dump::Text(theStream)))
  {
    Message_AlertExtended::AddAlert(aReport,
                                    new Message_AttributeStream(theStream, myName),
                                    theGravity);
    myName.Clear();
  }
  else
  {
    if (!myName.IsEmpty())
    {
      TCollection_AsciiString aName = myName;
      myName.Clear();
      send(aName, theGravity);
    }
    myName = Standard_Dump::Text(theStream);
  }
}

//=================================================================================================

void Message_PrinterToReport::SendObject(const Handle(Standard_Transient)& theObject,
                                         const Message_Gravity             theGravity) const
{
  const Handle(Message_Report)& aReport = Report();
  if (!aReport->ActiveMetrics().IsEmpty())
  {
    sendMetricAlert(myName, theGravity);
    return;
  }

  Message_AlertExtended::AddAlert(aReport,
                                  new Message_AttributeObject(theObject, myName),
                                  theGravity);
}

//=================================================================================================

void Message_PrinterToReport::send(const TCollection_AsciiString& theString,
                                   const Message_Gravity          theGravity) const
{
  if (!myName.IsEmpty())
  {
    TCollection_AsciiString aName = myName;
    myName.Clear();
    send(aName, theGravity);
  }

  const Handle(Message_Report)& aReport = Report();
  if (!aReport->ActiveMetrics().IsEmpty())
  {
    sendMetricAlert(theString, theGravity);
    return;
  }
  Message_AlertExtended::AddAlert(aReport, new Message_Attribute(theString), theGravity);
}

//=================================================================================================

void Message_PrinterToReport::sendMetricAlert(const TCollection_AsciiString& theValue,
                                              const Message_Gravity          theGravity) const
{
  Message_AlertExtended::AddAlert(Report(), new Message_AttributeMeter(theValue), theGravity);
}
