// File:        BRepMesh_Triangle.cxx
// Created:     Thu Sep 23 13:10:58 1993
// Author:      Didier PIFFAULT
//              <dpf@zerox>

#include <BRepMesh_Triangle.ixx>

BRepMesh_Triangle::BRepMesh_Triangle()
: Edge1(0), Edge2(0), Edge3(0), myMovability(BRepMesh_Free)
{}

BRepMesh_Triangle::BRepMesh_Triangle (const Standard_Integer e1, 
                                      const Standard_Integer e2,
                                      const Standard_Integer e3,
                                      const Standard_Boolean o1, 
                                      const Standard_Boolean o2,
                                      const Standard_Boolean o3,
                                      const BRepMesh_DegreeOfFreedom  canMove)
                                      : Edge1(e1),  Orientation1(o1),Edge2(e2), Orientation2(o2), 
                                      Edge3(e3), Orientation3(o3), 
                                      myMovability(canMove)
{}

void  BRepMesh_Triangle::Initialize(const Standard_Integer e1,
                                    const Standard_Integer e2,
                                    const Standard_Integer e3,
                                    const Standard_Boolean o1, 
                                    const Standard_Boolean o2,
                                    const Standard_Boolean o3,
                                    const BRepMesh_DegreeOfFreedom  canMove)
{
  Edge1        =e1;
  Edge2        =e2;
  Edge3        =e3;
  Orientation1 =o1;
  Orientation2 =o2;
  Orientation3 =o3;
  myMovability =canMove;
}

void  BRepMesh_Triangle::Edges(Standard_Integer& e1,
                               Standard_Integer& e2,
                               Standard_Integer& e3,
                               Standard_Boolean& o1, 
                               Standard_Boolean& o2,
                               Standard_Boolean& o3)const 
{
  e1=Edge1;
  e2=Edge2;
  e3=Edge3;
  o1=Orientation1;
  o2=Orientation2;
  o3=Orientation3;
}

void  BRepMesh_Triangle::SetMovability(const BRepMesh_DegreeOfFreedom  Move)
{
  myMovability =Move;
}

Standard_Integer BRepMesh_Triangle::HashCode
(const Standard_Integer Upper)const 
{
  return ::HashCode(Edge1+Edge2+Edge3, Upper);
}

Standard_Boolean BRepMesh_Triangle::IsEqual
(const BRepMesh_Triangle& Other)const 
{
  if (myMovability==BRepMesh_Deleted || Other.myMovability==BRepMesh_Deleted)
    return Standard_False;
  if (Edge1==Other.Edge1 && Edge2==Other.Edge2 && Edge3==Other.Edge3)
    return Standard_True;
  if (Edge1==Other.Edge2 && Edge2==Other.Edge3 && Edge3==Other.Edge1)
    return Standard_True;
  if (Edge1==Other.Edge3 && Edge2==Other.Edge1 && Edge3==Other.Edge2)
    return Standard_True;
  return Standard_False;
}
