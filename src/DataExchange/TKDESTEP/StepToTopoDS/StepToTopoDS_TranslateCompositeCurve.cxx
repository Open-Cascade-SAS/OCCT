// Created on: 1999-02-12
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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

//: o3 abv 17.02.99: r0301_db.stp #57082: apply FixReorder to composite curve
//: s5 abv 22.04.99  Adding debug printouts in catch {} blocks

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Precision.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <StepData_StepModel.hxx>
#include <StepGeom_CompositeCurve.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
#include <StepGeom_Pcurve.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepToGeom.hxx>
#include <StepToTopoDS_TranslateCompositeCurve.hxx>
#include <StepToTopoDS_TranslateEdge.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <Transfer_TransientProcess.hxx>

//=================================================================================================

StepToTopoDS_TranslateCompositeCurve::StepToTopoDS_TranslateCompositeCurve()
    : myInfiniteSegment(false)
{
}

//=================================================================================================

StepToTopoDS_TranslateCompositeCurve::StepToTopoDS_TranslateCompositeCurve(
  const occ::handle<StepGeom_CompositeCurve>&   CC,
  const occ::handle<Transfer_TransientProcess>& TP,
  const StepData_Factors&                       theLocalFactors)
{
  Init(CC, TP, theLocalFactors);
}

//=================================================================================================

StepToTopoDS_TranslateCompositeCurve::StepToTopoDS_TranslateCompositeCurve(
  const occ::handle<StepGeom_CompositeCurve>&   CC,
  const occ::handle<Transfer_TransientProcess>& TP,
  const occ::handle<StepGeom_Surface>&          S,
  const occ::handle<Geom_Surface>&              Surf,
  const StepData_Factors&                       theLocalFactors)
{
  Init(CC, TP, S, Surf, theLocalFactors);
}

//=================================================================================================

bool StepToTopoDS_TranslateCompositeCurve::Init(const occ::handle<StepGeom_CompositeCurve>&   CC,
                                                const occ::handle<Transfer_TransientProcess>& TP,
                                                const StepData_Factors& theLocalFactors)
{
  occ::handle<StepGeom_Surface> S;
  occ::handle<Geom_Surface>     Surf;
  return Init(CC, TP, S, Surf, theLocalFactors);
}

//=================================================================================================

bool StepToTopoDS_TranslateCompositeCurve::Init(const occ::handle<StepGeom_CompositeCurve>&   CC,
                                                const occ::handle<Transfer_TransientProcess>& TP,
                                                const occ::handle<StepGeom_Surface>&          S,
                                                const occ::handle<Geom_Surface>&              Surf,
                                                const StepData_Factors& theLocalFactors)
{
  myWire.Nullify();
  myInfiniteSegment = false;
  if (CC.IsNull())
    return false;

  bool SurfMode = (!S.IsNull() && !Surf.IsNull());
  bool isClosed = false;

  occ::handle<StepData_StepModel> aStepModel = occ::down_cast<StepData_StepModel>(TP->Model());

  if (SurfMode)
  {
    int modepcurve = aStepModel->InternalParameters.ReadSurfaceCurveMode;
    if (modepcurve == -3)
      SurfMode = false;
  }

  occ::handle<ShapeExtend_WireData> sbwd = new ShapeExtend_WireData;
  int                               nbs  = CC->NbSegments();
  for (int i = 1; i <= nbs; i++)
  {
    occ::handle<StepGeom_CompositeCurveSegment> ccs = CC->SegmentsValue(i);
    if (ccs.IsNull())
    {
      TP->AddFail(CC, "Null segment");
      return false;
    }
    occ::handle<StepGeom_Curve> crv = ccs->ParentCurve();
    if (crv.IsNull())
    {
      TP->AddFail(CC, "Segment has null parent curve");
      return false;
    }
    isClosed = (ccs->Transition() != StepGeom_tcDiscontinuous);

    // if segment is itself a composite_curve, translate recursively
    if (crv->IsKind(STANDARD_TYPE(StepGeom_CompositeCurve)))
    {
      if (crv == CC)
      { // cyclic reference protection
        TP->AddFail(ccs, "Cyclic reference; segment dropped");
        continue;
      }
      occ::handle<StepGeom_CompositeCurve> cc = occ::down_cast<StepGeom_CompositeCurve>(crv);
      if (!Init(cc, TP, S, Surf, theLocalFactors) || myWire.IsNull())
        continue;
      int nb = sbwd->NbEdges() + 1;
      for (TopoDS_Iterator it(myWire); it.More(); it.Next())
      {
        TopoDS_Edge edge = TopoDS::Edge(it.Value());
        if (ccs->SameSense())
          sbwd->Add(edge);
        else
        {
          edge.Reverse();
          sbwd->Add(edge, nb > sbwd->NbEdges() ? 0 : nb);
        }
      }
      myWire.Nullify();
      continue;
    }

    // ordinary segment

    // detect pcurve and 3d curve
    occ::handle<StepGeom_Pcurve> pcurve = occ::down_cast<StepGeom_Pcurve>(crv);
    if (pcurve.IsNull())
    {
      occ::handle<StepGeom_SurfaceCurve> sc = occ::down_cast<StepGeom_SurfaceCurve>(crv);
      if (!sc.IsNull())
      {
        crv = sc->Curve3d();
        if (SurfMode)
        { // find proper pcurve
          for (int j = 1; j <= sc->NbAssociatedGeometry(); j++)
          {
            StepGeom_PcurveOrSurface     PCorS = sc->AssociatedGeometryValue(j);
            occ::handle<StepGeom_Pcurve> pc    = PCorS.Pcurve();
            if (pc.IsNull() || pc->BasisSurface() != S)
              continue;
            pcurve = pc;
            if (ccs->SameSense())
              break;
          }
        }
      }
    }
    else
    {
      if (!SurfMode || pcurve->BasisSurface() != S)
        pcurve.Nullify();
      crv.Nullify();
    }

    // prepare edge
    TopoDS_Edge edge;

    // translate 3d curve, if present
    if (!crv.IsNull())
    {
      try
      {
        OCC_CATCH_SIGNALS
        occ::handle<Geom_Curve> c3d = StepToGeom::MakeCurve(crv, theLocalFactors);
        if (!c3d.IsNull())
        {
          BRepBuilderAPI_MakeEdge MkEdge(c3d, c3d->FirstParameter(), c3d->LastParameter());
          if (MkEdge.IsDone())
          {
            if (Precision::IsNegativeInfinite(c3d->FirstParameter())
                || Precision::IsPositiveInfinite(c3d->LastParameter()))
            {
              myInfiniteSegment = true;
              TP->AddWarning(CC, "Segment with infinite parameters");
            }
            edge = MkEdge.Edge();
          }
        }
      }
      catch (Standard_Failure const& anException)
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning: StepToTopoDS_TranslateCompositeCurve: Exception: ";
        anException.Print(std::cout);
        std::cout << std::endl;
#endif
        (void)anException;
      }
    }

    // translate pcurve, if available
    if (!pcurve.IsNull())
    {
      try
      {
        OCC_CATCH_SIGNALS
        StepToTopoDS_TranslateEdge TrE;
        occ::handle<Geom2d_Curve>  c2d = TrE.MakePCurve(pcurve, Surf, theLocalFactors);
        if (!c2d.IsNull())
        {
          if (edge.IsNull())
          {
            BRepBuilderAPI_MakeEdge MkEdge(c2d, Surf, c2d->FirstParameter(), c2d->LastParameter());
            if (MkEdge.IsDone())
            {
              if (Precision::IsNegativeInfinite(c2d->FirstParameter())
                  || Precision::IsPositiveInfinite(c2d->LastParameter()))
              {
                myInfiniteSegment = true;
                TP->AddWarning(CC, "Segment with infinite parameters");
              }
              edge = MkEdge.Edge();
            }
          }
          else
          {
            BRep_Builder    B;
            TopLoc_Location L;
            B.UpdateEdge(edge, c2d, Surf, L, 0.);
            B.Range(edge, Surf, L, c2d->FirstParameter(), c2d->LastParameter());
            B.SameRange(edge, false);
            B.SameParameter(edge, false);
          }
        }
      }
      catch (Standard_Failure const& anException)
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning: StepToTopoDS_TranslateCompositeCurve: Exception: ";
        anException.Print(std::cout);
        std::cout << std::endl;
#endif
        (void)anException;
      }
    }

    if (edge.IsNull())
    {
      TP->AddFail(crv, "Curve can not be translated");
      continue;
    }

    if (!ccs->SameSense())
      edge.Reverse();
    sbwd->Add(edge);
  }
  if (sbwd->NbEdges() <= 0)
  {
    TP->AddFail(CC, "Translation gives no result");
    return false;
  }

  // connect wire; all other fixes are left for caller
  double                     preci = Precision();
  occ::handle<ShapeFix_Wire> sfw   = new ShapeFix_Wire;
  sfw->Load(sbwd);
  sfw->SetPrecision(preci);
  sfw->ClosedWireMode() = isClosed;
  sfw->FixReorder(); //: o3 abv 17 Feb 99: r0301_db.stp #57082
  if (sfw->StatusReorder(ShapeExtend_DONE))
  {
    TP->AddWarning(CC, "Segments were disordered; fixed");
  }
  sfw->FixConnected(preci);
  if (sfw->StatusConnected(ShapeExtend_FAIL))
  {
    TP->AddWarning(CC, "Segments are not connected");
  }

  myWire = sbwd->Wire();
  done   = (sbwd->NbEdges() > 0);
  return true;
}

//=================================================================================================

const TopoDS_Wire& StepToTopoDS_TranslateCompositeCurve::Value() const
{
  return myWire;
}
