// File:	BOPTools_CoupleOfInteger.cxx
// Created:	Mon Apr  1 10:21:26 2002
// Author:	Peter KURNEV
//		<pkv@irinox>

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
