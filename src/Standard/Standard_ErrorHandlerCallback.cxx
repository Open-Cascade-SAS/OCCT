// Created on: 2006-04-13
// Created by: Andrey BETENEV
// Copyright (c) 2006-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


// If either NO_CXX_EXCEPTION or OCC_CONVERT_SIGNALS is defined,
// provide complete implementation

#include <Standard_ErrorHandlerCallback.ixx>
#include <Standard_ErrorHandler.hxx>

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
