// File:	BOPTools_VEInterference.cxx
// Created:	Tue Nov 21 15:43:57 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_VEInterference.ixx>

//=======================================================================
//function :  BOPTools_VEInterference
//purpose  : 
//=======================================================================
  BOPTools_VEInterference::BOPTools_VEInterference()
:
  BOPTools_ShapeShapeInterference(0,0)
{}

//=======================================================================
//function :  BOPTools_ESInterference
//purpose  : 
//=======================================================================
  BOPTools_VEInterference::BOPTools_VEInterference
  (const Standard_Integer anIndex1,
   const Standard_Integer anIndex2,
   const Standard_Real aT)
:  
  BOPTools_ShapeShapeInterference(anIndex1,anIndex2),
  myParameter(aT)
{}

//=======================================================================
//function :  SetParameter
//purpose  : 
//=======================================================================
  void BOPTools_VEInterference::SetParameter  (const Standard_Real aT)
{
  myParameter=aT;
}
//=======================================================================
//function :  Parameter
//purpose  : 
//=======================================================================
  Standard_Real BOPTools_VEInterference::Parameter  ()const 
{
  return myParameter;
}
