// Created on: 2002-04-01
// Created by: Peter KURNEV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BOPTools_CoupleOfInteger.ixx>

//=======================================================================
// function: BOPTools_CoupleOfInteger::BOPTools_CoupleOfInteger
// purpose: 
//=======================================================================
BOPTools_CoupleOfInteger::BOPTools_CoupleOfInteger()
:
  myFirst(99),
  mySecond(99)
{}

//=======================================================================
// function: BOPTools_CoupleOfInteger::BOPTools_CoupleOfInteger
// purpose: 
//=======================================================================
  BOPTools_CoupleOfInteger::BOPTools_CoupleOfInteger(const Standard_Integer aFirst,
						     const Standard_Integer aSecond)
:
  myFirst(aFirst),
  mySecond(aSecond)
{}

//=======================================================================
// function: SetCouple
// purpose: 
//=======================================================================
  void BOPTools_CoupleOfInteger::SetCouple(const Standard_Integer aFirst,
					   const Standard_Integer aSecond)
{
  myFirst=aFirst;
  mySecond=aSecond;
}

//=======================================================================
// function: SetFirst
// purpose: 
//=======================================================================
  void BOPTools_CoupleOfInteger::SetFirst(const Standard_Integer aFirst)
{
  myFirst=aFirst;
}


//=======================================================================
// function: SetSecond
// purpose: 
//=======================================================================
  void BOPTools_CoupleOfInteger::SetSecond(const Standard_Integer aSecond)
{
  mySecond=aSecond;
}

//=======================================================================
// function: Couple
// purpose: 
//=======================================================================
  void BOPTools_CoupleOfInteger::Couple(Standard_Integer& aFirst,
					Standard_Integer& aSecond)const
{
  aFirst=myFirst;
  aSecond=mySecond;
}
//=======================================================================
// function: First
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_CoupleOfInteger::First()const
{
  return myFirst;
}
//=======================================================================
// function: Second
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_CoupleOfInteger::Second()const
{
  return mySecond;
}
//=======================================================================
// function: IsEqual
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_CoupleOfInteger::IsEqual(const BOPTools_CoupleOfInteger& aOther)const
{
  Standard_Boolean b1, b2;
  //
  b1=(Standard_Boolean)((myFirst==aOther.myFirst ) && (mySecond==aOther.mySecond));
  b2=(Standard_Boolean)((myFirst==aOther.mySecond) && (mySecond==aOther.myFirst ));
  return (Standard_Boolean)(b1||b2);
}
//=======================================================================
// function: HashCode
// purpose: 
//=======================================================================
  Standard_Integer BOPTools_CoupleOfInteger::HashCode(const Standard_Integer aUpper)const
{
  return (myFirst+mySecond)%aUpper;
}
