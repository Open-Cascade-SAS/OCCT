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

#include <Standard_Macro.hxx>
#include <Standard_DefineHandle.hxx>

//! Defines an exception class \a C1 that inherits an exception class \a C2.
/*! \a C2 must be Standard_Failure or its ancestor.
    The macro defines empty constructor, copy constructor and static methods Raise() and NewInstance().
    Since Standard_Failure implements class manipulated by handle, DEFINE_STANDARD_RTTI macro is also
    added to enable RTTI.

    When using DEFINE_STANDARD_EXCEPTION in your code make sure you also insert a macro
    DEFINE_STANDARD_HANDLE(C1,C2) before it.

    \sa IMPLEMENT_STANDARD_EXCEPTION.
*/
#define DEFINE_STANDARD_EXCEPTION(C1,C2) \
 \
class C1 : public C2 { \
  Standard_EXPORT virtual void Throw() const; \
public: \
  C1() : C2() {} \
  C1(const Standard_CString AString) : C2(AString) {} \
  Standard_EXPORT static void Raise(const Standard_CString aMessage = ""); \
  Standard_EXPORT static void Raise(Standard_SStream& aReason); \
  Standard_EXPORT static Handle(C1) NewInstance(const Standard_CString aMessage = ""); \
 \
  DEFINE_STANDARD_RTTI(C1) \
};


//! Implements an exception class \a C1 declared with DEFINE_STANDARD_EXCEPTION macro.
/*! If you are using IMPLEMENT_STANDARD_EXCEPTION in your code make sure you also call
    IMPLEMENT_STANDARD_HANDLE(C1,C2) and IMPLEMENT_STANDARD_RTTIEXT(C1,C2).
*/
#define IMPLEMENT_STANDARD_EXCEPTION(C1) \
 \
void C1::Raise(Standard_SStream& aReason) \
{ \
  Handle(C1) _E = new C1; \
  _E->Reraise (aReason); \
} \
 \
void C1::Raise(const Standard_CString AString) \
{ \
  Handle(C1) _E = new C1; \
  _E->Reraise(AString); \
} \
 \
Handle(C1) C1::NewInstance(const Standard_CString aMessage) \
{ \
  return new C1(aMessage); \
} \
void C1::Throw () const \
{ \
  throw *this; \
}

#endif
