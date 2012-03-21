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

#include <StlMesh_MeshDomain.ixx>
#include <StlMesh_MeshTriangle.hxx>
#include <Precision.hxx>
#include <gp_XYZ.hxx>

//=======================================================================
//function : StlMesh_MeshDomain
//design   : 
//warning  : 
//=======================================================================

StlMesh_MeshDomain::StlMesh_MeshDomain() : deflection (Precision::Confusion ()), nbVertices (0), nbTriangles (0) 
{ 
}


//=======================================================================
//function : StlMesh_MeshDomain
//design   : 
//warning  : 
//=======================================================================

StlMesh_MeshDomain::StlMesh_MeshDomain(const Standard_Real Deflection)
     : deflection (Deflection), nbVertices (0), nbTriangles (0) { }


//=======================================================================
//function : AddTriangle
//design   : 
//warning  : 
//=======================================================================

     Standard_Integer StlMesh_MeshDomain::AddTriangle(const Standard_Integer V1, 
						      const Standard_Integer V2, const Standard_Integer V3, 
						      const Standard_Real Xn, const Standard_Real Yn, 
						      const Standard_Real Zn)
{
  const Handle (StlMesh_MeshTriangle) tri = new StlMesh_MeshTriangle (V1, V2, V3, Xn, Yn, Zn);
  trianglesVertex.Append (tri);
  nbTriangles++;
  return nbTriangles;
}

//=======================================================================
//function : AddVertex
//design   : 
//warning  : 
//=======================================================================

Standard_Integer StlMesh_MeshDomain::AddVertex(const Standard_Real X, const Standard_Real Y, const Standard_Real Z)
{
  gp_XYZ Vx (X, Y, Z);
  vertexCoords.Append (Vx);
  nbVertices++;
  return nbVertices;
}

//=======================================================================
//function : AddOnlyNewVertex
//design   : Adds the vertex only if X and Y and Z doesn`t already exists.
//=======================================================================

Standard_Integer StlMesh_MeshDomain::AddOnlyNewVertex(const Standard_Real X, 
						      const Standard_Real Y, 
						      const Standard_Real Z, 
						      Standard_Boolean& IsNew)
{
  gp_XYZ Vx (X, Y, Z);
  IsNew = Standard_True;
  vertexCoords.Append (Vx);
  nbVertices++;
  return nbVertices;
}






