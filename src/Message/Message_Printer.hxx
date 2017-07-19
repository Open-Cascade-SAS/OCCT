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

#ifndef _Message_Printer_HeaderFile
#define _Message_Printer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Message_Gravity.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
class TCollection_ExtendedString;
class TCollection_AsciiString;


class Message_Printer;
DEFINE_STANDARD_HANDLE(Message_Printer, Standard_Transient)

//! Abstract interface class defining printer as output context for text messages
//!
//! The message, besides being text string, has associated gravity
//! level, which can be used by printer to decide either to process a message or ignore it.
class Message_Printer : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Message_Printer, Standard_Transient)
public:

  //! Return trace level used for filtering messages;
  //! messages with lover gravity will be ignored.
  Message_Gravity GetTraceLevel() const { return myTraceLevel; }

  //! Set trace level used for filtering messages.
  //! By default, trace level is Message_Info, so that all messages are output
  void SetTraceLevel (const Message_Gravity theTraceLevel) { myTraceLevel = theTraceLevel; }

  //! Send a string message with specified trace level.
  //! The parameter theToPutEol specified whether end-of-line should be added to the end of the message.
  //! This method must be redefined in descentant.
  Standard_EXPORT virtual void Send (const TCollection_ExtendedString& theString, const Message_Gravity theGravity, const Standard_Boolean theToPutEol) const = 0;
  
  //! Send a string message with specified trace level.
  //! The parameter theToPutEol specified whether end-of-line should be added to the end of the message.
  //! Default implementation calls first method Send().
  Standard_EXPORT virtual void Send (const Standard_CString theString, const Message_Gravity theGravity, const Standard_Boolean theToPutEol) const;
  
  //! Send a string message with specified trace level.
  //! The parameter theToPutEol specified whether end-of-line should be added to the end of the message.
  //! Default implementation calls first method Send().
  Standard_EXPORT virtual void Send (const TCollection_AsciiString& theString, const Message_Gravity theGravity, const Standard_Boolean theToPutEol) const;

protected:

  //! Empty constructor with Message_Info trace level
  Standard_EXPORT Message_Printer();

protected:

  Message_Gravity myTraceLevel;

};

#endif // _Message_Printer_HeaderFile
