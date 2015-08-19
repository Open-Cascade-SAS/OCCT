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

#include <Standard_Address.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Transient.hxx>

#include <type_traits>

class Standard_Transient;

namespace opencascade {

  //! Intrusive smart pointer for use with Standard_Transient class and its descendants.
  //!
  //! This class is similar to boost::intrusive_ptr<>, with additional
  //! feature historically supported by Handles in OCCT:
  //! it has type conversion to const reference to handle to the base types,
  //! which allows it to be passed by reference
  //! in functions accepring reference to handle to base class.
  template <class T>
  class handle
  {
  public:
    //! STL-compliant typedef of contained type
    typedef T element_type;

  public:
  
    //! Empty constructor
    handle () : entity(0) {}

    //! Constructor from pointer to new object
    handle (const T *thePtr) : entity(const_cast<T*>(thePtr))
    {
      BeginScope();
    }
/* TODO: uncomment and remove const from method above
    //! Constructor from const pointer to new object;
    //! will raise exception if object's reference counter is zero 
    explicit handle (const T *thePtr) : entity(thePtr->This()) 
    {
      BeginScope();
    }
*/
    //! Copy constructor
    handle (const handle& theHandle) : entity(theHandle.entity)
    {
      BeginScope();
    }

    //! Destructor
    ~handle ()
    {
      EndScope();
    }

    //! Nullify the handle
    void Nullify()
    {
      EndScope();
    }

    //! Check for being null
    bool IsNull() const { return entity == 0; } 

    //! Reset by new pointer
    void reset (T* thePtr)
    {
      Assign (thePtr);
    }

    //! Assignment operator
    handle& operator= (const handle& theHandle)
    {
      Assign (theHandle.entity);
      return *this;
    }

    //! Assignment to pointer
    handle& operator= (const T* thePtr)
    {
      Assign (const_cast<T*>(thePtr));
      return *this;
    }
/* uncomment along with constructor 
    //! Assignment to pointer to const object
    handle& operator= (const T* thePtr)
    {
      Assign (thePtr->This());
      return *this;
    }
*/
    //! STL-like cast to pointer to referred object
    const T* get () const { return static_cast<const T*>(this->entity); }

    //! STL-like cast to pointer to referred object
    T* get () { return static_cast<T*>(this->entity); }

    //! Member access operator (note non-const)
    T* operator-> () const { return static_cast<T*>(this->entity); }

    //! Dereferencing operator
    T& operator* () { return *get(); }

    //! Const dereferencing operator
    const T& operator*() const { return *get(); }

    //! Check for equality
    template <class T2>
    bool operator== (const handle<T2>& theHandle) const
    { 
      return get() == theHandle.get();
    }

    //! Check for equality
    template <class T2>
    bool operator== (const T2 *thePtr) const
    { 
      return get() == thePtr;
    }

    //! Check for equality
    template <class T2>
    friend bool operator== (const T2 *left, const handle& right)
    {
      return left == right.get();
    }

    //! Check for inequality
    template <class T2>
    bool operator!= (const handle<T2>& theHandle) const
    {
      return get() != theHandle.get();
    }

    //! Check for inequality
    template <class T2>
    bool operator!= (const T2 *thePtr) const
    {
      return get() != thePtr;
    }

    //! Check for inequality
    template <class T2>
    friend bool operator!= (const T2 *left, const handle& right)
    {
      return left != right.get();
    }

    //! Compare operator for possible use in std::map<> etc. 
    template <class T2>
    bool operator< (const handle<T2>& theHandle) const
    { 
      return get() < theHandle.get();
    }

    //! Down casting operator
    template <class T2>
    static handle DownCast (const handle<T2>& theObject)
    {
      return handle (dynamic_cast<T*>(const_cast<T2*>(theObject.get())));
    }

    //! Down casting operator
    template <class T2>
    static handle DownCast (const T2* thePtr)
    {
      return handle (dynamic_cast<T*>(const_cast<T2*>(thePtr)));
    }

#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800) || (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 6)

    //! Conversion to bool for use in conditional expressions
    explicit operator bool () const
    { 
      return entity != nullptr;
    }

#else /* fallback version for compilers not supporting explicit conversion operators (VC10, VC11, GCC below 4.5) */

    //! Conversion to bool-compatible type for use in conditional expressions
    operator Standard_Transient* handle::* () const
    { 
      return entity ? &handle::entity : 0;
    }

#endif

    //! Upcast to const reference to base type.
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800) || (defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 3)

    //! Upcast to const reference to base type.
    template <class T2, typename = typename std::enable_if<std::is_base_of<T2, T>::value>::type>
    operator const handle<T2>& () const
    {
      return reinterpret_cast<const handle<T2>&>(*this);
    }

    //! Upcast to non-const reference to base type.
    //! NB: this cast can be dangerous, but required for legacy code; see #26377
    template <class T2, typename = typename std::enable_if<std::is_base_of<T2, T>::value>::type>
    operator handle<T2>& ()
    {
      return reinterpret_cast<handle<T2>&>(*this);
    }

#else /* fallback version for compilers not supporting default arguments of function templates (VC10, VC11, GCC below 4.3) */

    //! Upcast to const reference to base type.
    //! NB: this implementation will cause ambiguity errors on calls to overloaded
    //! functions accepting handles to different types, since compatibility is 
    //! checked in the cast code rather than ensured by SFINAE (possible with C++11)
    template <class T2>
    operator const handle<T2>& () const
    {
      // error "type is not a member of enable_if" will be generated if T2 is not sub-type of T
      // (handle is being cast to const& to handle of non-base type)
      return reinterpret_cast<typename std::enable_if<std::is_base_of<T2, T>::value, const handle<T2>&>::type>(*this);
    }

    //! Upcast to non-const reference to base type.
    //! NB: this cast can be dangerous, but required for legacy code; see #26377
    template <class T2>
    operator handle<T2>& ()
    {
      // error "type is not a member of enable_if" will be generated if T2 is not sub-type of T
      // (handle is being cast to const& to handle of non-base type)
      return reinterpret_cast<typename std::enable_if<std::is_base_of<T2, T>::value, handle<T2>&>::type>(*this);
    }

#endif

  private:

    //! Assignment
    void Assign (Standard_Transient *thePtr)
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

    template <class T2> friend class handle;

  private:
    Standard_Transient* entity;
  };

} // namespace opencascade

//! Define Handle() macro
#define Handle(Class) opencascade::handle<Class>

//! Global method HashCode(), for use in hash maps
template <class T>
inline Standard_Integer HashCode (const Handle(T)& theHandle, const Standard_Integer theUpper)
{
  return ::HashCode (const_cast<Standard_Address>(static_cast<const void*>(theHandle.get())), theUpper);
}

//! For compatibility with previous versions of OCCT, defines typedef opencascade::handle<Class> Handle(Class)
#define DEFINE_STANDARD_HANDLECLASS(C1,C2,BC) class C1; typedef Handle(C1) Handle_##C1;
#define DEFINE_STANDARD_HANDLE(C1,C2) DEFINE_STANDARD_HANDLECLASS(C1,C2,Standard_Transient)
#define DEFINE_STANDARD_PHANDLE(C1,C2) DEFINE_STANDARD_HANDLECLASS(C1,C2,Standard_Persistent)

#endif 
