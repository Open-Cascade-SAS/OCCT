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
