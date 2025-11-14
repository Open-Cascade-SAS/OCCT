// Copyright (c) 1998-1999 Matra Datavision
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

#include <Standard_Failure.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Macro.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_Type.hxx>
#include <Standard_TypeMismatch.hxx>

#include <string.h>

IMPLEMENT_STANDARD_RTTIEXT(Standard_Failure, Standard_Transient)

namespace
{
//! Global parameter defining default length of stack trace.
static Standard_Integer Standard_Failure_DefaultStackTraceLength = 0;
} // namespace

//=================================================================================================

Standard_Failure::StringRef* Standard_Failure::StringRef::allocate_message(
  const Standard_CString theString)
{
  if (theString == NULL || *theString == '\0')
  {
    return NULL;
  }

  const Standard_Size aLen = strlen(theString);
  StringRef* aStrPtr = (StringRef*)Standard::AllocateOptimal(aLen + sizeof(Standard_Integer) + 1);
  if (aStrPtr != NULL)
  {
    strcpy((char*)&aStrPtr->Message[0], theString);
    aStrPtr->Counter = 1;
  }
  return aStrPtr;
}

//=================================================================================================

Standard_Failure::StringRef* Standard_Failure::StringRef::copy_message(
  Standard_Failure::StringRef* theString)
{
  if (theString == NULL)
  {
    return NULL;
  }

  ++theString->Counter;
  return theString;
}

//=================================================================================================

void Standard_Failure::StringRef::deallocate_message(Standard_Failure::StringRef* theString)
{
  if (theString != NULL)
  {
    if (--theString->Counter == 0)
    {
      Standard::Free((void*)theString);
    }
  }
}

//=================================================================================================

Standard_Failure::Standard_Failure()
    : myMessage(NULL),
      myStackTrace(NULL)
{
  const Standard_Integer aStackLength = Standard_Failure_DefaultStackTraceLength;
  if (aStackLength > 0)
  {
    int   aStackBufLen = std::max(aStackLength * 200, 2048);
    char* aStackBuffer = (char*)alloca(aStackBufLen);
    if (aStackBuffer != NULL)
    {
      memset(aStackBuffer, 0, aStackBufLen);
      if (Standard::StackTrace(aStackBuffer, aStackBufLen, aStackLength, NULL, 1))
      {
        myStackTrace = StringRef::allocate_message(aStackBuffer);
      }
    }
  }
}

//=================================================================================================

Standard_Failure::Standard_Failure(const Standard_CString theDesc)
    : myMessage(NULL),
      myStackTrace(NULL)
{
  myMessage                           = StringRef::allocate_message(theDesc);
  const Standard_Integer aStackLength = Standard_Failure_DefaultStackTraceLength;
  if (aStackLength > 0)
  {
    int   aStackBufLen = std::max(aStackLength * 200, 2048);
    char* aStackBuffer = (char*)alloca(aStackBufLen);
    if (aStackBuffer != NULL)
    {
      memset(aStackBuffer, 0, aStackBufLen);
      Standard::StackTrace(aStackBuffer, aStackBufLen, aStackLength, NULL, 1);
      myStackTrace = StringRef::allocate_message(aStackBuffer);
    }
  }
}

//=================================================================================================

Standard_Failure::Standard_Failure(const Standard_CString theDesc,
                                   const Standard_CString theStackTrace)
    : myMessage(NULL),
      myStackTrace(NULL)
{
  myMessage    = StringRef::allocate_message(theDesc);
  myStackTrace = StringRef::allocate_message(theStackTrace);
}

//=================================================================================================

Standard_Failure::Standard_Failure(const Standard_Failure& theFailure)
    : Standard_Transient(theFailure),
      myMessage(NULL),
      myStackTrace(NULL)
{
  myMessage    = StringRef::copy_message(theFailure.myMessage);
  myStackTrace = StringRef::copy_message(theFailure.myStackTrace);
}

//=================================================================================================

Standard_Failure::~Standard_Failure()
{
  StringRef::deallocate_message(myMessage);
  StringRef::deallocate_message(myStackTrace);
}

//=================================================================================================

Standard_CString Standard_Failure::GetMessageString() const
{
  return myMessage != NULL ? myMessage->GetMessage() : "";
}

//=================================================================================================

void Standard_Failure::SetMessageString(const Standard_CString theDesc)
{
  if (theDesc == GetMessageString())
  {
    return;
  }

  StringRef::deallocate_message(myMessage);
  myMessage = StringRef::allocate_message(theDesc);
}

//=================================================================================================

Standard_CString Standard_Failure::GetStackString() const
{
  return myStackTrace != NULL ? myStackTrace->GetMessage() : "";
}

//=================================================================================================

void Standard_Failure::SetStackString(const Standard_CString theStack)
{
  if (theStack == GetStackString())
  {
    return;
  }

  StringRef::deallocate_message(myStackTrace);
  myStackTrace = StringRef::allocate_message(theStack);
}

//=================================================================================================

void Standard_Failure::Raise(const Standard_CString theDesc)
{
  Handle(Standard_Failure) aFailure = new Standard_Failure();
  aFailure->Reraise(theDesc);
}

//=================================================================================================

void Standard_Failure::Raise(const Standard_SStream& theReason)
{
  Handle(Standard_Failure) aFailure = new Standard_Failure();
  aFailure->Reraise(theReason);
}

//=================================================================================================

void Standard_Failure::Reraise(const Standard_CString theDesc)
{
  SetMessageString(theDesc);
  Reraise();
}

//=================================================================================================

void Standard_Failure::Reraise(const Standard_SStream& theReason)
{
  SetMessageString(theReason.str().c_str());
  Reraise();
}

//=================================================================================================

void Standard_Failure::Reraise()
{
  Throw();
}

//=================================================================================================

void Standard_Failure::Jump()
{
#if defined(OCC_CONVERT_SIGNALS)
  Standard_ErrorHandler::Error(this);
  Standard_ErrorHandler::Abort(this);
#else
  Throw();
#endif
}

//=================================================================================================

void Standard_Failure::Throw() const
{
  throw *this;
}

//=================================================================================================

void Standard_Failure::Print(Standard_OStream& theStream) const
{
  if (myMessage != NULL)
  {
    theStream << DynamicType() << ": " << GetMessageString();
  }
  else
  {
    theStream << DynamicType();
  }
  if (myStackTrace != NULL)
  {
    theStream << GetStackString();
  }
}

//=================================================================================================

Handle(Standard_Failure) Standard_Failure::NewInstance(Standard_CString theString)
{
  return new Standard_Failure(theString);
}

//=================================================================================================

Handle(Standard_Failure) Standard_Failure::NewInstance(Standard_CString theMessage,
                                                       Standard_CString theStackTrace)
{
  return new Standard_Failure(theMessage, theStackTrace);
}

//=================================================================================================

Standard_Integer Standard_Failure::DefaultStackTraceLength()
{
  return Standard_Failure_DefaultStackTraceLength;
}

//=================================================================================================

void Standard_Failure::SetDefaultStackTraceLength(Standard_Integer theNbStackTraces)
{
  Standard_Failure_DefaultStackTraceLength = theNbStackTraces;
}
