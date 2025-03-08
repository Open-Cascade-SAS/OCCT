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

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd.hxx>
#include <TDataXtd_Point.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(TDataXtd_Point, TDataStd_GenericEmpty)

//=================================================================================================

const Standard_GUID& TDataXtd_Point::GetID()
{
  static Standard_GUID TDataXtd_PointID("2a96b60d-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_PointID;
}

//=================================================================================================

Handle(TDataXtd_Point) TDataXtd_Point::Set(const TDF_Label& L)
{
  Handle(TDataXtd_Point) A;
  if (!L.FindAttribute(TDataXtd_Point::GetID(), A))
  {
    A = new TDataXtd_Point();
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

Handle(TDataXtd_Point) TDataXtd_Point::Set(const TDF_Label& L, const gp_Pnt& P)
{
  Handle(TDataXtd_Point) A = Set(L);

  Handle(TNaming_NamedShape) aNS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(), aNS))
  {
    if (!aNS->Get().IsNull())
      if (aNS->Get().ShapeType() == TopAbs_VERTEX)
      {
        gp_Pnt anOldPnt = BRep_Tool::Pnt(TopoDS::Vertex(aNS->Get()));
        if (anOldPnt.X() == P.X() && anOldPnt.Y() == P.Y() && anOldPnt.Z() == P.Z())
          return A;
      }
  }

  TNaming_Builder B(L);
  B.Generated(BRepBuilderAPI_MakeVertex(P));
  return A;
}

//=================================================================================================

TDataXtd_Point::TDataXtd_Point() {}

//=================================================================================================

const Standard_GUID& TDataXtd_Point::ID() const
{
  return GetID();
}

//=================================================================================================

Standard_OStream& TDataXtd_Point::Dump(Standard_OStream& anOS) const
{
  anOS << "Point";
  return anOS;
}
