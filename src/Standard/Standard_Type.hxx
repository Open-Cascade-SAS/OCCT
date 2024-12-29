// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2024 OPEN CASCADE SAS
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
#include <Standard_OStream.hxx>
#include <Standard_Transient.hxx>

#include <functional>
#include <typeinfo>
#include <typeindex>

namespace opencascade
{
  template <class T> class handle;
}

class Standard_Transient;

class Standard_Type
{
public:

  Standard_Type(const std::type_info& theInfo,
                const Standard_CString theName,
                const Standard_Size theSize,
                const Standard_Type* theParent) :
    myInfo(theInfo),
    myName(theName),
    mySize(theSize),
    myParent(theParent) {}

  Standard_Type(const Standard_Type& theType) :
    myInfo(theType.myInfo),
    myName(theType.myName),
    mySize(theType.mySize),
    myParent(theType.myParent) {}

  Standard_Type(Standard_Type&& theType) noexcept :
    myInfo(theType.myInfo),
    myName(theType.myName),
    mySize(theType.mySize),
    myParent(theType.myParent) {}

  Standard_Type& operator=(const Standard_Type& theOther)
  {
    myInfo = theOther.myInfo;
    myName = theOther.myName;
    mySize = theOther.mySize;
    myParent = theOther.myParent;
    return *this;
  }

  //! Returns the system type name of the class (typeinfo.name)
  Standard_CString SystemName() const { return myInfo.get().name(); }

  //! Returns the hash code of the class (typeinfo.hash_code)
  Standard_Size HashCode() const { return myInfo.get().hash_code(); }

  //! Returns the given name of the class type (get_type_name)
  Standard_CString Name() const { return myName; }

  //! Returns the size of the class instance in bytes
  Standard_Size Size() const { return mySize; }

  //! Returns descriptor of the base class in the hierarchy. Can be null
  const Standard_Type* Parent() const { return myParent; }

  bool operator==(const Standard_Type& theType) const { return myInfo.get() == theType.myInfo.get(); }

  bool operator==(const std::type_info& theInfo) const { return myInfo.get() == theInfo; }

  //! Returns True if this type is the same as theOther, or inherits from theOther.
  //! Note that multiple inheritance is not supported.
  Standard_EXPORT Standard_Boolean SubType (const Handle(Standard_Type)& theOther) const;

  //! Returns True if this type is the same as theOther, or inherits from theOther.
  //! Note that multiple inheritance is not supported.
  Standard_EXPORT Standard_Boolean SubType (const Standard_CString theOther) const;

  //! Prints type (address of descriptor + name) to a stream
  Standard_EXPORT void Print (Standard_OStream& theStream) const;

  template<class T>
  static const opencascade::handle<Standard_Type>& Instance()
  {
    return T::get_type_descriptor();
  }

private:
  std::reference_wrapper<const std::type_info> myInfo; //!< STL Type descriptor of current class
  Standard_CString myName; //!< Given name of the class
  Standard_Size mySize;    //!< Size of the class instance, in bytes
  const Standard_Type* myParent; //!< Type descriptor of parent class
};

namespace opencascade
{
  template <>
  class handle<Standard_Type>
  {
  public:
    //! STL-compliant typedef of contained type
    typedef Standard_Type element_type;
  public:
    //! Empty constructor
    handle() : myObject(nullptr) {}
    //! Constructor from pointer to new object
    handle (const Standard_Type* theType) : myObject(theType) {}
    //! Copy constructor
    handle (const handle& theHandle) : myObject(theHandle.myObject) {}
    //! Assignment operator
    handle& operator= (const handle& theHandle)
    {
      myObject = theHandle.myObject;
      return *this;
    }
  
    bool IsNull() const { return myObject == nullptr; }
  
    bool operator== (const handle& theHandle) const
    {
      return myObject == theHandle.myObject;
    }
  
    bool operator!= (const handle& theHandle) const
    {
      return !(myObject == theHandle.myObject);
    }

    //! STL-like cast to pointer to referred object (note non-const).
    //! @sa std::shared_ptr::get()
    const Standard_Type* get() const { return myObject; }
    //! Member access operator (note non-const)
    const Standard_Type* operator-> () const { return myObject; }
    //! Dereferencing operator (note non-const)
    const Standard_Type& operator* () const { return *myObject; }
  private:
    const Standard_Type* myObject;
  };
}

//! Operator printing type descriptor to stream
inline Standard_OStream& operator << (Standard_OStream& theStream, const opencascade::handle<Standard_Type>& theType) 
{
  theStream << theType->Name();
  return theStream;
}

// by default, check only the base class
#define OCCT_CHECK_BASE_CLASS(Class,Base) \
  static_assert(opencascade::is_base_but_not_same<Base, Class>::value, "OCCT RTTI definition is incorrect: " #Base " is not base class of " #Class);

namespace std
{
  template <>
  struct hash<Standard_Type>
  {
    size_t operator()(const Standard_Type& theType) const noexcept
    {
      return theType.HashCode();
    }
  };
}

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
  static const char* get_type_name() { return #Class; } \
  static const Handle(Standard_Type)& get_type_descriptor() \
  { \
    static const Handle(Standard_Type) THE_TYPE_INSTANCE(new Standard_Type(typeid(Class), get_type_name(), \
                                                         sizeof(Class), Base::get_type_descriptor().get())); \
    return THE_TYPE_INSTANCE; \
  } \
  virtual const Handle(Standard_Type)& DynamicType() const Standard_OVERRIDE { return get_type_descriptor (); }

//! Helper macro to be included in definition of the classes inheriting
//! Standard_Transient to enable use of OCCT RTTI.
//!
//! Out-of-line version, requires IMPLEMENT_STANDARD_RTTIEXT.
#define DEFINE_STANDARD_RTTIEXT(Class,Base) \
public: \
  typedef Base base_type; \
  static const char* get_type_name() { return #Class; } \
  Standard_EXPORT static const Handle(Standard_Type)& get_type_descriptor(); \
  Standard_EXPORT virtual const Handle(Standard_Type)& DynamicType() const Standard_OVERRIDE;

//! Defines implementation of type descriptor and DynamicType() function
#define IMPLEMENT_STANDARD_RTTIEXT(Class,Base) \
  const Handle(Standard_Type)& Class::get_type_descriptor() \
  { \
    static const Handle(Standard_Type) THE_TYPE_INSTANCE(new Standard_Type(typeid(Class), get_type_name(), \
                                                         sizeof(Class), Base::get_type_descriptor().get())); \
    return THE_TYPE_INSTANCE; \
  } \
  const Handle(Standard_Type)& Class::DynamicType() const { return get_type_descriptor(); }

#endif // _Standard_Type_HeaderFile
