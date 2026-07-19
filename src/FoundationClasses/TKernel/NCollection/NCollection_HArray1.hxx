// Copyright (c) 2002-2024 OPEN CASCADE SAS
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

#ifndef NCollection_HArray1_HeaderFile
#define NCollection_HArray1_HeaderFile

#include <NCollection_Array1.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>

//! Template class for Handle-managed 1D arrays.
//! Inherits from both NCollection_Array1<TheItemType> and Standard_Transient,
//! providing reference-counted array functionality.
template <typename TheItemType>
class NCollection_HArray1 : public NCollection_Array1<TheItemType>, public Standard_Transient
{
public:
  DEFINE_STANDARD_ALLOC
  DEFINE_NCOLLECTION_ALLOC

  typedef NCollection_Array1<TheItemType> Array1Type;
  typedef TheItemType                     value_type;

public:
  //! Default constructor.
  NCollection_HArray1()
      : Array1Type()
  {
  }

  //! Constructor with bounds.
  //! @param theLower lower bound of the array
  //! @param theUpper upper bound of the array
  NCollection_HArray1(const int theLower, const int theUpper)
      : Array1Type(theLower, theUpper)
  {
  }

  //! Constructor with bounds and initial value.
  //! @param theLower lower bound of the array
  //! @param theUpper upper bound of the array
  //! @param theValue initial value for all elements
  NCollection_HArray1(const int theLower, const int theUpper, const TheItemType& theValue)
      : Array1Type(theLower, theUpper)
  {
    Array1Type::Init(theValue);
  }

  //! Constructor from C array.
  //! @param theBegin reference to the first element of a C array
  //! @param theLower lower bound of the array
  //! @param theUpper upper bound of the array
  //! @param theUseBuffer flag indicating whether to use external buffer (must be explicit)
  explicit NCollection_HArray1(const TheItemType& theBegin,
                               const int          theLower,
                               const int          theUpper,
                               const bool         theUseBuffer)
      : Array1Type(theBegin, theLower, theUpper, theUseBuffer)
  {
  }

  //! Copy constructor from array.
  //! @param theOther the array to copy from
  NCollection_HArray1(const Array1Type& theOther)
      : Array1Type(theOther)
  {
  }

  //! Returns const reference to the underlying array.
  const Array1Type& Array1() const noexcept { return *this; }

  //! Returns mutable reference to the underlying array.
  Array1Type& ChangeArray1() noexcept { return *this; }

  DEFINE_STANDARD_RTTI_INLINE(NCollection_HArray1, Standard_Transient)
};

#endif // NCollection_HArray1_HeaderFile
