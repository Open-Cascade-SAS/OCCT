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

#include <OcctJni_MsgPrinter.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <android/log.h>

IMPLEMENT_STANDARD_RTTIEXT(OcctJni_MsgPrinter, Message_Printer)

// =======================================================================
// function : OcctJni_MsgPrinter
// purpose  :
// =======================================================================
OcctJni_MsgPrinter::OcctJni_MsgPrinter (JNIEnv* theJEnv,
                                        jobject theJObj)
: myJEnv (theJEnv),
  myJObj (theJEnv->NewGlobalRef (theJObj)),
  myJMet (NULL)
{
  jclass aJClass = theJEnv->GetObjectClass (theJObj);
  myJMet = theJEnv->GetMethodID (aJClass, "postMessage", "(Ljava/lang/String;)V");
  if (myJMet == NULL)
  {
    __android_log_write (ANDROID_LOG_FATAL, "jniSample", "Broken initialization of OcctJni_MsgPrinter!");
  }
}

// =======================================================================
// function : ~OcctJni_MsgPrinter
// purpose  :
// =======================================================================
OcctJni_MsgPrinter::~OcctJni_MsgPrinter()
{
  //myJEnv->DeleteGlobalRef (myJObj);
}

// =======================================================================
// function : Send
// purpose  :
// =======================================================================
void OcctJni_MsgPrinter::Send (const TCollection_ExtendedString& theString,
                               const Message_Gravity             theGravity,
                               const Standard_Boolean            theToPutEndl) const
{
  if (theGravity >= myTraceLevel)
  {
    const TCollection_AsciiString aStr (theString);
    OcctJni_MsgPrinter::Send (aStr, theGravity, theToPutEndl);
  }
}

// =======================================================================
// function : Send
// purpose  :
// =======================================================================
void OcctJni_MsgPrinter::Send (const TCollection_AsciiString& theString,
                               const Message_Gravity          theGravity,
                               const Standard_Boolean         theToPutEndl) const
{
  if (theGravity < myTraceLevel)
  {
    return;
  }

  ///__android_log_write (ANDROID_LOG_DEBUG, "OcctJni_MsgPrinter", (TCollection_AsciiString(" @@ ") + theString).ToCString());
  if (myJMet == NULL)
  {
    return;
  }

  jstring aJStr = myJEnv->NewStringUTF ((theString + "\n").ToCString());
  myJEnv->CallObjectMethod (myJObj, myJMet, aJStr);
  myJEnv->DeleteLocalRef (aJStr);
}

// =======================================================================
// function : Send
// purpose  :
// =======================================================================
void OcctJni_MsgPrinter::Send (const Standard_CString& theString,
                               const Message_Gravity   theGravity,
                               const Standard_Boolean  theToPutEndl) const
{
  if (theGravity >= myTraceLevel)
  {
    OcctJni_MsgPrinter::Send (TCollection_AsciiString (theString), theGravity, theToPutEndl);
  }
}
