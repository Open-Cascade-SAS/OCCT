// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2023 OPEN CASCADE SAS
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

#ifndef _Standard_Transient_HeaderFile
#define _Standard_Transient_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <stddef.h>
#include <stdlib.h>
#include <Standard_Macro.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Character.hxx>
#include <Standard_ExtCharacter.hxx>
#include <Standard_CString.hxx>

#include <atomic>

class Standard_Type;

namespace opencascade
{
template <class T>
class handle;
}

//! Abstract class which forms the root of the entire
//! Transient class hierarchy.

class Standard_Transient
{
public:
  // Standard OCCT memory allocation stuff
  DEFINE_STANDARD_ALLOC

public:
  //! Empty constructor
  Standard_Transient()
      : myRefCount_(0)
  {
  }

  //! Copy constructor -- does nothing
  Standard_Transient(const Standard_Transient&)
      : myRefCount_(0)
  {
  }

  //! Assignment operator, needed to avoid copying reference counter
  Standard_Transient& operator=(const Standard_Transient&) { return *this; }

  //! Destructor must be virtual
  virtual ~Standard_Transient() = default;

public:
  //!@name Support of run-time type information (RTTI)

  typedef void base_type;

  static constexpr const char* get_type_name() { return "Standard_Transient"; }

  //! Returns type descriptor of Standard_Transient class
  Standard_EXPORT static const opencascade::handle<Standard_Type>& get_type_descriptor();

  //! Returns a type descriptor about this object.
  Standard_EXPORT virtual const opencascade::handle<Standard_Type>& DynamicType() const;

  //! Returns a true value if this is an instance of Type.
  Standard_EXPORT bool IsInstance(const opencascade::handle<Standard_Type>& theType) const;

  //! Returns a true value if this is an instance of TypeName.
  Standard_EXPORT bool IsInstance(const char* const theTypeName) const;

  //! Returns true if this is an instance of Type or an
  //! instance of any class that inherits from Type.
  //! Note that multiple inheritance is not supported by OCCT RTTI mechanism.
  Standard_EXPORT bool IsKind(const opencascade::handle<Standard_Type>& theType) const;

  //! Returns true if this is an instance of TypeName or an
  //! instance of any class that inherits from TypeName.
  //! Note that multiple inheritance is not supported by OCCT RTTI mechanism.
  Standard_EXPORT bool IsKind(const char* const theTypeName) const;

  //! Returns non-const pointer to this object (like const_cast).
  //! For protection against creating handle to objects allocated in stack
  //! or call from constructor, it will raise exception Standard_ProgramError
  //! if reference counter is zero.
  Standard_EXPORT Standard_Transient* This() const;

public:
  //!@name Reference counting, for use by handle<>

  //! Get the reference counter of this object
  inline int GetRefCount() const noexcept { return myRefCount_.load(std::memory_order_relaxed); }

  //! Increments the reference counter of this object.
  //! Uses relaxed memory ordering since incrementing only requires atomicity,
  //! not synchronization with other memory operations.
  inline void IncrementRefCounter() noexcept
  {
    myRefCount_.fetch_add(1, std::memory_order_relaxed);
  }

  //! Decrements the reference counter of this object;
  //! returns the decremented value.
  //! Uses release ordering for the decrement to ensure all writes to the object
  //! are visible before the count reaches zero. An acquire fence is added only
  //! when the count reaches zero, ensuring proper synchronization before deletion.
  //! This is more efficient than using acq_rel for every decrement.
  inline int DecrementRefCounter() noexcept
  {
    const int aResult = myRefCount_.fetch_sub(1, std::memory_order_release) - 1;
    if (aResult == 0)
    {
      std::atomic_thread_fence(std::memory_order_acquire);
    }
    return aResult;
  }

  //! Memory deallocator for transient classes
  virtual void Delete() const { delete this; }

private:
  //! Reference counter.
  //! Note use of underscore, aimed to reduce probability
  //! of conflict with names of members of derived classes.
  std::atomic_int myRefCount_;
};

//! Definition of opencascade::handle<Standard_Transient> as typedef for compatibility

#endif
