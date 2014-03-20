// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Standard_DefineHandle_HeaderFile
#define _Standard_DefineHandle_HeaderFile

#include <Standard_Macro.hxx>

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
  typedef C1 element_type;\
  \
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
  C1& operator*() const \
  { \
    return *(C1 *)ControlAccess(); \
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
