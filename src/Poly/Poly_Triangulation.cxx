// Created on: 1995-03-06
// Created by: Laurent PAINNOT
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


#include <gp_Pnt.hxx>
#include <Poly_Triangle.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>

//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================
Poly_Triangulation::Poly_Triangulation(const Standard_Integer NbNodes, 
                                       const Standard_Integer NbTriangles,
                                       const Standard_Boolean UVNodes) :
   myDeflection(0),
   myNbNodes(NbNodes),
   myNbTriangles(NbTriangles),
   myNodes(1, NbNodes),
   myTriangles(1, NbTriangles)
{
  if (UVNodes) myUVNodes = new TColgp_HArray1OfPnt2d(1, myNbNodes);
}

//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================

Poly_Triangulation::Poly_Triangulation(const TColgp_Array1OfPnt&    Nodes, 
                                       const Poly_Array1OfTriangle& Triangles) :
   myDeflection(0),
   myNbNodes(Nodes.Length()),
   myNbTriangles(Triangles.Length()),
   myNodes(1, Nodes.Length()),
   myTriangles(1, Triangles.Length())
{
  myNodes = Nodes;
  myTriangles = Triangles;
}



//=======================================================================
//function : Poly_Triangulation
//purpose  : 
//=======================================================================

Poly_Triangulation::Poly_Triangulation(const TColgp_Array1OfPnt&    Nodes,
                                       const TColgp_Array1OfPnt2d&  UVNodes,
                                       const Poly_Array1OfTriangle& Triangles) :
   myDeflection(0),
   myNbNodes(Nodes.Length()),
   myNbTriangles(Triangles.Length()),
   myNodes(1, Nodes.Length()),
   myTriangles(1, Triangles.Length())
{
  myNodes = Nodes;
  myTriangles = Triangles;
  myUVNodes = new TColgp_HArray1OfPnt2d(1, myNbNodes);
  myUVNodes->ChangeArray1() = UVNodes;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Poly_Triangulation) Poly_Triangulation::Copy()
{
  Handle(Poly_Triangulation) aCopy;
  if (HasUVNodes())
    aCopy = new Poly_Triangulation(Nodes(), UVNodes(), Triangles());
  else
    aCopy = new Poly_Triangulation(Nodes(), Triangles());
  aCopy->Deflection(myDeflection);
  if (HasNormals())
    aCopy->myNormals = new TShort_HArray1OfShortReal(myNormals->Array1());

  return aCopy;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

Standard_Real Poly_Triangulation::Deflection() const 
{
  return myDeflection;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

void Poly_Triangulation::Deflection(const Standard_Real D)
{
  myDeflection = D;
}



//=======================================================================
//function : RemoveUVNodes
//purpose  : 
//=======================================================================

void Poly_Triangulation::RemoveUVNodes()
{
  myUVNodes.Nullify();
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

const TColgp_Array1OfPnt& Poly_Triangulation::Nodes() const 
{
  return myNodes;
}

//=======================================================================
//function : ChangeNodes
//purpose  : 
//=======================================================================

TColgp_Array1OfPnt& Poly_Triangulation::ChangeNodes()
{
  return myNodes;
}

//=======================================================================
//function : UVNodes
//purpose  : 
//=======================================================================

const TColgp_Array1OfPnt2d& Poly_Triangulation::UVNodes() const 
{
  return myUVNodes->Array1();
}

//=======================================================================
//function : ChangeUVNodes
//purpose  : 
//=======================================================================

TColgp_Array1OfPnt2d& Poly_Triangulation::ChangeUVNodes()
{
  return myUVNodes->ChangeArray1();
}

//=======================================================================
//function : Triangles
//purpose  : 
//=======================================================================

const Poly_Array1OfTriangle& Poly_Triangulation::Triangles() const 
{
  return myTriangles;
}

//=======================================================================
//function : ChangeTriangles
//purpose  : 
//=======================================================================

Poly_Array1OfTriangle& Poly_Triangulation::ChangeTriangles()
{
  return myTriangles;
}


//=======================================================================
//function : SetNormals
//purpose  : 
//=======================================================================

void Poly_Triangulation::SetNormals
                        (const Handle(TShort_HArray1OfShortReal)& theNormals)
{

  if(theNormals.IsNull() || theNormals->Length() != 3*myNbNodes) {
    Standard_DomainError::Raise("Poly_Triangulation::SetNormals : wrong length");
  }

  myNormals = theNormals;

}

//=======================================================================
//function : Normals
//purpose  : 
//=======================================================================

const TShort_Array1OfShortReal& Poly_Triangulation::Normals() const
{

  if(myNormals.IsNull() || myNormals->Length() != 3*myNbNodes) {
    Standard_NullObject::Raise("Poly_Triangulation::Normals : "
                               "wrong length or null array");
  }

  return myNormals->Array1();

}

//=======================================================================
//function : ChangeNormals
//purpose  : 
//=======================================================================

TShort_Array1OfShortReal& Poly_Triangulation::ChangeNormals() 
{

  if(myNormals.IsNull() || myNormals->Length() != 3*myNbNodes) {
    Standard_NullObject::Raise("Poly_Triangulation::ChangeNormals : "
                               "wrong length or null array");
  }

  return myNormals->ChangeArray1();

}

//=======================================================================
//function : HasNormals
//purpose  : 
//=======================================================================

Standard_Boolean Poly_Triangulation::HasNormals() const
{

  if(myNormals.IsNull() || myNormals->Length() != 3*myNbNodes) {
    return Standard_False;
  }
  return Standard_True;
}



