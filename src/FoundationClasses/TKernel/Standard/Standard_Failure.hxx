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

#include <Standard.hxx>
#include <Standard_OStream.hxx>
#include <Standard_SStream.hxx>

#include <exception>

//! Forms the root of the entire exception hierarchy.
//! Inherits from std::exception and implements what() interface.
class Standard_Failure : public std::exception
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a status object of type "Failure".
  Standard_EXPORT Standard_Failure();

  //! Copy constructor
  Standard_EXPORT Standard_Failure(const Standard_Failure& theOther);

  //! Creates a status object of type "Failure" with message.
  //! @param[in] theMessage exception description
  Standard_EXPORT Standard_Failure(const char* theMessage);

  //! Creates a status object of type "Failure" with message and stack trace.
  //! @param[in] theMessage    exception description
  //! @param[in] theStackTrace stack trace string
  Standard_EXPORT Standard_Failure(const char* const theMessage, const char* const theStackTrace);

  //! Assignment operator
  Standard_EXPORT Standard_Failure& operator=(const Standard_Failure& theOther);

  //! Destructor
  Standard_EXPORT ~Standard_Failure() override;

  //! Returns error message (implements std::exception interface).
  //! Returns empty string "" if no message was set.
  Standard_EXPORT const char* what() const noexcept override;

  //! Returns error message
  Standard_DEPRECATED("Use what() instead")
  const char* GetMessageString() const noexcept { return what(); }

  //! Returns the exception type name.
  //! Default implementation returns "Standard_Failure".
  //! Derived classes override this to return their own type name.
  virtual const char* ExceptionType() const noexcept { return "Standard_Failure"; }

  //! Returns the stack trace string (empty string if not available).
  Standard_EXPORT const char* GetStackString() const;

  //! Prints on the stream @p theStream the exception name followed by the error message.
  //!
  //! Note: there is a short-cut @c operator<< (Standard_OStream&, const Standard_Failure&)
  Standard_EXPORT void Print(Standard_OStream& theStream) const;

public:
  //! Returns the default length of stack trace to be captured by Standard_Failure constructor;
  //! 0 by default meaning no stack trace.
  Standard_EXPORT static int DefaultStackTraceLength();

  //! Sets default length of stack trace to be captured by Standard_Failure constructor.
  Standard_EXPORT static void SetDefaultStackTraceLength(int theNbStackTraces);

private:
  //! Reference-counted string using malloc/free for exception safety.
  //! Memory block has a 4-byte header (int for reference count).
  struct StringRef
  {
    int  Counter;
    char Message[1];

    //! Return message string.
    const char* GetMessage() const { return &Message[0]; }

    //! Allocate reference-counted message string.
    static StringRef* Allocate(const char* theString);

    //! Copy reference-counted message string (increments counter).
    static StringRef* Copy(StringRef* theString);

    //! Release reference-counted message string.
    static void Free(StringRef* theString);
  };

  //! Captures stack trace if configured.
  void captureStackTrace();

private:
  StringRef* myMessage;    //!< Exception message
  StringRef* myStackTrace; //!< Stack trace (optional)
};

//=================================================================================================

inline Standard_OStream& operator<<(Standard_OStream& theStream, const Standard_Failure& theFailure)
{
  theFailure.Print(theStream);
  return theStream;
}

#endif // _Standard_Failure_HeaderFile
