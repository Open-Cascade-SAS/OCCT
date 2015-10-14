// Created on: 2000-05-18
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _IntTools_CArray1OfReal_HeaderFile
#define _IntTools_CArray1OfReal_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Standard_ConstructionError;
class Standard_OutOfRange;
class Standard_OutOfMemory;



class IntTools_CArray1OfReal 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Creates an array  of given Length.
  Standard_EXPORT IntTools_CArray1OfReal(const Standard_Integer Length = 0);
  

  //! Creates an array sharing datas with an other.
  //! Example:
  //! Item tab[100];
  //! CArray1OfItem thetab (tab[0],100);
  //!
  //! CArray1OfItem aArray1(100);
  //! CArray1OfItem anSharedArray1(aArray1.ChangeValue(0),aArray1.Length());
  //!
  //! Warning:
  //! The validity of length are under the responsability
  //! of the user.
  //! The sahred array must have a valid address during the life of
  //! the Array1.
  Standard_EXPORT IntTools_CArray1OfReal(const Standard_Real& Item, const Standard_Integer Length);
  

  //! Initializes the array with a given value.
  Standard_EXPORT void Init (const Standard_Real& V);
  

  //! destroy current content and realloc the new size
  //! does nothing if Length() == theLength
  Standard_EXPORT void Resize (const Standard_Integer theNewLength);
  

  //! Frees the  allocated   area  corresponding  to the
  //! array.
  Standard_EXPORT void Destroy();
~IntTools_CArray1OfReal()
{
  Destroy();
}
  

  //! Returns the number of elements of <me>.
    Standard_Integer Length() const;
  
  Standard_EXPORT void Append (const Standard_Real& Value);
  

  //! Sets  the   <Index>th  element  of   the  array to
  //! <Value>.
  Standard_EXPORT void SetValue (const Standard_Integer Index, const Standard_Real& Value);
  

  //! Returns the value of  the  <Index>th element of the
  //! array.
  Standard_EXPORT const Standard_Real& Value (const Standard_Integer Index) const;
const Standard_Real& operator () (const Standard_Integer Index) const
{
  return Value(Index);
}
  

  //! Returns the value  of the <Index>th element  of the
  //! array.
  Standard_EXPORT Standard_Real& ChangeValue (const Standard_Integer Index);
Standard_Real& operator () (const Standard_Integer Index)
{
  return ChangeValue(Index);
}
  

  //! Applys the == operator on each array item
  Standard_EXPORT Standard_Boolean IsEqual (const IntTools_CArray1OfReal& Other) const;
Standard_Boolean operator == (const IntTools_CArray1OfReal& Other) const
{
  return IsEqual(Other);
}




protected:





private:

  

  //! Prohibits the creator by copy
  Standard_EXPORT IntTools_CArray1OfReal(const IntTools_CArray1OfReal& AnArray);


  Standard_Address myStart;
  Standard_Integer myLength;
  Standard_Boolean myIsAllocated;


};

#define Array1Item Standard_Real
#define Array1Item_hxx <Standard_Real.hxx>
#define IntTools_CArray1 IntTools_CArray1OfReal
#define IntTools_CArray1_hxx <IntTools_CArray1OfReal.hxx>

#include <IntTools_CArray1.lxx>

#undef Array1Item
#undef Array1Item_hxx
#undef IntTools_CArray1
#undef IntTools_CArray1_hxx




#endif // _IntTools_CArray1OfReal_HeaderFile
