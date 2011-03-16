// File:	BOPTColStd_ShapeWithRank.cxx
// Created:	Fri Jun  8 17:19:46 2001
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <BOPTColStd_ShapeWithRank.ixx>

//=======================================================================
//function : BOPTColStd_ShapeWithRank::BOPTColStd_ShapeWithRank
//purpose  : 
//=======================================================================
  BOPTColStd_ShapeWithRank::BOPTColStd_ShapeWithRank()
:
  myRank(0)
{}

//=======================================================================
//function : SetShape
//purpose  : 
//=======================================================================
  void BOPTColStd_ShapeWithRank::SetShape(const TopoDS_Shape& aS)
{
  myShape=aS;
}

//=======================================================================
//function : SetRank
//purpose  : 
//=======================================================================
  void BOPTColStd_ShapeWithRank::SetRank(const Standard_Integer aR)
{
  myRank=aR;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
  const TopoDS_Shape& BOPTColStd_ShapeWithRank::Shape()const 
{
  return myShape;
}

//=======================================================================
//function : Rank
//purpose  : 
//=======================================================================
  Standard_Integer BOPTColStd_ShapeWithRank::Rank()const
{
  return myRank;
}


//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
  Standard_Integer BOPTColStd_ShapeWithRank::HashCode(const Standard_Integer Upper)const
{
  return myShape.HashCode(Upper);
}
//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================
  Standard_Boolean  BOPTColStd_ShapeWithRank::IsEqual(const BOPTColStd_ShapeWithRank& aSR)const
{
  return (myShape.IsSame(aSR.myShape) && myRank==aSR.myRank);
}

