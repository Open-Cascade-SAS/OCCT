// Created on: 2001-06-08
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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

