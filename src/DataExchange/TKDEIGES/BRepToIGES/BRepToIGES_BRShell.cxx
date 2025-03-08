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

//: n3 abv 8 Feb 99: PRO17820: BRepTools::OuterWire() -> ShapeAnalysis::OuterWire
// szv#4 S4163

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepToIGES_BRShell.hxx>
#include <BRepToIGES_BRWire.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomToIGES_GeomSurface.hxx>
#include <gp_Trsf.hxx>
#include <IGESBasic_Group.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <Interface_Macros.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <Transfer_FinderProcess.hxx>

//=============================================================================
// BRepToIGES_BRShell
//=============================================================================
BRepToIGES_BRShell::BRepToIGES_BRShell() {}

//=============================================================================
// BRepToIGES_BRShell
//=============================================================================

BRepToIGES_BRShell::BRepToIGES_BRShell(const BRepToIGES_BREntity& BR)
    : BRepToIGES_BREntity(BR)
{
}

//=============================================================================
// TransferShell
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGES_BRShell ::TransferShell(
  const TopoDS_Shape&          start,
  const Message_ProgressRange& theProgress)
{
  Handle(IGESData_IGESEntity) res;

  if (start.IsNull())
    return res;

  if (start.ShapeType() == TopAbs_FACE)
  {
    TopoDS_Face F = TopoDS::Face(start);
    res           = TransferFace(F, theProgress);
  }
  else if (start.ShapeType() == TopAbs_SHELL)
  {
    TopoDS_Shell S = TopoDS::Shell(start);
    res            = TransferShell(S, theProgress);
  }
  else
  {
    // message d`erreur
  }
  return res;
}

//=============================================================================
// TransferFace
//
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGES_BRShell ::TransferFace(const TopoDS_Face& start,
                                                              const Message_ProgressRange&)
{
  Handle(IGESData_IGESEntity) res;

  if (start.IsNull())
  {
    return res;
  }

  // pour explorer la face , il faut la mettre fORWARD.
  TopoDS_Face aFace = start;
  // Associates the input face (start) and its sub-shapes with the reversed variant,
  // if the input face has a Reversed orientation
  TopTools_DataMapOfShapeShape aShapeShapeMap;
  if (start.Orientation() == TopAbs_REVERSED)
  {
    BRepBuilderAPI_Copy aCopy;
    aCopy.Perform(aFace);

    // create face with redirected surface
    BRep_Builder         B;
    TopLoc_Location      aLoc;
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface(start, aLoc);
    while (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      // take basis surface, because pcurves will be transformed, so trim will be shifted,
      // accorded to new face bounds
      Handle(Geom_RectangularTrimmedSurface) aTrimmedSurf =
        Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf);
      aSurf = aTrimmedSurf->BasisSurface();
    }
    aSurf              = aSurf->UReversed();
    Standard_Real aTol = BRep_Tool::Tolerance(start);
    B.MakeFace(aFace, aSurf, aLoc, aTol);
    // set specifics flags of a Face
    B.NaturalRestriction(aFace, BRep_Tool::NaturalRestriction(start));
    // add wires
    TopoDS_Wire     anOuter = TopoDS::Wire(ShapeAlgo::AlgoContainer()->OuterWire(start));
    TopExp_Explorer ex;
    for (ex.Init(start, TopAbs_WIRE); ex.More(); ex.Next())
    {
      TopoDS_Wire W         = TopoDS::Wire(ex.Current());
      TopoDS_Wire aCopyWire = TopoDS::Wire(aCopy.ModifiedShape(W));
      aCopyWire             = TopoDS::Wire(aCopyWire.Oriented(W.Orientation()));
      if (!W.IsNull() && W.IsSame(anOuter))
      {
        B.Add(aFace, aCopyWire);
        aShapeShapeMap.Bind(aCopyWire, W); // Bind the reversed copy of Wire to the original
        break;
      }
    }
    for (ex.Init(start, TopAbs_WIRE); ex.More(); ex.Next())
    {
      TopoDS_Wire W         = TopoDS::Wire(ex.Current());
      TopoDS_Wire aCopyWire = TopoDS::Wire(aCopy.ModifiedShape(W));
      aCopyWire             = TopoDS::Wire(aCopyWire.Oriented(W.Orientation()));
      if (!W.IsNull() && !W.IsSame(anOuter))
      {
        B.Add(aFace, aCopyWire);
        aShapeShapeMap.Bind(aCopyWire, W); // Bind the reversed copy of Wire to the original
      }
    }

    // mirror pcurves
    Standard_Real U1, U2, V1, V2;
    aSurf->Bounds(U1, U2, V1, V2);
    Standard_Real aCenter = 0.5 * (U1 + U2);
    gp_Trsf2d     T;
    gp_Ax2d       axis(gp_Pnt2d(aCenter, V1), gp_Dir2d(0., 1.));
    T.SetMirror(axis);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap(101, new NCollection_IncAllocator);
    for (ex.Init(start, TopAbs_EDGE); ex.More(); ex.Next())
    {
      TopoDS_Edge anOrigEdge = TopoDS::Edge(ex.Current());
      TopoDS_Edge aCopyEdge  = TopoDS::Edge(aCopy.ModifiedShape(anOrigEdge));
      aCopyEdge              = TopoDS::Edge(aCopyEdge.Oriented(anOrigEdge.Orientation()));
      if (!aMap.Add(aCopyEdge))
        // seam edge has been already updated
        continue;
      Standard_Real        f, l;
      Handle(Geom2d_Curve) aCurve1, aCurve2;
      aCurve1 =
        BRep_Tool::CurveOnSurface(aCopyEdge, TopoDS::Face(aCopy.ModifiedShape(start)), f, l);
      aTol = BRep_Tool::Tolerance(aCopyEdge);
      if (!aCurve1.IsNull())
      {
        aCurve1 = Handle(Geom2d_Curve)::DownCast(aCurve1->Transformed(T));
        if (BRepTools::IsReallyClosed(aCopyEdge, TopoDS::Face(aCopy.ModifiedShape(start))))
        {
          TopoDS_Edge revEdge = TopoDS::Edge(aCopyEdge.Reversed());
          aCurve2 =
            BRep_Tool::CurveOnSurface(revEdge, TopoDS::Face(aCopy.ModifiedShape(start)), f, l);
          if (!aCurve2.IsNull())
          {
            aCurve2 = Handle(Geom2d_Curve)::DownCast(aCurve2->Transformed(T));
            if (aCopyEdge.Orientation() == TopAbs_FORWARD)
            {
              B.UpdateEdge(aCopyEdge, aCurve1, aCurve2, aFace, aTol);
            }
            else
            {
              B.UpdateEdge(aCopyEdge, aCurve2, aCurve1, aFace, aTol);
            }
          }
          else
          {
            B.UpdateEdge(aCopyEdge, aCurve1, aFace, aTol);
          }
        }
        else
        {
          B.UpdateEdge(aCopyEdge, aCurve1, aFace, aTol);
        }
        // set range for degenerated edges
        if (BRep_Tool::Degenerated(aCopyEdge))
        {
          B.Range(aCopyEdge, aFace, f, l);
        }
      }
      // clang-format off
      aShapeShapeMap.Bind(aCopyEdge, anOrigEdge); // Bind the reversed copy of Edge to the original
      // clang-format on
    }
    aShapeShapeMap.Bind(start, aFace); // Bind the original face to the reversed copy
  }

  // Standard_Integer Nb = 0; //szv#4:S4163:12Mar99 unused
  Standard_Real               Length = 1.;
  Handle(IGESData_IGESEntity) ISurf;

  // returns the face surface
  // ------------------------

  Handle(Geom_Surface) Surf = BRep_Tool::Surface(aFace);

  if (!Surf.IsNull())
  {
    Standard_Real U1, U2, V1, V2;
    // pour limiter les surfaces de base
    BRepTools::UVBounds(aFace, U1, U2, V1, V2);
    GeomToIGES_GeomSurface GS;
    GS.SetModel(GetModel());
    ISurf = GS.TransferSurface(Surf, U1, U2, V1, V2);
    if (ISurf.IsNull())
    {
      AddWarning(start, "the basic surface is a null entity");
      return res;
    }
    Length = GS.Length();
  }

  // returns the wires of the face
  // -----------------------------

  BRepToIGES_BRWire           BW(*this);
  Standard_Integer            Imode   = 0;
  Standard_Integer            Iprefer = 0;
  Handle(IGESData_IGESEntity) ICurve2d;

  // outer wire
  //: n3  TopoDS_Wire Outer = BRepTools::OuterWire(myface);
  TopoDS_Wire                     Outer  = ShapeAlgo::AlgoContainer()->OuterWire(aFace); //: n3
  Handle(IGESGeom_CurveOnSurface) IOuter = new IGESGeom_CurveOnSurface;
  if (!Outer.IsNull())
  {
    Handle(IGESData_IGESEntity) ICurve3d =
      BW.TransferWire(Outer, aFace, aShapeShapeMap, ICurve2d, Length);
    if ((!ICurve3d.IsNull()) && (!ICurve2d.IsNull()))
      Iprefer = 3;
    if ((!ICurve3d.IsNull()) && (ICurve2d.IsNull()))
      Iprefer = 2;
    if ((ICurve3d.IsNull()) && (!ICurve2d.IsNull()))
      Iprefer = 1;
    IOuter->Init(Imode, ISurf, ICurve2d, ICurve3d, Iprefer);
  }

  // inners wires
  TopExp_Explorer                      Ex;
  Handle(TColStd_HSequenceOfTransient) Seq = new TColStd_HSequenceOfTransient();

  for (Ex.Init(aFace, TopAbs_WIRE); Ex.More(); Ex.Next())
  {
    TopoDS_Wire                     W     = TopoDS::Wire(Ex.Current());
    Handle(IGESGeom_CurveOnSurface) Curve = new IGESGeom_CurveOnSurface;
    if (W.IsNull())
    {
      AddWarning(start, " an Wire is a null entity");
    }
    else if (!W.IsSame(Outer))
    {
      Handle(IGESData_IGESEntity) ICurve3d =
        BW.TransferWire(W, aFace, aShapeShapeMap, ICurve2d, Length);
      if ((!ICurve3d.IsNull()) && (!ICurve2d.IsNull()))
        Iprefer = 3;
      if ((!ICurve3d.IsNull()) && (ICurve2d.IsNull()))
        Iprefer = 2;
      if ((ICurve3d.IsNull()) && (!ICurve2d.IsNull()))
        Iprefer = 1;
      Curve->Init(Imode, ISurf, ICurve2d, ICurve3d, Iprefer);
      if (!Curve.IsNull())
        Seq->Append(Curve);
    }
  }

  // all inners edges not in a wire
  for (Ex.Init(aFace, TopAbs_EDGE, TopAbs_WIRE); Ex.More(); Ex.Next())
  {
    TopoDS_Edge                     E     = TopoDS::Edge(Ex.Current());
    Handle(IGESGeom_CurveOnSurface) Curve = new IGESGeom_CurveOnSurface;
    if (E.IsNull())
    {
      AddWarning(start, " an Edge is a null entity");
    }
    else
    {
      Handle(IGESData_IGESEntity) ICurve3d = BW.TransferEdge(E, aShapeShapeMap, Standard_False);
      Handle(IGESData_IGESEntity) newICurve2d =
        BW.TransferEdge(E, aFace, aShapeShapeMap, Length, Standard_False);
      if ((!ICurve3d.IsNull()) && (!newICurve2d.IsNull()))
        Iprefer = 3;
      if ((!ICurve3d.IsNull()) && (newICurve2d.IsNull()))
        Iprefer = 2;
      if ((ICurve3d.IsNull()) && (!newICurve2d.IsNull()))
        Iprefer = 1;
      Curve->Init(Imode, ISurf, newICurve2d, ICurve3d, Iprefer);
      if (!Curve.IsNull())
        Seq->Append(Curve);
    }
  }

  Standard_Integer                         nbent = Seq->Length();
  Handle(IGESGeom_HArray1OfCurveOnSurface) Tab;
  if (nbent >= 1)
  {
    Tab = new IGESGeom_HArray1OfCurveOnSurface(1, nbent);
    for (Standard_Integer itab = 1; itab <= nbent; itab++)
    {
      Handle(IGESGeom_CurveOnSurface) item = GetCasted(IGESGeom_CurveOnSurface, Seq->Value(itab));
      Tab->SetValue(itab, item);
    }
  }

  // protection against faces on infinite surfaces with mistaken natural restriction flag
  Standard_Boolean isWholeSurface = BRep_Tool::NaturalRestriction(start);
  if ((Surf->IsKind(STANDARD_TYPE(Geom_Plane))
       || Surf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))
       || Surf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
      && !IOuter.IsNull())
    isWholeSurface = Standard_False;
  // returns the TrimmedSurface
  // --------------------------
  Handle(IGESGeom_TrimmedSurface) TrimmedSurf = new IGESGeom_TrimmedSurface;
  if (isWholeSurface)
  {
    // if face bounds and surface bounds are same, outer wire is unnecessary
    TrimmedSurf->Init(ISurf, Standard_False, NULL, Tab);
  }
  else
    TrimmedSurf->Init(ISurf, Standard_True, IOuter, Tab);

  res = TrimmedSurf;
  SetShapeResult(start, res);
  return res;
}

//=============================================================================
// TransferShell
//=============================================================================

Handle(IGESData_IGESEntity) BRepToIGES_BRShell::TransferShell(
  const TopoDS_Shell&          start,
  const Message_ProgressRange& theProgress)
{
  Handle(IGESData_IGESEntity) res;
  if (start.IsNull())
    return res;

  TopExp_Explorer                      Ex;
  Handle(IGESBasic_Group)              IGroup = new IGESBasic_Group;
  Handle(TColStd_HSequenceOfTransient) Seq    = new TColStd_HSequenceOfTransient();
  Handle(IGESData_IGESEntity)          IFace;

  Standard_Integer nbshapes = 0;
  for (Ex.Init(start, TopAbs_FACE); Ex.More(); Ex.Next())
    nbshapes++;
  Message_ProgressScope aPS(theProgress, NULL, nbshapes);
  for (Ex.Init(start, TopAbs_FACE); Ex.More() && aPS.More(); Ex.Next())
  {
    Message_ProgressRange aRange = aPS.Next();
    TopoDS_Face           F      = TopoDS::Face(Ex.Current());
    if (F.IsNull())
    {
      AddWarning(start, " a Face is a null entity");
    }
    else
    {
      IFace = TransferFace(F, aRange);
      if (!IFace.IsNull())
        Seq->Append(IFace);
    }
  }

  Standard_Integer                     nbfaces = Seq->Length();
  Handle(IGESData_HArray1OfIGESEntity) Tab;
  if (nbfaces >= 1)
  {
    Tab = new IGESData_HArray1OfIGESEntity(1, nbfaces);
    for (Standard_Integer itab = 1; itab <= nbfaces; itab++)
    {
      Handle(IGESData_IGESEntity) item = GetCasted(IGESData_IGESEntity, Seq->Value(itab));
      Tab->SetValue(itab, item);
    }
  }

  if (nbfaces == 1)
  {
    res = IFace;
  }
  else
  {
    IGroup->Init(Tab);
    res = IGroup;
  }

  SetShapeResult(start, res);

  return res;
}
