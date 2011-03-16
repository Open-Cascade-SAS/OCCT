// File:	IntTools_Compare.cxx
// Created:	Mon May 22 17:02:11 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


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






