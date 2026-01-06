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

#include <algorithm>
#include <cstring>

namespace
{
//! Global parameter defining default length of stack trace.
static int Standard_Failure_DefaultStackTraceLength = 0;
} // namespace

//=================================================================================================

Standard_Failure::StringRef* Standard_Failure::StringRef::Allocate(const char* theString)
{
  if (theString == nullptr || theString[0] == '\0')
  {
    return nullptr;
  }

  const size_t aLen    = std::strlen(theString);
  StringRef*   aStrPtr = (StringRef*)Standard::AllocateOptimal(aLen + sizeof(int) + 1);
  if (aStrPtr != nullptr)
  {
    std::strcpy(&aStrPtr->Message[0], theString);
    aStrPtr->Counter = 1;
  }
  return aStrPtr;
}

//=================================================================================================

Standard_Failure::StringRef* Standard_Failure::StringRef::Copy(StringRef* theString)
{
  if (theString == nullptr)
  {
    return nullptr;
  }

  ++theString->Counter;
  return theString;
}

//=================================================================================================

void Standard_Failure::StringRef::Free(StringRef* theString)
{
  if (theString != nullptr)
  {
    if (--theString->Counter == 0)
    {
      Standard::Free(theString);
    }
  }
}

//=================================================================================================

void Standard_Failure::captureStackTrace()
{
  const int aStackLength = Standard_Failure_DefaultStackTraceLength;
  if (aStackLength > 0)
  {
    // Limit stack allocation to 64KB to prevent stack overflow
    const int aStackBufLen = std::min(std::max(aStackLength * 200, 2048), 65536);
    char*     aStackBuffer = (char*)alloca(aStackBufLen);
    if (aStackBuffer != nullptr)
    {
      std::memset(aStackBuffer, 0, aStackBufLen);
      if (Standard::StackTrace(aStackBuffer, aStackBufLen, aStackLength, nullptr, 1))
      {
        myStackTrace = StringRef::Allocate(aStackBuffer);
      }
    }
  }
}

//=================================================================================================

Standard_Failure::Standard_Failure()
    : myMessage(nullptr),
      myStackTrace(nullptr)
{
  captureStackTrace();
}

//=================================================================================================

Standard_Failure::Standard_Failure(const char* theMessage)
    : myMessage(nullptr),
      myStackTrace(nullptr)
{
  myMessage = StringRef::Allocate(theMessage);
  captureStackTrace();
}

//=================================================================================================

Standard_Failure::Standard_Failure(const char* theMessage, const char* theStackTrace)
    : myMessage(nullptr),
      myStackTrace(nullptr)
{
  myMessage    = StringRef::Allocate(theMessage);
  myStackTrace = StringRef::Allocate(theStackTrace);
}

//=================================================================================================

Standard_Failure::Standard_Failure(const Standard_Failure& theOther)
    : std::exception(theOther),
      myMessage(nullptr),
      myStackTrace(nullptr)
{
  myMessage    = StringRef::Copy(theOther.myMessage);
  myStackTrace = StringRef::Copy(theOther.myStackTrace);
}

//=================================================================================================

Standard_Failure& Standard_Failure::operator=(const Standard_Failure& theOther)
{
  if (this != &theOther)
  {
    StringRef::Free(myMessage);
    StringRef::Free(myStackTrace);
    myMessage    = StringRef::Copy(theOther.myMessage);
    myStackTrace = StringRef::Copy(theOther.myStackTrace);
  }
  return *this;
}

//=================================================================================================

Standard_Failure::~Standard_Failure()
{
  StringRef::Free(myMessage);
  StringRef::Free(myStackTrace);
}

//=================================================================================================

const char* Standard_Failure::what() const noexcept
{
  return myMessage != nullptr ? myMessage->GetMessage() : "";
}

//=================================================================================================

const char* Standard_Failure::GetStackString() const
{
  return myStackTrace != nullptr ? myStackTrace->GetMessage() : "";
}

//=================================================================================================

void Standard_Failure::Print(Standard_OStream& theStream) const
{
  if (myMessage != nullptr)
  {
    theStream << ExceptionType() << ": " << myMessage->GetMessage();
  }
  else
  {
    theStream << ExceptionType();
  }
  if (myStackTrace != nullptr)
  {
    theStream << myStackTrace->GetMessage();
  }
}

//=================================================================================================

int Standard_Failure::DefaultStackTraceLength()
{
  return Standard_Failure_DefaultStackTraceLength;
}

//=================================================================================================

void Standard_Failure::SetDefaultStackTraceLength(int theNbStackTraces)
{
  Standard_Failure_DefaultStackTraceLength = theNbStackTraces;
}
