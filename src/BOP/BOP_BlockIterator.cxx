// File:	BOP_BlockIterator.cxx
// Created:	Tue Mar 23 15:37:30 1993
// Author:	Jean Yves LEBEY
//		<jyl@phobox>
 
#include <BOP_BlockIterator.ixx>

//=======================================================================
//function : BOP_BlockIterator::BOP_BlockIterator
//purpose  : 
//=======================================================================
  BOP_BlockIterator::BOP_BlockIterator() 
:
  myLower(0),
  myUpper(0),
  myValue(1)
{
}

//=======================================================================
//function :  BOP_BlockIterator::BOP_BlockIterator
//purpose  :
//=======================================================================
  BOP_BlockIterator::BOP_BlockIterator(const Standard_Integer Lower, 
				       const Standard_Integer Upper)
:
  myLower(Lower),
  myUpper(Upper),
  myValue(Lower)
{
}
//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
  void BOP_BlockIterator::Initialize()
{
  myValue = myLower;
}
//=======================================================================
//function : More
//purpose  : 
//=======================================================================
  Standard_Boolean BOP_BlockIterator::More() const
{
  Standard_Boolean b = (myValue <= myUpper);
  return b;
}
//=======================================================================
//function : Next
//purpose  : 
//=======================================================================
  void BOP_BlockIterator::Next()
{
  myValue++;
}
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
  Standard_Integer BOP_BlockIterator::Value() const
{
  return myValue;
}
//=======================================================================
//function : Extent
//purpose  : 
//=======================================================================
  Standard_Integer BOP_BlockIterator::Extent() const
{
  if (myLower != 0) {
    Standard_Integer n = myUpper - myLower + 1;
    return n;
  }
  return 0;
}
