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

#ifndef _Standard_ErrorHandlerCallback_HeaderFile
#define _Standard_ErrorHandlerCallback_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
class Standard_ErrorHandler;


//! Defines a base class for callback objects that can be registered
//! in the OCC error handler (the class simulating C++ exceptions)
//! so as to be correctly destroyed when error handler is activated.
//!
//! Note that this is needed only when Open CASCADE is compiled with
//! NO_CXX_EXCEPTION or OCC_CONVERT_SIGNALS options (i.e. on UNIX/Linux).
//! In that case, raising OCC exception and/or signal will not cause
//! C++ stack unwinding and destruction of objects created in the stack.
//!
//! This class is intended to protect critical objects and operations in
//! the try {} catch {} block from being bypassed by OCC signal or exception.
//!
//! Inherit your object from that class, implement DestroyCallback() function,
//! and call Register/Unregister in critical points.
//!
//! Note that you must ensure that your object has life span longer than
//! that of the try {} block in which it calls Register().
class Standard_ErrorHandlerCallback 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Registers this callback object in the current error handler
  //! (if found).
    void RegisterCallback();
  
  //! Unregisters this callback object from the error handler.
    void UnregisterCallback();
virtual Standard_EXPORT ~Standard_ErrorHandlerCallback ();
  
  //! The callback function to perform necessary callback action.
  //! Called by the exception handler when it is being destroyed but
  //! still has this callback registered.
  Standard_EXPORT virtual void DestroyCallback() = 0;


friend class Standard_ErrorHandler;


protected:

  
  //! Empty constructor
    Standard_ErrorHandlerCallback();




private:



  Standard_Address myHandler;
  Standard_Address myPrev;
  Standard_Address myNext;


};


#include <Standard_ErrorHandlerCallback.lxx>





#endif // _Standard_ErrorHandlerCallback_HeaderFile
