// File:        BRepMesh_Vertex.cxx
// Created:     Thu Sep 23 12:46:51 1993
// Author:      Didier PIFFAULT
//              <dpf@zerox>

#include <BRepMesh_Vertex.ixx>
#include <Precision.hxx>


BRepMesh_Vertex::BRepMesh_Vertex()
: myLocation(0), myMovability(BRepMesh_Free)
{}

BRepMesh_Vertex::BRepMesh_Vertex(const gp_XY& UV,
                                 const Standard_Integer Locat3d,
                                 const BRepMesh_DegreeOfFreedom Move)
                                 : myUV(UV), myLocation(Locat3d), myMovability(Move)
{}

BRepMesh_Vertex::BRepMesh_Vertex(const Standard_Real U,
                                 const Standard_Real V,
                                 const BRepMesh_DegreeOfFreedom Move)
                                 : myUV(U, V), myLocation(0), myMovability(Move)
{}

void  BRepMesh_Vertex::Initialize(const gp_XY& UV,
                                  const Standard_Integer Locat3d,
                                  const BRepMesh_DegreeOfFreedom Move)
{
  myUV=UV;
  myLocation=Locat3d;
  myMovability=Move;
}

void  BRepMesh_Vertex::SetMovability(const BRepMesh_DegreeOfFreedom Move)
{
  myMovability=Move;
}

//=======================================================================
//function : HashCode IsEqual 
//purpose  : Services for Map
//=======================================================================
Standard_Integer  BRepMesh_Vertex::HashCode(const Standard_Integer Upper)const
{
  return ::HashCode (Floor(1e5*myUV.X())*Floor(1e5*myUV.Y()), Upper);
}

Standard_Boolean  BRepMesh_Vertex::IsEqual(const BRepMesh_Vertex& Other)const
{
  if (myMovability!=BRepMesh_Deleted && Other.myMovability!=BRepMesh_Deleted)
    return (myUV.IsEqual(Other.myUV, Precision::PConfusion()));
  return Standard_False;
}

