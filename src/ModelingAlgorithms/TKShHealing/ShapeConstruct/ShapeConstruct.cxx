// Created on: 1999-06-17
// Created by: data exchange team
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

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeConstruct.hxx>
#include <ShapeConstruct_Curve.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

//=================================================================================================

occ::handle<Geom_BSplineCurve> ShapeConstruct::ConvertCurveToBSpline(
  const occ::handle<Geom_Curve>& C3D,
  const double                   First,
  const double                   Last,
  const double                   Tol3d,
  const GeomAbs_Shape            Continuity,
  const int                      MaxSegments,
  const int                      MaxDegree)
{
  int                            MaxDeg = MaxDegree;
  occ::handle<Geom_BSplineCurve> aBSpline;
  if (C3D->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
    aBSpline = occ::down_cast<Geom_BSplineCurve>(C3D);
  else
  {
    if (C3D->IsKind(STANDARD_TYPE(Geom_Conic)))
      MaxDeg = std::min(MaxDeg, 6);

    // clang-format off
    occ::handle<Geom_Curve> tcurve = new Geom_TrimmedCurve(C3D,First,Last); //protection against parabols ets
    // clang-format on
    try
    {
      OCC_CATCH_SIGNALS
      GeomConvert_ApproxCurve approx(tcurve, Tol3d, Continuity, MaxSegments, MaxDeg);
      if (approx.HasResult())
        aBSpline = approx.Curve();
      else
        aBSpline = GeomConvert::CurveToBSplineCurve(C3D, Convert_QuasiAngular);
    }
    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: GeomConvert_ApproxSurface Exception:  ";
      anException.Print(std::cout);
      std::cout << std::endl;
#endif
      (void)anException;
      aBSpline = GeomConvert::CurveToBSplineCurve(C3D, Convert_QuasiAngular);
    }
  }
  return aBSpline;
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> ShapeConstruct::ConvertCurveToBSpline(
  const occ::handle<Geom2d_Curve>& C2D,
  const double                     First,
  const double                     Last,
  const double                     Tol2d,
  const GeomAbs_Shape              Continuity,
  const int                        MaxSegments,
  const int                        MaxDegree)
{
  occ::handle<Geom2d_BSplineCurve> aBSpline2d;
  if (C2D->IsKind(STANDARD_TYPE(Geom2d_Conic)))
  {
    // clang-format off
    occ::handle<Geom2d_Curve> tcurve = new Geom2d_TrimmedCurve(C2D,First,Last); //protection against parabols ets
    // clang-format on
    Geom2dConvert_ApproxCurve approx(tcurve, Tol2d, Continuity, MaxSegments, MaxDegree);
    if (approx.HasResult())
      aBSpline2d = approx.Curve();
    else
      aBSpline2d = Geom2dConvert::CurveToBSplineCurve(tcurve, Convert_QuasiAngular);
  }
  else if (!C2D->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    aBSpline2d = Geom2dConvert::CurveToBSplineCurve(C2D, Convert_QuasiAngular);
  }
  else
    aBSpline2d = occ::down_cast<Geom2d_BSplineCurve>(C2D);

  return aBSpline2d;
}

//=================================================================================================

// Note: this method has the same purpose as GeomConvert::SurfaceToBSplineSurface(),
// but treats more correctly offset surfaces and takes parameters such as UV limits
// and degree as arguments instead of deducing them from the surface.
// Eventually it may be merged back to GeomConvert.

occ::handle<Geom_BSplineSurface> ShapeConstruct::ConvertSurfaceToBSpline(
  const occ::handle<Geom_Surface>& surf,
  const double                     UF,
  const double                     UL,
  const double                     VF,
  const double                     VL,
  const double                     Tol3d,
  const GeomAbs_Shape              Continuity,
  const int                        MaxSegments,
  const int                        MaxDegree)
{
  occ::handle<Geom_BSplineSurface> res;

  occ::handle<Geom_Surface> S = surf;
  if (surf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> RTS =
      occ::down_cast<Geom_RectangularTrimmedSurface>(surf);
    S = RTS->BasisSurface();
  }

  // use GeomConvert for direct conversion of analytic surfaces
  if (S->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> aRTS =
      new Geom_RectangularTrimmedSurface(S, UF, UL, VF, VL);
    return GeomConvert::SurfaceToBSplineSurface(aRTS);
  }

  if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
  {
    occ::handle<Geom_SurfaceOfLinearExtrusion> extr =
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(S);
    occ::handle<Geom_Curve> basis = extr->BasisCurve();
    // gp_Dir direction = extr->Direction(); // direction not used (skl)

    GeomAbs_Shape                  cnt = (Continuity > GeomAbs_C2 ? GeomAbs_C2 : Continuity);
    occ::handle<Geom_BSplineCurve> bspl =
      ConvertCurveToBSpline(basis, UF, UL, Tol3d, cnt, MaxSegments, MaxDegree);

    gp_Trsf shiftF, shiftL;
    shiftF.SetTranslation(extr->Value(UF, 0), extr->Value(UF, VF));
    shiftL.SetTranslation(extr->Value(UF, 0), extr->Value(UF, VL));

    int                              nbPoles = bspl->NbPoles();
    const NCollection_Array1<gp_Pnt>& poles   = bspl->Poles();
    const NCollection_Array1<double>& weights = bspl->WeightsArray();
    const NCollection_Array1<double>& knots = bspl->Knots();
    const NCollection_Array1<int>&    mults = bspl->Multiplicities();

    NCollection_Array2<gp_Pnt> resPoles(1, nbPoles, 1, 2);
    NCollection_Array2<double> resWeigth(1, nbPoles, 1, 2);
    for (int j = 1; j <= nbPoles; j++)
    {
      resPoles(j, 1)  = poles(j).Transformed(shiftF);
      resPoles(j, 2)  = poles(j).Transformed(shiftL);
      resWeigth(j, 1) = weights(j);
      resWeigth(j, 2) = weights(j);
    }

    NCollection_Array1<double> vknots(1, 2);
    NCollection_Array1<int>    vmults(1, 2);
    vknots(1) = VF;
    vknots(2) = VL;
    vmults(1) = vmults(2) = 2;

    occ::handle<Geom_BSplineSurface> bspline = new Geom_BSplineSurface(resPoles,
                                                                       resWeigth,
                                                                       knots,
                                                                       vknots,
                                                                       mults,
                                                                       vmults,
                                                                       bspl->Degree(),
                                                                       1,
                                                                       bspl->IsPeriodic(),
                                                                       false);
    return bspline;
  }

  if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
  {
    occ::handle<Geom_SurfaceOfRevolution> revol = occ::down_cast<Geom_SurfaceOfRevolution>(S);
    occ::handle<Geom_Curve>               basis = revol->BasisCurve();
    if (basis->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
    {
      GeomAbs_Shape cnt = basis->Continuity();
      cnt               = (cnt > GeomAbs_C2 ? GeomAbs_C2 : cnt);
      occ::handle<Geom_BSplineCurve> bspl =
        ConvertCurveToBSpline(basis, VF, VL, Tol3d, cnt, MaxSegments, MaxDegree);
      gp_Ax1                                axis     = revol->Axis();
      occ::handle<Geom_SurfaceOfRevolution> newRevol = new Geom_SurfaceOfRevolution(bspl, axis);
#ifdef OCCT_DEBUG
      std::cout << " Revolution on offset converted" << std::endl;
#endif
      S = newRevol;
    }
  }

  occ::handle<Geom_Surface>        aSurface = new Geom_RectangularTrimmedSurface(S, UF, UL, VF, VL);
  occ::handle<Geom_BSplineSurface> errSpl;
  for (int cnt = (Continuity > GeomAbs_C3 ? GeomAbs_C3 : Continuity); cnt >= 0;)
  {
    try
    {
      OCC_CATCH_SIGNALS
      GeomAbs_Shape aCont = (GeomAbs_Shape)cnt;
      GeomConvert_ApproxSurface
           anApprox(aSurface, Tol3d / 2, aCont, aCont, MaxDegree, MaxDegree, MaxSegments, 0);
      bool Done = anApprox.IsDone();
      if (anApprox.MaxError() <= Tol3d && Done)
      {

#ifdef OCCT_DEBUG
        int nbOfSpan = (anApprox.Surface()->NbUKnots() - 1) * (anApprox.Surface()->NbVKnots() - 1);
        std::cout << "\terror = " << anApprox.MaxError() << "\tspans = " << nbOfSpan << std::endl;
        std::cout << " Surface is approximated with continuity " << (GeomAbs_Shape)cnt << std::endl;
#endif
        S                                    = anApprox.Surface();
        occ::handle<Geom_BSplineSurface> Bsc = occ::down_cast<Geom_BSplineSurface>(S);
        return Bsc;
      }
      else
      {
        if (anApprox.HasResult())
          errSpl = anApprox.Surface();
#ifdef OCCT_DEBUG
        std::cout << "\terror = " << anApprox.MaxError() << std::endl;
#endif
        break;
      }
    }

    catch (Standard_Failure const& anException)
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: GeomConvert_ApproxSurface Exception: try to decrease continuity ";
      anException.Print(std::cout);
      std::cout << std::endl;
#endif
      (void)anException;
      if (cnt > 0)
        cnt--;
      continue;
    }
  }

  return errSpl;
}

//=================================================================================================

bool ShapeConstruct::JoinPCurves(const occ::handle<NCollection_HSequence<TopoDS_Shape>>& edges,
                                 const TopoDS_Face&                                      theFace,
                                 TopoDS_Edge&                                            theEdge)
{
  ShapeAnalysis_Edge sae;
  BRep_Builder       B;

  try
  {
    OCC_CATCH_SIGNALS
    // check if current face is plane.
    occ::handle<Geom_Surface> aGeomSurf = BRep_Tool::Surface(theFace);
    while (aGeomSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {

      aGeomSurf = occ::down_cast<Geom_RectangularTrimmedSurface>(aGeomSurf)->BasisSurface();
    }
    if (aGeomSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
      return true;

    bool                      IsEdgeSeam = false;
    occ::handle<Geom2d_Curve> aCrvRes1, aCrvRes2;
    TopAbs_Orientation        resOrient;
    double                    newf = 0., newl = 0.;
    // iterates on edges
    int i = 1;
    for (; i <= edges->Length(); i++)
    {
      TopoDS_Edge Edge = TopoDS::Edge(edges->Value(i));
      if (i == 1)
        IsEdgeSeam = sae.IsSeam(Edge, theFace);
      else if (IsEdgeSeam && (!sae.IsSeam(Edge, theFace)))
        break; // different cases
      else if (!IsEdgeSeam && (sae.IsSeam(Edge, theFace)))
        break; // different cases

      resOrient = TopAbs_FORWARD;
      occ::handle<Geom2d_Curve> c2d, c2d2;
      double                    first, last, first2, last2;
      if (!sae.PCurve(Edge, theFace, c2d, first, last, false))
        break;

      if (IsEdgeSeam)
      {
        TopoDS_Edge tmpE1 = TopoDS::Edge(Edge.Reversed());
        sae.PCurve(tmpE1, theFace, c2d2, first2, last2, false);
      }

      if (i == 1)
      {
        aCrvRes1 = c2d;
        if (IsEdgeSeam)
        {
          aCrvRes2 = c2d2;
        }
        newf      = first;
        newl      = last;
        resOrient = Edge.Orientation();
      }
      else
      {
        occ::handle<Geom2d_Curve> newCrv;
        bool                      isRev1, isRev2;
        if (!JoinCurves(aCrvRes1,
                        c2d,
                        resOrient,
                        Edge.Orientation(),
                        newf,
                        newl,
                        first,
                        last,
                        newCrv,
                        isRev1,
                        isRev2))
          break;

        if (IsEdgeSeam)
        {
          occ::handle<Geom2d_Curve> newCrv2;
          double                    newf2 = newf, newl2 = newl;

          if (!JoinCurves(aCrvRes2,
                          c2d2,
                          resOrient,
                          Edge.Orientation(),
                          newf2,
                          newl2,
                          first2,
                          last2,
                          newCrv2,
                          isRev1,
                          isRev2))
            break;
          aCrvRes2 = newCrv2;
        }
        aCrvRes1    = newCrv;
        double fp2d = newCrv->FirstParameter();
        double lp2d = newCrv->LastParameter();
        newl += (last - first);
        if (fp2d > newf)
          newf = fp2d;
        if (lp2d < newl)
          newl = lp2d;
      }
    }
    if (IsEdgeSeam)
      B.UpdateEdge(theEdge, aCrvRes1, aCrvRes2, theFace, 0);
    else
      B.UpdateEdge(theEdge, aCrvRes1, theFace, 0);
    B.Range(theEdge, theFace, newf, newl);
    B.SameRange(theEdge, false);
    B.SameParameter(theEdge, false);
    return (i <= edges->Length());
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout
      << "Error: ShapeConstruct::JoinPCurves Exception in GeomConvert_CompCurveToBSplineCurve: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
  }
  return false;
}

//=================================================================================================

template <class HCurve>
static inline HCurve GetCurveCopy(const HCurve&             curve,
                                  double&                   first,
                                  double&                   last,
                                  const TopAbs_Orientation& orient)
{
  if (orient == TopAbs_REVERSED)
  {
    double cf = first;
    first     = curve->ReversedParameter(last);
    last      = curve->ReversedParameter(cf);
    return curve->Reversed();
  }
  return HCurve::DownCast(curve->Copy());
}

template <class HCurve>
static inline void SegmentCurve(HCurve& curve, const double first, const double last)
{
  if (curve->FirstParameter() < first - Precision::PConfusion()
      || curve->LastParameter() > last + Precision::PConfusion())
  {
    if (curve->IsPeriodic())
      curve->Segment(first, last);
    else
      curve->Segment(std::max(curve->FirstParameter(), first),
                     std::min(curve->LastParameter(), last));
  }
}

template <class HPoint>
static inline void GetReversedParameters(const HPoint& p11,
                                         const HPoint& p12,
                                         const HPoint& p21,
                                         const HPoint& p22,
                                         bool&         isRev1,
                                         bool&         isRev2)
{
  isRev1 = false;
  isRev2 = false;
  // gka protection against crossing seem on second face

  double d11 = p11.Distance(p21);
  double d21 = p12.Distance(p21);

  double d12   = p11.Distance(p22);
  double d22   = p22.Distance(p12);
  double Dmin1 = std::min(d11, d21);
  double Dmin2 = std::min(d12, d22);
  if (fabs(Dmin1 - Dmin2) <= Precision::Confusion() || Dmin2 > Dmin1)
  {
    isRev1 = (d11 < d21);
  }
  else if (Dmin2 < Dmin1)
  {
    isRev1 = (d12 < d22);
    isRev2 = true;
  }
}

//=================================================================================================

bool ShapeConstruct::JoinCurves(const occ::handle<Geom_Curve>& ac3d1,
                                const occ::handle<Geom_Curve>& ac3d2,
                                const TopAbs_Orientation       Orient1,
                                const TopAbs_Orientation       Orient2,
                                double&                        first1,
                                double&                        last1,
                                double&                        first2,
                                double&                        last2,
                                occ::handle<Geom_Curve>&       c3dOut,
                                bool&                          isRev1,
                                bool&                          isRev2)

{
  occ::handle<Geom_Curve> c3d1, c3d2;

  c3d1 = GetCurveCopy(ac3d1, first1, last1, Orient1);
  c3d2 = GetCurveCopy(ac3d2, first2, last2, Orient2);
  ShapeConstruct_Curve           scc;
  bool                           After = true;
  occ::handle<Geom_BSplineCurve> bsplc1 =
    scc.ConvertToBSpline(c3d1, first1, last1, Precision::Confusion());
  occ::handle<Geom_BSplineCurve> bsplc2 =
    scc.ConvertToBSpline(c3d2, first2, last2, Precision::Confusion());
  //  newf = first1;
  //  newl = last1 + last2 - first2;

  if (bsplc1.IsNull() || bsplc2.IsNull())
    return false;

  SegmentCurve(bsplc1, first1, last1);
  SegmentCurve(bsplc2, first2, last2);

  // regression on file 866026_M-f276-f311.brep bug OCC482
  gp_Pnt pp11 = bsplc1->Pole(1);
  gp_Pnt pp12 = bsplc1->Pole(bsplc1->NbPoles());

  gp_Pnt pp21 = bsplc2->Pole(1);
  gp_Pnt pp22 = bsplc2->Pole(bsplc2->NbPoles());

  GetReversedParameters(pp11, pp12, pp21, pp22, isRev1, isRev2);

  if (isRev1)
  {
    bsplc1->Reverse();
  }
  if (isRev2)
    bsplc2->Reverse();

  gp_Pnt pmid = 0.5 * (bsplc1->Pole(bsplc1->NbPoles()).XYZ() + bsplc2->Pole(1).XYZ());
  bsplc1->SetPole(bsplc1->NbPoles(), pmid);
  bsplc2->SetPole(1, pmid);
  GeomConvert_CompCurveToBSplineCurve connect3d(bsplc1);
  if (!connect3d.Add(bsplc2, Precision::Confusion(), After, false))
    return false;
  c3dOut = connect3d.BSplineCurve();
  return true;
}

//=================================================================================================

bool ShapeConstruct::JoinCurves(const occ::handle<Geom2d_Curve>& aC2d1,
                                const occ::handle<Geom2d_Curve>& aC2d2,
                                const TopAbs_Orientation         Orient1,
                                const TopAbs_Orientation         Orient2,
                                double&                          first1,
                                double&                          last1,
                                double&                          first2,
                                double&                          last2,
                                occ::handle<Geom2d_Curve>&       C2dOut,
                                bool&                            isRev1,
                                bool&                            isRev2,
                                const bool                       isError)
{
  occ::handle<Geom2d_Curve> c2d1, c2d2;
  c2d1 = GetCurveCopy(aC2d1, first1, last1, Orient1);
  c2d2 = GetCurveCopy(aC2d2, first2, last2, Orient2);
  ShapeConstruct_Curve scc;
  bool                 After = true;

  occ::handle<Geom2d_BSplineCurve> bsplc12d =
    scc.ConvertToBSpline(c2d1, first1, last1, Precision::Confusion());
  occ::handle<Geom2d_BSplineCurve> bsplc22d =
    scc.ConvertToBSpline(c2d2, first2, last2, Precision::Confusion());

  if (bsplc12d.IsNull() || bsplc22d.IsNull())
    return false;

  SegmentCurve(bsplc12d, first1, last1);
  SegmentCurve(bsplc22d, first2, last2);
  // gka protection against crossing seem on second face
  gp_Pnt2d pp112d = bsplc12d->Pole(1).XY();
  gp_Pnt2d pp122d = bsplc12d->Pole(bsplc12d->NbPoles()).XY();

  gp_Pnt2d pp212d = bsplc22d->Pole(1).XY();
  gp_Pnt2d pp222d = bsplc22d->Pole(bsplc22d->NbPoles()).XY();

  GetReversedParameters(pp112d, pp122d, pp212d, pp222d, isRev1, isRev2);

  // regression on file 866026_M-f276-f311.brep bug OCC482
  // if(isRev1 || isRev2)
  //   return newedge1;
  if (isRev1)
  {
    bsplc12d->Reverse();
  }
  if (isRev2)
    bsplc22d->Reverse();

  //---------------------------------------------------------
  // protection against invalid topology Housing(sam1296.brep(face 707) - bugmergeedges4.brep)
  if (isError)
  {
    gp_Pnt2d pp1 = bsplc12d->Value(bsplc12d->FirstParameter());
    gp_Pnt2d pp2 = bsplc12d->Value(bsplc12d->LastParameter());
    gp_Pnt2d pp3 = bsplc12d->Value((bsplc12d->FirstParameter() + bsplc12d->LastParameter()) * 0.5);

    double leng     = pp1.Distance(pp2);
    bool   isCircle = (leng < pp1.Distance(pp3) + Precision::PConfusion());
    if ((pp1.Distance(bsplc22d->Pole(1)) < leng) && !isCircle)
      return false;
  }
  //-------------------------------------------------------
  gp_Pnt2d pmid1 = 0.5 * (bsplc12d->Pole(bsplc12d->NbPoles()).XY() + bsplc22d->Pole(1).XY());
  bsplc12d->SetPole(bsplc12d->NbPoles(), pmid1);
  bsplc22d->SetPole(1, pmid1);

  // abv 01 Sep 99: Geom2dConvert ALWAYS performs reparametrisation of the
  // second curve before merging; this is quite not suitable
  // Use 3d tool instead
  //      Geom2dConvert_CompCurveToBSplineCurve connect2d(bsplc12d);
  gp_Pnt                         vPnt(0, 0, 0);
  gp_Vec                         vDir(0, 0, 1);
  gp_Pln                         vPln(vPnt, vDir);
  occ::handle<Geom_BSplineCurve> bspl1 =
    occ::down_cast<Geom_BSplineCurve>(GeomAPI::To3d(bsplc12d, vPln));
  occ::handle<Geom_BSplineCurve> bspl2 =
    occ::down_cast<Geom_BSplineCurve>(GeomAPI::To3d(bsplc22d, vPln));
  GeomConvert_CompCurveToBSplineCurve connect2d(bspl1);
  if (!connect2d.Add(bspl2, Precision::PConfusion(), After, false))
    return false;
  C2dOut = GeomAPI::To2d(connect2d.BSplineCurve(), vPln);

  return true;
}
