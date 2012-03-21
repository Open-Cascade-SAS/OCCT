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

#include <StlMesh_MeshTriangle.ixx>
#include <Precision.hxx>
#include <gp_XYZ.hxx>

//=======================================================================
//function : StlMesh_MeshTriangle
//design   : 
//warning  : 
//=======================================================================

StlMesh_MeshTriangle::StlMesh_MeshTriangle()
     : MyV1 (0), MyV2 (0), MyV3 (0), MyXn (0.0), MyYn (0.0), MyZn (0.0) { }


//=======================================================================
//function : StlMesh_MeshTriangle
//design   : 
//warning  : 
//=======================================================================

     StlMesh_MeshTriangle::StlMesh_MeshTriangle(const Standard_Integer V1, 
						const Standard_Integer V2, 
						const Standard_Integer V3, 
						const Standard_Real Xn, 
						const Standard_Real Yn, 
						const Standard_Real Zn)
: MyV1 (V1), MyV2 (V2), MyV3 (V3), MyXn (Xn), MyYn (Yn), MyZn (Zn) { }


//=======================================================================
//function : GetVertexAndOrientation
//design   : 
//warning  : 
//=======================================================================

void StlMesh_MeshTriangle::GetVertexAndOrientation(Standard_Integer& V1, 
						   Standard_Integer& V2, 
						   Standard_Integer& V3, 
						   Standard_Real& Xn, 
						   Standard_Real& Yn, 
						   Standard_Real& Zn) const 
{
  V1 = MyV1;
  V2 = MyV2;
  V3 = MyV3;
  Xn = MyXn;
  Yn = MyYn;
  Zn = MyZn;
}

//=======================================================================
//function : SetVertexAndOrientation
//design   : 
//warning  : 
//=======================================================================

void StlMesh_MeshTriangle::SetVertexAndOrientation(const Standard_Integer V1, const Standard_Integer V2, 
						   const Standard_Integer V3, const Standard_Real Xn, 
						   const Standard_Real Yn, const Standard_Real Zn)
{
  MyV1 = V1;
  MyV2 = V2;
  MyV3 = V3;
  MyXn = Xn;
  MyYn = Yn;
  MyZn = Zn;
}

//=======================================================================
//function : GetVertex
//design   : 
//warning  : 
//=======================================================================

void StlMesh_MeshTriangle::GetVertex(Standard_Integer& V1, Standard_Integer& V2, Standard_Integer& V3) const 
{
  V1 = MyV1;
  V2 = MyV2;
  V3 = MyV3;
}

//=======================================================================
//function : SetVertex
//design   : 
//warning  : 
//=======================================================================

void StlMesh_MeshTriangle::SetVertex(const Standard_Integer V1, const Standard_Integer V2, const Standard_Integer V3)
{
  MyV1 = V1;
  MyV2 = V2;
  MyV3 = V3;
}



