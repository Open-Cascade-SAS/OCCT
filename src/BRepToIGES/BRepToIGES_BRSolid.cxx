// Created on: 1995-01-30
// Created by: Marie Jose MARTZ
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


#include <BRep_Tool.hxx>
#include <BRepToIGES_BREntity.hxx>
#include <BRepToIGES_BRShell.hxx>
#include <BRepToIGES_BRSolid.hxx>
#include <BRepToIGES_BRWire.hxx>
#include <BRepTools.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

// At first only the geometry is translated (point, curve...)
//=============================================================================
// BRepToIGES_BRSolid
//=============================================================================
BRepToIGES_BRSolid::BRepToIGES_BRSolid()
{
}


//=============================================================================
// BRepToIGES_BRSolid
//=============================================================================

BRepToIGES_BRSolid::BRepToIGES_BRSolid
(const BRepToIGES_BREntity& BR)
: BRepToIGES_BREntity(BR)
{
}


//=============================================================================
// TransferSolid
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGES_BRSolid ::TransferSolid(const TopoDS_Shape& start)
{
  Handle(IGESData_IGESEntity) res;

  if (start.IsNull())  return  res;

  if (start.ShapeType() == TopAbs_SOLID) {
    TopoDS_Solid M =  TopoDS::Solid(start);
    res = TransferSolid(M);
  }  
  else if (start.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_CompSolid C =  TopoDS::CompSolid(start);
    res = TransferCompSolid(C);
  }  
  else if (start.ShapeType() == TopAbs_COMPOUND) {
    TopoDS_Compound C =  TopoDS::Compound(start);
    res = TransferCompound(C);
  }  
  else {
    // error message
  }  
  return res;
}


//=============================================================================
// TransferSolid
// 
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGES_BRSolid ::TransferSolid(const TopoDS_Solid& start)
{
  Handle(IGESData_IGESEntity) res;
  if ( start.IsNull()) return res;

  TopExp_Explorer Ex;
  Handle(IGESData_IGESEntity) IShell;
  BRepToIGES_BRShell BS(*this);
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();

  for (Ex.Init(start,TopAbs_SHELL); Ex.More(); Ex.Next()) {
    TopoDS_Shell S = TopoDS::Shell(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," an Shell is a null entity");
    }
    else {
      IShell = BS.TransferShell(S);
      if (!IShell.IsNull()) Seq->Append(IShell);
    }
  }


  Standard_Integer nbshells = Seq->Length();
  Handle(IGESData_HArray1OfIGESEntity) Tab;
  if ( nbshells >= 1) {
    Tab =  new IGESData_HArray1OfIGESEntity(1,nbshells);
    for (Standard_Integer itab = 1; itab <= nbshells; itab++) {
      Handle(IGESData_IGESEntity) item = GetCasted(IGESData_IGESEntity, Seq->Value(itab));
      Tab->SetValue(itab,item);
    }
  }

  if (nbshells == 1) {
    res = IShell;
  }
  else {
    Handle(IGESBasic_Group) IGroup = new IGESBasic_Group;
    IGroup->Init(Tab);
    res = IGroup;
  }

  SetShapeResult ( start, res );

  return res;
}


//=============================================================================
// TransferCompSolid
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGES_BRSolid ::TransferCompSolid(const TopoDS_CompSolid& start)
{
  Handle(IGESData_IGESEntity) res;
  if ( start.IsNull()) return res;

  TopExp_Explorer Ex;
  Handle(IGESData_IGESEntity) ISolid;
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();

  for (Ex.Init(start,TopAbs_SOLID); Ex.More(); Ex.Next()) {
    TopoDS_Solid S = TopoDS::Solid(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," an Solid is a null entity");
    }
    else {
      ISolid = TransferSolid(S);
      if (!ISolid.IsNull()) Seq->Append(ISolid);
    }
  }


  Standard_Integer nbsolids = Seq->Length();
  Handle(IGESData_HArray1OfIGESEntity) Tab;
  if ( nbsolids >= 1) {
    Tab =  new IGESData_HArray1OfIGESEntity(1,nbsolids);
    for (Standard_Integer itab = 1; itab <= nbsolids; itab++) {
      Handle(IGESData_IGESEntity) item = GetCasted(IGESData_IGESEntity, Seq->Value(itab));
      Tab->SetValue(itab,item);
    }
  }

  if (nbsolids == 1) {
    res = ISolid;
  }
  else {
    Handle(IGESBasic_Group) IGroup = new IGESBasic_Group;
    IGroup->Init(Tab);
    res = IGroup;
  }

  SetShapeResult ( start, res );

  return res;
}


//=============================================================================
// TransferCompound
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGES_BRSolid ::TransferCompound(const TopoDS_Compound& start)
{
  Handle(IGESData_IGESEntity) res;
  if ( start.IsNull()) return res;


  TopExp_Explorer Ex;
  Handle(IGESData_IGESEntity) IShape;
  BRepToIGES_BRShell BS(*this);
  BRepToIGES_BRWire BW(*this);
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();

  // take all Solids
  for (Ex.Init(start, TopAbs_SOLID); Ex.More(); Ex.Next()) {
    TopoDS_Solid S = TopoDS::Solid(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," a Solid is a null entity");
    }
    else {
      IShape = TransferSolid(S);
      if (!IShape.IsNull()) Seq->Append(IShape);
    }
  }

  // take all isolated Shells 
  for (Ex.Init(start, TopAbs_SHELL, TopAbs_SOLID); Ex.More(); Ex.Next()) {
    TopoDS_Shell S = TopoDS::Shell(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," a Shell is a null entity");
    }
    else {
      IShape = BS.TransferShell(S);
      if (!IShape.IsNull()) Seq->Append(IShape);
    }
  }


  // take all isolated Faces 
  for (Ex.Init(start, TopAbs_FACE, TopAbs_SHELL); Ex.More(); Ex.Next()) {
    TopoDS_Face S = TopoDS::Face(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," a Face is a null entity");
    }
    else {
      IShape = BS.TransferFace(S);
      if (!IShape.IsNull()) Seq->Append(IShape);
    }
  }


  // take all isolated Wires 
  for (Ex.Init(start, TopAbs_WIRE, TopAbs_FACE); Ex.More(); Ex.Next()) {
    TopoDS_Wire S = TopoDS::Wire(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," a Wire is a null entity");
    }
    else {
      IShape = BW.TransferWire(S);
      if (!IShape.IsNull()) Seq->Append(IShape);
    }
  }


  // take all isolated Edges 
  for (Ex.Init(start, TopAbs_EDGE, TopAbs_WIRE); Ex.More(); Ex.Next()) {
    TopoDS_Edge S = TopoDS::Edge(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," an Edge is a null entity");
    }
    else {
      IShape = BW.TransferEdge(S, Standard_False);
      if (!IShape.IsNull()) Seq->Append(IShape);
    }
  }


  // take all isolated Vertices 
  for (Ex.Init(start, TopAbs_VERTEX, TopAbs_EDGE); Ex.More(); Ex.Next()) {
    TopoDS_Vertex S = TopoDS::Vertex(Ex.Current());
    if (S.IsNull()) {
      AddWarning(start," a Vertex is a null entity");
    }
    else {
      IShape = BW.TransferVertex(S);
      if (!IShape.IsNull()) Seq->Append(IShape);
    }
  }

  // construct the group
  Standard_Integer nbshapes = Seq->Length();
  Handle(IGESData_HArray1OfIGESEntity) Tab;
  if (nbshapes >=1) {
    Tab =  new IGESData_HArray1OfIGESEntity(1,nbshapes);
    for (Standard_Integer itab = 1; itab <= nbshapes; itab++) {
      Handle(IGESData_IGESEntity) item = GetCasted(IGESData_IGESEntity, Seq->Value(itab));
      Tab->SetValue(itab,item);
    }
  }

  if (nbshapes == 1) {
    res = IShape;
  }
  else {
    Handle(IGESBasic_Group) IGroup = new IGESBasic_Group;
    IGroup->Init(Tab);
    res = IGroup;
  }

  SetShapeResult ( start, res );

  return res;
}
