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

#include <Message_SequenceOfPrinters.hxx>

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
//! For convenience, a set of methods Send...() returning a string
//! stream buffer is defined for use of stream-like syntax with operator << 
//!
//! Example:
//! ~~~~~
//! Messenger->SendFail() << " Unknown fail at line " << aLineNo << " in file " << aFile;
//! ~~~~~
//!
//! The message is sent to messenger on destruction of the stream buffer,
//! call to Flush(), or passing manipulator std::ends, std::endl, or std::flush.
//! Empty messages are not sent except if manipulator is used.
class Message_Messenger : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Message_Messenger, Standard_Transient)
public:
  //! Auxiliary class wrapping std::stringstream thus allowing constructing
  //! message via stream interface, and putting result into its creator
  //! Message_Messenger within destructor.
  //!
  //! It is intended to be used either as temporary object or as local
  //! variable, note that content will be lost if it is copied.
  class StreamBuffer
  {
  public:

    //! Destructor flushing constructed message.
    ~StreamBuffer() { Flush(); }

    //! Flush collected string to messenger
    void Flush(Standard_Boolean doForce = Standard_False)
    {
      myStream.flush();
      if (doForce || myStream.tellp() != std::streampos(0))
      {
        if (myMessenger)
        {
          myMessenger->Send(myStream, myGravity);
        }
        myStream.str(std::string()); // empty the buffer for possible reuse
      }
    }

    //! Formal copy constructor.
    //!
    //! Since buffer is intended for use as temporary object or local
    //! variable, copy (or move) is needed only formally to be able to
    //! return the new instance from relevant creation method.
    //! In practice it should never be called because modern compilers
    //! create such instances in place.
    //! However note that if this constructor is called, the buffer
    //! content (string) will not be copied  (move is not supported for
    //! std::stringstream class on old compilers such as gcc 4.4, msvc 9).
    StreamBuffer (const StreamBuffer& theOther)
    : myMessenger(theOther.myMessenger), myGravity(theOther.myGravity)
    {
    }

    //! Wrapper for operator << of the stream
    template <typename T>
    StreamBuffer& operator << (const T& theArg)
    {
      myStream << theArg;
      return *this;
    }

    //! Operator << for manipulators of ostream (ends, endl, flush),
    //! flushes the buffer (sends the message)
    StreamBuffer& operator << (std::ostream& (*)(std::ostream&))
    {
      Flush(Standard_True);
      return *this;
    }

    //! Access to the stream object
    Standard_SStream& Stream () { return myStream; }

    //! Cast to OStream&
    operator Standard_OStream& () { return myStream; }

    //! Access to the messenger
    Message_Messenger* Messenger () { return myMessenger; }

  private:
    friend class Message_Messenger;

    //! Main constructor creating temporary buffer.
    //! Accessible only to Messenger class.
    StreamBuffer (Message_Messenger* theMessenger, Message_Gravity theGravity)
    : myMessenger (theMessenger),
      myGravity (theGravity)
    {}

  private:
    Message_Messenger* myMessenger; // don't make a Handle since this object should be created on stack
    Message_Gravity    myGravity;
    Standard_SStream   myStream;
  };

public:

  //! Empty constructor; initializes by single printer directed to std::cout.
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
  Standard_EXPORT void Send (const Standard_CString theString,
                             const Message_Gravity theGravity = Message_Warning) const;
  
  //! See above
  Standard_EXPORT void Send (const Standard_SStream& theStream,
                             const Message_Gravity theGravity = Message_Warning) const;

  //! See above
  Standard_EXPORT void Send (const TCollection_AsciiString& theString,
                             const Message_Gravity theGravity = Message_Warning) const;
  
  //! See above
  Standard_EXPORT void Send (const TCollection_ExtendedString& theString,
                             const Message_Gravity theGravity = Message_Warning) const;

  //! Create string buffer for message of specified type
  StreamBuffer Send (Message_Gravity theGravity) { return StreamBuffer (this, theGravity); }

  //! See above
  Standard_EXPORT void Send (const Handle(Standard_Transient)& theObject, const Message_Gravity theGravity = Message_Warning) const;

  //! Create string buffer for sending Fail message
  StreamBuffer SendFail () { return Send (Message_Fail); }

  //! Create string buffer for sending Alarm message
  StreamBuffer SendAlarm () { return Send (Message_Alarm); }

  //! Create string buffer for sending Warning message
  StreamBuffer SendWarning () { return Send (Message_Warning); }

  //! Create string buffer for sending Info message
  StreamBuffer SendInfo () { return Send (Message_Info); }

  //! Create string buffer for sending Trace message
  StreamBuffer SendTrace () { return Send (Message_Trace); }

  //! Short-cut to Send (theMessage, Message_Fail)
  void SendFail (const TCollection_AsciiString& theMessage) { Send (theMessage, Message_Fail); }

  //! Short-cut to Send (theMessage, Message_Alarm)
  void SendAlarm (const TCollection_AsciiString& theMessage) { Send (theMessage, Message_Alarm); }

  //! Short-cut to Send (theMessage, Message_Warning)
  void SendWarning (const TCollection_AsciiString& theMessage) { Send (theMessage, Message_Warning); }

  //! Short-cut to Send (theMessage, Message_Info)
  void SendInfo (const TCollection_AsciiString& theMessage) { Send (theMessage, Message_Info); }

  //! Short-cut to Send (theMessage, Message_Trace)
  void SendTrace (const TCollection_AsciiString& theMessage) { Send (theMessage, Message_Trace); }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

private:

  Message_SequenceOfPrinters myPrinters;

};

#endif // _Message_Messenger_HeaderFile
