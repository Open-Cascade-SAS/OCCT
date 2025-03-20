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

namespace opencascade
{
template <class T, class... Args>
std::shared_ptr<T> make_shared(Args&&... theArgs)
{
  return std::allocate_shared<T, NCollection_Allocator<T>, Args...>(NCollection_Allocator<T>(),
                                                                    std::forward<Args>(theArgs)...);
}

template <class T, class... Args>
std::shared_ptr<T> make_oshared(const Handle(NCollection_BaseAllocator)& theAlloc,
                                Args&&... theArgs)
{
  return std::allocate_shared<T, NCollection_OccAllocator<T>, Args...>(
    NCollection_OccAllocator<T>(theAlloc),
    std::forward<Args>(theArgs)...);
}

template <class T, class... Args>
std::shared_ptr<T> make_oshared(const NCollection_OccAllocator<T>& theAlloc, Args&&... theArgs)
{
  return std::allocate_shared<T, NCollection_OccAllocator<T>, Args...>(
    theAlloc,
    std::forward<Args>(theArgs)...);
}

template <class T, class... Args>
std::shared_ptr<T> make_oshared(NCollection_OccAllocator<T>&& theAlloc, Args&&... theArgs)
{
  return std::allocate_shared<T, NCollection_OccAllocator<T>, Args...>(
    std::forward < NCollection_OccAllocator<T>(> theAlloc),
    std::forward<Args>(theArgs)...);
}

template <typename T, class... Args>
std::unique_ptr<T> make_unique(Args&&... theArgs)
{
  return std::unique_ptr<T>(new T(std::forward<Args>(theArgs)...));
}
} // namespace opencascade

#endif
