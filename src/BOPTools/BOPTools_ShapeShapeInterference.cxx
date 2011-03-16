// File:	BOPTools_ShapeShapeInterference.cxx
// Created:	Tue Nov 21 15:21:34 2000
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTools_ShapeShapeInterference.ixx>

//=======================================================================
//function :  BOPTools_ShapeShapeInterference::BOPTools_ShapeShapeInterference
//purpose  : 
//=======================================================================
  BOPTools_ShapeShapeInterference::BOPTools_ShapeShapeInterference()
:
  myIndex1(0), myIndex2(0), myNewShape(0)
{}

//=======================================================================
//function :  BOPTools_ShapeShapeInterference::BOPTools_ShapeShapeInterference
//purpose  : 
//=======================================================================
  BOPTools_ShapeShapeInterference::BOPTools_ShapeShapeInterference
  (const Standard_Integer anIndex1,
   const Standard_Integer anIndex2)
:  
  myIndex1(anIndex1),
  myIndex2(anIndex2),
  myNewShape(0)
{}

//=======================================================================
//function : SetIndex1
//purpose  : 
//=======================================================================
  void BOPTools_ShapeShapeInterference::SetIndex1(const Standard_Integer anIndex1)
{
  myIndex1=anIndex1;
}

//=======================================================================
//function : SetIndex2
//purpose  : 
//=======================================================================
  void BOPTools_ShapeShapeInterference::SetIndex2(const Standard_Integer anIndex2)
{
  myIndex2=anIndex2;
}

//=======================================================================
//function : SetNewShape
//purpose  : 
//=======================================================================
  void BOPTools_ShapeShapeInterference::SetNewShape(const Standard_Integer anIndex)
{
  myNewShape=anIndex;
}

//=======================================================================
//function : Index1
//purpose  : 
//=======================================================================
  Standard_Integer BOPTools_ShapeShapeInterference::Index1() const
{
  return myIndex1;
}

//=======================================================================
//function : Index2
//purpose  : 
//=======================================================================
  Standard_Integer BOPTools_ShapeShapeInterference::Index2() const
{
  return myIndex2;
}
//=======================================================================
//function : OppositeIndex
//purpose  : 
//=======================================================================
  Standard_Integer BOPTools_ShapeShapeInterference::OppositeIndex
    (const Standard_Integer anIndex) const
{
  if (anIndex==myIndex1) {
    return myIndex2;
  }
  else if(anIndex==myIndex2) {
    return myIndex1;
  }
  else {
    return 0;
  }
}

//=======================================================================
//function : Indices
//purpose  : 
//=======================================================================
  void BOPTools_ShapeShapeInterference::Indices(Standard_Integer& i1,
						Standard_Integer& i2) const
{
  i1=myIndex1;
  i2=myIndex2;
}

//=======================================================================
//function : NewShape
//purpose  : 
//=======================================================================
  Standard_Integer BOPTools_ShapeShapeInterference::NewShape() const
{
  return myNewShape;
}
