// Created on: 1995-01-30
// Created by: Marie Jose MARTZ
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

//:n3 abv 8 Feb 99: PRO17820: BRepTools::OuterWire() -> ShapeAnalysis::OuterWire
//szv#4 S4163

#include <BRepToIGES_BRShell.ixx>
#include <BRepToIGES_BRWire.hxx>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <gp.hxx>
#include <gp_Trsf.hxx>

#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>

#include <GeomToIGES_GeomSurface.hxx>

#include <IGESBasic_Group.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>

#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_HArray1OfCurveOnSurface.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <IGESGeom_SurfaceOfRevolution.hxx>

#include <Interface_Macros.hxx>

#include <TColStd_HSequenceOfTransient.hxx>

#include <TopLoc_Location.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shape.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>

#include <Message_ProgressIndicator.hxx>
#include <Transfer_FinderProcess.hxx>

//=============================================================================
// BRepToIGES_BRShell
//=============================================================================

BRepToIGES_BRShell::BRepToIGES_BRShell()
{
}


//=============================================================================
// BRepToIGES_BRShell
//=============================================================================

BRepToIGES_BRShell::BRepToIGES_BRShell
(const BRepToIGES_BREntity& BR)
: BRepToIGES_BREntity(BR)
{
}


//=============================================================================
// TransferShell
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGES_BRShell ::TransferShell(const TopoDS_Shape& start)
{
  Handle(IGESData_IGESEntity) res;

  if (start.IsNull())  return  res;

  if (start.ShapeType() == TopAbs_FACE) {
    TopoDS_Face F =  TopoDS::Face(start);
    res = TransferFace(F);
  }  
  else if (start.ShapeType() == TopAbs_SHELL) {
    TopoDS_Shell S =  TopoDS::Shell(start);
    res = TransferShell(S);
  }  
  else {
    // message d`erreur
  }  
  return res;
}


//=============================================================================
// TransferFace
// 
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGES_BRShell ::TransferFace(const TopoDS_Face& start)
{
  Handle(IGESData_IGESEntity) res;

  Handle(Message_ProgressIndicator) progress = GetTransferProcess()->GetProgress();
  if ( ! progress.IsNull() ) {
    if ( progress->UserBreak() ) return res;
    progress->Increment();
  }
  
  if ( start.IsNull()) {
    return res;
  }
  //Standard_Integer Nb = 0; //szv#4:S4163:12Mar99 unused
  Standard_Real Length = 1.;
  Handle(IGESData_IGESEntity) ISurf;

  // returns the face surface
  // ------------------------

  Handle(Geom_Surface) Surf = BRep_Tool::Surface(start);
  Handle(Geom_Surface) Surf1;

  if (!Surf.IsNull()) {
    Standard_Real U1, U2, V1, V2;
    // pour limiter les surfaces de base
    BRepTools::UVBounds(start, U1, U2, V1, V2); 
    GeomToIGES_GeomSurface GS;
    GS.SetModel(GetModel());
    ISurf = GS.TransferSurface(Surf, U1, U2, V1, V2);
    if (ISurf.IsNull()) {
      AddWarning (start, "the basic surface is a null entity");
      return res;
    }
    Length = GS.Length();

    // modif mjm du 17/07/97
    if (Surf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) { 
      DeclareAndCast(Geom_RectangularTrimmedSurface, rectang, Surf);
      Surf1 = rectang->BasisSurface();
    }
    else 
      Surf1 = Surf;
  }


  // returns the wires of the face
  // -----------------------------

  BRepToIGES_BRWire BW(*this);
  Standard_Integer Imode = 0; 
  Standard_Integer Iprefer = 0; 
  Handle(IGESData_IGESEntity) ICurve2d;
  // pour explorer la face , il faut la mettre fORWARD.
  TopoDS_Face myface = start;
  Standard_Boolean IsReversed = Standard_False;
  if (start.Orientation() == TopAbs_REVERSED) {
    myface.Reverse();
    IsReversed = Standard_True;
  }

  // outer wire
//:n3  TopoDS_Wire Outer = BRepTools::OuterWire(myface);
  TopoDS_Wire Outer = ShapeAlgo::AlgoContainer()->OuterWire(myface); //:n3 
  Handle(IGESGeom_CurveOnSurface) IOuter = new IGESGeom_CurveOnSurface;
  if (!Outer.IsNull()) {
    Handle(IGESData_IGESEntity) ICurve3d = 
      BW.TransferWire(Outer, myface, ICurve2d, Length);
    if ((!ICurve3d.IsNull()) && (!ICurve2d.IsNull())) Iprefer = 3; 
    if ((!ICurve3d.IsNull()) && (ICurve2d.IsNull())) Iprefer = 2; 
    if ((ICurve3d.IsNull()) && (!ICurve2d.IsNull())) Iprefer = 1; 
    IOuter -> Init (Imode, ISurf, ICurve2d, ICurve3d, Iprefer);
  }

  // inners wires
  TopExp_Explorer Ex;
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();

  for (Ex.Init(myface,TopAbs_WIRE); Ex.More(); Ex.Next()) {
    TopoDS_Wire W = TopoDS::Wire(Ex.Current());
    Handle(IGESGeom_CurveOnSurface) Curve = new IGESGeom_CurveOnSurface;
    if (W.IsNull()) {
      AddWarning(start," an Wire is a null entity");
    }
    else if (!W.IsSame(Outer)) {
      Handle(IGESData_IGESEntity) ICurve3d = 
	BW.TransferWire(W, myface, ICurve2d, Length);
      if ((!ICurve3d.IsNull()) && (!ICurve2d.IsNull())) Iprefer = 3; 
      if ((!ICurve3d.IsNull()) && (ICurve2d.IsNull())) Iprefer = 2; 
      if ((ICurve3d.IsNull()) && (!ICurve2d.IsNull())) Iprefer = 1; 
      Curve-> Init (Imode, ISurf, ICurve2d, ICurve3d, Iprefer);
      if (!Curve.IsNull()) Seq->Append(Curve);
    }
  }

  // all inners edges not in a wire
  for (Ex.Init(myface,TopAbs_EDGE,TopAbs_WIRE); Ex.More(); Ex.Next()) {
    TopoDS_Edge E = TopoDS::Edge(Ex.Current());
    Handle(IGESGeom_CurveOnSurface) Curve = new IGESGeom_CurveOnSurface;
    if (E.IsNull()) {
      AddWarning(start," an Edge is a null entity");
    }
    else {
      Handle(IGESData_IGESEntity) ICurve3d = BW.TransferEdge(E, Standard_False);
      Handle(IGESData_IGESEntity) newICurve2d = BW.TransferEdge(E, myface, Length, Standard_False);
      if ((!ICurve3d.IsNull()) && (!newICurve2d.IsNull())) Iprefer = 3; 
      if ((!ICurve3d.IsNull()) && (newICurve2d.IsNull())) Iprefer = 2; 
      if ((ICurve3d.IsNull()) && (!newICurve2d.IsNull())) Iprefer = 1; 
      Curve-> Init (Imode, ISurf, newICurve2d, ICurve3d, Iprefer);
      if (!Curve.IsNull()) Seq->Append(Curve);
    }
  }


  Standard_Integer nbent = Seq->Length();
  Handle(IGESGeom_HArray1OfCurveOnSurface) Tab;
  if (nbent >=1) {
    Tab = new IGESGeom_HArray1OfCurveOnSurface(1,nbent);
    for (Standard_Integer itab = 1; itab <= nbent; itab++) {
      Handle(IGESGeom_CurveOnSurface) item = GetCasted(IGESGeom_CurveOnSurface, Seq->Value(itab));
      Tab->SetValue(itab,item);
    }
  }

  // returns the TrimmedSurface
  // --------------------------
  Standard_Boolean Flag = Standard_True; // pour l`instant
  Handle(IGESGeom_TrimmedSurface) TrimmedSurf = new IGESGeom_TrimmedSurface;
  TrimmedSurf-> Init (ISurf, Flag, IOuter, Tab);

  res = TrimmedSurf;
  if (IsReversed) myface.Reverse();

  SetShapeResult ( start, res );

  return res;
}


//=============================================================================
// TransferShell
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGES_BRShell::TransferShell(const TopoDS_Shell& start)
{
  Handle(IGESData_IGESEntity) res;
  if ( start.IsNull()) return res;

  TopExp_Explorer Ex;
  Handle(IGESBasic_Group) IGroup = new IGESBasic_Group;
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();
  Handle(IGESData_IGESEntity) IFace;

  for (Ex.Init(start,TopAbs_FACE); Ex.More(); Ex.Next()) {
    TopoDS_Face F = TopoDS::Face(Ex.Current());
    if (F.IsNull()) {
      AddWarning(start," a Face is a null entity");
    }
    else {
      IFace = TransferFace(F);
      if (!IFace.IsNull()) Seq->Append(IFace);
    }
  }


  Standard_Integer nbfaces = Seq->Length();  
  Handle(IGESData_HArray1OfIGESEntity) Tab;
  if ( nbfaces >= 1) {
    Tab = new IGESData_HArray1OfIGESEntity(1,nbfaces);
    for (Standard_Integer itab = 1; itab <= nbfaces; itab++) {
      Handle(IGESData_IGESEntity) item = GetCasted(IGESData_IGESEntity, Seq->Value(itab));
      Tab->SetValue(itab,item);
    }
  }

  if (nbfaces == 1) {
    res = IFace;
  }
  else {
    IGroup->Init(Tab);
    res = IGroup;
  }

  SetShapeResult ( start, res );

  return res;
}




