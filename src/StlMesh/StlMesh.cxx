// Created on: 1996-06-21
// Created by: Bruno TACCI
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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


#include <StlMesh.ixx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
#include <StlMesh_MeshTriangle.hxx>
#include <TColgp_SequenceOfXYZ.hxx>

//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

Handle(StlMesh_Mesh) StlMesh::Merge(const Handle(StlMesh_Mesh)& mesh1, const Handle(StlMesh_Mesh)& mesh2)
{
  Handle(StlMesh_Mesh) mergeMesh = new StlMesh_Mesh;
  StlMesh_SequenceOfMeshTriangle aSeqOfTriangle;
  TColgp_SequenceOfXYZ aSeqOfVertex;
  Standard_Real xn,yn,zn;
  Standard_Integer v1,v2,v3;

  // Chargement de mesh1 dans mergeMesh
  // Boucle sur les domaines puis sur les triangles

  Standard_Integer idom;
  for (idom = 1; idom <= mesh1->NbDomains(); idom++) {
    aSeqOfTriangle = mesh1->Triangles(idom);
    aSeqOfVertex = mesh1->Vertices(idom);
    mergeMesh->AddDomain(mesh1->Deflection(idom));
    
    for (Standard_Integer itri = 1; itri <= mesh1->NbTriangles(idom); itri++) {
      const Handle(StlMesh_MeshTriangle) aTrian = aSeqOfTriangle.Value(itri);
      aTrian->GetVertexAndOrientation(v1,v2,v3,xn,yn,zn);
      mergeMesh->AddTriangle(v1,v2,v3,xn,yn,zn);
    }
    
    for (Standard_Integer iver = 1; iver <= mesh1->NbVertices(idom); iver++) {
      mergeMesh->AddVertex(aSeqOfVertex.Value(iver).X(),
			   aSeqOfVertex.Value(iver).Y(),
			   aSeqOfVertex.Value(iver).Z());
    }
    
  }
  // Idem avec mesh2
  
  for (idom = 1; idom <= mesh2->NbDomains(); idom++) {
    aSeqOfTriangle = mesh2->Triangles(idom);
    aSeqOfVertex = mesh2->Vertices(idom);
    mergeMesh->AddDomain(mesh2->Deflection(idom));
    
    for (Standard_Integer itri = 1; itri <= mesh2->NbTriangles(idom); itri++) {
      const Handle(StlMesh_MeshTriangle) aTrian = aSeqOfTriangle.Value(itri);
      aTrian->GetVertexAndOrientation(v1,v2,v3,xn,yn,zn);
      mergeMesh->AddTriangle(v1,v2,v3,xn,yn,zn);
    }
    
    for (Standard_Integer iver = 1; iver <= mesh2->NbVertices(idom); iver++) {
      mergeMesh->AddVertex(aSeqOfVertex.Value(iver).X(),
			   aSeqOfVertex.Value(iver).Y(),
			   aSeqOfVertex.Value(iver).Z());
    }
  }
  return mergeMesh;
}
