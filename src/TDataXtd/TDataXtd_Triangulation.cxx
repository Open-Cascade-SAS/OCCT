// Created on: 2016-11-10
// Created by: Anton KOZULIN
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <TDataXtd_Triangulation.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

//=======================================================================
//function : GetID
//purpose  : Returns the ID of the triangulation attribute.
//=======================================================================
const Standard_GUID& TDataXtd_Triangulation::GetID()
{
  static Standard_GUID TDataXtd_TriangulationID ("27AE2C44-60B0-41AE-AC18-BA3FDA538D03");
  return TDataXtd_TriangulationID; 
}

//=======================================================================
//function : Set
//purpose  : Finds or creates a triangulation attribute.
//=======================================================================
Handle(TDataXtd_Triangulation) TDataXtd_Triangulation::Set(const TDF_Label& theLabel)
{
  Handle(TDataXtd_Triangulation) A;
  if (!theLabel.FindAttribute (TDataXtd_Triangulation::GetID(), A))
  {
    A = new TDataXtd_Triangulation;
    theLabel.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : Set
//purpose  : Finds or creates a triangulation attribute.
//           Initializes the attribute by a Poly_Triangulation object.
//=======================================================================
Handle(TDataXtd_Triangulation) TDataXtd_Triangulation::Set(const TDF_Label& theLabel, const Handle(Poly_Triangulation)& theMesh)
{
  Handle(TDataXtd_Triangulation) M = TDataXtd_Triangulation::Set(theLabel);
  M->Set(theMesh);
  return M;
}

//=======================================================================
//function : TDataXtd_Triangulation
//purpose  : A constructor.
//           Don't use it directly, 
//           use please the static method Set(),
//           which returns the attribute attached to a label.
//=======================================================================
TDataXtd_Triangulation::TDataXtd_Triangulation()
{

}

//=======================================================================
//function : TDataXtd_Triangulation
//purpose  : Sets the triangulation.
//=======================================================================
void TDataXtd_Triangulation::Set(const Handle(Poly_Triangulation)& theTriangulation)
{
  Backup();
  myTriangulation = theTriangulation;
}

//=======================================================================
//function : TDataXtd_Triangulation
//purpose  : Returns the underlying mesh.
//=======================================================================
const Handle(Poly_Triangulation)& TDataXtd_Triangulation::Get() const
{
  return myTriangulation;
}

// Poly_Triangulation methods

// The methods are "covered" by this attribute to prevent direct modification of the mesh.
// There is no performance problem to call Poly_Triangulation method through this attribute.
// The most of the methods are considered as "inline" by the compiler in release mode.

//=======================================================================
//function : Deflection
//purpose  : Returns the deflection of this triangulation.
//=======================================================================
Standard_Real TDataXtd_Triangulation::Deflection() const
{
  return myTriangulation->Deflection();
}

//=======================================================================
//function : Deflection
//purpose  : Sets the deflection of this triangulation to theDeflection.
//           See more on deflection in Polygon2D
//=======================================================================
void TDataXtd_Triangulation::Deflection (const Standard_Real theDeflection)
{
  Backup();
  myTriangulation->Deflection(theDeflection);
}

//=======================================================================
//function : RemoveUVNodes
//purpose  : Deallocates the UV nodes.
//=======================================================================
void TDataXtd_Triangulation::RemoveUVNodes()
{
  Backup();
  myTriangulation->RemoveUVNodes();
}

//=======================================================================
//function : NbNodes
//purpose  : return the number of nodes for this triangulation.
//=======================================================================
Standard_Integer TDataXtd_Triangulation::NbNodes() const
{
  return myTriangulation->NbNodes();
}

//=======================================================================
//function : NbTriangles
//purpose  : return the number of triangles for this triangulation.
//=======================================================================
Standard_Integer TDataXtd_Triangulation::NbTriangles() const
{
  return myTriangulation->NbTriangles();
}

//=======================================================================
//function : HasUVNodes
//purpose  : return Standard_True if 2D nodes are associated with 3D nodes for this triangulation.
//=======================================================================
Standard_Boolean TDataXtd_Triangulation::HasUVNodes() const
{
  return myTriangulation->HasUVNodes();
}

//=======================================================================
//function : Node
//purpose  : return node at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
//=======================================================================
const gp_Pnt& TDataXtd_Triangulation::Node (const Standard_Integer theIndex) const
{
  return myTriangulation->Node(theIndex);
}

//=======================================================================
//function : SetNode
//purpose  : The method differs from Poly_Triangulation
//           Sets a node at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
//=======================================================================
void TDataXtd_Triangulation::SetNode (const Standard_Integer theIndex, const gp_Pnt& theNode)
{
  Backup();
  myTriangulation->ChangeNode(theIndex) = theNode;
}

//=======================================================================
//function : UVNode
//purpose  : return UVNode at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
//=======================================================================
const gp_Pnt2d& TDataXtd_Triangulation::UVNode (const Standard_Integer theIndex) const
{
  return myTriangulation->UVNode(theIndex);
}

//=======================================================================
//function : SetUVNode
//purpose  : The method differs from Poly_Triangulation
//           Sets a UVNode at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbNodes.
//=======================================================================
void TDataXtd_Triangulation::SetUVNode (const Standard_Integer theIndex, const gp_Pnt2d& theUVNode)
{
  Backup();
  myTriangulation->ChangeUVNode(theIndex) = theUVNode;
}

//=======================================================================
//function : Triangle
//purpose  : return triangle at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbTriangles.
//=======================================================================
const Poly_Triangle& TDataXtd_Triangulation::Triangle (const Standard_Integer theIndex) const
{
  return myTriangulation->Triangle(theIndex);
}

//=======================================================================
//function : SetTriangle
//purpose  : The method differs from Poly_Triangulation
//           Sets a triangle at the given index.
//           Raises Standard_OutOfRange exception if theIndex is less than 1 or greater than NbTriangles.
//=======================================================================
void TDataXtd_Triangulation::SetTriangle (const Standard_Integer theIndex, const Poly_Triangle& theTriangle)
{
  Backup();
  myTriangulation->ChangeTriangle(theIndex) = theTriangle;
}

//=======================================================================
//function : SetNormals
//purpose  : Sets the table of node normals.
//           Raises exception if length of theNormals = 3 * NbNodes
//=======================================================================
void TDataXtd_Triangulation::SetNormals (const Handle(TShort_HArray1OfShortReal)& theNormals)
{
  Backup();
  myTriangulation->SetNormals(theNormals);
}

//=======================================================================
//function : SetNormal
//purpose  : Changes normal at the given index.
//           Raises Standard_OutOfRange exception.
//=======================================================================
void TDataXtd_Triangulation::SetNormal (const Standard_Integer theIndex,
                               const gp_Dir&          theNormal)
{
  Backup();
  myTriangulation->SetNormal(theIndex, theNormal);
}

//=======================================================================
//function : HasNormals
//purpose  : Returns Standard_True if nodal normals are defined.
//=======================================================================
Standard_Boolean TDataXtd_Triangulation::HasNormals() const
{
  return myTriangulation->HasNormals();
}

//=======================================================================
//function : Normal
//purpose  : return normal at the given index.
//           Raises Standard_OutOfRange exception.
//=======================================================================
const gp_Dir TDataXtd_Triangulation::Normal (const Standard_Integer theIndex) const
{
  return myTriangulation->Normal(theIndex);
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_Triangulation::ID () const
{
  return GetID();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataXtd_Triangulation::NewEmpty () const
{  
  return new TDataXtd_Triangulation(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataXtd_Triangulation::Restore(const Handle(TDF_Attribute)& theAttribute)
{
  myTriangulation.Nullify();
  Handle(TDataXtd_Triangulation) M = Handle(TDataXtd_Triangulation)::DownCast(theAttribute);
  if (!M->myTriangulation.IsNull())
  {
    Handle(Poly_Triangulation) T = M->myTriangulation->Copy();
    if (!T.IsNull())
      myTriangulation = T;
  }
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataXtd_Triangulation::Paste (const Handle(TDF_Attribute)& theIntoAttribute,
                                    const Handle(TDF_RelocationTable)& ) const
{
  Handle(TDataXtd_Triangulation) M = Handle(TDataXtd_Triangulation)::DownCast(theIntoAttribute);
  M->myTriangulation.Nullify();
  if (!myTriangulation.IsNull())
  {
      Handle(Poly_Triangulation) T = myTriangulation->Copy();
      if (!T.IsNull())
        M->myTriangulation = T;
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataXtd_Triangulation::Dump (Standard_OStream& anOS) const
{
  anOS << "Triangulation";
  //TODO: Make a good dump.
  return anOS;
}
