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


#include <IntTools_Range.hxx>

//=======================================================================
//function : IntTools_Range::IntTools_Range
//purpose  : 
//=======================================================================
IntTools_Range::IntTools_Range():myFirst(0.), myLast(0.) {}

//=======================================================================
//function : IntTools_Range::IntTools_Range
//purpose  : 
//=======================================================================
  IntTools_Range::IntTools_Range(const Standard_Real aFirst,const Standard_Real aLast)
{
  myFirst=aFirst;
  myLast=aLast;
}
//=======================================================================
//function : SetFirst
//purpose  : 
//=======================================================================
  void IntTools_Range::SetFirst(const Standard_Real aFirst) 
{
  myFirst=aFirst;
}
//=======================================================================
//function : SetLast
//purpose  : 
//=======================================================================
  void IntTools_Range::SetLast(const Standard_Real aLast) 
{
  myLast=aLast;
}
//=======================================================================
//function : First
//purpose  : 
//=======================================================================
  Standard_Real IntTools_Range::First() const
{
  return myFirst;
}
//=======================================================================
//function : Last
//purpose  : 
//=======================================================================
  Standard_Real IntTools_Range::Last() const
{
  return myLast;
}
//=======================================================================
//function : Range
//purpose  : 
//=======================================================================
  void IntTools_Range::Range(Standard_Real& aFirst,Standard_Real& aLast) const
{
  aFirst=myFirst;
  aLast =myLast;
}
