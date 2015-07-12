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
#include <Standard_Type.hxx>
#include <StlMesh_MeshTriangle.hxx>

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



