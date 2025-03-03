// Copyright (c) 2023 OPEN CASCADE SAS
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

#ifndef _Standard_MemoryUtils_HeaderFile
#define _Standard_MemoryUtils_HeaderFile

#include <NCollection_Allocator.hxx>
#include <NCollection_OccAllocator.hxx>

#include <memory>
#include <mutex>

namespace opencascade
{
//! Makes shared pointer with OCCT allocator
template <class T, class... Args>
std::shared_ptr<T> make_shared(Args&&... theArgs)
{
  return std::allocate_shared<T, NCollection_Allocator<T>, Args...>(NCollection_Allocator<T>(),
                                                                    std::forward<Args>(theArgs)...);
}

//! Makes shared pointer with OCCT allocator
template <class T, class... Args>
std::shared_ptr<T> make_oshared(const Handle(NCollection_BaseAllocator)& theAlloc,
                                Args&&... theArgs)
{
  return std::allocate_shared<T, NCollection_OccAllocator<T>, Args...>(
    NCollection_OccAllocator<T>(theAlloc),
    std::forward<Args>(theArgs)...);
}

//! Makes shared pointer with OCCT allocator
template <class T, class... Args>
std::shared_ptr<T> make_oshared(const NCollection_OccAllocator<T>& theAlloc, Args&&... theArgs)
{
  return std::allocate_shared<T, NCollection_OccAllocator<T>, Args...>(
    theAlloc,
    std::forward<Args>(theArgs)...);
}

//! Makes shared pointer with OCCT allocator
template <class T, class... Args>
std::shared_ptr<T> make_oshared(NCollection_OccAllocator<T>&& theAlloc, Args&&... theArgs)
{
  return std::allocate_shared<T, NCollection_OccAllocator<T>, Args...>(
    std::forward < NCollection_OccAllocator<T>(> theAlloc),
    std::forward<Args>(theArgs)...);
}

//! Makes unique pointer
template <typename T, class... Args>
std::unique_ptr<T> make_unique(Args&&... theArgs)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(theArgs)...));
}

//! A scoped lock type for mutex pointers.
//!
//! A ptr_lock_guard controls mutex ownership within a scope, releasing
//! ownership in the destructor. Unlike std::lock_guard, it works with mutex pointers
//! and handles the case when the pointer is null.
template <typename _Mutex>
class lock_guard
{
public:
  typedef _Mutex mutex_type;

  //! Constructor locks the mutex if it's not null.
  //! @param[in] theMutexPtr pointer to mutex
  explicit lock_guard(mutex_type* theMutexPtr)
      : myMutexPtr(theMutexPtr)
  {
    if (myMutexPtr)
    {
      myMutexPtr->lock();
    }
  }

  //! Constructor with adopt_lock assumes the mutex is already locked.
  //! @param[in] theMutexPtr pointer to mutex (assumed to be locked)
  //! @param[in] adopt_lock parameter indicating the mutex is already locked
  lock_guard(mutex_type* theMutexPtr, std::adopt_lock_t) noexcept
      : myMutexPtr(theMutexPtr)
  {
  } // calling thread owns mutex

  //! Destructor unlocks the mutex if it's not null.
  ~lock_guard()
  {
    if (myMutexPtr)
    {
      myMutexPtr->unlock();
    }
  }

  lock_guard(const ptr_lock_guard&)            = delete;
  lock_guard& operator=(const ptr_lock_guard&) = delete;

private:
  mutex_type* myMutexPtr;
};

} // namespace opencascade

#endif // _Standard_MemoryUtils_HeaderFile
