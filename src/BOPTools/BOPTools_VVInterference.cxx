// File:	BOPTools_VVInterference.cxx
// Created:	Tue Nov 21 15:45:28 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_VVInterference.ixx>
//=======================================================================
//function :  BOPTools_VVInterference
//purpose  : 
//=======================================================================
  BOPTools_VVInterference::BOPTools_VVInterference()
:
  BOPTools_ShapeShapeInterference(0,0)
{}

//=======================================================================
//function :  BOPTools_VVInterference
//purpose  : 
//=======================================================================
  BOPTools_VVInterference::BOPTools_VVInterference
  (const Standard_Integer anIndex1,
   const Standard_Integer anIndex2)
:  
  BOPTools_ShapeShapeInterference(anIndex1,anIndex2)
{}
