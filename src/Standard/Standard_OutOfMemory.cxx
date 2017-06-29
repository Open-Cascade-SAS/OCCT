// Created on: 2016-01-06
// Created by: Andrey Betenev
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Standard_OutOfMemory.hxx>

#ifdef min
#undef min
#endif

#include <algorithm>
#include <stdlib.h>

IMPLEMENT_STANDARD_RTTIEXT(Standard_OutOfMemory,Standard_ProgramError)

//=======================================================================
//function : Standard_OutOfMemory
//purpose  :
//=======================================================================

Standard_OutOfMemory::Standard_OutOfMemory(const Standard_CString theMessage)
{
  // call explicitly own method (non-virtual call)
  Standard_OutOfMemory::SetMessageString (theMessage);
}

//=======================================================================
//function : GetMessageString
//purpose  :
//=======================================================================

Standard_CString Standard_OutOfMemory::GetMessageString() const
{
  return myBuffer;
}
  
//=======================================================================
//function : SetMessageString
//purpose  :
//=======================================================================

void Standard_OutOfMemory::SetMessageString (const Standard_CString theMessage)
{
  // restrict length of the message by buffer size
  size_t n = (theMessage ? std::min (strlen (theMessage), sizeof(myBuffer) - 1) : 0);

  // first set line end symbol to be safe in case of concurrent call
  myBuffer[n] = '\0';
  if (n > 0)
    memcpy (myBuffer, theMessage, n);
}

//=======================================================================
//function : Raise
//purpose  :
//=======================================================================

void Standard_OutOfMemory::Raise(const Standard_CString theMessage)
{
  NewInstance(theMessage)->Reraise();
}

//=======================================================================
//function : Raise
//purpose  :
//=======================================================================

void Standard_OutOfMemory::Raise(Standard_SStream& theMessage)
{
  NewInstance(theMessage.str().c_str())->Reraise();
}

//=======================================================================
//function : NewInstance
//purpose  :
//=======================================================================

// global instance must be allocated at load-time
static Handle(Standard_OutOfMemory) anOutOfMemInstance = new Standard_OutOfMemory;

Handle(Standard_OutOfMemory) Standard_OutOfMemory::NewInstance(const Standard_CString theMessage)
{
  anOutOfMemInstance->SetMessageString (theMessage);
  return anOutOfMemInstance;
}

//=======================================================================
//function : Throw
//purpose  :
//=======================================================================

void Standard_OutOfMemory::Throw () const
{
  throw *this;
}
