// Created on: 2000-11-21
// Created by: Peter KURNEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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
