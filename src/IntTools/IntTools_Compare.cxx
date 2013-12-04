// Created on: 2000-05-22
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

#include <IntTools_Compare.ixx>
//=======================================================================
//function :IntTools_Compare::IntTools_Compare
//purpose  : 
//=======================================================================
IntTools_Compare::IntTools_Compare() :myTol(1.e-12) {}

//=======================================================================
//function :IntTools_Compare::IntTools_Compare
//purpose  : 
//=======================================================================
  IntTools_Compare::IntTools_Compare(const Standard_Real aTol)
{
  myTol=aTol;
}

//=======================================================================
//function :IsLower
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Compare::IsLower(const IntTools_Root& aLeft,
					     const IntTools_Root& aRight)const
{
  return aLeft.Root()<aRight.Root();
}

//=======================================================================
//function :IsGreater
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Compare::IsGreater(const IntTools_Root& aLeft,
					       const IntTools_Root& aRight)const
{
  return !IsLower(aLeft,aRight);
}

//=======================================================================
//function :IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean IntTools_Compare::IsEqual(const IntTools_Root& aLeft,
					     const IntTools_Root& aRight)const
{
  Standard_Real a, b;
  a=aLeft.Root();
  b=aRight.Root();
  return fabs(a-b) < myTol;
}






