// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Message_PrinterSystemLog_HeaderFile
#define _Message_PrinterSystemLog_HeaderFile

#include <Message_Printer.hxx>
#include <TCollection_AsciiString.hxx>

//! Implementation of a message printer associated with system log.
//! Implemented for the following systems:
//! - Windows, through ReportEventW().
//! - Android, through __android_log_write().
//! - UNIX/Linux, through syslog().
class Message_PrinterSystemLog : public Message_Printer
{
  DEFINE_STANDARD_RTTIEXT(Message_PrinterSystemLog, Message_Printer)
public:
  //! Main constructor.
  Standard_EXPORT Message_PrinterSystemLog(const TCollection_AsciiString& theEventSourceName,
                                           const Message_Gravity theTraceLevel = Message_Info);

  //! Destructor.
  Standard_EXPORT ~Message_PrinterSystemLog() override;

protected:
  //! Puts a message to the system log.
  Standard_EXPORT void send(const TCollection_AsciiString& theString,
                                    const Message_Gravity          theGravity) const override;

private:
  TCollection_AsciiString myEventSourceName;
#ifdef _WIN32
  void* myEventSource;
#endif
};

#endif // _Message_PrinterSystemLog_HeaderFile
