// Created on: 2000-02-07
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepTools.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomInt_WLApprox.hxx>
#include <gp_Pnt.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf_LineOn2S.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Precision.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeAlgo_ToolContainer.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeConstruct.hxx>
#include <ShapeCustom_Surface.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_ShapeDivideContinuity.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeAlgo_AlgoContainer, Standard_Transient)

//=================================================================================================

ShapeAlgo_AlgoContainer::ShapeAlgo_AlgoContainer()
{
  myTC = new ShapeAlgo_ToolContainer;
}

//=================================================================================================

bool ShapeAlgo_AlgoContainer::ConnectNextWire(const occ::handle<ShapeAnalysis_Wire>&   saw,
                                              const occ::handle<ShapeExtend_WireData>& nextsewd,
                                              const double                             maxtol,
                                              double&                                  distmin,
                                              bool&                                    revsewd,
                                              bool& revnextsewd) const
{
  distmin = 0;
  revsewd = revnextsewd = false;
  if (nextsewd->NbEdges() == 0)
    return true;

  occ::handle<ShapeExtend_WireData> sewd = saw->WireData();
  // add edges into empty WireData
  if (sewd->NbEdges() == 0)
  {
    sewd->Add(nextsewd);
    return true;
  }

  double tailhead, tailtail, headtail, headhead;
  saw->CheckShapeConnect(tailhead, tailtail, headtail, headhead, nextsewd->Wire(), maxtol);
  distmin          = tailhead;
  double precision = saw->Precision();

  if (tailhead > precision && tailtail > precision
      && (saw->LastCheckStatus(ShapeExtend_DONE4) || saw->LastCheckStatus(ShapeExtend_DONE3)))
  {
    sewd->Reverse();
    distmin = headhead;
    revsewd = true;
    if (saw->LastCheckStatus(ShapeExtend_DONE3))
    {
      nextsewd->Reverse();
      revnextsewd = true;
      distmin     = headtail;
    }
  }
  else if (!saw->LastCheckStatus(ShapeExtend_FAIL) && !saw->LastCheckStatus(ShapeExtend_DONE5))
  {
    nextsewd->Reverse();
    revnextsewd = true;
    distmin     = tailtail;
  }
  bool OK = !saw->LastCheckStatus(ShapeExtend_FAIL);
  if (OK)
    sewd->Add(nextsewd);
  return OK;
}

//=================================================================================================

void ShapeAlgo_AlgoContainer::ApproxBSplineCurve(
  const occ::handle<Geom_BSplineCurve>&          bspline,
  NCollection_Sequence<occ::handle<Geom_Curve>>& seq) const
{
  seq.Clear();
  occ::handle<Geom_BSplineCurve>                res, modifCurve;
  NCollection_Sequence<occ::handle<Geom_Curve>> SCurve;

  // si la BSpline est de degre 1 , on approxime .
  // on passe par le programme des intersections ou tout le travail
  // est deja fait !!! ( il faut faire des paquets de 30 points
  // maximum , travailler dans un espace 0,1 pour tenir la precision)

  if (bspline->Degree() != 1)
  {
    seq.Append(bspline);
    return;
  }

  // on detecte d`eventuelles cassures par la multiplicite des poles.
  // Puis on approxime chaque "partie" de BSpline

  int                               NbPoles = bspline->NbPoles();
  const NCollection_Array1<gp_Pnt>& Poles   = bspline->Poles();
  const NCollection_Array1<double>& Weigs   = bspline->WeightsArray();
  const NCollection_Array1<double>& Knots   = bspline->Knots();
  const NCollection_Array1<int>&    Mults   = bspline->Multiplicities();
  int                               deg     = bspline->Degree();

  int jpole = 1;
  int j, PoleIndex, I1;
  PoleIndex = 1;
  I1        = 1;
  for (int ipole = 1; ipole < NbPoles; ipole++)
  {
    if (Poles(ipole).IsEqual(Poles(ipole + 1), Precision::Confusion()))
    {
      if (jpole == 1)
      {
        PoleIndex++;
      }
      else
      {
        NCollection_Array1<gp_Pnt> newPoles(1, jpole);
        NCollection_Array1<double> newWeigs(1, jpole);
        newWeigs.Init(1.);
        int                        NbNew = jpole - deg + 1;
        NCollection_Array1<double> newKnots(1, NbNew);
        NCollection_Array1<int>    newMults(1, NbNew);
        for (j = 1; j <= NbNew; j++)
        {
          newKnots(j) = Knots(I1 + j - 1);
          newMults(j) = Mults(I1 + j - 1);
        }
        newMults(1) = newMults(NbNew) = deg + 1;
        for (j = 1; j <= jpole; j++)
        {
          newWeigs(j) = Weigs(PoleIndex);
          newPoles(j) = Poles(PoleIndex++);
        }

        occ::handle<Geom_BSplineCurve> newC =
          new Geom_BSplineCurve(newPoles, newWeigs, newKnots, newMults, deg);
        SCurve.Append(newC);
        I1    = ipole + 1;
        jpole = 1;
      }
    }
    else
    {
      jpole++;
    }
  }

  occ::handle<Geom_BSplineCurve> mycurve;
  int                            nbcurves = SCurve.Length();
  if (nbcurves == 0)
  {
    nbcurves = 1;
    SCurve.Append(bspline);
  }

  for (int itab = 1; itab <= nbcurves; itab++)
  {
    mycurve = occ::down_cast<Geom_BSplineCurve>(SCurve.Value(itab));
    jpole   = mycurve->NbPoles();
    if (jpole > 2)
    {
      const NCollection_Array1<gp_Pnt>& newP = mycurve->Poles();
      occ::handle<IntSurf_LineOn2S>     R    = new IntSurf_LineOn2S();
      double                            u1, v1, u2, v2;
      u1 = v1 = 0.;
      u2 = v2 = 1.;
      for (j = 1; j <= jpole; j++)
      {
        IntSurf_PntOn2S POn2S;
        POn2S.SetValue(newP(j), u1, v1, u2, v2);
        R->Add(POn2S);
      }
      GeomInt_WLApprox theapp3d;
      constexpr double Tol = Precision::Approximation();
      theapp3d.SetParameters(Tol, Tol, 4, 8, 0, 30, true);
      occ::handle<IntPatch_WLine> WL        = new IntPatch_WLine(R, false);
      int                         indicemin = 1;
      int                         indicemax = jpole;
      theapp3d.Perform(WL, true, false, false, indicemin, indicemax);
      if (!theapp3d.IsDone())
      {
        modifCurve = mycurve;
      }
      else if (theapp3d.NbMultiCurves() != 1)
      {
        modifCurve = mycurve;
      }
      else
      {
        const AppParCurves_MultiBSpCurve& mbspc   = theapp3d.Value(1);
        int                               nbpoles = mbspc.NbPoles();
        NCollection_Array1<gp_Pnt>        tpoles(1, nbpoles);
        mbspc.Curve(1, tpoles);
        modifCurve =
          new Geom_BSplineCurve(tpoles, mbspc.Knots(), mbspc.Multiplicities(), mbspc.Degree());
      }
    }
    else
    {
      modifCurve = mycurve;
    }
    seq.Append(modifCurve);
  }
}

//=================================================================================================

void ShapeAlgo_AlgoContainer::ApproxBSplineCurve(
  const occ::handle<Geom2d_BSplineCurve>&          bspline,
  NCollection_Sequence<occ::handle<Geom2d_Curve>>& seq) const
{
  seq.Clear();
  occ::handle<Geom2d_BSplineCurve>                res, modifCurve;
  NCollection_Sequence<occ::handle<Geom2d_Curve>> SCurve;

  // si la BSpline est de degre 1 , on approxime .
  // on passe par le programme des intersections ou tout le travail
  // est deja fait !!! ( il faut faire des paquets de 30 points
  // maximum , travailler dans un espace 0,1 pour tenir la precision
  // puis reconstruire une BSpline somme des toutes les Bspline).

  if (bspline->Degree() != 1)
  {
    seq.Append(bspline);
    return;
  }

  // on detecte d`eventuelles cassures par la multiplicite des poles.
  // Puis on approxime chaque "partie" de BSpline et on reconstruit
  // une BSpline = somme des BSplines traitees

  int                                 NbPoles = bspline->NbPoles();
  const NCollection_Array1<gp_Pnt2d>& Poles   = bspline->Poles();
  const NCollection_Array1<double>&   Weigs   = bspline->WeightsArray();
  const NCollection_Array1<double>&   Knots   = bspline->Knots();
  const NCollection_Array1<int>&      Mults   = bspline->Multiplicities();
  int                                 deg     = bspline->Degree();

  int jpole = 1;
  int j, PoleIndex, I1;
  PoleIndex = 1;
  I1        = 1;
  for (int ipole = 1; ipole < NbPoles; ipole++)
  {
    if (Poles(ipole).IsEqual(Poles(ipole + 1), Precision::PConfusion()))
    {
      if (jpole == 1)
      {
        PoleIndex++;
      }
      else
      {
        NCollection_Array1<gp_Pnt2d> newPoles(1, jpole);
        NCollection_Array1<double>   newWeigs(1, jpole);
        newWeigs.Init(1.);
        int                        NbNew = jpole - deg + 1;
        NCollection_Array1<double> newKnots(1, NbNew);
        NCollection_Array1<int>    newMults(1, NbNew);
        for (j = 1; j <= NbNew; j++)
        {
          newKnots(j) = Knots(I1 + j - 1);
          newMults(j) = Mults(I1 + j - 1);
        }
        newMults(1) = newMults(NbNew) = deg + 1;
        for (j = 1; j <= jpole; j++)
        {
          newWeigs(j) = Weigs(PoleIndex);
          newPoles(j) = Poles(PoleIndex++);
        }

        occ::handle<Geom2d_BSplineCurve> newC =
          new Geom2d_BSplineCurve(newPoles, newWeigs, newKnots, newMults, deg);
        SCurve.Append(newC);
        I1    = ipole + 1;
        jpole = 1;
      }
    }
    else
    {
      jpole++;
    }
  }

  occ::handle<Geom2d_BSplineCurve> mycurve;
  int                              nbcurves = SCurve.Length();
  if (nbcurves == 0)
  {
    nbcurves = 1;
    SCurve.Append(bspline);
  }

  for (int itab = 1; itab <= nbcurves; itab++)
  {
    mycurve = occ::down_cast<Geom2d_BSplineCurve>(SCurve.Value(itab));
    jpole   = mycurve->NbPoles();
    if (jpole > 10)
    {
      NCollection_Array1<gp_Pnt>          P(1, jpole);
      const NCollection_Array1<gp_Pnt2d>& newP = mycurve->Poles();
      occ::handle<IntSurf_LineOn2S>       R    = new IntSurf_LineOn2S();
      double                              u2, v2;
      u2 = v2 = 1.;
      for (j = 1; j <= jpole; j++)
      {
        IntSurf_PntOn2S POn2S;
        POn2S.SetValue(P(j), newP(j).X(), newP(j).Y(), u2, v2);
        R->Add(POn2S);
      }
      GeomInt_WLApprox theapp3d;
      constexpr double Tol = Precision::PApproximation();
      theapp3d.SetParameters(Tol, Tol, 4, 8, 0, 30, true);
      occ::handle<IntPatch_WLine> WL        = new IntPatch_WLine(R, false);
      int                         indicemin = 1;
      int                         indicemax = jpole;
      theapp3d.Perform(WL, false, true, false, indicemin, indicemax);
      if (!theapp3d.IsDone())
      {
        modifCurve = mycurve;
      }
      else if (theapp3d.NbMultiCurves() != 1)
      {
        modifCurve = mycurve;
      }
      else
      {
        const AppParCurves_MultiBSpCurve& mbspc   = theapp3d.Value(1);
        int                               nbpoles = mbspc.NbPoles();
        NCollection_Array1<gp_Pnt2d>      tpoles(1, nbpoles);
        mbspc.Curve(1, tpoles);
        for (j = 1; j <= jpole; j++)
        {
        }
        modifCurve =
          new Geom2d_BSplineCurve(tpoles, mbspc.Knots(), mbspc.Multiplicities(), mbspc.Degree());
      }
    }
    else
    {
      modifCurve = mycurve;
    }
    seq.Append(modifCurve);
  }
}

//=================================================================================================

TopoDS_Shape ShapeAlgo_AlgoContainer::C0ShapeToC1Shape(const TopoDS_Shape& shape,
                                                       const double        tol) const
{
  ShapeUpgrade_ShapeDivideContinuity sdc(shape);
  sdc.SetTolerance(tol);
  sdc.SetBoundaryCriterion(GeomAbs_C1);
  sdc.SetSurfaceCriterion(GeomAbs_C1);
  sdc.Perform();
  return sdc.Result();
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> ShapeAlgo_AlgoContainer::ConvertSurfaceToBSpline(
  const occ::handle<Geom_Surface>& surf,
  const double                     UF,
  const double                     UL,
  const double                     VF,
  const double                     VL) const
{
  return ShapeConstruct::ConvertSurfaceToBSpline(surf,
                                                 UF,
                                                 UL,
                                                 VF,
                                                 VL,
                                                 Precision::Confusion(),
                                                 GeomAbs_C1,
                                                 100,
                                                 Geom_BSplineSurface::MaxDegree());
}

//=================================================================================================

bool ShapeAlgo_AlgoContainer::HomoWires(const TopoDS_Wire& wireIn1,
                                        const TopoDS_Wire& wireIn2,
                                        TopoDS_Wire&       wireOut1,
                                        TopoDS_Wire&       wireOut2,
                                        const bool) const
{
  // bool res = false; //szv#4:S4163:12Mar99 not needed
  TopoDS_Iterator Cook, Perry;
  TopoDS_Edge     edge1, edge2;
  TopLoc_Location loc1, loc2;
  //  BRepBuilderAPI_MakeWire makeWire1, makeWire2;
  ShapeExtend_WireData makeWire1, makeWire2;
  bool                 iterCook, iterPerry;
  int                  nEdges1, nEdges2;
  double               length1, length2;
  double               first1, first2;
  double               last1, last2;
  double               delta1, delta2;

  occ::handle<Geom_Curve> crv1;
  occ::handle<Geom_Curve> crv2;

  // bool notEnd  = true; //szv#4:S4163:12Mar99 unused
  int nbCreatedEdges = 0;
  // gka
  // TopoDS_Vertex v11,v12,v21,v22
  occ::handle<ShapeFix_Wire> sfw = new ShapeFix_Wire();
  sfw->Load(wireIn1);
  sfw->FixReorder();
  TopoDS_Wire wireIn11 = sfw->Wire();
  sfw->Load(wireIn2);
  sfw->FixReorder();
  TopoDS_Wire wireIn22 = sfw->Wire();

  iterCook = iterPerry = true;
  length1 = length2 = 0.;
  nEdges1 = nEdges2 = 0;
  for (Cook.Initialize(wireIn11); Cook.More(); Cook.Next())
  {
    nEdges1++;
    edge1 = TopoDS::Edge(Cook.Value());
    crv1  = BRep_Tool::Curve(edge1, loc1, first1, last1);
    length1 += last1 - first1;
  }
  for (Perry.Initialize(wireIn22); Perry.More(); Perry.Next())
  {
    nEdges2++;
    edge2 = TopoDS::Edge(Perry.Value());
    crv2  = BRep_Tool::Curve(edge2, loc2, first2, last2);
    length2 += last2 - first2;
  }
  double epsilon = Precision::PConfusion() * (length1 + length2);
  if (nEdges1 == 1 && nEdges2 == 1)
  {
    wireOut1 = wireIn11;
    wireOut2 = wireIn22;
    return true; // szv#4:S4163:12Mar99 `res=` not needed
  }

  if (length1 < epsilon)
  {
    Cook.Initialize(wireIn11);
    for (Perry.Initialize(wireIn22); Perry.More(); Perry.Next())
    {
      edge1 = TopoDS::Edge(Cook.Value());
      makeWire1.Add(edge1);
    }
    wireOut1 = makeWire1.Wire();
    wireOut2 = wireIn22;
    return true; // szv#4:S4163:12Mar99 `res=` not needed
  }
  if (length2 < epsilon)
  {
    Perry.Initialize(wireIn22);
    for (Cook.Initialize(wireIn11); Cook.More(); Cook.Next())
    {
      edge2 = TopoDS::Edge(Perry.Value());
      makeWire2.Add(edge2);
    }
    wireOut1 = wireIn11;
    wireOut2 = makeWire2.Wire();
    return true; // szv#4:S4163:12Mar99 `res=` not needed
  }

  double ratio = length2 / length1;

  Cook.Initialize(wireIn11);
  Perry.Initialize(wireIn22);
  edge1 = TopoDS::Edge(Cook.Value());
  edge2 = TopoDS::Edge(Perry.Value());
  // essai mjm du 22/05/97
  bool IsToReverse1 = false;
  bool IsToReverse2 = false;
  if (edge1.Orientation() == TopAbs_REVERSED)
    IsToReverse1 = true;
  if (edge2.Orientation() == TopAbs_REVERSED)
    IsToReverse2 = true;
  crv1   = BRep_Tool::Curve(edge1, loc1, first1, last1);
  crv2   = BRep_Tool::Curve(edge2, loc2, first2, last2);
  delta1 = last1 - first1;
  delta2 = last2 - first2;
  while (nbCreatedEdges < (nEdges1 + nEdges2 - 1))
  { /*just a security. */

    if ((delta1 * ratio - delta2) > epsilon)
    {
      BRepBuilderAPI_MakeEdge makeEdge1;
      if (!IsToReverse1)
      {
        makeEdge1.Init(crv1, first1, first1 + delta2 / ratio);
        first1 += delta2 / ratio;
      }
      else
      { // gka BUC60685
        makeEdge1.Init(crv1, last1 - delta2 / ratio, last1);
        last1 -= delta2 / ratio;
      }
      BRepBuilderAPI_MakeEdge makeEdge2(crv2, first2, last2);
      edge1 = makeEdge1.Edge();
      edge2 = makeEdge2.Edge();
      // essai mjm du 22/05/97
      iterCook = false;
      // first1   += delta2/ratio;
      delta1    = last1 - first1;
      iterPerry = true;
      nbCreatedEdges++;
    }
    else if (std::abs(delta1 * ratio - delta2) <= epsilon)
    {
      BRepBuilderAPI_MakeEdge makeEdge1(crv1, first1, last1);
      BRepBuilderAPI_MakeEdge makeEdge2(crv2, first2, last2);
      edge1     = makeEdge1.Edge();
      edge2     = makeEdge2.Edge();
      iterCook  = true;
      iterPerry = true;
      nbCreatedEdges += 2;
    }
    else /*((delta1*ratio - delta2) < -epsilon)*/
    {
      BRepBuilderAPI_MakeEdge makeEdge1(crv1, first1, last1);
      edge1 = makeEdge1.Edge();
      BRepBuilderAPI_MakeEdge makeEdge2;
      if (!IsToReverse2)
      {
        makeEdge2.Init(crv2, first2, first2 + delta1 * ratio);
        first2 += delta1 * ratio;
      }
      else
      { // gka BUC60685
        makeEdge2.Init(crv2, last2 - delta1 * ratio, last2);
        last2 -= delta1 * ratio;
      }
      edge1     = makeEdge1.Edge();
      edge2     = makeEdge2.Edge();
      iterCook  = true;
      iterPerry = false;
      // first2   += delta1*ratio;
      delta2 = last2 - first2;
      nbCreatedEdges++;
    }
    edge1.Move(loc1);
    edge2.Move(loc2);
    if (IsToReverse1)
      edge1.Reverse();
    if (IsToReverse2)
      edge2.Reverse();
    makeWire1.Add(edge1);
    makeWire2.Add(edge2);

    if (iterCook && iterPerry)
    {
      TopoDS_Iterator Copernic = Cook;
      if (Copernic.More())
        Copernic.Next();
      if (!Copernic.More())
      {
        wireOut1 = makeWire1.Wire();
        wireOut2 = makeWire2.Wire();
        return true; // szv#4:S4163:12Mar99 `res=` not needed
      }
    }
    if (iterCook)
    {
      Cook.Next();
      edge1        = TopoDS::Edge(Cook.Value());
      IsToReverse1 = edge1.Orientation() == TopAbs_REVERSED;
      crv1         = BRep_Tool::Curve(edge1, loc1, first1, last1);
      delta1       = last1 - first1;
    }
    if (iterPerry)
    {
      Perry.Next();
      edge2        = TopoDS::Edge(Perry.Value());
      IsToReverse2 = edge2.Orientation() == TopAbs_REVERSED;
      crv2         = BRep_Tool::Curve(edge2, loc2, first2, last2);
      delta2       = last2 - first2;
    }
  }
  return false; // szv#4:S4163:12Mar99 `res=` not needed
}

//=================================================================================================

bool ShapeAlgo_AlgoContainer::C0BSplineToSequenceOfC1BSplineCurve(
  const occ::handle<Geom_BSplineCurve>&                               BS,
  occ::handle<NCollection_HSequence<occ::handle<Geom_BoundedCurve>>>& seqBS) const
{
  return ShapeUpgrade::C0BSplineToSequenceOfC1BSplineCurve(BS, seqBS);
}

//=================================================================================================

bool ShapeAlgo_AlgoContainer::C0BSplineToSequenceOfC1BSplineCurve(
  const occ::handle<Geom2d_BSplineCurve>&                               BS,
  occ::handle<NCollection_HSequence<occ::handle<Geom2d_BoundedCurve>>>& seqBS) const
{
  return ShapeUpgrade::C0BSplineToSequenceOfC1BSplineCurve(BS, seqBS);
}

//=================================================================================================

TopoDS_Wire ShapeAlgo_AlgoContainer::OuterWire(const TopoDS_Face& face) const
{
  return ShapeAnalysis::OuterWire(face);
}

//=================================================================================================

occ::handle<Geom_Surface> ShapeAlgo_AlgoContainer::ConvertToPeriodic(
  const occ::handle<Geom_Surface>& surf) const
{
  ShapeCustom_Surface scs(surf);
  return scs.ConvertToPeriodic(false);
}

//=================================================================================================

void ShapeAlgo_AlgoContainer::GetFaceUVBounds(const TopoDS_Face& F,
                                              double&            Umin,
                                              double&            Umax,
                                              double&            Vmin,
                                              double&            Vmax) const
{
  ShapeAnalysis::GetFaceUVBounds(F, Umin, Umax, Vmin, Vmax);
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> ShapeAlgo_AlgoContainer::ConvertCurveToBSpline(
  const occ::handle<Geom_Curve>& C3D,
  const double                   First,
  const double                   Last,
  const double                   Tol3d,
  const GeomAbs_Shape            Continuity,
  const int                      MaxSegments,
  const int                      MaxDegree) const
{
  return ShapeConstruct::ConvertCurveToBSpline(C3D,
                                               First,
                                               Last,
                                               Tol3d,
                                               Continuity,
                                               MaxSegments,
                                               MaxDegree);
}
