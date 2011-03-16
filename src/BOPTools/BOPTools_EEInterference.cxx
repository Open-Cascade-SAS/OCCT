// File:	BOPTools_EEInterference.cxx
// Created:	Tue Nov 21 15:42:17 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_EEInterference.ixx>

//=======================================================================
//function :  BOPTools_EEInterference
//purpose  : 
//=======================================================================
  BOPTools_EEInterference::BOPTools_EEInterference()
:
  BOPTools_ShapeShapeInterference(0,0)
{}

//=======================================================================
//function :  BOPTools_ESInterference
//purpose  : 
//=======================================================================
  BOPTools_EEInterference::BOPTools_EEInterference
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
  const IntTools_CommonPrt& BOPTools_EEInterference::CommonPrt() const 
{
  return myCommonPart;
}
