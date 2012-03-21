// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Standard_DefineHandle_HeaderFile
#define _Standard_DefineHandle_HeaderFile

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class Standard_Transient;
class Standard_Persistent;
class Handle_Standard_Type;

// COMMON
//
#define IMPLEMENT_DOWNCAST(C1,BC) \
Handle(C1) Handle(C1)::DownCast(const Handle(BC)& AnObject)  \
{ \
  Handle(C1) _anOtherObject; \
 \
  if (!AnObject.IsNull()) { \
    if (AnObject->IsKind(STANDARD_TYPE(C1))) { \
      _anOtherObject = Handle(C1)((Handle(C1)&)AnObject); \
    } \
  } \
 \
  return _anOtherObject ; \
}

#define DEFINE_STANDARD_HANDLECLASS(C1,C2,BC) \
class C1; \
Standard_EXPORT const Handle(Standard_Type)& STANDARD_TYPE(C1); \
 \
class Handle(C1) : public Handle(C2) { \
public: \
  Handle(C1)():Handle(C2)() {} \
  \
  Handle(C1)(const Handle(C1)& aHandle) : Handle(C2)(aHandle) {} \
  \
  Handle(C1)(const C1* anItem) : Handle(C2)((C2 *)anItem) {} \
  \
  Handle(C1)& operator=(const Handle(C1)& aHandle) \
  { \
    Assign(aHandle.Access()); \
    return *this; \
  } \
  \
  Handle(C1)& operator=(const C1* anItem) \
  { \
    Assign((BC *)anItem); \
    return *this; \
  } \
  \
  C1* operator->() const \
  { \
    return (C1 *)ControlAccess(); \
  } \
  \
  Standard_EXPORT static Handle(C1) DownCast(const Handle(BC)& AnObject); \
};

// TRANSIENT
//
#define DEFINE_STANDARD_HANDLE(C1,C2) DEFINE_STANDARD_HANDLECLASS(C1,C2,Standard_Transient)

#define IMPLEMENT_STANDARD_HANDLE(C1,C2) IMPLEMENT_DOWNCAST(C1,Standard_Transient)

// PERSISTENT
//
#define DEFINE_STANDARD_PHANDLE(C1,C2) DEFINE_STANDARD_HANDLECLASS(C1,C2,Standard_Persistent)

#define IMPLEMENT_STANDARD_PHANDLE(C1,C2) IMPLEMENT_DOWNCAST(C1,Standard_Persistent)

// TYPE MANAGEMENT
//
#define DEFINE_STANDARD_RTTI(C1) \
Standard_EXPORT virtual const Handle(Standard_Type)& DynamicType() const;

#define IMPLEMENT_STANDARD_RTTI(C1) \
const Handle(Standard_Type)& C1::DynamicType() const \
{ \
  return STANDARD_TYPE(C1); \
}

#define IMPLEMENT_STANDARD_TYPE(C1) \
static Handle(Standard_Type) aType##C1 = STANDARD_TYPE(C1); \
 \
const Handle(Standard_Type)& STANDARD_TYPE(C1) \
{

#define IMPLEMENT_STANDARD_SUPERTYPE(Cn) /* Left to ensure source compatibility with Open CASCADE 6.3 and earlier */

#define IMPLEMENT_STANDARD_SUPERTYPE_ARRAY() \
static Handle(Standard_Transient) _Ancestors[]= {

#define IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(Cn) \
  STANDARD_TYPE(Cn),

#define IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END() \
  NULL \
};

#define IMPLEMENT_STANDARD_TYPE_END(C1) \
static Handle(Standard_Type) _aType = new Standard_Type(#C1,sizeof(C1),1, \
                                                        (Standard_Address)_Ancestors, \
                                                        (Standard_Address)NULL); \
  return _aType; \
}

#define IMPLEMENT_STANDARD_RTTIEXT(C1,C2) \
IMPLEMENT_STANDARD_RTTI(C1) \
IMPLEMENT_STANDARD_TYPE(C1) \
IMPLEMENT_STANDARD_SUPERTYPE(C2) \
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY() \
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_ENTRY(C2) \
IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END() \
IMPLEMENT_STANDARD_TYPE_END(C1)

#endif
