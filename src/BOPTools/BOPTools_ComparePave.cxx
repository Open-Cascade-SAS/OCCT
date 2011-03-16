// File:	BOPTools_ComparePave.cxx
// Created:	Fri Feb 16 15:47:07 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_ComparePave.ixx>

//=======================================================================
// function: BOPTools_ComparePave::BOPTools_ComparePave
// purpose: 
//=======================================================================
  BOPTools_ComparePave::BOPTools_ComparePave()
  :myTol(1.e-12) 
  {}

//=======================================================================
// function: BOPTools_ComparePave::BOPTools_ComparePave
// purpose: 
//=======================================================================
  BOPTools_ComparePave::BOPTools_ComparePave(const Standard_Real aTol)
{
  myTol=aTol;
}
//=======================================================================
// function: IsLower
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_ComparePave::IsLower(const BOPTools_Pave& aLeft,
						 const BOPTools_Pave& aRight)const
{
  return aLeft.Param()<aRight.Param();
}
//=======================================================================
// function: IsGreater
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_ComparePave::IsGreater(const BOPTools_Pave& aLeft,
						   const BOPTools_Pave& aRight)const
{
  return aLeft.Param()>aRight.Param();
}
//=======================================================================
// function: IsEqual
// purpose: 
//=======================================================================
  Standard_Boolean BOPTools_ComparePave::IsEqual(const BOPTools_Pave& aLeft,
						 const BOPTools_Pave& aRight)const
{
  Standard_Real a, b;
  a=aLeft.Param();
  b=aRight.Param();
  return fabs(a-b) < myTol;
}
