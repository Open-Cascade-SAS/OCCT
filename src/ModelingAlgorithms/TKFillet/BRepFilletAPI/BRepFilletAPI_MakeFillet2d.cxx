// Created on: 1995-08-31
// Created by: Remi LEQUETTE
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

#include <BRepFilletAPI_MakeFillet2d.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

BRepFilletAPI_MakeFillet2d::BRepFilletAPI_MakeFillet2d() {}

//=================================================================================================

BRepFilletAPI_MakeFillet2d::BRepFilletAPI_MakeFillet2d(const TopoDS_Face& F)
{
  myMakeChFi2d.Init(F);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet2d::Init(const TopoDS_Face& F)
{
  myMakeChFi2d.Init(F);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet2d::Init(const TopoDS_Face& RefFace, const TopoDS_Face& ModFace)
{
  myMakeChFi2d.Init(RefFace, ModFace);
}

//=================================================================================================

TopoDS_Edge BRepFilletAPI_MakeFillet2d::AddFillet(const TopoDS_Vertex& V, const double Radius)
{
  return myMakeChFi2d.AddFillet(V, Radius);
}

//=================================================================================================

TopoDS_Edge BRepFilletAPI_MakeFillet2d::ModifyFillet(const TopoDS_Edge& Fillet, const double Radius)
{
  return myMakeChFi2d.ModifyFillet(Fillet, Radius);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet2d::RemoveFillet(const TopoDS_Edge& Fillet)
{
  return myMakeChFi2d.RemoveFillet(Fillet);
}

//=================================================================================================

TopoDS_Edge BRepFilletAPI_MakeFillet2d::AddChamfer(const TopoDS_Edge& E1,
                                                   const TopoDS_Edge& E2,
                                                   const double       D1,
                                                   const double       D2)
{
  return myMakeChFi2d.AddChamfer(E1, E2, D1, D2);
}

//=================================================================================================

TopoDS_Edge BRepFilletAPI_MakeFillet2d::AddChamfer(const TopoDS_Edge&   E,
                                                   const TopoDS_Vertex& V,
                                                   const double         D,
                                                   const double         Ang)
{
  return myMakeChFi2d.AddChamfer(E, V, D, Ang);
}

//=================================================================================================

TopoDS_Edge BRepFilletAPI_MakeFillet2d::ModifyChamfer(const TopoDS_Edge& Chamfer,
                                                      const TopoDS_Edge& E1,
                                                      const TopoDS_Edge& E2,
                                                      const double       D1,
                                                      const double       D2)
{
  return myMakeChFi2d.ModifyChamfer(Chamfer, E1, E2, D1, D2);
}

//=================================================================================================

TopoDS_Edge BRepFilletAPI_MakeFillet2d::ModifyChamfer(const TopoDS_Edge& Chamfer,
                                                      const TopoDS_Edge& E,
                                                      const double       D,
                                                      const double       Ang)
{
  return myMakeChFi2d.ModifyChamfer(Chamfer, E, D, Ang);
}

//=================================================================================================

TopoDS_Vertex BRepFilletAPI_MakeFillet2d::RemoveChamfer(const TopoDS_Edge& Chamfer)
{
  return myMakeChFi2d.RemoveChamfer(Chamfer);
}

//=================================================================================================

const TopoDS_Edge& BRepFilletAPI_MakeFillet2d::BasisEdge(const TopoDS_Edge& E) const
{
  return myMakeChFi2d.BasisEdge(E);
}

//=================================================================================================

void BRepFilletAPI_MakeFillet2d::Build(const Message_ProgressRange& /*theRange*/)
{
  // test if the operation is done
  if (Status() == ChFi2d_IsDone)
  {
    Done();
    myShape = myMakeChFi2d.Result();
  }
  else
    NotDone();
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepFilletAPI_MakeFillet2d::Modified(const TopoDS_Shape& E)
{
  myGenerated.Clear();
  myGenerated.Append(DescendantEdge(TopoDS::Edge(E)));
  return myGenerated;
}

//=================================================================================================

int BRepFilletAPI_MakeFillet2d::NbCurves() const
{
  return NbFillet() + NbChamfer();
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepFilletAPI_MakeFillet2d::NewEdges(const int I)
{
  myGenerated.Clear();
  if (I <= NbFillet())
    myGenerated.Append(FilletEdges()(I));
  else
    myGenerated.Append(ChamferEdges()(I - NbFillet()));

  return myGenerated;
}
