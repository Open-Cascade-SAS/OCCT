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

#ifndef _NCollection_OccAllocator_HeaderFile
#define _NCollection_OccAllocator_HeaderFile

#include <NCollection_BaseAllocator.hxx>
#include <Standard.hxx>

#include <memory>

//! Implements allocator requirements as defined in ISO C++ Standard 2003, section 20.1.5.
/*! The allocator uses a standard OCCT mechanism for memory
  allocation and deallocation. It can be used with standard
  containers (std::vector, std::map, etc.) to take advantage of OCCT memory optimizations.

  Example of use:
  \code
  NCollection_OccAllocator<TopoDS_Shape> anSAllocator();
  std::list<TopoDS_Shape, NCollection_OccAllocator<TopoDS_Shape>> aList(anSAllocator);
  TopoDS_Solid aSolid = BRepPrimAPI_MakeBox(10., 20., 30.);
  aList.push_back(aSolid);
  \endcode
*/
template <typename ItemType>
class NCollection_OccAllocator
{
public:

  typedef ItemType value_type;
  typedef value_type* pointer;
  typedef const value_type* const_pointer;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef std::false_type propagate_on_container_move_assignment; // std::false_type

  template <typename OtherType>
  struct rebind
  {
    typedef NCollection_OccAllocator<OtherType> other;
  };

  //! Constructor.
  //! Creates an object using the default Open CASCADE allocation mechanism, i.e., which uses
  //! Standard::Allocate() and Standard::Free() underneath.
  NCollection_OccAllocator() :
    myAllocator(nullptr)
  {}

  //! Constructor.
  NCollection_OccAllocator(const Handle(NCollection_BaseAllocator)& theAlloc) :
    myAllocator(theAlloc)
  {}

  //! Constructor.
  NCollection_OccAllocator(Handle(NCollection_BaseAllocator)&& theAlloc) :
    myAllocator(theAlloc)
  {}

  //! Constructor.
  NCollection_OccAllocator(const NCollection_OccAllocator& theOther) :
    myAllocator(theOther.myAllocator)
  {}

  //! Constructor.
  NCollection_OccAllocator(NCollection_OccAllocator&& theOther) noexcept :
    myAllocator(theOther.myAllocator)
  {}

  //! Assignment operator
  NCollection_OccAllocator& operator=(const NCollection_OccAllocator& theOther)
  {
    myAllocator = theOther.myAllocator;
    return *this;
  }

  //! Assignment operator
  NCollection_OccAllocator& operator=(NCollection_OccAllocator&& theOther) noexcept
  {
    myAllocator = theOther.myAllocator;
    return *this;
  }

  //! Assignment operator
  template <typename OtherType>
  NCollection_OccAllocator& operator=(const NCollection_OccAllocator<OtherType>& theOther)
  {
    myAllocator = theOther.myAllocator;
    return *this;
  }

  //! Constructor.
  //! Creates an object using the default Open CASCADE allocation mechanism, i.e., which uses
  //! Standard::Allocate() and Standard::Free() underneath.
  template <typename OtherType>
  NCollection_OccAllocator(const NCollection_OccAllocator<OtherType>& theOther) :
    myAllocator(theOther.Allocator())
  {}

  void SetAllocator(const Handle(NCollection_BaseAllocator)& theAlloc)
  {
    myAllocator = theAlloc;
  }

  const Handle(NCollection_BaseAllocator)& Allocator() const
  {
    return myAllocator;
  }

  //! Allocates memory for theSize objects.
  pointer allocate(size_type theSize, const void* = 0)
  {
    return static_cast<pointer> (myAllocator.IsNull() ?
                                 Standard::AllocateOptimal(theSize * sizeof(ItemType)) :
                                 myAllocator->AllocateOptimal(theSize * sizeof(ItemType)));
  }

  //! Template version of function Free(), nullifies the argument pointer
  //! @param thePtr - previously allocated memory block to be freed
  template <typename T>
  void deallocate(T* thePnt, size_type)
  {
    myAllocator.IsNull() ?
      Standard::Free(thePnt) :
      myAllocator->Free(thePnt);
  }

  //! Frees previously allocated memory.
  void deallocate(pointer thePnt, size_type)
  {
    myAllocator.IsNull() ?
      Standard::Free(thePnt) :
      myAllocator->Free(thePnt);
  }

  //! Constructs an object.
  //! Uses placement new operator and copy constructor to construct an object.
  template<class _Objty, class... _Types>
  void construct(_Objty* _Ptr, _Types&&... _Args)
  {
    ::new ((void*)_Ptr) _Objty(std::forward<_Types>(_Args)...);
  }

  //! Returns an object address.
  pointer address(reference theItem) const
  {
    return &theItem;
  }

  //! Returns an object address.
  const_pointer address(const_reference theItem) const
  {
    return &theItem;
  }

  //! Destroys the object.
  //! Uses the object destructor.
  template<class _Uty>
  void destroy(_Uty* _Ptr)
  {
    (void)_Ptr; _Ptr->~_Uty();
  }

  //! Estimate maximum array size
  size_t max_size() const noexcept
  {
    return ((size_t)(-1) / sizeof(ItemType));
  }

  bool operator==(const NCollection_OccAllocator& theOther) const
  {
    return theOther.Allocator() == myAllocator;
  }

  template<class U>
  bool operator==(const NCollection_OccAllocator<U>& theOther) const
  {
    return theOther.Allocator() == myAllocator;
  }

  bool operator!=(const NCollection_OccAllocator& theOther) const
  {
    return theOther.Allocator() != myAllocator;
  }

  template<class U>
  bool operator!=(const NCollection_OccAllocator<U>& theOther) const
  {
    return theOther.Allocator() != myAllocator;
  }

private:

  Handle(NCollection_BaseAllocator) myAllocator;
};

template<class U, class V>
bool operator==(const NCollection_OccAllocator<U>& theFirst, const NCollection_OccAllocator<V>& theSecond)
{
  return theFirst.Allocator() == theSecond.Allocator();
}

#endif
