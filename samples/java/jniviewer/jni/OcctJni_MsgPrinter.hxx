// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef OcctJni_MsgPrinter_H
#define OcctJni_MsgPrinter_H

#include <Message_Printer.hxx>

#include <jni.h>

// Class providing connection between messenger interfaces in C++ and Java layers.
class OcctJni_MsgPrinter : public Message_Printer
{
public:

  //! Default constructor
  OcctJni_MsgPrinter (JNIEnv* theJEnv,
                      jobject theJObj);

  //! Destructor.
  ~OcctJni_MsgPrinter();

  //! Redirection to TCollection_AsciiString method
  virtual void Send (const TCollection_ExtendedString& theString,
                     const Message_Gravity             theGravity,
                     const Standard_Boolean            theToPutEndl) const;

  //! Redirection to TCollection_AsciiString method
  virtual void Send (const Standard_CString&           theString,
                     const Message_Gravity             theGravity,
                     const Standard_Boolean            theToPutEndl) const;

  //! Main printing method
  virtual void Send (const TCollection_AsciiString&    theString,
                     const Message_Gravity             theGravity,
                     const Standard_Boolean            theToPutEndl) const;

private:

  JNIEnv*   myJEnv;
  jobject   myJObj;
  jmethodID myJMet;

public:

  DEFINE_STANDARD_RTTIEXT(OcctJni_MsgPrinter,Message_Printer)

};

DEFINE_STANDARD_HANDLE(OcctJni_MsgPrinter, Message_Printer)

#endif // OcctJni_MsgPrinter_H
