//=======================================================================
// File:	StlMesh_MeshDomain.cxx
// Created:	Mon Sep 25 11:24:02 1995
// Author:	Philippe GIRODENGO
// Copyright:    Matra Datavision	

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






