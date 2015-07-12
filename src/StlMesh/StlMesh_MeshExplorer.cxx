// Created on: 1995-09-25
// Created by: Philippe GIRODENGO
// Copyright (c) 1995-1999 Matra Datavision
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

//=======================================================================

#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshExplorer.hxx>
#include <StlMesh_MeshTriangle.hxx>

//=======================================================================
//function : StlMesh_MeshExplorer
//design   : 
//warning  : 
//=======================================================================
StlMesh_MeshExplorer::StlMesh_MeshExplorer(const Handle(StlMesh_Mesh)& M)
     : domainIndex (0), nbTriangles (0) ,triangleIndex (0)  { mesh = M;}
     

//=======================================================================
//function : Deflection
//design   : 
//warning  : 
//=======================================================================

Standard_Real StlMesh_MeshExplorer::Deflection() const 
{ return mesh->Deflection (domainIndex);}


//=======================================================================
//function : InitTriangle
//design   : 
//warning  : 
//=======================================================================

void StlMesh_MeshExplorer::InitTriangle(const Standard_Integer DomainIndex)
{
  triangleIndex = 1;
  domainIndex = DomainIndex;
  nbTriangles = mesh->NbTriangles (domainIndex);
  if (nbTriangles > 0) {
    trianglesdef.Assign (mesh->Triangles (DomainIndex));
    trianglesVertex.Assign (mesh->Vertices (DomainIndex));
    const Handle (StlMesh_MeshTriangle) trian = trianglesdef.First();
    trian->GetVertexAndOrientation (v1,v2,v3,xn,yn,zn);
  }
}

//=======================================================================
//function : NextTriangle
//design   : 
//warning  : 
//=======================================================================

void StlMesh_MeshExplorer::NextTriangle()
{
  triangleIndex++;
  if (triangleIndex <= nbTriangles) {
    const Handle (StlMesh_MeshTriangle) trian = trianglesdef.Value (triangleIndex);
    trian->GetVertexAndOrientation (v1,v2,v3,xn,yn,zn);
  }
}

//=======================================================================
//function : TriangleVertices
//design   : 
//warning  : 
//=======================================================================

void StlMesh_MeshExplorer::TriangleVertices(Standard_Real& X1, Standard_Real& Y1, 
					    Standard_Real& Z1, Standard_Real& X2, 
					    Standard_Real& Y2, Standard_Real& Z2, 
					    Standard_Real& X3, Standard_Real& Y3, 
					    Standard_Real& Z3) const 
{
  Standard_NoSuchObject_Raise_if (triangleIndex > nbTriangles, " StlMesh_MeshExplorer::TriangleVertices");
  
  X1 = (trianglesVertex.Value(v1)).X();
  Y1 = (trianglesVertex.Value(v1)).Y();
  Z1 = (trianglesVertex.Value(v1)).Z();
  X2 = (trianglesVertex.Value(v2)).X();
  Y2 = (trianglesVertex.Value(v2)).Y();
  Z2 = (trianglesVertex.Value(v2)).Z();
  X3 = (trianglesVertex.Value(v3)).X();
  Y3 = (trianglesVertex.Value(v3)).Y();
  Z3 = (trianglesVertex.Value(v3)).Z();
}

//=======================================================================
//function : TriangleDirection
//design   : 
//warning  : 
//=======================================================================

void StlMesh_MeshExplorer::TriangleOrientation(Standard_Real& Xn, Standard_Real& Yn, Standard_Real& Zn) const 
{
  Xn = xn;
  Yn = yn;
  Zn = zn;
}






