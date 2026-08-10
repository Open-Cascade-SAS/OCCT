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
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

//=================================================================================================

const Standard_GUID& TDataXtd_Triangulation::GetID()
{
  static Standard_GUID TDataXtd_TriangulationID("27AE2C44-60B0-41AE-AC18-BA3FDA538D03");
  return TDataXtd_TriangulationID;
}

//=================================================================================================

occ::handle<TDataXtd_Triangulation> TDataXtd_Triangulation::Set(const TDF_Label& theLabel)
{
  occ::handle<TDataXtd_Triangulation> A;
  if (!theLabel.FindAttribute(TDataXtd_Triangulation::GetID(), A))
  {
    A = new TDataXtd_Triangulation;
    theLabel.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

occ::handle<TDataXtd_Triangulation> TDataXtd_Triangulation::Set(
  const TDF_Label&                       theLabel,
  const occ::handle<Poly_Triangulation>& theMesh)
{
  occ::handle<TDataXtd_Triangulation> M = TDataXtd_Triangulation::Set(theLabel);
  M->Set(theMesh);
  return M;
}

//=================================================================================================

TDataXtd_Triangulation::TDataXtd_Triangulation() = default;

//=================================================================================================

void TDataXtd_Triangulation::Set(const occ::handle<Poly_Triangulation>& theTriangulation)
{
  Backup();
  myTriangulation = theTriangulation;
}

//=================================================================================================

const occ::handle<Poly_Triangulation>& TDataXtd_Triangulation::Get() const
{
  return myTriangulation;
}

// Poly_Triangulation methods

// The methods are "covered" by this attribute to prevent direct modification of the mesh.
// There is no performance problem to call Poly_Triangulation method through this attribute.
// The most of the methods are considered as "inline" by the compiler in release mode.

//=================================================================================================

double TDataXtd_Triangulation::Deflection() const
{
  return myTriangulation->Deflection();
}

//=================================================================================================

void TDataXtd_Triangulation::Deflection(const double theDeflection)
{
  Backup();
  myTriangulation->Deflection(theDeflection);
}

//=================================================================================================

void TDataXtd_Triangulation::RemoveUVNodes()
{
  Backup();
  myTriangulation->RemoveUVNodes();
}

//=================================================================================================

int TDataXtd_Triangulation::NbNodes() const
{
  return myTriangulation->NbNodes();
}

//=================================================================================================

int TDataXtd_Triangulation::NbTriangles() const
{
  return myTriangulation->NbTriangles();
}

//=================================================================================================

bool TDataXtd_Triangulation::HasUVNodes() const
{
  return myTriangulation->HasUVNodes();
}

//=================================================================================================

gp_Pnt TDataXtd_Triangulation::Node(const int theIndex) const
{
  return myTriangulation->Node(theIndex);
}

//=================================================================================================

void TDataXtd_Triangulation::SetNode(const int theIndex, const gp_Pnt& theNode)
{
  Backup();
  myTriangulation->SetNode(theIndex, theNode);
}

//=================================================================================================

gp_Pnt2d TDataXtd_Triangulation::UVNode(const int theIndex) const
{
  return myTriangulation->UVNode(theIndex);
}

//=================================================================================================

void TDataXtd_Triangulation::SetUVNode(const int theIndex, const gp_Pnt2d& theUVNode)
{
  Backup();
  myTriangulation->SetUVNode(theIndex, theUVNode);
}

//=================================================================================================

Poly_Triangle TDataXtd_Triangulation::Triangle(const int theIndex) const
{
  return myTriangulation->Triangle(theIndex);
}

//=================================================================================================

void TDataXtd_Triangulation::SetTriangle(const int theIndex, const Poly_Triangle& theTriangle)
{
  Backup();
  myTriangulation->SetTriangle(theIndex, theTriangle);
}

//=================================================================================================

void TDataXtd_Triangulation::SetNormal(const int theIndex, const gp_Dir& theNormal)
{
  Backup();
  myTriangulation->SetNormal(theIndex, theNormal);
}

//=================================================================================================

bool TDataXtd_Triangulation::HasNormals() const
{
  return myTriangulation->HasNormals();
}

//=================================================================================================

gp_Dir TDataXtd_Triangulation::Normal(const int theIndex) const
{
  return myTriangulation->Normal(theIndex);
}

//=================================================================================================

const Standard_GUID& TDataXtd_Triangulation::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataXtd_Triangulation::NewEmpty() const
{
  return new TDataXtd_Triangulation();
}

//=================================================================================================

void TDataXtd_Triangulation::Restore(const occ::handle<TDF_Attribute>& theAttribute)
{
  myTriangulation.Nullify();
  occ::handle<TDataXtd_Triangulation> M = occ::down_cast<TDataXtd_Triangulation>(theAttribute);
  if (!M->myTriangulation.IsNull())
  {
    occ::handle<Poly_Triangulation> T = M->myTriangulation->Copy();
    if (!T.IsNull())
    {
      myTriangulation = T;
    }
  }
}

//=================================================================================================

void TDataXtd_Triangulation::Paste(const occ::handle<TDF_Attribute>& theIntoAttribute,
                                   const occ::handle<TDF_RelocationTable>&) const
{
  occ::handle<TDataXtd_Triangulation> M = occ::down_cast<TDataXtd_Triangulation>(theIntoAttribute);
  M->myTriangulation.Nullify();
  if (!myTriangulation.IsNull())
  {
    occ::handle<Poly_Triangulation> T = myTriangulation->Copy();
    if (!T.IsNull())
    {
      M->myTriangulation = T;
    }
  }
}

//=================================================================================================

Standard_OStream& TDataXtd_Triangulation::Dump(Standard_OStream& anOS) const
{
  anOS << "Triangulation";
  // TODO: Make a good dump.
  return anOS;
}
