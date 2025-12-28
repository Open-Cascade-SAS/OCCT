// Created on: 2000-05-25
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeUpgrade_ClosedEdgeDivide.hxx>
#include <ShapeUpgrade_SplitCurve2d.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_ClosedEdgeDivide, ShapeUpgrade_EdgeDivide)

//=================================================================================================

ShapeUpgrade_ClosedEdgeDivide::ShapeUpgrade_ClosedEdgeDivide()

  = default;

//=================================================================================================

bool ShapeUpgrade_ClosedEdgeDivide::Compute(const TopoDS_Edge& anEdge)
{
  Clear();
  ShapeAnalysis_Edge sae;
  TopoDS_Vertex      V1 = sae.FirstVertex(anEdge);
  TopoDS_Vertex      V2 = sae.LastVertex(anEdge);
  if (V1.IsSame(V2) && !BRep_Tool::Degenerated(anEdge))
  {
    const int nbPoints = 23;
    gp_Pnt    pntV     = BRep_Tool::Pnt(V1);
    double    TolV1    = LimitTolerance(BRep_Tool::Tolerance(V1));
    TolV1              = TolV1 * TolV1;
    double                  f, l;
    occ::handle<Geom_Curve> curve3d = BRep_Tool::Curve(anEdge, f, l);
    myHasCurve3d                    = !curve3d.IsNull();
    double                    f2d = 0., l2d = 0.;
    occ::handle<Geom2d_Curve> pcurve1;
    if (!myFace.IsNull())
    { // process free edges
      sae.PCurve(anEdge, myFace, pcurve1, f2d, l2d, false);
    }
    myHasCurve2d = !pcurve1.IsNull();

    if (myHasCurve3d)
    {
      double maxPar = f, dMax = 0;
      double step  = (l - f) / (nbPoints - 1);
      double param = f + step;
      for (int i = 1; i < 23; i++, param += step)
      {
        gp_Pnt curPnt = curve3d->Value(param);
        double dist   = pntV.SquareDistance(curPnt);
        if (dist > dMax)
        {
          maxPar = param;
          dMax   = dist;
        }
      }
      if (dMax > TolV1)
      {
        occ::handle<ShapeUpgrade_SplitCurve3d> theSplit3dTool = GetSplitCurve3dTool();
        theSplit3dTool->Init(curve3d, f, l);

        occ::handle<NCollection_HSequence<double>> values = new NCollection_HSequence<double>;
        values->Append(maxPar);
        theSplit3dTool->SetSplitValues(values);
        myKnots3d = theSplit3dTool->SplitValues();

        if (myHasCurve2d)
        {
          occ::handle<ShapeUpgrade_SplitCurve2d> theSplit2dTool = GetSplitCurve2dTool();
          theSplit2dTool->Init(pcurve1, f2d, l2d);
          myKnots2d = theSplit2dTool->SplitValues();
        }
        return true;
      }
      else
        return false;
    }

    if (myHasCurve2d)
    {
      occ::handle<Geom_Surface> surf   = BRep_Tool::Surface(myFace);
      double                    maxPar = f2d, dMax = 0;
      double                    step  = (l2d - f2d) / (nbPoints - 1);
      double                    param = f2d + step;
      for (int i = 1; i < 23; i++, param += step)
      {
        gp_Pnt2d p2d    = pcurve1->Value(param);
        gp_Pnt   curPnt = surf->Value(p2d.X(), p2d.Y());
        double   dist   = pntV.SquareDistance(curPnt);
        if (dist > dMax)
        {
          maxPar = param;
          dMax   = dist;
        }
      }
      if (dMax > TolV1)
      {

        occ::handle<ShapeUpgrade_SplitCurve2d> theSplit2dTool = GetSplitCurve2dTool();
        theSplit2dTool->Init(pcurve1, f2d, l2d);

        occ::handle<NCollection_HSequence<double>> values = new NCollection_HSequence<double>;
        values->Append(maxPar);
        theSplit2dTool->SetSplitValues(values);
        myKnots2d = theSplit2dTool->SplitValues();
        return true;
      }
      else
        return false;
    }

    return false;
  }
  else
    return false;
}
