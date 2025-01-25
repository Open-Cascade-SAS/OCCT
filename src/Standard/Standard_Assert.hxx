// Created on: 2001-03-20
// Created by: Andrey BETENEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef Standard_Assert_HeaderFile
#define Standard_Assert_HeaderFile

#include <Standard_ProgramError.hxx>

//!@file
//! This header file defines a set of ASSERT macros intended for use
//! in algorithms for debugging purposes and as a tool to organise
//! checks for abnormal situations in the uniform way.
//!
//! In contrast to C assert() function that terminates the process, these
//! macros provide choice of the action to be performed if assert failed,
//! thus allowing execution to continue when possible.
//! Except for the message for developer that appears only in Debug mode,
//! the macros behave in the same way in both Release and Debug modes.
//!
//!
//! The ASSERT macros differ in the way they react on a wrong situation:
//! - Standard_ASSERT_RAISE:  raises exception Standard_ProgramError
//! - Standard_ASSERT_RETURN: returns specified value (last argument may
//!                           be left empty to return void)
//! - Standard_ASSERT_SKIP:   does nothing
//! - Standard_ASSERT_VOID:   does nothing; even does not evaluate first arg
//!                           when in Release mode
//! - Standard_ASSERT_INVOKE: causes unconditional assert
//! - Standard_ASSERT:        base macro (used by other macros);
//!                           does operation indicated in argument "todo"
//!
//! The assertion is assumed to fail if the first argument is
//! evaluated to zero (false).
//! The first argument is evaluated by all macros except Standard_ASSERT_VOID
//! which does not evaluate first argument when in Release mode.
//! The mode is triggered by preprocessor macro _DEBUG: if it is defined,
//! Debug mode is assumed, Release otherwise.
//!
//! In debug mode, if condition is not satisfied the macros call
//! Standard_ASSERT_INVOKE_ which:
//! - on Windows (under VC++), stops code execution and prompts to attach
//!   debugger to the process immediately.
//! - on POSIX systems, prints message to cerr and raises signal SIGTRAP to stop
//!   execution when under debugger (may terminate the process if not under debugger).
//!
//! The second argument (message) should be string constant ("...").
//!
//! The Standard_STATIC_ASSERT macro is to be used for compile time checks.
//! To use this macro, write:
//!
//!   Standard_STATIC_ASSERT(const_expression);
//!
//! If const_expression is false, a compiler error occurs.
//!
//! The macros are formed as functions and require semicolon at the end.

// Stub function used to make macros complete C++ operator
inline void Standard_ASSERT_DO_NOTHING() {}

// User messages are activated in debug mode only
#ifdef _DEBUG
  #if (defined(_WIN32) || defined(__WIN32__))
    #if defined(_MSC_VER) || defined(__MINGW64__)
      // VS-specific intrinsic
      #define Standard_ASSERT_DBGBREAK_() __debugbreak()
    #else
      // WinAPI function
      #include <windows.h>
      #define Standard_ASSERT_DBGBREAK_() DebugBreak()
    #endif
  #elif defined(__EMSCRIPTEN__)
    #include <emscripten.h>
    #define Standard_ASSERT_DBGBREAK_() emscripten_debugger()
  #else
    // POSIX systems
    #include <signal.h>
    #define Standard_ASSERT_DBGBREAK_() raise(SIGTRAP)
  #endif

  #if defined(_MSC_VER)
    #include <crtdbg.h>

// use debug CRT built-in function that show up message box to user
// with formatted assert description and 3 possible actions
inline Standard_Boolean Standard_ASSERT_REPORT_(const char* theFile,
                                                const int   theLine,
                                                const char* theExpr,
                                                const char* theDesc)
{
  // 1 means user pressed Retry button
  return _CrtDbgReport(_CRT_ASSERT,
                       theFile,
                       theLine,
                       NULL,
                       "%s\n(Condition: \"%s\")\n",
                       theDesc,
                       theExpr)
         == 1;
}
  #else
// just log assertion description into standard error stream
inline Standard_Boolean Standard_ASSERT_REPORT_(const char* theFile,
                                                const int   theLine,
                                                const char* theExpr,
                                                const char* theDesc)
{
  std::cerr << "ERROR: statement '" << theExpr << "' is not TRUE!\n"
            << "\nFile: '" << theFile << "'"
            << "\nLine: " << theLine << "\n";
  if (theDesc != NULL && *theDesc != '\0')
    std::cerr << "Description: " << theDesc << "\n";

  std::cerr << std::flush;
  return Standard_True;
}
  #endif

  // report issue and add debug breakpoint or abort execution
  #define Standard_ASSERT_INVOKE_(theExpr, theDesc)                                                \
    if (Standard_ASSERT_REPORT_(__FILE__, __LINE__, #theExpr, theDesc))                            \
    {                                                                                              \
      Standard_ASSERT_DBGBREAK_();                                                                 \
    }                                                                                              \
    else                                                                                           \
      Standard_ASSERT_DO_NOTHING()

  // Basic ASSERT macros
  #define Standard_ASSERT(theExpr, theDesc, theAction)                                             \
    if (!(theExpr))                                                                                \
    {                                                                                              \
      Standard_ASSERT_INVOKE_(theExpr, theDesc);                                                   \
      theAction;                                                                                   \
    }                                                                                              \
    else                                                                                           \
      Standard_ASSERT_DO_NOTHING()
  #define Standard_ASSERT_SKIP(theExpr, theDesc) Standard_ASSERT(theExpr, theDesc, )
  #define Standard_ASSERT_VOID(theExpr, theDesc) Standard_ASSERT(theExpr, theDesc, )
#else

  // dummy block
  #define Standard_ASSERT_INVOKE_(theExpr, theDesc) Standard_ASSERT_DO_NOTHING()

  // Basic ASSERT macros
  #define Standard_ASSERT(theExpr, theDesc, theAction)                                             \
    if (!(theExpr))                                                                                \
    {                                                                                              \
      theAction;                                                                                   \
    }                                                                                              \
    else                                                                                           \
      Standard_ASSERT_DO_NOTHING()
  #define Standard_ASSERT_SKIP(theExpr, theDesc) theExpr
  #define Standard_ASSERT_VOID(theExpr, theDesc) Standard_ASSERT_DO_NOTHING()

#endif

//! Raise exception (Standard_ProgramError) with the provided message
#define Standard_ASSERT_RAISE(theExpr, theDesc)                                                    \
  Standard_ASSERT(theExpr,                                                                         \
                  theDesc,                                                                         \
                  throw Standard_ProgramError("*** ERROR: ASSERT in file '" __FILE__               \
                                              "': \n" theDesc " (" #theExpr ")"))

//! Return from the current function with specified value (empty
//! if the function returns void)
#define Standard_ASSERT_RETURN(theExpr, theDesc, theReturnValue)                                   \
  Standard_ASSERT(theExpr, theDesc, return theReturnValue)

//! Raise debug message
#define Standard_ASSERT_INVOKE(theDesc) Standard_ASSERT_INVOKE_(always, theDesc)

//! Static assert --
//! empty default template
template <bool condition>
struct Standard_Static_Assert
{
};

//! Static assert -- specialization for condition being true
template <>
struct Standard_Static_Assert<true>
{
  static void assert_ok() {}
};

//! Cause compiler error if argument is not constant expression or
//! evaluates to false
#define Standard_STATIC_ASSERT(theExpr) Standard_Static_Assert<theExpr>::assert_ok();

#endif // Standard_Assert_HeaderFile

#ifdef _MSC_VER
#pragma once
#endif
