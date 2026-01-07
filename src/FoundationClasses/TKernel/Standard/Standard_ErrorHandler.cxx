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

#include <Standard_ErrorHandler.hxx>

// During [sig]setjmp()/[sig]longjmp() K_SETJMP is non zero (try)
// So if there is an abort request and if K_SETJMP is non zero, the abort
// request will be ignored. If the abort request do a raise during a setjmp
// or a longjmp, there will be a "terminating SEGV" impossible to handle.

// The top of the Errors Stack

static thread_local Standard_ErrorHandler* Top = nullptr;

Standard_ErrorHandler::Standard_ErrorHandler()
{
  myPrevious = Top;
  Top        = this;
}

//=================================================================================================

void Standard_ErrorHandler::Destroy()
{
  Unlink();
}

//=================================================================================================

void Standard_ErrorHandler::Unlink()
{
  // Unlink handlers that were created after this one (shouldn't happen in normal usage)
  while (Top != nullptr && Top != this)
  {
    Top->Unlink();
  }

  if (Top == this)
  {
    Top = myPrevious;
  }

  myPrevious = nullptr;

  // unlink and destroy all registered callbacks
  void* aPtr    = myCallbackPtr;
  myCallbackPtr = nullptr;
  while (aPtr)
  {
    Standard_ErrorHandler::Callback* aCallback = (Standard_ErrorHandler::Callback*)aPtr;
    aPtr                                       = aCallback->myNext;
    // Call destructor explicitly, as we know that it will not be called automatically
    aCallback->DestroyCallback();
  }
}

//=================================================================================================

bool Standard_ErrorHandler::IsInTryBlock()
{
  Standard_ErrorHandler* anActive = FindHandler();
  return anActive != nullptr;
}

//=================================================================================================

void Standard_ErrorHandler::Raise()
{
  if (std::holds_alternative<std::monostate>(myCaughtError))
  {
    std::cerr << "*** Abort *** an exception handler was called, but no exception object is set."
              << std::endl;
    exit(1);
  }

  // Visit the variant and throw the appropriate exception type
  std::visit(
    [](auto&& theException) {
      using T = std::decay_t<decltype(theException)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        throw theException;
      }
    },
    myCaughtError);
}

//=================================================================================================

Standard_ErrorHandler* Standard_ErrorHandler::FindHandler()
{
  return Top;
}

#if defined(OCC_CONVERT_SIGNALS)

Standard_ErrorHandler::Callback::Callback() {}

Standard_ErrorHandler::Callback::~Callback()
{
  UnregisterCallback();
}

void Standard_ErrorHandler::Callback::RegisterCallback()
{
  if (myHandler)
    return; // already registered

  // find current active exception handler
  Standard_ErrorHandler* aHandler = Standard_ErrorHandler::FindHandler();

  // if found, add this callback object first to the list
  if (aHandler)
  {
    myHandler = aHandler;
    myNext    = aHandler->myCallbackPtr;
    if (myNext)
      ((Standard_ErrorHandler::Callback*)myNext)->myPrev = this;
    aHandler->myCallbackPtr = this;
  }
}

void Standard_ErrorHandler::Callback::UnregisterCallback()
{
  if (!myHandler)
    return;
  if (myNext)
    ((Standard_ErrorHandler::Callback*)myNext)->myPrev = myPrev;
  if (myPrev)
    ((Standard_ErrorHandler::Callback*)myPrev)->myNext = myNext;
  else if (((Standard_ErrorHandler*)myHandler)->myCallbackPtr == this)
    ((Standard_ErrorHandler*)myHandler)->myCallbackPtr = (Standard_ErrorHandler::Callback*)myNext;
  myHandler = myNext = myPrev = nullptr;
}
#endif
