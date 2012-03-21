// Created on: 1995-09-25
// Created by: Philippe GIRODENGO
// Copyright (c) 1995-1999 Matra Datavision
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

//=======================================================================
#include <StlMesh_MeshExplorer.ixx>
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






