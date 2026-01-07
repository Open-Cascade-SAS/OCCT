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

#include <setjmp.h>
#include <variant>
#include <iostream>

// Signal exception types for variant storage
#include <OSD_SIGBUS.hxx>
#include <OSD_SIGHUP.hxx>
#include <OSD_SIGILL.hxx>
#include <OSD_SIGINT.hxx>
#include <OSD_SIGKILL.hxx>
#include <OSD_SIGQUIT.hxx>
#include <OSD_SIGSEGV.hxx>
#include <OSD_SIGSYS.hxx>
#include <OSD_Exception_ACCESS_VIOLATION.hxx>
#include <OSD_Exception_ARRAY_BOUNDS_EXCEEDED.hxx>
#include <OSD_Exception_ILLEGAL_INSTRUCTION.hxx>
#include <OSD_Exception_IN_PAGE_ERROR.hxx>
#include <OSD_Exception_INT_OVERFLOW.hxx>
#include <OSD_Exception_INVALID_DISPOSITION.hxx>
#include <OSD_Exception_NONCONTINUABLE_EXCEPTION.hxx>
#include <OSD_Exception_PRIV_INSTRUCTION.hxx>
#include <OSD_Exception_STACK_OVERFLOW.hxx>
#include <OSD_Exception_STATUS_NO_MEMORY.hxx>
#include <Standard_DivideByZero.hxx>
#include <Standard_NumericError.hxx>
#include <Standard_Overflow.hxx>
#include <Standard_ProgramError.hxx>
#include <Standard_Underflow.hxx>

//! @file
//! Support of handling of C signals as C++-style exceptions, and implementation
//! of C++ exception handling on platforms that do not implement these natively.
//!
//! The implementation is based on C long jumps.
//!
//! If macro OCC_CONVERT_SIGNALS is defined, this enables macro OCC_CATCH_SIGNALS
//! that can be used in the code (often inside try {} blocks) to convert C-style
//! signals to standard C++ exceptions. This works only when OSD::SetSignal()
//! is called to set appropriate signal handler. In the case of signal (like
//! access violation, division by zero, etc.) it will jump to the nearest
//! OCC_CATCH_SIGNALS in the call stack, which will then throw a C++ exception.
//! This method is useful for Unix and Linux systems where C++ exceptions
//! cannot be thrown from C signal handler.
//!
//! On Windows with MSVC compiler, exceptions can be thrown directly from
//! signal handler, this OCC_CONVERT_SIGNALS is not needed. Note however that
//! this requires that compiler option /EHa is used.

#if defined(OCC_CONVERT_SIGNALS)

  // Exceptions are raised as usual, signal cause jumps in the nearest
  // OCC_CATCH_SIGNALS and then thrown as exceptions.
  #define OCC_CATCH_SIGNALS                                                                        \
    Standard_ErrorHandler _aHandler;                                                               \
    if (setjmp(_aHandler.Label()))                                                                 \
    {                                                                                              \
      _aHandler.Raise();                                                                           \
    }

  // Suppress GCC warning "variable ...  might be clobbered by 'longjmp' or 'vfork'"
  #if defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__clang__)
    #pragma GCC diagnostic ignored "-Wclobbered"
  #endif

#else

  // Normal Exceptions (for example WNT with MSVC and option /GHa)
  #define OCC_CATCH_SIGNALS

#endif

//! Class implementing mechanics of conversion of signals to exceptions.
//!
//! Each instance of it stores data for jump placement,
//! and callbacks to be called during jump (for proper resource release).
//!
//! The active handlers are stored in the global stack, which is used
//! to find appropriate handler when signal is raised.
class Standard_ErrorHandler
{
public:
  //! Variant type holding all possible signal exceptions.
  //! Used to store exception across longjmp without heap allocation.
  using SignalException = std::variant<std::monostate, // Empty state (no exception)
                                       OSD_SIGBUS,
                                       OSD_SIGHUP,
                                       OSD_SIGILL,
                                       OSD_SIGINT,
                                       OSD_SIGKILL,
                                       OSD_SIGQUIT,
                                       OSD_SIGSEGV,
                                       OSD_SIGSYS,
                                       OSD_Exception_ACCESS_VIOLATION,
                                       OSD_Exception_ARRAY_BOUNDS_EXCEEDED,
                                       OSD_Exception_ILLEGAL_INSTRUCTION,
                                       OSD_Exception_IN_PAGE_ERROR,
                                       OSD_Exception_INT_OVERFLOW,
                                       OSD_Exception_INVALID_DISPOSITION,
                                       OSD_Exception_NONCONTINUABLE_EXCEPTION,
                                       OSD_Exception_PRIV_INSTRUCTION,
                                       OSD_Exception_STACK_OVERFLOW,
                                       OSD_Exception_STATUS_NO_MEMORY,
                                       Standard_DivideByZero,
                                       Standard_NumericError,
                                       Standard_Overflow,
                                       Standard_ProgramError,
                                       Standard_Underflow>;

public:
  DEFINE_STANDARD_ALLOC

  //! Create a ErrorHandler (to be used with try{}catch(){}).
  //! It uses the "setjmp" and "longjmp" routines.
  Standard_EXPORT Standard_ErrorHandler();

  //! Unlinks and checks if there is a raised exception.
  Standard_EXPORT void Destroy();

  //! Destructor
  ~Standard_ErrorHandler() { Destroy(); }

  //! Throws C++ exception if exception object set,
  //! otherwise prints error and terminates program.
  Standard_EXPORT void Raise();

  //! Returns label for jump
  jmp_buf& Label() { return myLabel; }

  //! Returns the current Error variant.
  const SignalException& Error() const { return myCaughtError; }

  //! Test if the code is currently running in a try block
  Standard_EXPORT static bool IsInTryBlock();

  //! Abort with specific exception type.
  //! Finds nearest error handler, stores exception, and performs longjmp.
  //! @tparam T Exception type (must be one of Standard_SignalException variant types)
  //! @param theError Exception to store and throw after longjmp
  template <typename T>
  static void Abort(const T& theError);

private:
  //! Removes handler from the list.
  void Unlink();

  //! Returns the current handler (closest in the stack in the current execution thread)
  Standard_EXPORT static Standard_ErrorHandler* FindHandler();

public:
  //! Defines a base class for callback objects that can be registered
  //! in the OCC error handler (the class simulating C++ exceptions)
  //! so as to be correctly destroyed when error handler is activated.
  //!
  //! Note that this is needed only when Open CASCADE is compiled with
  //! OCC_CONVERT_SIGNALS options (i.e. on UNIX/Linux).
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
  class Callback
  {
  public:
    DEFINE_STANDARD_ALLOC

//! Registers this callback object in the current error handler (if found).
#if defined(OCC_CONVERT_SIGNALS)
    Standard_EXPORT
#endif
      void
      RegisterCallback();

//! Unregisters this callback object from the error handler.
#if defined(OCC_CONVERT_SIGNALS)
    Standard_EXPORT
#endif
      void
      UnregisterCallback();

//! Destructor
#if defined(OCC_CONVERT_SIGNALS)
    Standard_EXPORT
#endif
      virtual ~Callback();

    //! The callback function to perform necessary callback action.
    //! Called by the exception handler when it is being destroyed but
    //! still has this callback registered.
    Standard_EXPORT virtual void DestroyCallback() = 0;

  protected:
//! Empty constructor
#if defined(OCC_CONVERT_SIGNALS)
    Standard_EXPORT
#endif
      Callback();

  private:
    void* myHandler = nullptr;
    void* myPrev    = nullptr;
    void* myNext    = nullptr;

    friend class Standard_ErrorHandler;
  };

private:
  SignalException        myCaughtError;
  Standard_ErrorHandler* myPrevious    = nullptr;
  Callback*              myCallbackPtr = nullptr;
  jmp_buf                myLabel       = {};
};

//! Template implementation of Abort - stores exception and performs longjmp.
template <typename T>
void Standard_ErrorHandler::Abort(const T& theError)
{
#ifndef OCC_CONVERT_SIGNALS
  throw theError;
#else
  Standard_ErrorHandler* anActive = FindHandler();
  if (anActive == nullptr)
  {
    std::cerr << "*** Abort *** an exception was raised, but no catch was found." << std::endl;
    std::cerr << "\t... The exception is: " << theError.what() << std::endl;
    exit(1);
  }
  anActive->myCaughtError = theError;
  longjmp(anActive->myLabel, true);
#endif
}

// If OCC_CONVERT_SIGNALS is not defined,
// provide empty inline implementation
#if !defined(OCC_CONVERT_SIGNALS)
inline Standard_ErrorHandler::Callback::Callback() {}

inline Standard_ErrorHandler::Callback::~Callback() {}

inline void Standard_ErrorHandler::Callback::RegisterCallback() {}

inline void Standard_ErrorHandler::Callback::UnregisterCallback() {}
#endif

// Definition of the old name "Standard_ErrorHandlerCallback" was kept for compatibility
typedef Standard_ErrorHandler::Callback Standard_ErrorHandlerCallback;

#endif // _Standard_ErrorHandler_HeaderFile
