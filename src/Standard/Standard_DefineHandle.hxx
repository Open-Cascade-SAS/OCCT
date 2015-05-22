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

//! @file
//! This file provides low-level helper macros for definition of OCCT handles and types.
//! It is not to be included in the user code  directly; include Standard_Type.hxx instead.

class Standard_Transient;
class Standard_Persistent;
class Standard_Type;

// Forward declarations of auxiliary template class for type descriptor instantiation
// and down casting function; defined in Standard_Type.hxx
namespace opencascade {
  template <typename T> class type_instance;
  template <class H1, class H2>  H1 down_cast (const H2& theObject);
}

//! Helper macro to get instance of a type descriptor for a class in a legacy way.
#define STANDARD_TYPE(theType) Standard_Type::Instance<theType>()

//! Helper macro to be included in definition of the classes inheriting
//! Standard_Transient to enable use of OCCT RTTI and smart pointers (handles).
#define DEFINE_STANDARD_RTTI(Class,Base) \
public: \
  typedef Base base_type; \
  static const char* get_type_name () { return #Class; } \
  virtual const Handle(Standard_Type)& DynamicType() const { return STANDARD_TYPE(Class); }

//! Define OCCT Handle for a class C1 inheriting C2, 
//! with BC being a root of the hierarchy
#define DEFINE_STANDARD_HANDLECLASS(C1,C2,BC) \
class C1; \
class Handle(C1) : public Handle(C2) { \
public: \
  typedef C1 element_type;\
  \
  Handle(C1)() {} \
  Handle(C1)(const Handle(C1)& aHandle) : Handle(C2)(aHandle) {} \
  Handle(C1)(const C1* anItem) : Handle(C2)((C2 *)anItem) {} \
  \
  Handle(C1)& operator=(const Handle(C1)& aHandle) \
  { \
    Assign(aHandle.Access()); \
    return *this; \
  } \
  Handle(C1)& operator=(const C1* anItem) \
  { \
    Assign((BC*)anItem); \
    return *this; \
  } \
  \
  C1& operator*() const { return *(C1 *)ControlAccess(); } \
  C1* operator->() const { return (C1 *)ControlAccess(); } \
  C1* get() const { return (C1 *)ControlAccess(); } \
  \
  template <class HBC> \
  static Handle(C1) DownCast(const HBC& theObject) \
  { \
    return opencascade::down_cast <Handle(C1), HBC> (theObject); \
  } \
};

// TRANSIENT
//
#define DEFINE_STANDARD_HANDLE(C1,C2) DEFINE_STANDARD_HANDLECLASS(C1,C2,Standard_Transient)

// PERSISTENT
//
#define DEFINE_STANDARD_PHANDLE(C1,C2) DEFINE_STANDARD_HANDLECLASS(C1,C2,Standard_Persistent)

// Obsolete macros kept for compatibility
#define IMPLEMENT_DOWNCAST(C1,BC)
#define IMPLEMENT_STANDARD_HANDLE(C1,C2)
#define IMPLEMENT_STANDARD_PHANDLE(C1,C2)
#define IMPLEMENT_STANDARD_RTTI(C1)
#define IMPLEMENT_STANDARD_TYPE(C1)
#define IMPLEMENT_STANDARD_SUPERTYPE(Cn)
#define IMPLEMENT_STANDARD_SUPERTYPE_ARRAY()
#define IMPLEMENT_STANDARD_SUPERTYPE_ARRAY_END()
#define IMPLEMENT_STANDARD_TYPE_END(C1)
#define IMPLEMENT_STANDARD_RTTIEXT(C1,C2)

#endif
