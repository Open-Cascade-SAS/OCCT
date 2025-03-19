// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#include <BRep_Builder.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd.hxx>
#include <TDataXtd_Shape.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(TDataXtd_Shape, TDataStd_GenericEmpty)

//=================================================================================================

Standard_Boolean TDataXtd_Shape::Find(const TDF_Label& current, Handle(TDataXtd_Shape)& S)
{
  TDF_Label              L = current;
  Handle(TDataXtd_Shape) SA;
  if (L.IsNull())
    return Standard_False;
  for (;;)
  {
    if (L.FindAttribute(TDataXtd_Shape::GetID(), SA))
      break;
    L = L.Father();
    if (L.IsNull())
      break;
  }

  if (!SA.IsNull())
  {
    S = SA;
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

Handle(TDataXtd_Shape) TDataXtd_Shape::New(const TDF_Label& label)
{
  if (label.HasAttribute())
  {
    throw Standard_DomainError("TDataXtd_Shape::New : not an empty label");
  }
  Handle(TDataXtd_Shape) A = new TDataXtd_Shape();
  label.AddAttribute(A);
  return A;
}

//=================================================================================================

Handle(TDataXtd_Shape) TDataXtd_Shape::Set(const TDF_Label& label, const TopoDS_Shape& shape)
{
  Handle(TDataXtd_Shape) A;
  if (!label.FindAttribute(TDataXtd_Shape::GetID(), A))
  {
    A = TDataXtd_Shape::New(label);
  }

  Handle(TNaming_NamedShape) aNS;
  if (label.FindAttribute(TNaming_NamedShape::GetID(), aNS))
  {
    if (!aNS->Get().IsNull())
      if (aNS->Get() == shape)
        return A;
  }

  TNaming_Builder B(label);
  B.Generated(shape);
  return A;
}

//=================================================================================================

TopoDS_Shape TDataXtd_Shape::Get(const TDF_Label& label)
{
  TopoDS_Shape shape;

  Handle(TNaming_NamedShape) NS;
  if (label.FindAttribute(TNaming_NamedShape::GetID(), NS))
  {
    shape = TNaming_Tool::GetShape(NS);
    return shape;
  }

  shape.Nullify();
  return shape;
}

//=================================================================================================

const Standard_GUID& TDataXtd_Shape::GetID()
{
  static Standard_GUID TDataXtd_ShapeID("2a96b620-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_ShapeID;
}

//=================================================================================================

TDataXtd_Shape::TDataXtd_Shape() {}

//=================================================================================================

const Standard_GUID& TDataXtd_Shape::ID() const
{
  return GetID();
}

//=================================================================================================

void TDataXtd_Shape::References(const Handle(TDF_DataSet)&) const {}

//=================================================================================================

Standard_OStream& TDataXtd_Shape::Dump(Standard_OStream& anOS) const
{
  anOS << "Shape";
  return anOS;
}
