// Created on: 1994-11-30
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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

// szv#4 S4163

#include <BRep_Tool.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeFix_Wire.hxx>
#include <StdFail_NotDone.hxx>
#include <StepData_Factors.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_PolyLoop.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepShape_Vertex.hxx>
#include <StepShape_VertexLoop.hxx>
#include <StepShape_VertexPoint.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDSToStep_MakeStepEdge.hxx>
#include <TopoDSToStep_MakeStepVertex.hxx>
#include <TopoDSToStep_MakeStepWire.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

//=================================================================================================

TopoDSToStep_MakeStepWire::TopoDSToStep_MakeStepWire()
    : myError(TopoDSToStep_WireOther)
{
  done = false;
}

TopoDSToStep_MakeStepWire::TopoDSToStep_MakeStepWire(const TopoDS_Wire&                    W,
                                                     TopoDSToStep_Tool&                    T,
                                                     const occ::handle<Transfer_FinderProcess>& FP,
                                                     const StepData_Factors& theLocalFactors)
{
  done = false;
  Init(W, T, FP, theLocalFactors);
}

//=================================================================================================

void TopoDSToStep_MakeStepWire::Init(const TopoDS_Wire&                    aWire,
                                     TopoDSToStep_Tool&                    aTool,
                                     const occ::handle<Transfer_FinderProcess>& FP,
                                     const StepData_Factors&               theLocalFactors)
{
  // ----------------------------------------------------------------
  // The Wire is given in its relative orientation (i.e. in the face)
  // ----------------------------------------------------------------
  aTool.SetCurrentWire(aWire);

  if (aTool.IsBound(aWire))
  {
    myError  = TopoDSToStep_WireDone;
    done     = true;
    myResult = aTool.Find(aWire);
    return;
  }

  if (aWire.Orientation() == TopAbs_INTERNAL || aWire.Orientation() == TopAbs_EXTERNAL)
  {
    occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aWire);
    FP->AddWarning(errShape, " Wire(internal/external) from Non Manifold Topology");
    myError = TopoDSToStep_NonManifoldWire;
    done    = false;
    return;
  }

  NCollection_Sequence<occ::handle<Standard_Transient>> mySeq;

  // --------
  // Polyloop
  // --------
  if (aTool.Faceted())
  {
    occ::handle<StepShape_VertexPoint>                   VertexPoint;
    occ::handle<StepGeom_Point>                          Point;
    occ::handle<StepShape_TopologicalRepresentationItem> Gpms;
    TopoDS_Vertex                                   TopoDSVertex1, TopoDSVertex2;

    TopoDSToStep_MakeStepVertex MkVertex;

    for (BRepTools_WireExplorer ItW(aWire, aTool.CurrentFace()); ItW.More(); ItW.Next())
    {
      const TopoDS_Edge& CurrentEdge = ItW.Current();
      if (CurrentEdge.Orientation() == TopAbs_FORWARD)
      {
        TopExp::Vertices(CurrentEdge, TopoDSVertex1, TopoDSVertex2);
      }
      else
      {
        TopExp::Vertices(CurrentEdge, TopoDSVertex2, TopoDSVertex1);
      }

      MkVertex.Init(TopoDSVertex1, aTool, FP, theLocalFactors);
      if (MkVertex.IsDone())
      {
        VertexPoint = occ::down_cast<StepShape_VertexPoint>(MkVertex.Value());
        Point       = VertexPoint->VertexGeometry();
        mySeq.Append(Point);
      }
      else
      {
        occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aWire);
        FP->AddWarning(errShape, " a Vertex Point not mapped");
        myError = TopoDSToStep_WireOther;
        done    = false;
        return;
      }
    }
    int nbPoints = mySeq.Length();
    if (nbPoints >= 3)
    {
      occ::handle<NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>> aPolygon =
        new NCollection_HArray1<occ::handle<StepGeom_CartesianPoint>>(1, nbPoints);
      for (int i = 1; i <= nbPoints; i++)
      {
        aPolygon->SetValue(i, occ::down_cast<StepGeom_CartesianPoint>(mySeq.Value(i)));
      }
      occ::handle<StepShape_PolyLoop>       PL    = new StepShape_PolyLoop();
      occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
      PL->Init(aName, aPolygon);

      aTool.Bind(aWire, PL);
      myError  = TopoDSToStep_WireDone;
      done     = true;
      myResult = PL;
      return;
    }
    else
    {
      occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aWire);
      FP->AddWarning(errShape, " PolyLoop: Wire has less than 3 points");
      myError = TopoDSToStep_WireOther;
      done    = false;
      return;
    }
  }
  // --------
  // EdgeLoop
  // --------
  else
  {
    occ::handle<StepShape_TopologicalRepresentationItem> Gpms;
    occ::handle<StepShape_Edge>                          Epms;
    occ::handle<StepShape_OrientedEdge>                  OrientedEdge;
    TopoDSToStep_MakeStepEdge                       MkEdge;

    const TopoDS_Wire     ForwardWire = TopoDS::Wire(aWire.Oriented(TopAbs_FORWARD));
    occ::handle<ShapeFix_Wire> STW =
      new ShapeFix_Wire(ForwardWire, aTool.CurrentFace(), Precision::Confusion());
    // for toroidal like surfaces we need to use both (3d and 2d) mode to correctly reorder the
    // edges
    STW->FixReorder(true);
    occ::handle<ShapeExtend_WireData> anExtWire = STW->WireData();

    //: abv 04.05.00: CAX-IF TRJ4: writing complete sphere with single vertex_loop
    // check that whole wire is one seam (perhaps made of several seam edges)
    // pdn remove degenerated pcurves

    // collect not degenerated edges
    occ::handle<ShapeExtend_WireData> anExtWire2 = new ShapeExtend_WireData;
    for (int ie = 1; ie <= anExtWire->NbEdges(); ie++)
    {
      TopoDS_Edge anEdge = anExtWire->Edge(ie);
      if (!BRep_Tool::Degenerated(anEdge))
      {
        anExtWire2->Add(anEdge);
      }
    }
    // check for seam edges
    int nb = anExtWire2->NbEdges();
    if (nb % 2 == 0)
    {
      int ie;
      // check if two adjacent edges are the same
      for (ie = 1; ie < nb; ie++)
      {
        if (anExtWire2->Edge(ie).IsSame(anExtWire2->Edge(ie + 1)))
        {
          break;
        }
      }
      // if found seam edges
      if (ie < nb)
      {
        // make the first edge from pair last
        anExtWire2->SetLast(ie);
        for (ie = nb / 2 + 1; ie <= nb; ie++)
        {
          if (!anExtWire2->Edge(ie).IsSame(anExtWire2->Edge(nb - ie + 1)))
          {
            break;
          }
        }
        if (ie > nb)
        {
          // make vertex_loop
          ShapeAnalysis_Edge               sae;
          TopoDS_Vertex                    V = sae.FirstVertex(anExtWire2->Edge(1));
          TopoDSToStep_MakeStepVertex      mkV(V, aTool, FP, theLocalFactors);
          occ::handle<StepShape_VertexLoop>     vloop = new StepShape_VertexLoop;
          occ::handle<TCollection_HAsciiString> name  = new TCollection_HAsciiString("");
          vloop->Init(name, occ::down_cast<StepShape_Vertex>(mkV.Value()));
          aTool.Bind(aWire, vloop);
          myError  = TopoDSToStep_WireDone;
          done     = true;
          myResult = vloop;
          return;
        }
      }
    }

    for (int nEdge = 1; nEdge <= anExtWire->NbEdges(); nEdge++)
    {
      const TopoDS_Edge anEdge = anExtWire->Edge(nEdge);
      // ---------------------------------
      // --- Is the edge Degenerated ? ---
      // ---------------------------------
      double        cf, cl;
      occ::handle<Geom2d_Curve> theC2d = BRep_Tool::CurveOnSurface(anEdge, aTool.CurrentFace(), cf, cl);
      if (BRep_Tool::Degenerated(anEdge))
      {
        occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aWire);
        FP->AddWarning(errShape, " EdgeLoop: Degenerated Pcurve not mapped");
        continue;
      }
      else
      {
        MkEdge.Init(anEdge, aTool, FP, theLocalFactors);
        if (MkEdge.IsDone())
        {
          OrientedEdge                           = new StepShape_OrientedEdge();
          Epms                                   = occ::down_cast<StepShape_Edge>(MkEdge.Value());
          occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
          OrientedEdge->Init(aName, Epms, (anEdge.Orientation() == TopAbs_FORWARD));
          mySeq.Append(OrientedEdge);
        }
        else
        {
          occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aWire);
          FP->AddWarning(errShape, " EdgeLoop: an Edge not mapped");
          myError = TopoDSToStep_WireOther;
          done    = false;
          return;
        }
      }
    }
    int nbEdges = mySeq.Length();
    if (nbEdges > 0)
    {
      occ::handle<NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>> aList =
        new NCollection_HArray1<occ::handle<StepShape_OrientedEdge>>(1, nbEdges);
      for (int i = 1; i <= nbEdges; i++)
      {
        aList->SetValue(i, occ::down_cast<StepShape_OrientedEdge>(mySeq.Value(i)));
      }
      occ::handle<StepShape_EdgeLoop>       Epmsl = new StepShape_EdgeLoop;
      occ::handle<TCollection_HAsciiString> aName = new TCollection_HAsciiString("");
      Epmsl->Init(aName, aList);
      aTool.Bind(aWire, Epmsl);
      done     = true;
      myResult = Epmsl;
      return;
    }
    else
    {
      occ::handle<TransferBRep_ShapeMapper> errShape = new TransferBRep_ShapeMapper(aWire);
      FP->AddWarning(errShape, " No Edges of this Wire were mapped");
      myError = TopoDSToStep_WireOther;
      done    = false;
      return;
    }
  }
}

//=================================================================================================

const occ::handle<StepShape_TopologicalRepresentationItem>& TopoDSToStep_MakeStepWire::Value() const
{
  StdFail_NotDone_Raise_if(!done, "TopoDSToStep_MakeStepWire::Value() - no result");
  return myResult;
}

//=================================================================================================

TopoDSToStep_MakeWireError TopoDSToStep_MakeStepWire::Error() const
{
  return myError;
}
