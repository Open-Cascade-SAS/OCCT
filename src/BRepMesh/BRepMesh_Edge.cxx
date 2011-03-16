// File:	BRepMesh_Edge.cxx
// Created:	Thu Sep 23 13:39:58 1993
// Author:	Didier PIFFAULT
//		<dpf@zerox>
  
#include <BRepMesh_Edge.ixx>
  
BRepMesh_Edge::BRepMesh_Edge(const Standard_Integer vDebut,
			     const Standard_Integer vFin,
			     const MeshDS_DegreeOfFreedom canMove)
     : myFirstNode(vDebut), myLastNode(vFin), myMovability(canMove)
{}

void  BRepMesh_Edge::SetMovability(const MeshDS_DegreeOfFreedom Move)
{
  myMovability =Move;
}

Standard_Integer  BRepMesh_Edge::HashCode(const Standard_Integer Upper)const 
{
  return ::HashCode(myFirstNode+myLastNode, Upper);
}

Standard_Boolean  BRepMesh_Edge::IsEqual(const BRepMesh_Edge& Other)const 
{
  if (myMovability==MeshDS_Deleted || Other.myMovability==MeshDS_Deleted)
    return Standard_False;
  return (myFirstNode==Other.myFirstNode && myLastNode==Other.myLastNode) ||
    (myFirstNode==Other.myLastNode && myLastNode==Other.myFirstNode);
}


Standard_Boolean  BRepMesh_Edge::SameOrientation
  (const BRepMesh_Edge& Other)const 
{
  return (myFirstNode==Other.myFirstNode && myLastNode==Other.myLastNode);
}
