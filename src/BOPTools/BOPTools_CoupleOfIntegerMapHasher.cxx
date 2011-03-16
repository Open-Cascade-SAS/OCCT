// File:	BOPTools_CoupleOfIntegerMapHasher.cxx
// Created:	Fri Dec  5 10:03:04 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_CoupleOfIntegerMapHasher.ixx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
  Standard_Integer BOPTools_CoupleOfIntegerMapHasher::HashCode(const BOPTools_CoupleOfInteger& aPKey,
							       const Standard_Integer Upper)
{
  return aPKey.HashCode(Upper);
}
//=======================================================================
//function :IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTools_CoupleOfIntegerMapHasher::IsEqual(const BOPTools_CoupleOfInteger& aPKey1,
							      const BOPTools_CoupleOfInteger& aPKey2)
{
  return aPKey1.IsEqual(aPKey2);
}
