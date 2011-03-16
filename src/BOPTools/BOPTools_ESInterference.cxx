// File:	BOPTools_ESInterference.cxx
// Created:	Tue Nov 21 15:35:58 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_ESInterference.ixx>

//=======================================================================
//function :  BOPTools_ESInterference
//purpose  : 
//=======================================================================
  BOPTools_ESInterference::BOPTools_ESInterference()
:
  BOPTools_ShapeShapeInterference(0,0)
{}

//=======================================================================
//function :  BOPTools_ESInterference
//purpose  : 
//=======================================================================
  BOPTools_ESInterference::BOPTools_ESInterference
  (const Standard_Integer anIndex1,
   const Standard_Integer anIndex2,
   const IntTools_CommonPrt& aCPart)
:  
  BOPTools_ShapeShapeInterference(anIndex1,anIndex2)
{
  myCommonPart=aCPart;
}

//=======================================================================
//function :  CommonPrt
//purpose  : 
//=======================================================================
  const IntTools_CommonPrt& BOPTools_ESInterference::CommonPrt() const 
{
  return myCommonPart;
}
