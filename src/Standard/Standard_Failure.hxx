// Created on: 1991-09-05
// Created by: Philippe COICADAN
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Standard_Failure_HeaderFile
#define _Standard_Failure_HeaderFile

#include <Standard_Type.hxx>

#include <Standard_CString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>
#include <Standard_SStream.hxx>

DEFINE_STANDARD_HANDLE(Standard_Failure, Standard_Transient)

//! Forms the root of the entire exception hierarchy.
class Standard_Failure : public Standard_Transient
{
public:

  //! Creates a status object of type "Failure".
  Standard_EXPORT Standard_Failure();

  //! Copy constructor
  Standard_EXPORT Standard_Failure (const Standard_Failure& f);

  //! Creates a status object of type "Failure".
  //! @param theDesc [in] exception description
  Standard_EXPORT Standard_Failure (const Standard_CString theDesc);

  //! Creates a status object of type "Failure" with stack trace.
  //! @param theDesc [in] exception description
  //! @param theStackTrace [in] associated stack trace
  Standard_EXPORT Standard_Failure (const Standard_CString theDesc,
                                    const Standard_CString theStackTrace);

  //! Assignment operator
  Standard_EXPORT Standard_Failure& operator= (const Standard_Failure& f);

  //! Destructor
  Standard_EXPORT ~Standard_Failure();

  //! Prints on the stream @p theStream the exception name followed by the error message.
  //!
  //! Note: there is a short-cut @c operator<< (Standard_OStream&, Handle(Standard_Failure)&)
  Standard_EXPORT void Print (Standard_OStream& theStream) const;
  
  //! Returns error message
  Standard_EXPORT virtual Standard_CString GetMessageString() const;
  
  //! Sets error message
  Standard_EXPORT virtual void SetMessageString (const Standard_CString theMessage);

  //! Returns the stack trace string
  Standard_EXPORT virtual Standard_CString GetStackString() const;

  //! Sets the stack trace string
  Standard_EXPORT virtual void SetStackString (const Standard_CString theStack);

  Standard_EXPORT void Reraise();
  
  Standard_EXPORT void Reraise (const Standard_CString aMessage);
  
  //! Reraises a caught exception and changes its error message.
  Standard_EXPORT void Reraise (const Standard_SStream& aReason);

public:

  //! Raises an exception of type "Failure" and associates
  //! an error message to it. The message can be printed
  //! in an exception handler.
  Standard_EXPORT static void Raise (const Standard_CString aMessage = "");
  
  //! Raises an exception of type "Failure" and associates
  //! an error message to it. The message can be constructed
  //! at run-time.
  Standard_EXPORT static void Raise (const Standard_SStream& aReason);
  
  //! Used to construct an instance of the exception object as a handle.
  //! Shall be used to protect against possible construction of exception object in C stack,
  //! which is dangerous since some of methods require that object was allocated dynamically.
  Standard_EXPORT static Handle(Standard_Failure) NewInstance (Standard_CString theMessage);

  //! Used to construct an instance of the exception object as a handle.
  Standard_EXPORT static Handle(Standard_Failure) NewInstance (Standard_CString theMessage,
                                                               Standard_CString theStackTrace);

  //! Returns the default length of stack trace to be captured by Standard_Failure constructor;
  //! 0 by default meaning no stack trace.
  Standard_EXPORT static Standard_Integer DefaultStackTraceLength();

  //! Sets default length of stack trace to be captured by Standard_Failure constructor.
  Standard_EXPORT static void SetDefaultStackTraceLength (Standard_Integer theNbStackTraces);

public:

  //! Used to throw CASCADE exception from C signal handler.
  //! On platforms that do not allow throwing C++ exceptions
  //! from this handler (e.g. Linux), uses longjump to get to
  //! the current active signal handler, and only then is
  //! converted to C++ exception.
  Standard_EXPORT void Jump();

  DEFINE_STANDARD_RTTIEXT(Standard_Failure,Standard_Transient)

protected:

  //! Used only if standard C++ exceptions are used.
  //! Throws exception of the same type as this by C++ throw,
  //! and stores current object as last thrown exception,
  //! to be accessible by method Caught()
  Standard_EXPORT virtual void Throw() const;

private:

  //! Reference-counted string,
  //! Memory block is allocated with an extra 4-byte header (int representing number of references)
  //! using low-level malloc() to avoid exceptions.
  struct StringRef
  {
    Standard_Integer   Counter;
    Standard_Character Message[1];

    //! Return message string.
    Standard_CString GetMessage() const { return (Standard_CString )&Message[0]; }

    //! Allocate reference-counted message string.
    static StringRef* allocate_message (Standard_CString theString);

    //! Copy reference-counted message string.
    static StringRef* copy_message (StringRef* theString);

    //! Release reference-counted message string.
    static void deallocate_message (StringRef* theString);
  };

private:

  StringRef* myMessage;
  StringRef* myStackTrace;

};

// =======================================================================
// function : operator<<
// purpose  :
// =======================================================================
inline Standard_OStream& operator<< (Standard_OStream& theStream,
                                     const Handle(Standard_Failure)& theFailure)
{
  theFailure->Print (theStream);
  return theStream;
}

// =======================================================================
// function : operator<<
// purpose  :
// =======================================================================
inline Standard_OStream& operator<< (Standard_OStream& theStream,
                                     const Standard_Failure& theFailure)
{
  theFailure.Print (theStream);
  return theStream;
}

#endif // _Standard_Failure_HeaderFile
