// File:	IntTools_Range.cxx
// Created:	Thu May 18 13:44:43 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <IntTools_Range.ixx>
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
