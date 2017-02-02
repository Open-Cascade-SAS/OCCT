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
#include <Standard_Type.hxx>

#include <Standard_CString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>
#include <Standard_SStream.hxx>
class Standard_NoSuchObject;


class Standard_Failure;
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
  Standard_EXPORT Standard_Failure(const Standard_CString aString);

  //! Assignment operator
  Standard_EXPORT Standard_Failure& operator= (const Standard_Failure& f);
  
  //! Destructor
  Standard_EXPORT ~Standard_Failure();

  //! Prints on the stream <s> the exception name followed by
  //! the error message.
  //! Level: Advanced
  //! Warning:
  //! The operator "OStream& operator<< (Standard_OStream&,
  //! Handle(Standard_Failure)&)"
  //! is implemented. (This operator uses the method Print)
  Standard_EXPORT void Print (Standard_OStream& s) const;
  
  //! Returns error message
  Standard_EXPORT virtual Standard_CString GetMessageString() const;
  
  //! Sets error message
  Standard_EXPORT virtual void SetMessageString (const Standard_CString aMessage);
  
  Standard_EXPORT void Reraise();
  
  Standard_EXPORT void Reraise (const Standard_CString aMessage);
  
  //! Reraises a caught exception and changes its error message.
  Standard_EXPORT void Reraise (const Standard_SStream& aReason);
  
  //! Raises an exception of type "Failure" and associates
  //! an error message to it. The message can be printed
  //! in an exception handler.
  Standard_EXPORT static void Raise (const Standard_CString aMessage = "");
  
  //! Raises an exception of type "Failure" and associates
  //! an error message to it. The message can be constructed
  //! at run-time.
  Standard_EXPORT static void Raise (const Standard_SStream& aReason);
  
  //! Used to construct an instance of the exception object
  //! as a handle. Shall be used to protect against possible
  //! construction of exception object in C stack -- that is
  //! dangerous since some of methods require that object
  //! was allocated dynamically.
  Standard_EXPORT static Handle(Standard_Failure) NewInstance (const Standard_CString aMessage);
  
  //! Used to throw CASCADE exception from C signal handler.
  //! On platforms that do not allow throwing C++ exceptions
  //! from this handler (e.g. Linux), uses longjump to get to
  //! the current active signal handler, and only then is
  //! converted to C++ exception.
  Standard_EXPORT void Jump();
  
  //! Returns the last caught exception.
  //! Needed when exceptions are emulated by C longjumps,
  //! in other cases is also provided for compatibility.
  Standard_DEPRECATED("This method is deprecated (not thread-safe), use standard C++ mechanism instead")
  Standard_EXPORT static Handle(Standard_Failure) Caught();



  DEFINE_STANDARD_RTTIEXT(Standard_Failure,Standard_Transient)

protected:

  
  //! Used only if standard C++ exceptions are used.
  //! Throws exception of the same type as this by C++ throw,
  //! and stores current object as last thrown exception,
  //! to be accessible by method Caught()
  Standard_EXPORT virtual void Throw() const;



private:


  Standard_CString myMessage;


};

inline Standard_OStream& operator << (Standard_OStream& AStream,
                                      const Handle(Standard_Failure)& AFailure)
{
  AFailure->Print(AStream);
  return AStream;
}

inline Standard_OStream& operator << (Standard_OStream& AStream,
                                      const Standard_Failure& AFailure)
{
  AFailure.Print(AStream);
  return AStream;
}

#endif // _Standard_Failure_HeaderFile
