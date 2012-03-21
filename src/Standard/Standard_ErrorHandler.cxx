// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

//============================================================================
//==== Titre: Standard_ErrorHandler.cxx
//==== Role : class "Standard_ErrorHandler" implementation.
//============================================================================
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_ErrorHandlerCallback.hxx>
#include <Standard_Mutex.hxx>
#include <Standard.hxx>

#ifndef WNT
#include <pthread.h>
#else
#include <windows.h>
#endif

// ===========================================================================
// The class "Standard_ErrorHandler" variables
// ===========================================================================

// During [sig]setjmp()/[sig]longjmp() K_SETJMP is non zero (try)
// So if there is an abort request and if K_SETJMP is non zero, the abort
// request will be ignored. If the abort request do a raise during a setjmp
// or a longjmp, there will be a "terminating SEGV" impossible to handle.

//==== The top of the Errors Stack ===========================================
static Standard_ErrorHandler* Top = 0;

// A mutex to protect from concurrent access to Top
// Note that we should NOT use Sentry while in this class, as Sentry
// would register mutex as callback in the current exception handler
static Standard_Mutex theMutex; 

static inline Standard_ThreadId GetThreadID()
{
#ifndef WNT
  return pthread_self();
#else
  return GetCurrentThreadId();
#endif
}

//============================================================================
//====  Constructor : Create a ErrorHandler structure. And add it at the 
//====                'Top' of "ErrorHandler's stack".
//============================================================================

Standard_ErrorHandler::Standard_ErrorHandler () : 
       myStatus(Standard_HandlerVoid), myCallbackPtr(0)
{
  myThread   = GetThreadID();

  if (Standard::IsReentrant())
    theMutex.Lock();
  myPrevious = Top;
  Top        = this;
  if (Standard::IsReentrant())
    theMutex.Unlock();
}


//============================================================================
//==== Destructor : Delete the ErrorHandler and Abort if there is a 'Error'.
//============================================================================

void Standard_ErrorHandler::Destroy()
{
  Unlink();
  if (myStatus == Standard_HandlerJumped)
  {
    // jumped, but not caught
    Abort (myCaughtError);
  }
}


//=======================================================================
//function : Unlink
//purpose  : 
//=======================================================================

void Standard_ErrorHandler::Unlink()
{
  // put a lock on the stack
  if (Standard::IsReentrant())
    theMutex.Lock();
  
  Standard_ErrorHandler* aPrevious = 0;
  Standard_ErrorHandler* aCurrent = Top;
  
  // locate this handler in the stack
  while(aCurrent!=0 && this!=aCurrent) {
    aPrevious = aCurrent;
    aCurrent = aCurrent->myPrevious;
  }
  
  if(aCurrent==0) {
    if (Standard::IsReentrant())
      theMutex.Unlock();
    return;
  }
  
  if(aPrevious==0) {
    // a top exception taken
    Top = aCurrent->myPrevious;
  }
  else {
    aPrevious->myPrevious=aCurrent->myPrevious;
  }
  myPrevious = 0;
  if (Standard::IsReentrant())
    theMutex.Unlock();

  // unlink and destroy all registered callbacks
  Standard_Address aPtr = aCurrent->myCallbackPtr;
  myCallbackPtr = 0;
  while ( aPtr ) {
    Standard_ErrorHandlerCallback* aCallback = (Standard_ErrorHandlerCallback*)aPtr;
    aPtr = aCallback->myNext;
    // Call destructor explicitly, as we know that it will not be called automatically
    aCallback->DestroyCallback();
  }
}

//=======================================================================
//function : IsInTryBlock
//purpose  :  test if the code is currently running in
//=======================================================================

Standard_Boolean Standard_ErrorHandler::IsInTryBlock()
{
  Standard_ErrorHandler* anActive = FindHandler(Standard_HandlerVoid, Standard_False);
  return anActive != NULL && anActive->myLabel != NULL;
}


//============================================================================
//==== Abort: make a longjmp to the saved Context.
//====    Abort if there is a non null 'Error'
//============================================================================

void Standard_ErrorHandler::Abort (const Handle(Standard_Failure)& theError)
{
  Standard_ErrorHandler* anActive = FindHandler(Standard_HandlerVoid, Standard_True);

  //==== Check if can do the "longjmp" =======================================
  if(anActive == NULL || anActive->myLabel == NULL) {
    cerr << "*** Abort *** an exception was raised, but no catch was found." << endl;
    if (!theError.IsNull())
      cerr << "\t... The exception is:" << theError->GetMessageString() << endl;
    exit(1);
  }

  anActive->myStatus = Standard_HandlerJumped;
  longjmp(anActive->myLabel, Standard_True);
}


//============================================================================
//==== Catches: If there is a 'Error', and it is in good type 
//====          returns True and clean 'Error', else returns False.
//============================================================================

Standard_Boolean Standard_ErrorHandler::Catches (const Handle(Standard_Type)& AType) 
{
  Standard_ErrorHandler* anActive = FindHandler(Standard_HandlerJumped, Standard_False);
  if(anActive==0)
    return Standard_False;
  
  if(anActive->myCaughtError.IsNull())
    return Standard_False;

  if(anActive->myCaughtError->IsKind(AType)){
    myStatus=Standard_HandlerProcessed;
    return Standard_True;
  } else {
    return Standard_False;
  }
}

Handle(Standard_Failure) Standard_ErrorHandler::LastCaughtError()
{
  Handle(Standard_Failure) aHandle;
  Standard_ErrorHandler* anActive = FindHandler(Standard_HandlerProcessed, Standard_False);
  if(anActive!=0) 
    aHandle = anActive->myCaughtError;
  
  return aHandle;
}

Handle(Standard_Failure) Standard_ErrorHandler::Error() const
{
  return myCaughtError;
}


void Standard_ErrorHandler::Error (const Handle(Standard_Failure)& theError)
{
  Standard_ErrorHandler* anActive = FindHandler (Standard_HandlerVoid, Standard_False);
  if (anActive == NULL)
    Abort (theError);

  anActive->myCaughtError = theError;
}


Standard_ErrorHandler* Standard_ErrorHandler::FindHandler(const Standard_HandlerStatus theStatus,
                                                          const Standard_Boolean theUnlink)
{
  // lock the stack
  if (Standard::IsReentrant())
    theMutex.Lock();
    
  // Find the current ErrorHandler Accordin tread
  Standard_ErrorHandler* aPrevious = 0;
  Standard_ErrorHandler* aCurrent = Top;
  Standard_ErrorHandler* anActive = 0;
  Standard_Boolean aStop = Standard_False;
  Standard_ThreadId aTreadId = GetThreadID();
  
  // searching an exception with correct ID number
  // which is not processed for the moment
  while(!aStop) {
    while(aCurrent!=NULL && aTreadId!=aCurrent->myThread) {
      aPrevious = aCurrent;
      aCurrent = aCurrent->myPrevious;
    }
    
    if(aCurrent!=NULL) {
      if(theStatus!=aCurrent->myStatus) {
        
        if(theUnlink) {
          //unlink current
          if(aPrevious==0) {
            // a top exception taken
            Top = aCurrent->myPrevious;
          }
          else {
            aPrevious->myPrevious=aCurrent->myPrevious;
          }
        }
        
        //shift
        aCurrent = aCurrent->myPrevious;
      }
      else {
	//found one
        anActive = aCurrent;
	aStop = Standard_True;
      }
    }
    else {
      //Current is NULL, means that no handlesr
      aStop = Standard_True;
    }
  }
  if (Standard::IsReentrant())
    theMutex.Unlock();
  
  return anActive;
}
