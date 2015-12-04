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

#ifndef _Standard_DefineException_HeaderFile
#define _Standard_DefineException_HeaderFile

#include <Standard_Type.hxx>

//! Defines an exception class \a C1 that inherits an exception class \a C2.
/*! \a C2 must be Standard_Failure or its ancestor.
    The macro defines empty constructor, copy constructor and static methods Raise() and NewInstance().
    Since Standard_Failure implements class manipulated by handle, DEFINE_STANDARD_RTTI macro is also
    added to enable RTTI.

    When using DEFINE_STANDARD_EXCEPTION in your code make sure you also insert a macro
    DEFINE_STANDARD_HANDLE(C1,C2) before it.
*/

#define DEFINE_STANDARD_EXCEPTION(C1,C2) \
 \
class C1 : public C2 { \
  void Throw () const Standard_OVERRIDE { throw *this; } \
public: \
  C1() : C2() {} \
  C1(const Standard_CString theMessage) : C2(theMessage) {} \
  static void Raise(const Standard_CString theMessage = "") { \
    Handle(C1) _E = new C1; \
    _E->Reraise(theMessage); \
  } \
  static void Raise(Standard_SStream& theMessage) { \
    Handle(C1) _E = new C1; \
    _E->Reraise (theMessage); \
  } \
  static Handle(C1) NewInstance(const Standard_CString theMessage = "") { return new C1(theMessage); } \
  DEFINE_STANDARD_RTTI_INLINE(C1,C2) \
};

//! Obsolete macro, kept for compatibility with old code
#define IMPLEMENT_STANDARD_EXCEPTION(C1) 

#endif
