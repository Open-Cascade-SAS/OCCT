// Created on: 1996-06-21
// Created by: Bruno TACCI
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <StlMesh.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshTriangle.hxx>
#include <StlMesh_SequenceOfMeshTriangle.hxx>
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
