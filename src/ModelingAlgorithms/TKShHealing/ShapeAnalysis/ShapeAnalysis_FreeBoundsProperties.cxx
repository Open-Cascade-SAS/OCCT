// Created on: 1998-08-04
// Created by: Pavel DURANDIN
// Copyright (c) 1998-1999 Matra Datavision
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

// 25.12.98 pdn renaming of ShapeAnalysis_FreeBounds and ShapeAnalysis_Wire methods
// szv#4 S4163

#include <BRep_Builder.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom_Curve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <ShapeAnalysis_FreeBoundsProperties.hxx>
#include <ShapeAnalysis_Wire.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <ShapeExtend_WireData.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

#define NbControl 23

static void ContourProperties(const TopoDS_Wire& wire, double& countourArea, double& countourLength)
{
  int    nbe    = 0;
  double length = 0.0;
  gp_XYZ area(.0, .0, .0);
  gp_XYZ prev, cont;

  for (BRepTools_WireExplorer exp(wire); exp.More(); exp.Next())
  {
    const TopoDS_Edge& Edge = exp.Current();
    nbe++;

    double                  First, Last;
    occ::handle<Geom_Curve> c3d;
    ShapeAnalysis_Edge      sae;
    if (!sae.Curve3d(Edge, c3d, First, Last))
      continue;

    int ibeg = 0;
    if (nbe == 1)
    {
      gp_Pnt pntIni = c3d->Value(First);
      prev          = pntIni.XYZ();
      cont          = prev;
      ibeg          = 1;
    }

    for (int i = ibeg; i < NbControl; i++)
    {
      double prm     = ((NbControl - 1 - i) * First + i * Last) / (NbControl - 1);
      gp_Pnt pntCurr = c3d->Value(prm);
      gp_XYZ curr    = pntCurr.XYZ();
      gp_XYZ delta   = curr - prev;
      length += delta.Modulus();
      area += curr ^ prev;
      prev = curr;
    }
  }

  area += cont ^ prev;
  countourArea   = area.Modulus() / 2;
  countourLength = length;
}

//=================================================================================================

ShapeAnalysis_FreeBoundsProperties::ShapeAnalysis_FreeBoundsProperties()
{
  myClosedFreeBounds = new NCollection_HSequence<occ::handle<ShapeAnalysis_FreeBoundData>>();
  myOpenFreeBounds   = new NCollection_HSequence<occ::handle<ShapeAnalysis_FreeBoundData>>();
  myTolerance        = 0.;
}

//=======================================================================
// function : ShapeAnalysis_FreeBoundsProperties
// purpose  : Creates the object and calls corresponding Init.
//    	     <shape> should be a compound of faces.
//=======================================================================

ShapeAnalysis_FreeBoundsProperties::ShapeAnalysis_FreeBoundsProperties(const TopoDS_Shape& shape,
                                                                       const double tolerance,
                                                                       const bool   splitclosed,
                                                                       const bool   splitopen)
{
  myClosedFreeBounds = new NCollection_HSequence<occ::handle<ShapeAnalysis_FreeBoundData>>();
  myOpenFreeBounds   = new NCollection_HSequence<occ::handle<ShapeAnalysis_FreeBoundData>>();
  Init(shape, tolerance, splitclosed, splitopen);
}

//=======================================================================
// function : ShapeAnalysis_FreeBoundsProperties
// purpose  : Creates the object and calls corresponding Init.
//    	     <shape> should be a compound of shells.
//=======================================================================

ShapeAnalysis_FreeBoundsProperties::ShapeAnalysis_FreeBoundsProperties(const TopoDS_Shape& shape,
                                                                       const bool splitclosed,
                                                                       const bool splitopen)
{
  myClosedFreeBounds = new NCollection_HSequence<occ::handle<ShapeAnalysis_FreeBoundData>>();
  myOpenFreeBounds   = new NCollection_HSequence<occ::handle<ShapeAnalysis_FreeBoundData>>();
  myTolerance        = 0.;
  Init(shape, splitclosed, splitopen);
}

//=======================================================================
// function : Init
// purpose  : Initializes the object with given parameters.
//   	     <shape> should be a compound of faces.
//=======================================================================

void ShapeAnalysis_FreeBoundsProperties::Init(const TopoDS_Shape& shape,
                                              const double        tolerance,
                                              const bool          splitclosed,
                                              const bool          splitopen)
{
  Init(shape, splitclosed, splitopen);
  myTolerance = tolerance;
}

//=======================================================================
// function : Init
// purpose  : Initializes the object with given parameters.
//   	     <shape> should be a compound of shells.
//=======================================================================

void ShapeAnalysis_FreeBoundsProperties::Init(const TopoDS_Shape& shape,
                                              const bool          splitclosed,
                                              const bool          splitopen)
{
  myShape       = shape;
  mySplitClosed = splitclosed;
  mySplitOpen   = splitopen;
}

//=======================================================================
// function : Perform
// purpose  : Builds and analyzes free bounds of the shape.
//           First calls ShapeAnalysis_FreeBounds for building free
//           bounds.
//           Then on each free bound computes its properties:
//           - area of the contour,
//           - perimeter of the contour,
//           - ratio of average length to average width of the contour,
//           - average width of contour,
//           - notches on the contour and for each notch
//            - maximum width of the notch.
// returns:   True  - if no fails and free bounds are found,
//           False - if fail or no free bounds are found
//=======================================================================

bool ShapeAnalysis_FreeBoundsProperties::Perform()
{
  bool result = false;
  result |= DispatchBounds();
  result |= CheckNotches();
  result |= CheckContours();
  return result;
}

//=================================================================================================

bool ShapeAnalysis_FreeBoundsProperties::DispatchBounds()
{
  if (!IsLoaded())
    return false;

  TopoDS_Compound tmpClosedBounds, tmpOpenBounds;
  if (myTolerance > 0.)
  {
    ShapeAnalysis_FreeBounds safb(myShape, myTolerance, mySplitClosed, mySplitOpen);
    tmpClosedBounds = safb.GetClosedWires();
    tmpOpenBounds   = safb.GetOpenWires();
  }
  else
  {
    ShapeAnalysis_FreeBounds safb(myShape, mySplitClosed, mySplitOpen);
    tmpClosedBounds = safb.GetClosedWires();
    tmpOpenBounds   = safb.GetOpenWires();
  }

  ShapeExtend_Explorer                             shexpl;
  occ::handle<NCollection_HSequence<TopoDS_Shape>> tmpSeq =
    shexpl.SeqFromCompound(tmpClosedBounds, false);
  int i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= tmpSeq->Length(); i++)
  {
    TopoDS_Wire                              wire   = TopoDS::Wire(tmpSeq->Value(i));
    occ::handle<ShapeAnalysis_FreeBoundData> fbData = new ShapeAnalysis_FreeBoundData();
    fbData->SetFreeBound(wire);
    myClosedFreeBounds->Append(fbData);
  }

  occ::handle<NCollection_HSequence<TopoDS_Shape>> tmpSeq2 =
    shexpl.SeqFromCompound(tmpOpenBounds, false);
  for (i = 1; i <= tmpSeq2->Length(); i++)
  {
    TopoDS_Wire                              wire   = TopoDS::Wire(tmpSeq2->Value(i));
    occ::handle<ShapeAnalysis_FreeBoundData> fbData = new ShapeAnalysis_FreeBoundData;
    fbData->SetFreeBound(wire);
    myOpenFreeBounds->Append(fbData);
  }

  return true;
}

//=================================================================================================

bool ShapeAnalysis_FreeBoundsProperties::CheckNotches(const double prec)
{
  int i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= myClosedFreeBounds->Length(); i++)
  {
    occ::handle<ShapeAnalysis_FreeBoundData> fbData = myClosedFreeBounds->Value(i);
    CheckNotches(fbData, prec);
  }
  for (i = 1; i <= myOpenFreeBounds->Length(); i++)
  {
    occ::handle<ShapeAnalysis_FreeBoundData> fbData = myOpenFreeBounds->Value(i);
    CheckNotches(fbData, prec);
  }

  return true;
}

//=================================================================================================

bool ShapeAnalysis_FreeBoundsProperties::CheckNotches(
  occ::handle<ShapeAnalysis_FreeBoundData>& fbData,
  const double                              prec)
{
  ShapeExtend_WireData swd(fbData->FreeBound());
  if (swd.NbEdges() > 1)
    for (int j = 1; j <= swd.NbEdges(); j++)
    {
      TopoDS_Wire notch;
      double      dMax;
      if (CheckNotches(fbData->FreeBound(), j, notch, dMax, prec))
        fbData->AddNotch(notch, dMax);
    }

  return true;
}

//=================================================================================================

bool ShapeAnalysis_FreeBoundsProperties::CheckContours(const double prec)
{
  bool status = false;
  int  i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= myClosedFreeBounds->Length(); i++)
  {
    occ::handle<ShapeAnalysis_FreeBoundData> fbData = myClosedFreeBounds->Value(i);
    status |= FillProperties(fbData, prec);
  }
  for (i = 1; i <= myOpenFreeBounds->Length(); i++)
  {
    occ::handle<ShapeAnalysis_FreeBoundData> fbData = myOpenFreeBounds->Value(i);
    status |= FillProperties(fbData, prec);
  }

  return status;
}

//=================================================================================================

bool ShapeAnalysis_FreeBoundsProperties::CheckNotches(const TopoDS_Wire& wire,
                                                      const int          num,
                                                      TopoDS_Wire&       notch,
                                                      double&            distMax,
                                                      const double /*prec*/)
{
  double                            tol = std::max(myTolerance, Precision::Confusion());
  occ::handle<ShapeExtend_WireData> wdt = new ShapeExtend_WireData(wire);
  BRep_Builder                      B;
  B.MakeWire(notch);

  if ((num <= 0) || (num > wdt->NbEdges()))
    return false;

  int n1 = (num > 0 ? num : wdt->NbEdges());
  int n2 = (n1 < wdt->NbEdges() ? n1 + 1 : 1);

  TopoDS_Edge E1 = wdt->Edge(n1);
  B.Add(notch, E1);

  occ::handle<ShapeAnalysis_Wire> saw = new ShapeAnalysis_Wire;
  saw->Load(wdt);
  saw->SetPrecision(myTolerance);
  if (saw->CheckSmall(n2, tol))
  {
    B.Add(notch, wdt->Edge(n2));
    n2 = (n2 < wdt->NbEdges() ? n2 + 1 : 1);
  }

  TopoDS_Edge E2 = wdt->Edge(n2);
  B.Add(notch, E2);

  double                  First1, Last1, First2, Last2;
  occ::handle<Geom_Curve> c3d1, c3d2;
  ShapeAnalysis_Edge      sae;
  // szv#4:S4163:12Mar99 optimized
  if (!sae.Curve3d(E1, c3d1, First1, Last1) || !sae.Curve3d(E2, c3d2, First2, Last2))
    return false;

  gp_Pnt pnt;
  gp_Vec vec1, vec2;
  c3d1->D1(Last1, pnt, vec1);
  c3d2->D1(First2, pnt, vec2);
  if (E1.Orientation() == TopAbs_REVERSED)
    vec1.Reverse();
  if (E2.Orientation() == TopAbs_REVERSED)
    vec2.Reverse();

  double angl = std::abs(vec1.Angle(vec2));
  if (angl > 0.95 * M_PI)
  {
    distMax = .0;
    for (int i = 0; i < NbControl; i++)
    {
      double prm     = ((NbControl - 1 - i) * First1 + i * Last1) / (NbControl - 1);
      gp_Pnt pntCurr = c3d1->Value(prm);

      double p1, p2;
      if (First2 < Last2)
      {
        p1 = First2;
        p2 = Last2;
      }
      else
      {
        p1 = Last2;
        p2 = First2;
      }

      // szv#4:S4163:12Mar99 warning
      GeomAPI_ProjectPointOnCurve ppc(pntCurr, c3d2, p1, p2);
      double                      newDist = (ppc.NbPoints() ? ppc.LowerDistance() : 0);
      if (newDist > distMax)
        distMax = newDist;
    }

    return true;
  }

  return false;
}

//=================================================================================================

bool ShapeAnalysis_FreeBoundsProperties::FillProperties(
  occ::handle<ShapeAnalysis_FreeBoundData>& fbData,
  const double /*prec*/)
{
  double area, length;
  ContourProperties(fbData->FreeBound(), area, length);

  double r    = 0;
  double aver = 0;

  if (length != 0.)
  {                                      // szv#4:S4163:12Mar99 anti-exception
    double k = area / (length * length); // szv#4:S4163:12Mar99
    // szv#4:S4163:12Mar99 optimized
    if (k != 0.)
    { // szv#4:S4163:12Mar99 anti-exception
      double aux = 1. - 16. * k;
      if (aux >= 0.)
      {
        r    = (1. + sqrt(aux)) / (8. * k);
        aver = length / (2. * r);
        r -= 1.;
      }
    }
  }

  fbData->SetArea(area);
  fbData->SetPerimeter(length);
  fbData->SetRatio(r);
  fbData->SetWidth(aver);

  return true;
}
