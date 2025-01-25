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

#ifndef _NCollection_Allocator_HeaderFile
#define _NCollection_Allocator_HeaderFile

#include <Standard.hxx>
#include <NCollection_BaseAllocator.hxx>

#include <utility>

//! Implements allocator requirements as defined in ISO C++ Standard 2003, section 20.1.5.
/*! The allocator uses a standard OCCT mechanism for memory
  allocation and deallocation. It can be used with standard
  containers (std::vector, std::map, etc.) to take advantage of OCCT memory optimizations.

  Example of use:
  \code
  NCollection_Allocator<TopoDS_Shape> anSAllocator();
  std::list<TopoDS_Shape, NCollection_Allocator<TopoDS_Shape>> aList(anSAllocator);
  TopoDS_Solid aSolid = BRepPrimAPI_MakeBox(10., 20., 30.);
  aList.push_back(aSolid);
  \endcode
*/
template <typename ItemType>
class NCollection_Allocator
{
public:
  typedef ItemType          value_type;
  typedef value_type*       pointer;
  typedef const value_type* const_pointer;
  typedef value_type&       reference;
  typedef const value_type& const_reference;
  typedef size_t            size_type;
  typedef ptrdiff_t         difference_type;

  template <typename OtherType>
  struct rebind
  {
    typedef NCollection_Allocator<OtherType> other;
  };

  //! Constructor.
  //! Creates an object using the default Open CASCADE allocation mechanism, i.e., which uses
  //! Standard::Allocate() and Standard::Free() underneath.
  NCollection_Allocator() noexcept {}

  //! Constructor.
  NCollection_Allocator(const Handle(NCollection_BaseAllocator)&) noexcept {}

  //! Assignment operator
  template <typename OtherType>
  NCollection_Allocator& operator=(const NCollection_Allocator<OtherType>&) noexcept
  {
    return *this;
  }

  //! Constructor.
  //! Creates an object using the default Open CASCADE allocation mechanism, i.e., which uses
  //! Standard::Allocate() and Standard::Free() underneath.
  template <typename OtherType>
  NCollection_Allocator(const NCollection_Allocator<OtherType>&) noexcept
  {
  }

  //! Returns an object address.
  pointer address(reference theItem) const { return &theItem; }

  //! Returns an object address.
  const_pointer address(const_reference theItem) const { return &theItem; }

  //! Allocates memory for theSize objects.
  pointer allocate(const size_type theSize, const void* /*hint*/ = 0) const
  {
    return static_cast<pointer>(Standard::AllocateOptimal(theSize * sizeof(ItemType)));
  }

  //! Frees previously allocated memory.
  void deallocate(pointer thePnt, const size_type) const
  {
    Standard::Free(static_cast<Standard_Address>(thePnt));
  }

  //! Reallocates memory for theSize objects.
  pointer reallocate(pointer thePnt, const size_type theSize) const
  {
    return static_cast<pointer>(Standard::Reallocate(thePnt, theSize * sizeof(ItemType)));
  }

  //! Constructs an object.
  //! Uses placement new operator and copy constructor to construct an object.
  template <class _Objty, class... _Types>
  void construct(_Objty* _Ptr, _Types&&... _Args)
  {
    ::new ((void*)_Ptr) _Objty(std::forward<_Types>(_Args)...);
  }

  //! Destroys the object.
  //! Uses the object destructor.
  void destroy(pointer thePnt)
  {
    (void)thePnt;
    thePnt->~value_type();
  }

  bool operator==(const NCollection_Allocator&) const { return true; }

  template <class U>
  bool operator==(const NCollection_Allocator<U>&) const noexcept
  {
    return true;
  }

  bool operator!=(const NCollection_Allocator&) const noexcept { return false; }

  template <class U>
  bool operator!=(const NCollection_Allocator<U>&) const noexcept
  {
    return false;
  }
};

template <class U, class V>
bool operator==(const NCollection_Allocator<U>&, const NCollection_Allocator<V>&)
{
  return true;
}

#endif
