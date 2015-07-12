// Created on: 2006-04-13
// Created by: Andrey BETENEV
// Copyright (c) 2006-2014 OPEN CASCADE SAS
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

// If either NO_CXX_EXCEPTION or OCC_CONVERT_SIGNALS is defined,
// provide complete implementation

#include <Standard_ErrorHandler.hxx>
#include <Standard_ErrorHandlerCallback.hxx>

#if defined(NO_CXX_EXCEPTION) || defined(OCC_CONVERT_SIGNALS)

Standard_ErrorHandlerCallback::Standard_ErrorHandlerCallback () 
  : myHandler(0), myPrev(0), myNext(0) 
{
}

Standard_ErrorHandlerCallback::~Standard_ErrorHandlerCallback () 
{
  UnregisterCallback();
}

void Standard_ErrorHandlerCallback::RegisterCallback () 
{
  if ( myHandler ) return; // already registered

  // find current active exception handler
  Standard_ErrorHandler *aHandler = 
    Standard_ErrorHandler::FindHandler(Standard_HandlerVoid, Standard_False);
 
  // if found, add this callback object first to the list
  if ( aHandler ) {
    myHandler = aHandler;
    myNext = aHandler->myCallbackPtr;
    if ( myNext ) ((Standard_ErrorHandlerCallback*)myNext)->myPrev = this;
    aHandler->myCallbackPtr = this;
  }
}

void Standard_ErrorHandlerCallback::UnregisterCallback () 
{
  if ( ! myHandler ) return;
  if ( myNext )
    ((Standard_ErrorHandlerCallback*)myNext)->myPrev = myPrev;
  if ( myPrev )
    ((Standard_ErrorHandlerCallback*)myPrev)->myNext = myNext;
  else if ( ((Standard_ErrorHandler*)myHandler)->myCallbackPtr == this)
    ((Standard_ErrorHandler*)myHandler)->myCallbackPtr = myNext;
  myHandler = myNext = myPrev = 0;
}

#endif
