// Created on: 2009-04-06
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
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GeomAbs_CurveType.hxx>
#include <gp_Lin.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd.hxx>
#include <TDataXtd_Axis.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(TDataXtd_Axis, TDataStd_GenericEmpty)

//=================================================================================================

const Standard_GUID& TDataXtd_Axis::GetID()
{
  static Standard_GUID TDataXtd_AxisID("2a96b601-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_AxisID;
}

//=================================================================================================

Handle(TDataXtd_Axis) TDataXtd_Axis::Set(const TDF_Label& L)
{
  Handle(TDataXtd_Axis) A;
  if (!L.FindAttribute(TDataXtd_Axis::GetID(), A))
  {
    A = new TDataXtd_Axis();
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

Handle(TDataXtd_Axis) TDataXtd_Axis::Set(const TDF_Label& L, const gp_Lin& line)
{
  Handle(TDataXtd_Axis) A = Set(L);

  Handle(TNaming_NamedShape) aNS;
  if (L.FindAttribute(TNaming_NamedShape::GetID(), aNS))
  {
    if (!aNS->Get().IsNull())
      if (aNS->Get().ShapeType() == TopAbs_EDGE)
      {
        TopoDS_Edge       anEdge = TopoDS::Edge(aNS->Get());
        BRepAdaptor_Curve anAdaptor(anEdge);
        if (anAdaptor.GetType() == GeomAbs_Line)
        {
          gp_Lin anOldLine = anAdaptor.Line();
          if (anOldLine.Direction().X() == line.Direction().X()
              && anOldLine.Direction().Y() == line.Direction().Y()
              && anOldLine.Direction().Z() == line.Direction().Z()
              && anOldLine.Location().X() == line.Location().X()
              && anOldLine.Location().Y() == line.Location().Y()
              && anOldLine.Location().Z() == line.Location().Z())
            return A;
        }
      }
  }
  TNaming_Builder B(L);
  B.Generated(BRepBuilderAPI_MakeEdge(line));
  return A;
}

//=================================================================================================

TDataXtd_Axis::TDataXtd_Axis() {}

//=================================================================================================

const Standard_GUID& TDataXtd_Axis::ID() const
{
  return GetID();
}

//=================================================================================================

Standard_OStream& TDataXtd_Axis::Dump(Standard_OStream& anOS) const
{
  anOS << "Axis";
  return anOS;
}
