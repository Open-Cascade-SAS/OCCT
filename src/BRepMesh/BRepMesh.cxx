// File:	BRepMesh.cxx
// Created:	Wed Aug  7 10:19:24 1996
// Author:	Laurent PAINNOT
//		<lpa@penox.paris1.matra-dtv.fr>


#include <BRepMesh.ixx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <TopoDS_Shape.hxx>


void BRepMesh::Mesh(const TopoDS_Shape& S,
		    const Standard_Real d)
{
  BRepMesh_IncrementalMesh M(S, d);

}
