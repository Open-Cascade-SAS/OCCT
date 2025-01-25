// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _Standard_Handle_HeaderFile
#define _Standard_Handle_HeaderFile

#include <Standard_Std.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Macro.hxx>

class Standard_Transient;

//! Namespace opencascade is intended for low-level template classes and functions
namespace opencascade
{

//! Intrusive smart pointer for use with Standard_Transient class and its descendants.
//!
//! This class is similar to boost::intrusive_ptr<>. The reference counter
//! is part of the base class (Standard_Transient), thus creation of a handle
//! does not require allocation of additional memory for the counter.
//! All handles to the same object share the common counter; object is deleted
//! when the last handle pointing on it is destroyed. It is safe to create a new
//! handle from plain C pointer to the object already pointed by another handle.
//! The same object can be referenced by handles of different types (as soon as
//! they are compatible with the object type).
//!
//! Handle has type cast operator to const reference to handle to the base
//! types, which allows it to be passed by reference in functions accepting
//! reference to handle to base class, without copying.
//!
//! By default, the type cast operator is provided also for non-const reference.
//! These casts (potentially unsafe) can be disabled by defining macro
//! OCCT_HANDLE_NOCAST; if it is defined, generalized copy constructor
//! and assignment operators are defined allowing to initialize handle
//! of base type from handle to derived type.
//!
//! Weak pointers are not supported.
template <class T>
class handle
{
public:
  //! STL-compliant typedef of contained type
  typedef T element_type;

public:
  //! Empty constructor
  handle()
      : entity(0)
  {
  }

  //! Constructor from pointer to new object
  handle(const T* thePtr)
      : entity(const_cast<T*>(thePtr))
  {
    BeginScope();
  }

  //! Copy constructor
  handle(const handle& theHandle)
      : entity(theHandle.entity)
  {
    BeginScope();
  }

  //! Move constructor
  handle(handle&& theHandle) Standard_Noexcept : entity(theHandle.entity) { theHandle.entity = 0; }

  //! Destructor
  ~handle() { EndScope(); }

  //! Nullify the handle
  void Nullify() { EndScope(); }

  //! Check for being null
  bool IsNull() const { return entity == 0; }

  //! Reset by new pointer
  void reset(T* thePtr) { Assign(thePtr); }

  //! Assignment operator
  handle& operator=(const handle& theHandle)
  {
    Assign(theHandle.entity);
    return *this;
  }

  //! Assignment to pointer
  handle& operator=(const T* thePtr)
  {
    Assign(const_cast<T*>(thePtr));
    return *this;
  }

  //! Move operator
  handle& operator=(handle&& theHandle) Standard_Noexcept
  {
    std::swap(this->entity, theHandle.entity);
    return *this;
  }

  //! STL-like cast to pointer to referred object (note non-const).
  //! @sa std::shared_ptr::get()
  T* get() const { return static_cast<T*>(this->entity); }

  //! Member access operator (note non-const)
  T* operator->() const { return static_cast<T*>(this->entity); }

  //! Dereferencing operator (note non-const)
  T& operator*() const { return *get(); }

  //! Check for equality
  template <class T2>
  bool operator==(const handle<T2>& theHandle) const
  {
    return get() == theHandle.get();
  }

  //! Check for equality
  template <class T2>
  bool operator==(const T2* thePtr) const
  {
    return get() == thePtr;
  }

  //! Check for equality
  template <class T2>
  friend bool operator==(const T2* left, const handle& right)
  {
    return left == right.get();
  }

  //! Check for inequality
  template <class T2>
  bool operator!=(const handle<T2>& theHandle) const
  {
    return get() != theHandle.get();
  }

  //! Check for inequality
  template <class T2>
  bool operator!=(const T2* thePtr) const
  {
    return get() != thePtr;
  }

  //! Check for inequality
  template <class T2>
  friend bool operator!=(const T2* left, const handle& right)
  {
    return left != right.get();
  }

  //! Compare operator for possible use in std::map<> etc.
  template <class T2>
  bool operator<(const handle<T2>& theHandle) const
  {
    return get() < theHandle.get();
  }

  //! Down casting operator from handle to base type
  template <class T2>
  static typename opencascade::std::enable_if<is_base_but_not_same<T2, T>::value, handle>::type
    DownCast(const handle<T2>& theObject)
  {
    return handle(dynamic_cast<T*>(const_cast<T2*>(theObject.get())));
  }

  //! Down casting operator from pointer to base type
  template <class T2>
  static typename opencascade::std::enable_if<is_base_but_not_same<T2, T>::value, handle>::type
    DownCast(const T2* thePtr)
  {
    return handle(dynamic_cast<T*>(const_cast<T2*>(thePtr)));
  }

  //! For compatibility, define down casting operator from non-base type, as deprecated
  template <class T2>
  Standard_DEPRECATED("down-casting from object of the same or unrelated type is meaningless")

  static handle DownCast(
    const handle<T2>& theObject,
    typename opencascade::std::enable_if<!is_base_but_not_same<T2, T>::value, void*>::type = 0)
  {
    return handle(dynamic_cast<T*>(const_cast<T2*>(theObject.get())));
  }

  //! For compatibility, define down casting operator from non-base type, as deprecated
  template <class T2>
  Standard_DEPRECATED("down-casting from object of the same or unrelated type is meaningless")

  static handle DownCast(
    const T2* thePtr,
    typename opencascade::std::enable_if<!is_base_but_not_same<T2, T>::value, void*>::type = 0)
  {
    return handle(dynamic_cast<T*>(const_cast<T2*>(thePtr)));
  }

#if (defined(__clang__)) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1300)                \
  || (defined(_MSC_VER) && _MSC_VER >= 1800)                                                       \
  || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))

  //! Conversion to bool for use in conditional expressions
  explicit operator bool() const { return entity != nullptr; }

#else /* fallback version for compilers not supporting explicit conversion operators (VC10, VC11,  \
         GCC below 4.5) */

  //! Conversion to bool-compatible type for use in conditional expressions
  operator Standard_Transient* handle::*() const { return entity ? &handle::entity : 0; }

#endif

  // Support of conversions to handle of base type:
  // - copy and move constructors and assignment operators if OCCT_HANDLE_NOCAST is defined
  // - operators of upcast to const reference to base type otherwise
#if (defined(__clang__)) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1206)                \
  || (defined(_MSC_VER) && _MSC_VER >= 1800)                                                       \
  || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)))

  #ifdef OCCT_HANDLE_NOCAST

  //! Generalized copy constructor.
  //! Constructs handle holding entity of base type (T) from the one which holds entity of derived
  //! type (T2).
  template <class T2, typename = typename std::enable_if<is_base_but_not_same<T, T2>::value>::type>
  handle(const handle<T2>& theHandle)
      : entity(theHandle.entity)
  {
    BeginScope();
  }

  //! Generalized move constructor
  template <class T2, typename = typename std::enable_if<is_base_but_not_same<T, T2>::value>::type>
  handle(handle<T2>&& theHandle)
      : entity(theHandle.entity)
  {
    theHandle.entity = 0;
  }

  //! Generalized assignment operator
  template <class T2, typename = typename std::enable_if<is_base_but_not_same<T, T2>::value>::type>
  handle operator=(const handle<T2>& theHandle)
  {
    Assign(theHandle.entity);
    return *this;
  }

  //! Generalized move operator
  template <class T2, typename = typename std::enable_if<is_base_but_not_same<T, T2>::value>::type>
  handle& operator=(handle<T2>&& theHandle)
  {
    std::swap(this->entity, theHandle.entity);
    return *this;
  }

  #else

  //! Upcast to const reference to base type.
  template <class T2, typename = typename std::enable_if<is_base_but_not_same<T2, T>::value>::type>
  operator const handle<T2>&() const
  {
    return reinterpret_cast<const handle<T2>&>(*this);
  }

  //! Upcast to non-const reference to base type.
  //! NB: this cast can be dangerous, but required for legacy code; see #26377
  template <class T2, typename = typename std::enable_if<is_base_but_not_same<T2, T>::value>::type>
  operator handle<T2>&()
  {
    return reinterpret_cast<handle<T2>&>(*this);
  }

  #endif /* OCCT_HANDLE_NOCAST */

#else /* fallback version for compilers not supporting default arguments of function templates     \
         (VC10, VC11, GCC below 4.3) */

  #ifdef OCCT_HANDLE_NOCAST

  //! Generalized copy constructor.
  //! Constructs handle holding entity of base type (T) from the one which holds entity of derived
  //! type (T2).
  template <class T2>
  handle(const handle<T2>& theHandle,
         typename std::enable_if<is_base_but_not_same<T, T2>::value>::type* = nullptr)
      : entity(theHandle.entity)
  {
    BeginScope();
  }

  //! Generalized move constructor
  template <class T2>
  handle(handle<T2>&& theHandle,
         typename std::enable_if<is_base_but_not_same<T, T2>::value>::type* = nullptr)
      : entity(theHandle.entity)
  {
    theHandle.entity = 0;
  }

  //! Generalized assignment operator.
  template <class T2>
  handle operator=(const handle<T2>& theHandle)
  {
    std::enable_if<is_base_but_not_same<T, T2>::value, void*>::type aTypeCheckHelperVar;
    (void)aTypeCheckHelperVar;
    Assign(theHandle.entity);
    return *this;
  }

  //! Generalized move operator
  template <class T2>
  handle& operator=(handle<T2>&& theHandle)
  {
    std::enable_if<is_base_but_not_same<T, T2>::value, void*>::type aTypeCheckHelperVar;
    (void)aTypeCheckHelperVar;
    std::swap(this->entity, theHandle.entity);
    return *this;
  }

  #else

  //! Upcast to const reference to base type.
  //! NB: this implementation will cause ambiguity errors on calls to overloaded
  //! functions accepting handles to different types, since compatibility is
  //! checked in the cast code rather than ensured by SFINAE (possible with C++11)
  template <class T2>
  operator const handle<T2>&() const
  {
    // error "type is not a member of enable_if" will be generated if T2 is not sub-type of T
    // (handle is being cast to const& to handle of non-base type)
    return reinterpret_cast<typename opencascade::std::enable_if<is_base_but_not_same<T2, T>::value,
                                                                 const handle<T2>&>::type>(*this);
  }

  //! Upcast to non-const reference to base type.
  //! NB: this cast can be dangerous, but required for legacy code; see #26377
  template <class T2>
  Standard_DEPRECATED("Passing non-const reference to handle of base type in function is unsafe; "
                      "use variable of exact type")

  operator handle<T2>&()
  {
    // error "type is not a member of enable_if" will be generated if T2 is not sub-type of T
    // (handle is being cast to const& to handle of non-base type)
    return reinterpret_cast<
      typename opencascade::std::enable_if<is_base_but_not_same<T2, T>::value, handle<T2>&>::type>(
      *this);
  }

  #endif /* OCCT_HANDLE_NOCAST */

#endif /* compiler switch */

private:
  //! Assignment
  void Assign(Standard_Transient* thePtr)
  {
    if (thePtr == entity)
      return;
    EndScope();
    entity = thePtr;
    BeginScope();
  }

  //! Increment reference counter of referred object
  void BeginScope()
  {
    if (entity != 0)
      entity->IncrementRefCounter();
  }

  //! Decrement reference counter and if 0, destroy referred object
  void EndScope()
  {
    if (entity != 0 && entity->DecrementRefCounter() == 0)
      entity->Delete();
    entity = 0;
  }

  template <class T2>
  friend class handle;

private:
  Standard_Transient* entity;
};

} // namespace opencascade

//! Define Handle() macro
#define Handle(Class) opencascade::handle<Class>

#include <Standard_HashUtils.hxx>

namespace std
{
template <class TheTransientType>
struct hash<Handle(TheTransientType)>
{
  size_t operator()(const Handle(TheTransientType)& theHandle) const noexcept
  {
    return static_cast<size_t>(reinterpret_cast<std::uintptr_t>(theHandle.get()));
  }
};
} // namespace std

//! For compatibility with previous versions of OCCT, define Handle_Class alias for
//! opencascade::handle<Class>.
#if (defined(_MSC_VER) && _MSC_VER >= 1800)
  //! For Visual Studio 2013+, define Handle_Class as non-template class to allow exporting this
  //! type in C++/CLI.
  #define DEFINE_STANDARD_HANDLECLASS(C1, C2, BC)                                                  \
    class C1;                                                                                      \
    class Standard_DEPRECATED(                                                                     \
      "This class will be removed right after 7.9 release. Use Handle(T) directly instead.")       \
      Handle_##C1 : public Handle(C1){                                                             \
        public : Handle_##C1(){} Handle_##C1(Handle(C1)&& theHandle) : Handle(C1)(                 \
          theHandle){} template <class T2,                                                         \
                                 typename =                                                        \
                                   typename std::enable_if<std::is_base_of<C1, T2>::value>::type>  \
        inline Handle_##C1(const opencascade::handle<T2>& theOther) : Handle(C1)(                  \
          theOther){} template <class T2,                                                          \
                                typename =                                                         \
                                  typename std::enable_if<std::is_base_of<C1, T2>::value>::type>   \
        inline Handle_##C1(const T2* theOther) : Handle(C1)(theOther){} template <typename T>      \
        inline Handle_##C1                                                                         \
        & operator=(T theOther){Handle(C1)::operator=(theOther);                                   \
    return *this;                                                                                  \
    }                                                                                              \
    }                                                                                              \
    ;
#else
  //! For other compilers, use simple typedef
  #define DEFINE_STANDARD_HANDLECLASS(C1, C2, BC)                                                  \
    class C1;                                                                                      \
    Standard_DEPRECATED("This typedef will be removed right after 7.9 release. Use Handle(T) "     \
                        "directly instead.")                                                       \
    typedef Handle(C1) Handle_##C1;
#endif

#define DEFINE_STANDARD_HANDLE(C1, C2) DEFINE_STANDARD_HANDLECLASS(C1, C2, Standard_Transient)
#define DEFINE_STANDARD_PHANDLE(C1, C2) DEFINE_STANDARD_HANDLECLASS(C1, C2, Standard_Persistent)

#endif
