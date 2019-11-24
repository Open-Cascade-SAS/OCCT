// Created on: 1999-07-29
// Created by: Roman LYGIN
// Copyright (c) 1999 Matra Datavision
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

#ifndef _Message_HeaderFile
#define _Message_HeaderFile

#include <Message_Messenger.hxx>

//! Defines
//! - tools to work with messages
//! - basic tools intended for progress indication
class Message
{
public:

  DEFINE_STANDARD_ALLOC

  //! Defines default messenger for OCCT applications.
  //! This is global static instance of the messenger.
  //! By default, it contains single printer directed to std::cout.
  //! It can be customized according to the application needs.
  //!
  //! The following syntax can be used to print messages:
  //! @begincode
  //!   Message::DefaultMessenger()->Send ("My Warning", Message_Warning);
  //!   Message::SendWarning ("My Warning"); // short-cut for Message_Warning
  //!   Message::SendWarning() << "My Warning with " << theCounter << " arguments";
  //!   Message::SendFail ("My Failure"); // short-cut for Message_Fail
  //! @endcode
  Standard_EXPORT static const Handle(Message_Messenger)& DefaultMessenger();

public:
  //!@name Short-cuts to DefaultMessenger

  static Message_Messenger::StreamBuffer Send(Message_Gravity theGravity)
  {
    return DefaultMessenger()->Send (theGravity);
  }

  static void Send(const TCollection_AsciiString& theMessage, Message_Gravity theGravity)
  {
    DefaultMessenger()->Send (theMessage, theGravity);
  }

  static Message_Messenger::StreamBuffer SendFail()    { return DefaultMessenger()->SendFail ();    }
  static Message_Messenger::StreamBuffer SendAlarm()   { return DefaultMessenger()->SendAlarm ();   }
  static Message_Messenger::StreamBuffer SendWarning() { return DefaultMessenger()->SendWarning (); }
  static Message_Messenger::StreamBuffer SendInfo()    { return DefaultMessenger()->SendInfo ();    }
  static Message_Messenger::StreamBuffer SendTrace()   { return DefaultMessenger()->SendTrace ();   }

  static void SendFail    (const TCollection_AsciiString& theMessage) { return DefaultMessenger()->SendFail    (theMessage); }
  static void SendAlarm   (const TCollection_AsciiString& theMessage) { return DefaultMessenger()->SendAlarm   (theMessage); }
  static void SendWarning (const TCollection_AsciiString& theMessage) { return DefaultMessenger()->SendWarning (theMessage); }
  static void SendInfo    (const TCollection_AsciiString& theMessage) { return DefaultMessenger()->SendInfo    (theMessage); }
  static void SendTrace   (const TCollection_AsciiString& theMessage) { return DefaultMessenger()->SendTrace   (theMessage); }

public:

  //! Returns the string filled with values of hours, minutes and seconds.
  //! Example:
  //! 1. (5, 12, 26.3345) returns "05h:12m:26.33s",
  //! 2. (0,  6, 34.496 ) returns "06m:34.50s",
  //! 3. (0,  0,  4.5   ) returns "4.50s"
  Standard_EXPORT static TCollection_AsciiString FillTime (const Standard_Integer Hour, const Standard_Integer Minute, const Standard_Real Second);

};

#endif // _Message_HeaderFile
