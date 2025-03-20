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

#ifndef _IntTools_Range_HeaderFile
#define _IntTools_Range_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>

//! The class describes the  1-d range
//! [myFirst, myLast].
class IntTools_Range
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT IntTools_Range();

  //! Initialize me by range boundaries
  Standard_EXPORT IntTools_Range(const Standard_Real aFirst, const Standard_Real aLast);

  //! Modifier
  Standard_EXPORT void SetFirst(const Standard_Real aFirst);

  //! Modifier
  Standard_EXPORT void SetLast(const Standard_Real aLast);

  //! Selector
  Standard_EXPORT Standard_Real First() const;

  //! Selector
  Standard_EXPORT Standard_Real Last() const;

  //! Selector
  Standard_EXPORT void Range(Standard_Real& aFirst, Standard_Real& aLast) const;

protected:
private:
  Standard_Real myFirst;
  Standard_Real myLast;
};

#endif // _IntTools_Range_HeaderFile
