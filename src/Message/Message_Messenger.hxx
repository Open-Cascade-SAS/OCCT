// Created on: 2007-06-28
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

#ifndef _Message_Messenger_HeaderFile
#define _Message_Messenger_HeaderFile

#include <Message_Gravity.hxx>
#include <Message_SequenceOfPrinters.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TCollection_HExtendedString.hxx>

class Message_Printer;

// resolve name collisions with WinAPI headers
#ifdef AddPrinter
  #undef AddPrinter
#endif

class Message_Messenger;
DEFINE_STANDARD_HANDLE(Message_Messenger, Standard_Transient)

//! Messenger is API class providing general-purpose interface for
//! libraries that may issue text messages without knowledge
//! of how these messages will be further processed.
//!
//! The messenger contains a sequence of "printers" which can be
//! customized by the application, and dispatches every received
//! message to all the printers.
//!
//! For convenience, a number of operators << are defined with left
//! argument being Handle(Message_Messenger); thus it can be used
//! with syntax similar to C++ streams.
//! Note that all these operators use trace level Warning.
class Message_Messenger : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Message_Messenger, Standard_Transient)
public:

  //! Empty constructor; initializes by single printer directed to cout.
  //! Note: the default messenger is not empty but directed to cout
  //! in order to protect against possibility to forget defining printers.
  //! If printing to cout is not needed, clear messenger by GetPrinters().Clear()
  Standard_EXPORT Message_Messenger();
  
  //! Create messenger with single printer
  Standard_EXPORT Message_Messenger(const Handle(Message_Printer)& thePrinter);
  
  //! Add a printer to the messenger.
  //! The printer will be added only if it is not yet in the list.
  //! Returns True if printer has been added.
  Standard_EXPORT Standard_Boolean AddPrinter (const Handle(Message_Printer)& thePrinter);
  
  //! Removes specified printer from the messenger.
  //! Returns True if this printer has been found in the list
  //! and removed.
  Standard_EXPORT Standard_Boolean RemovePrinter (const Handle(Message_Printer)& thePrinter);
  
  //! Removes printers of specified type (including derived classes)
  //! from the messenger.
  //! Returns number of removed printers.
  Standard_EXPORT Standard_Integer RemovePrinters (const Handle(Standard_Type)& theType);
  
  //! Returns current sequence of printers
  const Message_SequenceOfPrinters& Printers() const { return myPrinters; }

  //! Returns sequence of printers
  //! The sequence can be modified.
  Message_SequenceOfPrinters& ChangePrinters() { return myPrinters; }

  //! Dispatch a message to all the printers in the list.
  //! Three versions of string representations are accepted for
  //! convenience, by default all are converted to ExtendedString.
  //! The parameter putEndl specifies whether the new line should
  //! be started after this message (default) or not (may have
  //! sense in some conditions).
  Standard_EXPORT void Send (const Standard_CString theString, const Message_Gravity theGravity = Message_Warning, const Standard_Boolean putEndl = Standard_True) const;
  
  //! See above
  Standard_EXPORT void Send (const TCollection_AsciiString& theString, const Message_Gravity theGravity = Message_Warning, const Standard_Boolean putEndl = Standard_True) const;
  
  //! See above
  Standard_EXPORT void Send (const TCollection_ExtendedString& theString, const Message_Gravity theGravity = Message_Warning, const Standard_Boolean putEndl = Standard_True) const;

private:

  Message_SequenceOfPrinters myPrinters;

};

// CString
inline const Handle(Message_Messenger)& operator<< (const Handle(Message_Messenger)& theMessenger,
                                                    const Standard_CString theStr)
{
  theMessenger->Send (theStr, Message_Info, Standard_False);
  return theMessenger;
}

// AsciiString
inline const Handle(Message_Messenger)& operator<< (const Handle(Message_Messenger)& theMessenger,
                                                    const TCollection_AsciiString& theStr)
{
  theMessenger->Send (theStr, Message_Info, Standard_False);
  return theMessenger;
}

// HAsciiString
inline const Handle(Message_Messenger)& operator<< (const Handle(Message_Messenger)& theMessenger,
                                                    const Handle(TCollection_HAsciiString)& theStr)
{
  theMessenger->Send (theStr->String(), Message_Info, Standard_False);
  return theMessenger;
}

// ExtendedString
inline const Handle(Message_Messenger)& operator<< (const Handle(Message_Messenger)& theMessenger,
                                                    const TCollection_ExtendedString& theStr)
{
  theMessenger->Send (theStr, Message_Info, Standard_False);
  return theMessenger;
}

// HExtendedString
inline const Handle(Message_Messenger)& operator<< (const Handle(Message_Messenger)& theMessenger,
                                                    const Handle(TCollection_HExtendedString)& theStr)
{
  theMessenger->Send (theStr->String(), Message_Info, Standard_False);
  return theMessenger;
}

// Integer
inline const Handle(Message_Messenger)& operator<< (const Handle(Message_Messenger)& theMessenger,
                                                    const Standard_Integer theVal)
{
  TCollection_AsciiString aStr (theVal);
  theMessenger->Send (aStr, Message_Info, Standard_False);
  return theMessenger;
}

// Real
inline const Handle(Message_Messenger)& operator<< (const Handle(Message_Messenger)& theMessenger,
                                                    const Standard_Real theVal)
{
  TCollection_AsciiString aStr (theVal);
  theMessenger->Send (aStr, Message_Info, Standard_False);
  return theMessenger;
}

// Stream
inline const Handle(Message_Messenger)& operator<< (const Handle(Message_Messenger)& theMessenger,
                                                    const Standard_SStream& theStream)
{
  theMessenger->Send (theStream.str().c_str(), Message_Info, Standard_False);
  return theMessenger;
}

// manipulators
inline const Handle(Message_Messenger)&
       operator << (const Handle(Message_Messenger)& theMessenger,
                    const Handle(Message_Messenger)& (*pman) (const Handle(Message_Messenger)&))
{
  return pman (theMessenger);
}

// endl
inline const Handle(Message_Messenger)& endl (const Handle(Message_Messenger)& theMessenger)
{
  theMessenger->Send ("", Message_Info, Standard_True);
  return theMessenger;
}

#endif // _Message_Messenger_HeaderFile
