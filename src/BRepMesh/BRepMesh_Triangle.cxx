// Created on: 1993-09-23
// Created by: Didier PIFFAULT
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BRepMesh_Triangle.hxx>

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_Triangle::BRepMesh_Triangle()
: myEdge1(0),
  myEdge2(0),
  myEdge3(0),
  myOrientation1(Standard_False),
  myOrientation2(Standard_False),
  myOrientation3(Standard_False),
  myMovability(BRepMesh_Free)
{
}

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BRepMesh_Triangle::BRepMesh_Triangle (const Standard_Integer          theEdge1,
                                      const Standard_Integer          theEdge2,
                                      const Standard_Integer          theEdge3,
                                      const Standard_Boolean          theOrientation1,
                                      const Standard_Boolean          theOrientation2,
                                      const Standard_Boolean          theOrientation3,
                                      const BRepMesh_DegreeOfFreedom  isCanMove)
: myEdge1(theEdge1),
  myEdge2(theEdge2),
  myEdge3(theEdge3),
  myOrientation1(theOrientation1),
  myOrientation2(theOrientation2), 
  myOrientation3(theOrientation3), 
  myMovability(isCanMove)
{
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================
void  BRepMesh_Triangle::Initialize(const Standard_Integer          theEdge1,
                                    const Standard_Integer          theEdge2,
                                    const Standard_Integer          theEdge3,
                                    const Standard_Boolean          theOrientation1,
                                    const Standard_Boolean          theOrientation2,
                                    const Standard_Boolean          theOrientation3,
                                    const BRepMesh_DegreeOfFreedom  isCanMove)
{
  myEdge1        = theEdge1;
  myEdge2        = theEdge2;
  myEdge3        = theEdge3;
  myOrientation1 = theOrientation1;
  myOrientation2 = theOrientation2;
  myOrientation3 = theOrientation3;
  myMovability   = isCanMove;
}

//=======================================================================
//function : Edges
//purpose  : 
//=======================================================================
void  BRepMesh_Triangle::Edges(Standard_Integer& theEdge1,
                               Standard_Integer& theEdge2,
                               Standard_Integer& theEdge3,
                               Standard_Boolean& theOrientation1,
                               Standard_Boolean& theOrientation2,
                               Standard_Boolean& theOrientation3) const
{
  theEdge1        = myEdge1;
  theEdge2        = myEdge2;
  theEdge3        = myEdge3;
  theOrientation1 = myOrientation1;
  theOrientation2 = myOrientation2;
  theOrientation3 = myOrientation3;
}

//=======================================================================
//function : SetMovability
//purpose  : 
//=======================================================================
void BRepMesh_Triangle::SetMovability(const BRepMesh_DegreeOfFreedom theMovability)
{
  myMovability = theMovability;
}

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================
Standard_Integer BRepMesh_Triangle::HashCode(const Standard_Integer theUpper)const 
{
  return ::HashCode(myEdge1 + myEdge2 + myEdge3, theUpper);
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================
Standard_Boolean BRepMesh_Triangle::IsEqual(const BRepMesh_Triangle& theOther) const 
{
  if (myMovability == BRepMesh_Deleted || theOther.myMovability == BRepMesh_Deleted)
    return Standard_False;

  if (myEdge1 == theOther.myEdge1 && 
      myEdge2 == theOther.myEdge2 && 
      myEdge3 == theOther.myEdge3)
  {
    return Standard_True;
  }

  if (myEdge1 == theOther.myEdge2 && 
      myEdge2 == theOther.myEdge3 && 
      myEdge3 == theOther.myEdge1)
  {
    return Standard_True;
  }

  if (myEdge1 == theOther.myEdge3 && 
      myEdge2 == theOther.myEdge1 && 
      myEdge3 == theOther.myEdge2)
  {
    return Standard_True;
  }

  return Standard_False;
}
