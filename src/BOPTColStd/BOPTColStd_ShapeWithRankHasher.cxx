// File:	BOPTColStd_ShapeWithRankHasher.cxx
// Created:	Fri Jun  8 17:32:30 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTColStd_ShapeWithRankHasher.ixx>
//=======================================================================
//function : BOPTColStd_ShapeWithRankHasher::HashCode
//purpose  : 
//=======================================================================
  Standard_Integer BOPTColStd_ShapeWithRankHasher::HashCode (const BOPTColStd_ShapeWithRank& aSR,
							     const Standard_Integer Upper)
{
  return aSR.HashCode(Upper);
}

//=======================================================================
//function : BOPTColStd_ShapeWithRankHasher::IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean BOPTColStd_ShapeWithRankHasher::IsEqual (const BOPTColStd_ShapeWithRank& aSR1,
							    const BOPTColStd_ShapeWithRank& aSR2)
{
  return aSR1.IsEqual(aSR2);
}

