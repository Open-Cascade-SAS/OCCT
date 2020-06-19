// Copyright (c) 2020 OPEN CASCADE SAS
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

#ifndef _TDF_DerivedAttribute_HeaderFile
#define _TDF_DerivedAttribute_HeaderFile

#include <NCollection_List.hxx>
#include <TDF_Attribute.hxx>

class TCollection_AsciiString;

//! @def DEFINE_DERIVED_ATTRIBUTE
//! Defines declaration of Handle method and declares methods for serialization
//! of derived attribute
#define DEFINE_DERIVED_ATTRIBUTE(Class, Base) \
  DEFINE_STANDARD_RTTIEXT(Class, Base); \
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

//! @def IMPLEMENT_DERIVED_ATTRIBUTE
//! Defines implementation of Handle method, serialization methods and registers the derived attribute
#define IMPLEMENT_DERIVED_ATTRIBUTE(Class, Base) \
  IMPLEMENT_STANDARD_RTTIEXT(Class, Base) \
  static Handle(TDF_Attribute) TDF_DERIVED_New##Class() { return new Class(); } \
  static TDF_DerivedAttribute::NewDerived TDF_DERIVED_##Class( \
    TDF_DerivedAttribute::Register(TDF_DERIVED_New##Class)); \
  Handle(TDF_Attribute) Class::NewEmpty() const { return TDF_DERIVED_##Class(); } \


//! @def IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE
//! Defines implementation of Handle method and registers the derived attribute
#define IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(Class, Base, NameSpace, TypeName) \
  IMPLEMENT_STANDARD_RTTIEXT(Class, Base) \
  static Handle(TDF_Attribute) TDF_DERIVED_New##Class() { return new Class(); } \
  static TDF_DerivedAttribute::NewDerived TDF_DERIVED_##Class( \
    TDF_DerivedAttribute::Register(TDF_DERIVED_New##Class, NameSpace, TypeName)); \
  Handle(TDF_Attribute) Class::NewEmpty() const { return TDF_DERIVED_##Class(); } \

//! Class provides global access (through static methods) to all derived attributres information.
//! It is used internally by macros for registration of derived attributes and driver-tables
//! for getting this data.
class TDF_DerivedAttribute
{
public:
  /// A function of derived attribute that returns a new attribute instance
  typedef Handle(TDF_Attribute) (*NewDerived) ();

  //! Registers a derived by the pointer to a method that creates a new derived attribute instance
  Standard_EXPORT static NewDerived Register (NewDerived       theNewAttributeFunction,
                                              Standard_CString theNameSpace = NULL,
                                              Standard_CString theTypeName  = NULL);

  //! Returns the derived registered attribute by its type.
  Standard_EXPORT static Handle(TDF_Attribute) Attribute (Standard_CString theType);

  //! Returns the type name of the registered attribute by its type.
  Standard_EXPORT static const TCollection_AsciiString& TypeName (Standard_CString theType);

  //! Returns all the derived registered attributes list.
  Standard_EXPORT static void Attributes (NCollection_List<Handle(TDF_Attribute)>& theList);

};

#endif // _TDF_DerivedAttribute_HeaderFile
