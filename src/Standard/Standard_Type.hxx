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

#ifndef _Standard_Type_HeaderFile
#define _Standard_Type_HeaderFile

#include <Standard.hxx>
#include <Standard_Handle.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>

#include <typeinfo>

// Auxiliary tools to check at compile time that class declared as base in 
// DEFINE_STANDARD_RTTI* macro is actually a base class.
#if ! defined(OCCT_CHECK_BASE_CLASS)

#if (defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 7) || (__GNUC__ > 4)))

// For GCC 4.7+, more strict check is possible -- ensuring that base class 
// is direct base -- using non-standard C++ reflection functionality.

#include <tr2/type_traits>
#include <tuple>

namespace opencascade 
{
  template<typename T>
  struct direct_base_class_as_tuple {};

  template<typename ... Ts>
  struct direct_base_class_as_tuple<std::tr2::__reflection_typelist<Ts...> >
  {
    typedef std::tuple<Ts...> type;
  };

  template <typename T, typename Tuple>
  struct has_type;

  template <typename T>
  struct has_type<T, std::tuple<> > : std::false_type {};

  template <typename T, typename U, typename... Ts>
  struct has_type<T, std::tuple<U, Ts...> > : has_type<T, std::tuple<Ts...> > {};

  template <typename T, typename... Ts>
  struct has_type<T, std::tuple<T, Ts...> > : std::true_type {};
}

#define OCCT_CHECK_BASE_CLASS(Class,Base) \
  using direct_base_classes = opencascade::direct_base_class_as_tuple<std::tr2::direct_bases<Class>::type>::type; \
  static_assert(opencascade::has_type<Base, direct_base_classes>::type::value, "OCCT RTTI definition is incorrect: " #Base " is not direct base class of " #Class); \
  static_assert(&get_type_name == &Class::get_type_name, "OCCT RTTI definition is misplaced: current class is not " #Class);

#elif (defined(_MSC_VER) && (_MSC_VER < 1600))

// VC9 does not support static_assert and decltype at all
#define OCCT_CHECK_BASE_CLASS(Class,Base)

#elif (defined(_MSC_VER) && (_MSC_VER >= 1900))

// VC14+ allow using address of member functions in static checks,
// that allows checking for the current type being correctly named in the macro
#define OCCT_CHECK_BASE_CLASS(Class,Base) \
  static_assert(opencascade::is_base_but_not_same<Base, Class>::value, "OCCT RTTI definition is incorrect: " #Base " is not base class of " #Class); \
  static_assert(&get_type_name == &Class::get_type_name, "OCCT RTTI definition is misplaced: current class is not " #Class);

#else

// by default, check only the base class
#define OCCT_CHECK_BASE_CLASS(Class,Base) \
  static_assert(opencascade::is_base_but_not_same<Base, Class>::value, "OCCT RTTI definition is incorrect: " #Base " is not base class of " #Class);

#endif

#endif /* ! defined(OCCT_CHECK_BASE_CLASS) */

//! Helper macro to get instance of a type descriptor for a class in a legacy way.
#define STANDARD_TYPE(theType) theType::get_type_descriptor()

//! Helper macro to be included in definition of the classes inheriting
//! Standard_Transient to enable use of OCCT RTTI.
//!
//! Inline version, does not require IMPLEMENT_STANDARD_RTTIEXT, but when used
//! for big hierarchies of classes may cause considerable increase of size of binaries.
#define DEFINE_STANDARD_RTTI_INLINE(Class,Base) \
public: \
  typedef Base base_type; \
  static const char* get_type_name () { return #Class; OCCT_CHECK_BASE_CLASS(Class,Base) } \
  static const Handle(Standard_Type)& get_type_descriptor () { return Standard_Type::Instance<Class>(); } \
  virtual const Handle(Standard_Type)& DynamicType() const Standard_OVERRIDE { return get_type_descriptor (); }

//! Helper macro to be included in definition of the classes inheriting
//! Standard_Transient to enable use of OCCT RTTI.
//!
//! Out-of-line version, requires IMPLEMENT_STANDARD_RTTIEXT.
#define DEFINE_STANDARD_RTTIEXT(Class,Base) \
public: \
  typedef Base base_type; \
  static const char* get_type_name () { return #Class; OCCT_CHECK_BASE_CLASS(Class,Base) } \
  Standard_EXPORT static const Handle(Standard_Type)& get_type_descriptor (); \
  Standard_EXPORT virtual const Handle(Standard_Type)& DynamicType() const Standard_OVERRIDE;

//! Defines implementation of type descriptor and DynamicType() function
#define IMPLEMENT_STANDARD_RTTIEXT(Class,Base) \
  const Handle(Standard_Type)& Class::get_type_descriptor () { return Standard_Type::Instance<Class>(); } \
  const Handle(Standard_Type)& Class::DynamicType() const { return STANDARD_TYPE(Class); }

// forward declaration of type_instance class
namespace opencascade {
  template <typename T>
  class type_instance;
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
  const Handle(Standard_Type)& Parent () const { return myParent; }
  
  //! Returns True if this type is the same as theOther, or inherits from theOther.
  //! Note that multiple inheritance is not supported.
  Standard_EXPORT Standard_Boolean SubType (const Handle(Standard_Type)& theOther) const;

  //! Returns True if this type is the same as theOther, or inherits from theOther.
  //! Note that multiple inheritance is not supported.
  Standard_EXPORT Standard_Boolean SubType (const Standard_CString theOther) const;

  //! Prints type (address of descriptor + name) to a stream
  Standard_EXPORT void Print (Standard_OStream& theStream) const;

  //! Template function returning instance of the type descriptor for an argument class.
  //!
  //! For optimization, each type is registered only once (due to use of the static variable).
  //!
  //! See helper macro DEFINE_STANDARD_RTTI for defining these items in the class.
  template <class T>
  static const Handle(Standard_Type)& Instance()
  {
    return opencascade::type_instance<T>::get();
  }

  //! Register a type; returns either new or existing descriptor.
  //!
  //! @param theSystemName name of the class as returned by typeid(class).name()
  //! @param theName name of the class to be stored in Name field
  //! @param theSize size of the class instance
  //! @param theParent base class in the Transient hierarchy
  //!
  //! Note that this function is intended for use by opencascade::type_instance only. 
  Standard_EXPORT static 
    Standard_Type* Register (const char* theSystemName, const char* theName,
                             Standard_Size theSize, const Handle(Standard_Type)& theParent);

  //! Destructor removes the type from the registry
  Standard_EXPORT ~Standard_Type ();

  // Define own RTTI
  DEFINE_STANDARD_RTTIEXT(Standard_Type,Standard_Transient)

private:

  //! Constructor is private
  Standard_Type (const char* theSystemName, const char* theName,
                 Standard_Size theSize, const Handle(Standard_Type)& theParent);

private:
  Standard_CString mySystemName;  //!< System name of the class (typeinfo.name)
  Standard_CString myName;        //!< Given name of the class
  Standard_Size mySize;           //!< Size of the class instance, in bytes
  Handle(Standard_Type) myParent; //!< Type descriptor of parent class
};

namespace opencascade {

  //! Template class providing instantiation of type descriptors as singletons.
  //! The descriptors are defined as static variables in function get(), which
  //! is essential to ensure that they are initialized in correct sequence.
  //!
  //! For compilers that do not provide thread-safe initialization of static
  //! variables (C++11 feature, N2660), additional global variable is
  //! defined for each type to hold its type descriptor. These globals ensure
  //! that all types get initialized during the library loading and thus no 
  //! concurrency occurs when type system is accessed from multiple threads.
  template <typename T>
  class type_instance
  {
    static Handle(Standard_Type) myInstance;
  public:
    static const Handle(Standard_Type)& get ();
  };

  //! Specialization of type descriptor instance for void; returns null handle
  template <>
  class type_instance<void>
  {
  public:
    static Handle(Standard_Type) get () { return 0; }
  };

  // Implementation of static function returning instance of the
  // type descriptor
  template <typename T>
  const Handle(Standard_Type)& type_instance<T>::get ()
  {
#if (defined(_MSC_VER) && _MSC_VER < 1900) || \
    (defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)) && \
     ! defined(__clang__) && ! defined(__INTEL_COMPILER))
    // ensure that myInstance is instantiated
    (void)myInstance;
#endif

    // static variable inside function ensures that descriptors
    // are initialized in correct sequence
    static Handle(Standard_Type) anInstance =
      Standard_Type::Register (typeid(T).name(), T::get_type_name(), sizeof(T), 
                               type_instance<typename T::base_type>::get());
    return anInstance;
  }

  // Static class field is defined to ensure initialization of all type
  // descriptors at load time of the library on compilers not supporting N2660:
  // - VC++ below 14 (VS 2015)
  // - GCC below 4.3
  // Intel compiler reports itself as GCC on Linux and VC++ on Windows,
  // and is claimed to support N2660 on Linux and on Windows "in VS2015 mode".
  // CLang should support N2660 since version 2.9, but it is not clear how to 
  // check its version reliably (on Linux it says it is GCC 4.2).
#if (defined(_MSC_VER) && _MSC_VER < 1900) || \
    (defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)) && \
     ! defined(__clang__) && ! defined(__INTEL_COMPILER))

  template <typename T>
  Handle(Standard_Type) type_instance<T>::myInstance (get());

#endif
}

//! Operator printing type descriptor to stream
inline Standard_OStream& operator << (Standard_OStream& theStream, const Handle(Standard_Type)& theType) 
{
  theType->Print (theStream);
  return theStream;
}

//! Definition of Handle_Standard_Type as typedef for compatibility
DEFINE_STANDARD_HANDLE(Standard_Type,Standard_Transient)

#endif // _Standard_Type_HeaderFile
