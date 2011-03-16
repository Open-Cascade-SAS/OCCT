// File:	IntTools_CompareRange.cxx
// Created:	Tue Oct 24 14:09:45 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


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



