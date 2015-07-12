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

#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <Standard_NegativeValue.hxx>
#include <Standard_NullValue.hxx>
#include <Standard_Type.hxx>
#include <StlMesh_MeshDomain.hxx>
#include <StlMesh_MeshTriangle.hxx>

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






