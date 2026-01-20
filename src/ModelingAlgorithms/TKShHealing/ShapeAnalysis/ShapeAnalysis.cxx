// Created on: 2000-01-20
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

#include <ShapeAnalysis.hxx>

#include <Bnd_Box2d.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <GProp_GProps.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeExtend_WireData.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Sequence.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

// PLANTAGE IsOuterBound, 15-SEP-1998
// static int numpb = 0;
//=================================================================================================

double ShapeAnalysis::AdjustByPeriod(const double Val,
                                            const double ToVal,
                                            const double Period)
{
  double diff = Val - ToVal;
  double D    = std::abs(diff);
  double P    = std::abs(Period);
  if (D <= 0.5 * P)
    return 0.;
  if (P < 1e-100)
    return diff;
  return (diff > 0 ? -P : P) * floor(D / P + 0.5);
}

//=================================================================================================

double ShapeAnalysis::AdjustToPeriod(const double Val,
                                            const double ValMin,
                                            const double ValMax)
{
  return AdjustByPeriod(Val, 0.5 * (ValMin + ValMax), ValMax - ValMin);
}

//=================================================================================================

void ShapeAnalysis::FindBounds(const TopoDS_Shape& shape, TopoDS_Vertex& V1, TopoDS_Vertex& V2)
{
  V1.Nullify();
  V2.Nullify();
  ShapeAnalysis_Edge EA;
  if (shape.ShapeType() == TopAbs_WIRE)
  {
    TopoDS_Wire W = TopoDS::Wire(shape);
    // invalid work with reversed wires replaced on TopExp
    TopExp::Vertices(W, V1, V2);
    // invalid work with reversed wires
    /*TopoDS_Iterator iterWire(W);
    //szv#4:S4163:12Mar99 optimized
    if (iterWire.More()) {
      TopoDS_Edge E = TopoDS::Edge (iterWire.Value());
      V1 = EA.FirstVertex (E); iterWire.Next();
      for ( ; iterWire.More(); iterWire.Next() ) E = TopoDS::Edge (iterWire.Value());
      V2 = EA.LastVertex (E);
    }*/
  }
  else if (shape.ShapeType() == TopAbs_EDGE)
  {
    V1 = EA.FirstVertex(TopoDS::Edge(shape));
    V2 = EA.LastVertex(TopoDS::Edge(shape));
  }
  else if (shape.ShapeType() == TopAbs_VERTEX)
    V1 = V2 = TopoDS::Vertex(shape);
}

//=================================================================================================

template <class HSequence>
static inline void ReverseSeq(HSequence& Seq)
{
  Seq.Reverse();
}

//=================================================================================================

double ShapeAnalysis::TotCross2D(const occ::handle<ShapeExtend_WireData>& sewd,
                                        const TopoDS_Face&                  aFace)
{
  int i, nbc = 0;
  gp_Pnt2d         fuv, luv, uv0;
  double    totcross = 0;
  for (i = 1; i <= sewd->NbEdges(); i++)
  {
    TopoDS_Edge          edge = sewd->Edge(i);
    double        f2d, l2d;
    occ::handle<Geom2d_Curve> c2d = BRep_Tool::CurveOnSurface(edge, aFace, f2d, l2d);
    if (!c2d.IsNull())
    {
      nbc++;
      NCollection_Sequence<gp_Pnt2d> SeqPnt;
      ShapeAnalysis_Curve::GetSamplePoints(c2d, f2d, l2d, SeqPnt);
      if (edge.Orientation() == 1)
        ReverseSeq(SeqPnt);
      if (nbc == 1)
      {
        fuv = SeqPnt.Value(1);
        uv0 = fuv;
      }
      int j = 1;
      for (; j <= SeqPnt.Length(); j++)
      {
        luv = SeqPnt.Value(j);
        totcross += (fuv.X() - luv.X()) * (fuv.Y() + luv.Y()) / 2;
        fuv = luv;
      }
    }
  }
  totcross += (fuv.X() - uv0.X()) * (fuv.Y() + uv0.Y()) / 2;
  return totcross;
}

//=================================================================================================

double ShapeAnalysis::ContourArea(const TopoDS_Wire& theWire)
// const occ::handle<ShapeExtend_WireData>& sewd)

{
  int nbc = 0;
  gp_Pnt           fuv, luv, uv0;
  // double totcross=0;
  gp_XYZ          aTotal(0., 0., 0.);
  TopoDS_Iterator aIte(theWire, false);
  // for(i=1; i<=sewd->NbEdges(); i++) {
  for (; aIte.More(); aIte.Next())
  {
    TopoDS_Edge        edge = TopoDS::Edge(aIte.Value()); // sewd->Edge(i);
    double      first, last;
    occ::handle<Geom_Curve> c3d = BRep_Tool::Curve(edge, first, last);
    if (!c3d.IsNull())
    {

      NCollection_Sequence<gp_Pnt> aSeqPnt;
      if (!ShapeAnalysis_Curve::GetSamplePoints(c3d, first, last, aSeqPnt))
        continue;
      nbc++;
      if (edge.Orientation() == TopAbs_REVERSED)
        ReverseSeq(aSeqPnt);
      if (nbc == 1)
      {
        fuv = aSeqPnt.Value(1);
        uv0 = fuv;
      }
      int j = 1;
      for (; j <= aSeqPnt.Length(); j++)
      {
        luv = aSeqPnt.Value(j);
        aTotal += luv.XYZ() ^ fuv.XYZ(); //
        fuv = luv;
      }
    }
  }
  aTotal += uv0.XYZ() ^ fuv.XYZ(); //
  double anArea = aTotal.Modulus() * 0.5;
  return anArea;
}

//=================================================================================================

bool ShapeAnalysis::IsOuterBound(const TopoDS_Face& face)
{
  TopoDS_Face F = face;
  TopoDS_Wire W;
  F.Orientation(TopAbs_FORWARD);
  int nbw = 0;
  for (TopExp_Explorer exp(F, TopAbs_WIRE); exp.More(); exp.Next())
  {
    W = TopoDS::Wire(exp.Current());
    nbw++;
  }
  // skl 08.04.2002
  if (nbw == 1)
  {
    occ::handle<ShapeExtend_WireData> sewd     = new ShapeExtend_WireData(W);
    double                totcross = TotCross2D(sewd, F);
    return (totcross >= 0);
  }
  else
  {
    BRepAdaptor_Surface     Ads(F, false);
    double           tol   = BRep_Tool::Tolerance(F);
    double           toluv = std::min(Ads.UResolution(tol), Ads.VResolution(tol));
    BRepTopAdaptor_FClass2d fcl(F, toluv);
    bool        rescl = (fcl.PerformInfinitePoint() == TopAbs_OUT);
    return rescl;
  }
}

//=======================================================================
// function : OuterWire
// purpose  : Returns positively oriented wire in the face.
//           If there is no one - returns the last wire of the face.
//=======================================================================

TopoDS_Wire ShapeAnalysis::OuterWire(const TopoDS_Face& theFace)
{
  TopoDS_Face aF = theFace;
  aF.Orientation(TopAbs_FORWARD);

  TopExp_Explorer anIt(aF, TopAbs_WIRE);
  while (anIt.More())
  {
    TopoDS_Wire aWire = TopoDS::Wire(anIt.Value());
    anIt.Next();

    // if current wire is the last one, return it without analysis
    if (!anIt.More())
      return aWire;

    // Check if the wire has positive area
    occ::handle<ShapeExtend_WireData> aSEWD    = new ShapeExtend_WireData(aWire);
    double                anArea2d = ShapeAnalysis::TotCross2D(aSEWD, aF);
    if (anArea2d >= 0.)
      return aWire;
  }
  return TopoDS_Wire();
}

//=================================================================================================

void ShapeAnalysis::GetFaceUVBounds(const TopoDS_Face& F,
                                    double&     UMin,
                                    double&     UMax,
                                    double&     VMin,
                                    double&     VMax)
{
  TopoDS_Face FF = F;
  FF.Orientation(TopAbs_FORWARD);
  TopExp_Explorer ex(FF, TopAbs_EDGE);
  if (!ex.More())
  {
    TopLoc_Location L;
    BRep_Tool::Surface(F, L)->Bounds(UMin, UMax, VMin, VMax);
    return;
  }

  Bnd_Box2d           B;
  ShapeAnalysis_Edge  sae;
  ShapeAnalysis_Curve sac;
  for (; ex.More(); ex.Next())
  {
    TopoDS_Edge          edge = TopoDS::Edge(ex.Current());
    occ::handle<Geom2d_Curve> c2d;
    double        f, l;
    if (!sae.PCurve(edge, F, c2d, f, l, false))
      continue;
    sac.FillBndBox(c2d, f, l, 20, true, B);
  }
  B.Get(UMin, VMin, UMax, VMax);
}
