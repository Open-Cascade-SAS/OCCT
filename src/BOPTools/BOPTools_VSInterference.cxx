// File:	BOPTools_VSInterference.cxx
// Created:	Tue Nov 21 15:39:57 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_VSInterference.ixx>

//=======================================================================
//function :  BOPTools_VSInterference
//purpose  : 
//=======================================================================
  BOPTools_VSInterference::BOPTools_VSInterference()
:
  BOPTools_ShapeShapeInterference(0,0)
{}

//=======================================================================
//function :  BOPTools_VSInterference
//purpose  : 
//=======================================================================
  BOPTools_VSInterference::BOPTools_VSInterference
  (const Standard_Integer anIndex1,
   const Standard_Integer anIndex2,
   const Standard_Real U,
   const Standard_Real V)
:  
  BOPTools_ShapeShapeInterference(anIndex1,anIndex2),
  myU(U), 
  myV(V)
{}

//=======================================================================
//function :  SetUV
//purpose  : 
//=======================================================================
  void BOPTools_VSInterference::SetUV (const Standard_Real U,
				       const Standard_Real V)
{
  myU=U; 
  myV=V;
}
//=======================================================================
//function :  UV
//purpose  : 
//=======================================================================
  void BOPTools_VSInterference::UV (Standard_Real& U, Standard_Real& V) const
{
  U=myU; 
  V=myV;
}
