// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2025 OPEN CASCADE SAS
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

#ifndef _Standard_Type_HeaderFile
#define _Standard_Type_HeaderFile

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>

#include <typeinfo>
#include <typeindex>

// Auxiliary tools to check at compile time that class declared as base in
// DEFINE_STANDARD_RTTI* macro is actually a base class.
#ifndef OCCT_CHECK_BASE_CLASS
  #include <type_traits>
  #define OCCT_CHECK_BASE_CLASS(Class, Base)                                                       \
    static_assert(std::is_base_of<Base, Class>::value && !std::is_same<Base, Class>::value,        \
                  "OCCT RTTI definition is incorrect: " #Base " is not base class of " #Class);
#endif /* ! defined(OCCT_CHECK_BASE_CLASS) */

//! Helper macro to get instance of a type descriptor for a class in a legacy way.
#define STANDARD_TYPE(theType) theType::get_type_descriptor()

//! Helper macro to be included in definition of the classes inheriting
//! Standard_Transient to enable use of OCCT RTTI.
//!
//! Inline version, does not require IMPLEMENT_STANDARD_RTTIEXT, but when used
//! for big hierarchies of classes may cause considerable increase of size of binaries.
#define DEFINE_STANDARD_RTTI_INLINE(Class, Base)                                                   \
public:                                                                                            \
  typedef Base                 base_type;                                                          \
  static constexpr const char* get_type_name()                                                     \
  {                                                                                                \
    OCCT_CHECK_BASE_CLASS(Class, Base);                                                            \
    return #Class;                                                                                 \
  }                                                                                                \
  static const Handle(Standard_Type)& get_type_descriptor()                                        \
  {                                                                                                \
    static const Handle(Standard_Type) THE_TYPE_INSTANCE =                                         \
      Standard_Type::Register(typeid(Class),                                                       \
                              get_type_name(),                                                     \
                              sizeof(Class),                                                       \
                              Base::get_type_descriptor());                                        \
    return THE_TYPE_INSTANCE;                                                                      \
  }                                                                                                \
  virtual const Handle(Standard_Type)& DynamicType() const Standard_OVERRIDE                       \
  {                                                                                                \
    return get_type_descriptor();                                                                  \
  }

//! Helper macro to be included in definition of the classes inheriting
//! Standard_Transient to enable use of OCCT RTTI.
//!
//! Out-of-line version, requires IMPLEMENT_STANDARD_RTTIEXT.
#define DEFINE_STANDARD_RTTIEXT(Class, Base)                                                       \
public:                                                                                            \
  typedef Base                 base_type;                                                          \
  static constexpr const char* get_type_name()                                                     \
  {                                                                                                \
    OCCT_CHECK_BASE_CLASS(Class, Base);                                                            \
    return #Class;                                                                                 \
  }                                                                                                \
  Standard_EXPORT static const Handle(Standard_Type)&  get_type_descriptor();                      \
  Standard_EXPORT virtual const Handle(Standard_Type)& DynamicType() const Standard_OVERRIDE;

//! Defines implementation of type descriptor and DynamicType() function
#define IMPLEMENT_STANDARD_RTTIEXT(Class, Base)                                                    \
  OCCT_CHECK_BASE_CLASS(Class, Base)                                                               \
  const Handle(Standard_Type)& Class::get_type_descriptor()                                        \
  {                                                                                                \
    static const Handle(Standard_Type) THE_TYPE_INSTANCE =                                         \
      Standard_Type::Register(typeid(Class),                                                       \
                              get_type_name(),                                                     \
                              sizeof(Class),                                                       \
                              Class::base_type::get_type_descriptor());                            \
    return THE_TYPE_INSTANCE;                                                                      \
  }                                                                                                \
  const Handle(Standard_Type)& Class::DynamicType() const                                          \
  {                                                                                                \
    return STANDARD_TYPE(Class);                                                                   \
  }

//! This class provides legacy interface (type descriptor) to run-time type
//! information (RTTI) for OCCT classes inheriting from Standard_Transient.
//!
//! In addition to features provided by standard C++ RTTI (type_info),
//! Standard_Type allows passing descriptor as an object and using it for
//! analysis of the type:
//! - get descriptor of a parent class
//! - get user-defined name of the class
//! - get size of the object
//!
//! Use static template method Instance() to get descriptor for a given type.
//! Objects supporting OCCT RTTI return their type descriptor by method DynamicType().
//!
//! To be usable with OCCT type system, the class should provide:
//! - typedef base_type to its base class in the hierarchy
//! - method get_type_name() returning programmer-defined name of the class
//!   (as a statically allocated constant C string or string literal)
//!
//! Note that user-defined name is used since typeid.name() is usually mangled in
//! compiler-dependent way.
//!
//! Only single chain of inheritance is supported, with a root base class Standard_Transient.

class Standard_Type : public Standard_Transient
{
public:
  //! Returns the system type name of the class (typeinfo.name)
  Standard_CString SystemName() const { return mySystemName; }

  //! Returns the given name of the class type (get_type_name)
  Standard_CString Name() const { return myName; }

  //! Returns the size of the class instance in bytes
  Standard_Size Size() const { return mySize; }

  //! Returns descriptor of the base class in the hierarchy
  const Handle(Standard_Type)& Parent() const { return myParent; }

  //! Returns True if this type is the same as theOther, or inherits from theOther.
  //! Note that multiple inheritance is not supported.
  Standard_EXPORT Standard_Boolean SubType(const Handle(Standard_Type)& theOther) const;

  //! Returns True if this type is the same as theOther, or inherits from theOther.
  //! Note that multiple inheritance is not supported.
  Standard_EXPORT Standard_Boolean SubType(const Standard_CString theOther) const;

  //! Prints type (address of descriptor + name) to a stream
  Standard_EXPORT void Print(Standard_OStream& theStream) const;

  //! Template function returning instance of the type descriptor for an argument class.
  //!
  //! For optimization, each type is registered only once (due to use of the static variable).
  //!
  //! See helper macro DEFINE_STANDARD_RTTI for defining these items in the class.
  template <class T>
  static const Handle(Standard_Type)& Instance()
  {
    return T::get_type_descriptor();
  }

  //! Register a type; returns either new or existing descriptor.
  //!
  //! @param theInfo object stores system name of the class
  //! @param theName name of the class to be stored in Name field
  //! @param theSize size of the class instance
  //! @param theParent base class in the Transient hierarchy
  //!
  //! Note that this function is intended for use by STANDARD_RTTIEXT macros only.
  Standard_EXPORT static Standard_Type* Register(const std::type_info&        theInfo,
                                                 const char*                  theName,
                                                 Standard_Size                theSize,
                                                 const Handle(Standard_Type)& theParent);

  //! Destructor removes the type from the registry
  Standard_EXPORT ~Standard_Type();

  // Define own RTTI
  DEFINE_STANDARD_RTTIEXT(Standard_Type, Standard_Transient)

private:
  //! Constructor is private
  Standard_Type(const char*                  theSystemName,
                const char*                  theName,
                Standard_Size                theSize,
                const Handle(Standard_Type)& theParent);

private:
  Standard_CString      mySystemName; //!< System name of the class
  Standard_CString      myName;       //!< Given name of the class
  Standard_Size         mySize;       //!< Size of the class instance, in bytes
  Handle(Standard_Type) myParent;     //!< Type descriptor of parent class
};

//! Operator printing type descriptor to stream
inline Standard_OStream& operator<<(Standard_OStream&            theStream,
                                    const Handle(Standard_Type)& theType)
{
  theType->Print(theStream);
  return theStream;
}

//! Definition of Handle_Standard_Type as typedef for compatibility
DEFINE_STANDARD_HANDLE(Standard_Type, Standard_Transient)

#endif // _Standard_Type_HeaderFile
