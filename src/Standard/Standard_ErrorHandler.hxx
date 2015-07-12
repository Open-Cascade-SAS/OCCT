// Created on: 1992-09-28
// Created by: Ramin BARRETO
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Standard_ErrorHandler_HeaderFile
#define _Standard_ErrorHandler_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_PErrorHandler.hxx>
#include <Standard_JmpBuf.hxx>
#include <Standard_HandlerStatus.hxx>
#include <Standard_ThreadId.hxx>
#include <Standard_Address.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>
class Standard_Failure;
class Standard_ErrorHandlerCallback;



class Standard_ErrorHandler 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Create a ErrorHandler (to be used with try{}catch(){}).
  //! It uses the "setjmp" and "longjmp" routines.
  Standard_EXPORT Standard_ErrorHandler();
  
  //! Unlinks and checks if there is a raised exception.
  Standard_EXPORT void Destroy();
~Standard_ErrorHandler()
{
  Destroy();
}
  
  //! Removes handler from the handlers list
  Standard_EXPORT void Unlink();
  
  //! Returns "True" if the caught exception has the same type
  //! or inherits from "aType"
  Standard_EXPORT Standard_Boolean Catches (const Handle(Standard_Type)& aType);
  
  //! Returns label for jump
    Standard_JmpBuf& Label();
  
  //! Returns the current Error.
  Standard_EXPORT Handle(Standard_Failure) Error() const;
  
  //! Returns the caught exception.
  Standard_EXPORT static Handle(Standard_Failure) LastCaughtError();
  
  //! Test if the code is currently running in a try block
  Standard_EXPORT static Standard_Boolean IsInTryBlock();


friend class Standard_Failure;
friend class Standard_ErrorHandlerCallback;


protected:





private:

  
  //! A exception is raised but it is not yet caught.
  //! So Abort the current function and transmit the exception
  //! to "calling routines".
  //! Warning: If no catch is prepared for this exception, it displays the
  //! exception name and calls "exit(1)".
  Standard_EXPORT static void Abort (const Handle(Standard_Failure)& theError);
  
  //! Set the Error which will be transmitted to "calling routines".
  Standard_EXPORT static void Error (const Handle(Standard_Failure)& aError);
  
  //! Returns the current handler (Top in former implemntations)
  Standard_EXPORT static Standard_PErrorHandler FindHandler (const Standard_HandlerStatus theStatus, const Standard_Boolean theUnlink);


  Standard_PErrorHandler myPrevious;
  Handle(Standard_Failure) myCaughtError;
  Standard_JmpBuf myLabel;
  Standard_HandlerStatus myStatus;
  Standard_ThreadId myThread;
  Standard_Address myCallbackPtr;


};


#include <Standard_ErrorHandler.lxx>





#endif // _Standard_ErrorHandler_HeaderFile
