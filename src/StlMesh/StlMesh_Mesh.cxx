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
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullValue.hxx>
#include <Standard_Type.hxx>
#include <StlMesh_Mesh.hxx>
#include <StlMesh_MeshDomain.hxx>

//=======================================================================
//function : StlMesh_Mesh
//design   : 
//warning  : 
//=======================================================================
StlMesh_Mesh::StlMesh_Mesh()
: nbTriangles (0), nbVertices (0), xyzmax (-(Precision::Infinite()), -(Precision::Infinite()), -(Precision::Infinite())), xyzmin (Precision::Infinite(), Precision::Infinite(), Precision::Infinite())
{ }

//=======================================================================
//function : AddDomain
//design   : 
//warning  : 
//=======================================================================

void StlMesh_Mesh::AddDomain()
{
  Handle(StlMesh_MeshDomain) MD = new StlMesh_MeshDomain;
  domains.Append (MD);
}

//=======================================================================
//function : AddDomain
//design   : 
//warning  : 
//=======================================================================

void StlMesh_Mesh::AddDomain(const Standard_Real Deflection)
{
  Handle(StlMesh_MeshDomain) MD = new StlMesh_MeshDomain (Deflection);
  domains.Append (MD);
}

//=======================================================================
//function : AddTriangle
//design   : 
//warning  : 
//=======================================================================

Standard_Integer StlMesh_Mesh::AddTriangle(const Standard_Integer V1, const Standard_Integer V2, 
					   const Standard_Integer V3, const Standard_Real Xn, 
					   const Standard_Real Yn, const Standard_Real Zn)
{
  nbTriangles++;
  return (domains.Last())->AddTriangle (V1, V2, V3, Xn, Yn, Zn);
}

//=======================================================================
//function : AddVertex
//design   : 
//warning  : 
//=======================================================================

Standard_Integer StlMesh_Mesh::AddVertex(const Standard_Real X, const Standard_Real Y, const Standard_Real Z)
{
  nbVertices++;
  if (X > xyzmax.X()) xyzmax.SetX (X);
  if (Y > xyzmax.Y()) xyzmax.SetY (Y);
  if (Z > xyzmax.Z()) xyzmax.SetZ (Z);
  if (X < xyzmin.X()) xyzmin.SetX (X);
  if (Y < xyzmin.Y()) xyzmin.SetY (Y);
  if (Z < xyzmin.Z()) xyzmin.SetZ (Z);
  
  return (domains.Last())->AddVertex (X, Y, Z);
}

//=======================================================================
//function : AddOnlyNewVertex
//design   : 
//warning  : 
//=======================================================================

Standard_Integer StlMesh_Mesh::AddOnlyNewVertex(const Standard_Real X, const Standard_Real Y, const Standard_Real Z)
{
  Standard_Boolean IsNew = Standard_True;
  Standard_Integer VIndex = (domains.Last())->AddOnlyNewVertex (X, Y, Z, IsNew); 
  if (IsNew) nbVertices++;
  return VIndex;
}

//=======================================================================
//function : Bounds
//design   : 
//warning  : 
//=======================================================================

void StlMesh_Mesh::Bounds(gp_XYZ& XYZmax, gp_XYZ& XYZmin) const 
{
  XYZmax = xyzmax;
  XYZmin = xyzmin;
}

//=======================================================================
//function : Clear
//design   : 
//warning  : 
//=======================================================================

void StlMesh_Mesh::Clear()
{
  nbTriangles = 0;
  nbVertices  = 0;
  xyzmax.SetCoord(-(Precision::Infinite()),-(Precision::Infinite()),-(Precision::Infinite()));
  xyzmin.SetCoord(Precision::Infinite(),Precision::Infinite(),Precision::Infinite()); 
  domains.Clear ();
}

//=======================================================================
//function : Deflection
//design   : 
//warning  : 
//=======================================================================

Standard_Real StlMesh_Mesh::Deflection(const Standard_Integer DomainIndex) const 
{return (domains.Value (DomainIndex))->Deflection ();}

//=======================================================================
//function : NbTriangles
//design   : 
//warning  : 
//=======================================================================

Standard_Integer StlMesh_Mesh::NbTriangles(const Standard_Integer DomainIndex) const 
{ return (domains.Value(DomainIndex))->NbTriangles ();}

//=======================================================================
//function : NbVertices
//design   : 
//warning  : 
//=======================================================================

Standard_Integer StlMesh_Mesh::NbVertices(const Standard_Integer DomainIndex) const 
{ return (domains.Value(DomainIndex))->NbVertices ();}

//=======================================================================
//function : Triangles
//design   : 
//warning  : 
//=======================================================================

const StlMesh_SequenceOfMeshTriangle& StlMesh_Mesh::Triangles(const Standard_Integer DomainIndex) const 
{ return (domains.Value (DomainIndex))->Triangles ();}

//=======================================================================
//function : Vertices
//design   : 
//warning  : 
//=======================================================================

const TColgp_SequenceOfXYZ& StlMesh_Mesh::Vertices(const Standard_Integer DomainIndex) const 
{ return (domains.Value (DomainIndex))->Vertices ();}

