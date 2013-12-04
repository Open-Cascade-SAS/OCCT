// Created on: 2000-10-24
// Created by: Peter KURNEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <IntTools_CompareRange.ixx>
//=======================================================================
//function :IntTools_CompareRange::IntTools_CompareRange
//purpose  : 
//=======================================================================
IntTools_CompareRange::IntTools_CompareRange() :myTol(1.e-12) {}

//=======================================================================
//function :IntTools_CompareRange::IntTools_CompareRange
//purpose  : 
//=======================================================================
  IntTools_CompareRange::IntTools_CompareRange(const Standard_Real aTol)
{
  myTol=aTol;
}

//=======================================================================
//function :IsLower
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_CompareRange::IsLower(const IntTools_Range& aLeft,
						  const IntTools_Range& aRight)const
{
  return aLeft.First()<aRight.First();
}

//=======================================================================
//function :IsGreater
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_CompareRange::IsGreater(const IntTools_Range& aLeft,
						    const IntTools_Range& aRight)const
{
  return !IsLower(aLeft,aRight);
}

//=======================================================================
//function :IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_CompareRange::IsEqual(const IntTools_Range& aLeft,
						  const IntTools_Range& aRight)const
{
  Standard_Real a, b;
  a=aLeft.First();
  b=aRight.First();
  return fabs(a-b) < myTol;
}



