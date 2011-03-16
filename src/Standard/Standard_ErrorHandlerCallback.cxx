// File:	Standard_ErrorHandlerCallback.cxx
// Created:	Thu Apr 13 11:32:40 2006
// Author:	Andrey BETENEV

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
