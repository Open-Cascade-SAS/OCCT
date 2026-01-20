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
#include <BRep_GCurve.hxx>
#include <BRep_PointRepresentation.hxx>
#include <NCollection_List.hxx>
#include <BRep_PointOnCurve.hxx>
#include <BRep_PointOnCurveOnSurface.hxx>
#include <BRep_PointOnSurface.hxx>
#include <BRep_PointRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeAnalysis_TransferParametersProj.hxx>
#include <ShapeBuild_Edge.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeAnalysis_TransferParametersProj, ShapeAnalysis_TransferParameters)

//=================================================================================================

ShapeAnalysis_TransferParametersProj::ShapeAnalysis_TransferParametersProj()
    : myPrecision(0.0)
{
  myMaxTolerance = 1; // Precision::Infinite(); ?? pdn
  myForceProj    = false;
  myInitOK       = false;
}

//=================================================================================================

ShapeAnalysis_TransferParametersProj::ShapeAnalysis_TransferParametersProj(const TopoDS_Edge& E,
                                                                           const TopoDS_Face& F)
{
  myMaxTolerance = 1; // Precision::Infinite(); ?? pdn
  myForceProj    = false;
  Init(E, F);
}

//=================================================================================================

void ShapeAnalysis_TransferParametersProj::Init(const TopoDS_Edge& E, const TopoDS_Face& F)
{
  myInitOK = false;
  ShapeAnalysis_TransferParameters::Init(E, F);
  myEdge      = E;
  myPrecision = BRep_Tool::Tolerance(E); // it is better - skl OCC2851
  // myPrecision = Precision::Confusion();

  myCurve = BRep_Tool::Curve(E, myFirst, myLast);
  if (myCurve.IsNull())
  {
    myFirst = 0.;
    myLast  = 1.;
    return;
  }

  if (F.IsNull())
    return;

  double      f2d, l2d;
  ShapeAnalysis_Edge sae;
  if (sae.PCurve(E, F, myCurve2d, f2d, l2d, false))
  {

    occ::handle<Geom2dAdaptor_Curve> AC2d     = new Geom2dAdaptor_Curve(myCurve2d, f2d, l2d);
    occ::handle<Geom_Surface>        aSurface = BRep_Tool::Surface(F, myLocation);
    occ::handle<GeomAdaptor_Surface> AdS      = new GeomAdaptor_Surface(aSurface);

    Adaptor3d_CurveOnSurface Ad1(AC2d, AdS);
    myAC3d   = Ad1; // new Adaptor3d_CurveOnSurface(AC2d,AdS);
    myInitOK = true;
  }
}

//=================================================================================================

occ::handle<NCollection_HSequence<double>> ShapeAnalysis_TransferParametersProj::Perform(
  const occ::handle<NCollection_HSequence<double>>& Knots,
  const bool                 To2d)
{
  // pdn
  if (!myInitOK
      || (!myForceProj && myPrecision < myMaxTolerance && BRep_Tool::SameParameter(myEdge)))
    return ShapeAnalysis_TransferParameters::Perform(Knots, To2d);

  occ::handle<NCollection_HSequence<double>> resKnots = new NCollection_HSequence<double>;

  int        len   = Knots->Length();
  constexpr double preci = 2 * Precision::PConfusion();

  double first   = (To2d ? myAC3d.FirstParameter() : myFirst);
  double last    = (To2d ? myAC3d.LastParameter() : myLast);
  double maxPar  = first;
  double lastPar = last;
  double prevPar = maxPar;

  int j; // svv Jan 10 2000 : porting on DEC
  for (j = 1; j <= len; j++)
  {
    double par = PreformSegment(Knots->Value(j), To2d, prevPar, lastPar);
    prevPar           = par;
    if (prevPar > lastPar)
      prevPar -= preci;
    resKnots->Append(par);
    if (par > maxPar)
      maxPar = par;
  }

  // pdn correcting on periodic
  if (myCurve->IsClosed())
  {
    for (j = len; j >= 1; j--)
      if (resKnots->Value(j) < maxPar)
        resKnots->SetValue(j,
                           (To2d ? myAC3d.LastParameter() : myCurve->LastParameter())
                             - (len - j) * preci);
      else
        break;
  }
  // pdn correction on range
  for (j = 1; j <= len; j++)
  {
    if (resKnots->Value(j) < first)
      resKnots->SetValue(j, first);
    if (resKnots->Value(j) > last)
      resKnots->SetValue(j, last);
  }

  return resKnots;
}

//=================================================================================================

double ShapeAnalysis_TransferParametersProj::PreformSegment(const double    Param,
                                                                   const bool To2d,
                                                                   const double    First,
                                                                   const double    Last)
{
  double linPar = ShapeAnalysis_TransferParameters::Perform(Param, To2d);
  if (!myInitOK
      || (!myForceProj && myPrecision < myMaxTolerance && BRep_Tool::SameParameter(myEdge)))
    return linPar;

  double linDev, projDev;

  ShapeAnalysis_Curve sac;
  gp_Pnt              pproj;
  double       ppar;
  if (To2d)
  {
    gp_Pnt                      p1   = myCurve->Value(Param).Transformed(myLocation.Inverted());
    occ::handle<Adaptor3d_Surface>   AdS  = myAC3d.GetSurface();
    occ::handle<Geom2dAdaptor_Curve> AC2d = new Geom2dAdaptor_Curve(myCurve2d, First, Last);
    Adaptor3d_CurveOnSurface    Ad1(AC2d, AdS);
    projDev = sac.Project(Ad1, p1, myPrecision, pproj, ppar); // pdn
    linDev  = p1.Distance(Ad1.Value(linPar));
  }
  else
  {
    gp_Pnt p1 = myAC3d.Value(Param).Transformed(myLocation);
    projDev   = sac.Project(myCurve, p1, myPrecision, pproj, ppar, First, Last, false);
    linDev    = p1.Distance(myCurve->Value(linPar));
  }

  if (linDev <= projDev || (linDev < myPrecision && linDev <= 2 * projDev))
    ppar = linPar;
  return ppar;
}

//=================================================================================================

double ShapeAnalysis_TransferParametersProj::Perform(const double    Knot,
                                                            const bool To2d)
{
  if (!myInitOK
      || (!myForceProj && myPrecision < myMaxTolerance && BRep_Tool::SameParameter(myEdge)))
    return ShapeAnalysis_TransferParameters::Perform(Knot, To2d);

  double res;
  if (To2d)
    res = PreformSegment(Knot, To2d, myAC3d.FirstParameter(), myAC3d.LastParameter());
  else
    res = PreformSegment(Knot, To2d, myFirst, myLast);

  // pdn correction on range
  double first = (To2d ? myAC3d.FirstParameter() : myFirst);
  double last  = (To2d ? myAC3d.LastParameter() : myLast);
  if (res < first)
    res = first;
  if (res > last)
    res = last;
  return res;
}

//=================================================================================================

static double CorrectParameter(const occ::handle<Geom2d_Curve>& crv, const double param)
{
  if (crv->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    occ::handle<Geom2d_TrimmedCurve> tmp = occ::down_cast<Geom2d_TrimmedCurve>(crv);
    return CorrectParameter(tmp->BasisCurve(), param);
  }
  else if (crv->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
  {
    occ::handle<Geom2d_OffsetCurve> tmp = occ::down_cast<Geom2d_OffsetCurve>(crv);
    return CorrectParameter(tmp->BasisCurve(), param);
  }
  else if (crv->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    occ::handle<Geom2d_BSplineCurve> bspline = occ::down_cast<Geom2d_BSplineCurve>(crv);
    for (int j = bspline->FirstUKnotIndex(); j <= bspline->LastUKnotIndex(); j++)
    {
      double valknot = bspline->Knot(j);
      if (std::abs(valknot - param) < Precision::PConfusion())
        return valknot;
    }
  }
  return param;
}

//=================================================================================================

void ShapeAnalysis_TransferParametersProj::TransferRange(TopoDS_Edge&           newEdge,
                                                         const double    prevPar,
                                                         const double    currPar,
                                                         const bool Is2d)
{
  if (!myInitOK
      || (!myForceProj && myPrecision < myMaxTolerance && BRep_Tool::SameParameter(myEdge)))
  {
    ShapeAnalysis_TransferParameters::TransferRange(newEdge, prevPar, currPar, Is2d);
    return;
  }

  BRep_Builder     B;
  bool samerange = true;
  ShapeBuild_Edge  sbe;
  sbe.CopyRanges(newEdge, myEdge);
  gp_Pnt                  p1;
  gp_Pnt                  p2;
  double           alpha = 0, beta = 1;
  constexpr double preci = Precision::PConfusion();
  double           firstPar, lastPar;
  if (prevPar < currPar)
  {
    firstPar = prevPar;
    lastPar  = currPar;
  }
  else
  {
    firstPar = currPar;
    lastPar  = prevPar;
  }
  if (Is2d)
  {
    p1 = myAC3d.Value(firstPar).Transformed(myLocation);
    if (Precision::IsInfinite(p1.X()) || Precision::IsInfinite(p1.Y())
        || Precision::IsInfinite(p1.Z()))
    {
      B.SameRange(newEdge, false);
      return;
    }
    p2 = myAC3d.Value(lastPar).Transformed(myLocation);
    if (Precision::IsInfinite(p2.X()) || Precision::IsInfinite(p2.Y())
        || Precision::IsInfinite(p2.Z()))
    {
      B.SameRange(newEdge, false);
      return;
    }
    double fact = myAC3d.LastParameter() - myAC3d.FirstParameter();
    if (fact > Epsilon(myAC3d.LastParameter()))
    {
      alpha = (firstPar - myAC3d.FirstParameter()) / fact;
      beta  = (lastPar - myAC3d.FirstParameter()) / fact;
    }
  }
  else
  {
    p1 = myCurve->Value(firstPar);
    if (Precision::IsInfinite(p1.X()) || Precision::IsInfinite(p1.Y())
        || Precision::IsInfinite(p1.Z()))
    {
      B.SameRange(newEdge, false);
      return;
    }
    p2 = myCurve->Value(lastPar);
    if (Precision::IsInfinite(p2.X()) || Precision::IsInfinite(p2.Y())
        || Precision::IsInfinite(p2.Z()))
    {
      B.SameRange(newEdge, false);
      return;
    }
    double fact = myLast - myFirst;
    if (fact > Epsilon(myLast))
    {
      alpha = (firstPar - myFirst) / fact;
      beta  = (lastPar - myFirst) / fact;
    }
  }
  const bool          useLinearFirst = (alpha < preci);
  const bool          useLinearLast  = (1 - beta < preci);
  TopLoc_Location                 EdgeLoc        = myEdge.Location();
  ShapeAnalysis_Curve             sac;
  gp_Pnt                          pproj;
  double                   ppar1, ppar2;
  NCollection_List<occ::handle<BRep_CurveRepresentation>>& tolist =
    (*((occ::handle<BRep_TEdge>*)&newEdge.TShape()))->ChangeCurves();
  occ::handle<BRep_GCurve> toGC;
  for (NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator toitcr(tolist); toitcr.More(); toitcr.Next())
  {
    toGC = occ::down_cast<BRep_GCurve>(toitcr.Value());
    if (toGC.IsNull())
      continue;
    TopLoc_Location loc = (EdgeLoc * toGC->Location()).Inverted();
    if (toGC->IsCurve3D())
    {
      if (!Is2d)
      {
        ppar1 = firstPar;
        ppar2 = lastPar;
      }
      else
      {
        occ::handle<Geom_Curve> C3d = toGC->Curve3D();
        if (C3d.IsNull())
          continue;
        double     first = toGC->First();
        double     last  = toGC->Last();
        double     len   = last - first;
        gp_Pnt            ploc1 = p1.Transformed(loc);
        gp_Pnt            ploc2 = p2.Transformed(loc);
        GeomAdaptor_Curve GAC(C3d, first, last);
        // CATIA bplseitli.model FAC1155 - Copy: protection for degenerated edges(3d case for
        // symmetry)
        double    linFirst = first + alpha * len;
        double    linLast  = first + beta * len;
        double    dist1 = sac.NextProject(linFirst, GAC, ploc1, myPrecision, pproj, ppar1);
        double    dist2 = sac.NextProject(linLast, GAC, ploc2, myPrecision, pproj, ppar2);
        bool useLinear = std::abs(ppar1 - ppar2) < preci;

        gp_Pnt        pos1 = C3d->Value(linFirst);
        gp_Pnt        pos2 = C3d->Value(linLast);
        double d01  = pos1.Distance(ploc1);
        double d02  = pos2.Distance(ploc2);
        if (useLinearFirst || useLinear || d01 <= dist1 || (d01 < myPrecision && d01 <= 2 * dist1))
          ppar1 = linFirst;
        if (useLinearLast || useLinear || d02 <= dist2 || (d02 < myPrecision && d02 <= 2 * dist2))
          ppar2 = linLast;
      }
      if (ppar1 > ppar2)
      {
        double tmpP = ppar2;
        ppar2              = ppar1;
        ppar1              = tmpP;
      }
      if (ppar2 - ppar1 < preci)
      {
        if (ppar1 - toGC->First() < preci)
          ppar2 += 2 * preci;
        else if (toGC->Last() - ppar2 < preci)
          ppar1 -= 2 * preci;
        else
        {
          ppar1 -= preci;
          ppar2 += preci;
        }
      }
      toGC->SetRange(ppar1, ppar2);
      // if(fabs(ppar1- firstPar) > Precision::PConfusion() ||
      //     fabs(ppar2 - lastPar) >Precision::PConfusion()) // by LSS
      if (ppar1 != firstPar || ppar2 != lastPar)
        samerange = false;
    }
    else if (toGC->IsCurveOnSurface())
    { // continue;  ||

      bool            localLinearFirst = useLinearFirst;
      bool            localLinearLast  = useLinearLast;
      occ::handle<Geom2d_Curve>        C2d              = toGC->PCurve();
      double               first            = toGC->First();
      double               last             = toGC->Last();
      double               len              = last - first;
      occ::handle<Geom2dAdaptor_Curve> AC2d = new Geom2dAdaptor_Curve(toGC->PCurve(), first, last);
      occ::handle<GeomAdaptor_Surface> AdS  = new GeomAdaptor_Surface(toGC->Surface());
      Adaptor3d_CurveOnSurface    Ad1(AC2d, AdS);
      ShapeAnalysis_Curve         sac1;

      // gp_Pnt p1 = Ad1.Value(prevPar);
      // gp_Pnt p2 = Ad1.Value(currPar);
      gp_Pnt ploc1 = p1.Transformed(loc);
      gp_Pnt ploc2 = p2.Transformed(loc);
      // CATIA bplseitli.model FAC1155 - Copy: protection for degenerated edges
      double linFirst = first + alpha * len;
      double linLast  = first + beta * len;
      double dist1    = sac1.NextProject(linFirst, Ad1, ploc1, myPrecision, pproj, ppar1);
      double dist2    = sac1.NextProject(linLast, Ad1, ploc2, myPrecision, pproj, ppar2);

      bool isFirstOnEnd = (ppar1 - first) / len < Precision::PConfusion();
      bool isLastOnEnd  = (last - ppar2) / len < Precision::PConfusion();
      bool useLinear    = std::abs(ppar1 - ppar2) < Precision::PConfusion();
      if (isFirstOnEnd && !localLinearFirst)
        localLinearFirst = true;
      if (isLastOnEnd && !localLinearLast)
        localLinearLast = true;

      gp_Pnt        pos1 = Ad1.Value(linFirst);
      gp_Pnt        pos2 = Ad1.Value(linLast);
      double d01  = pos1.Distance(ploc1);
      double d02  = pos2.Distance(ploc2);
      if (localLinearFirst || useLinear || d01 <= dist1 || (d01 < myPrecision && d01 <= 2 * dist1))
        ppar1 = linFirst;
      if (localLinearLast || useLinear || d02 <= dist2 || (d02 < myPrecision && d02 <= 2 * dist2))
        ppar2 = linLast;

      if (ppar1 > ppar2)
      {
        double tmpP = ppar2;
        ppar2              = ppar1;
        ppar1              = tmpP;
      }
      ppar1 = CorrectParameter(C2d, ppar1);
      ppar2 = CorrectParameter(C2d, ppar2);
      if (ppar2 - ppar1 < preci)
      {
        if (ppar1 - toGC->First() < preci)
          ppar2 += 2 * preci;
        else if (toGC->Last() - ppar2 < preci)
          ppar1 -= 2 * preci;
        else
        {
          ppar1 -= preci;
          ppar2 += preci;
        }
      }
      toGC->SetRange(ppar1, ppar2);
      // if(fabs(ppar1 - firstPar) > Precision::PConfusion() ||
      //    fabs(ppar2 -lastPar) > Precision::PConfusion())// by LSS
      if (ppar1 != firstPar || ppar2 != lastPar)
        samerange = false;
    }
  }
  B.SameRange(newEdge, samerange);
}

//=================================================================================================

bool ShapeAnalysis_TransferParametersProj::IsSameRange() const
{

  if (!myInitOK
      || (!myForceProj && myPrecision < myMaxTolerance && BRep_Tool::SameParameter(myEdge)))
    return ShapeAnalysis_TransferParameters::IsSameRange();
  else
    return false;
}

//=================================================================================================

bool& ShapeAnalysis_TransferParametersProj::ForceProjection()
{
  return myForceProj;
}

//=================================================================================================

TopoDS_Vertex ShapeAnalysis_TransferParametersProj::CopyNMVertex(const TopoDS_Vertex& theV,
                                                                 const TopoDS_Edge&   toedge,
                                                                 const TopoDS_Edge&   fromedge)
{
  TopoDS_Vertex anewV;
  if (theV.Orientation() != TopAbs_INTERNAL && theV.Orientation() != TopAbs_EXTERNAL)
    return anewV;

  TopLoc_Location           fromLoc;
  double             f1, l1;
  const occ::handle<Geom_Curve>& C1 = BRep_Tool::Curve(fromedge, fromLoc, f1, l1);
  fromLoc                      = fromLoc.Predivided(theV.Location());

  double      f2, l2;
  occ::handle<Geom_Curve> C2 = BRep_Tool::Curve(toedge, f2, l2);

  anewV      = TopoDS::Vertex(theV.EmptyCopied());
  gp_Pnt apv = BRep_Tool::Pnt(anewV);

  NCollection_List<occ::handle<BRep_PointRepresentation>>& alistrep =
    (*((occ::handle<BRep_TVertex>*)&anewV.TShape()))->ChangePoints();

  NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itpr(
    (*((occ::handle<BRep_TVertex>*)&theV.TShape()))->Points());

  double    aOldPar = RealLast();
  bool hasRepr = false;
  for (; itpr.More(); itpr.Next())
  {
    const occ::handle<BRep_PointRepresentation>& pr = itpr.Value();
    if (pr.IsNull())
      continue;
    if (pr->IsPointOnCurve(C1, fromLoc))
    {
      aOldPar = pr->Parameter();
      hasRepr = true;
      continue;
    }
    else if (pr->IsPointOnSurface())
    {
      occ::handle<BRep_PointOnSurface> aOld = occ::down_cast<BRep_PointOnSurface>(pr);
      occ::handle<BRep_PointOnSurface> aPS  = new BRep_PointOnSurface(aOld->Parameter(),
                                                                aOld->Parameter2(),
                                                                aOld->Surface(),
                                                                aOld->Location());
      alistrep.Append(aPS);
      continue;
    }
    else if (pr->IsPointOnCurveOnSurface())
    {
      bool                             found = false;
      NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator fromitcr(
        (*((occ::handle<BRep_TEdge>*)&fromedge.TShape()))->ChangeCurves());

      for (; fromitcr.More() && !found; fromitcr.Next())
      {
        occ::handle<BRep_GCurve> fromGC = occ::down_cast<BRep_GCurve>(fromitcr.Value());
        if (fromGC.IsNull() || !fromGC->IsCurveOnSurface())
          continue;

        TopLoc_Location      aL       = fromGC->Location().Predivided(theV.Location());
        occ::handle<Geom_Surface> surface1 = fromGC->Surface();
        occ::handle<Geom2d_Curve> ac2d1    = fromGC->PCurve();
        if (pr->IsPointOnCurveOnSurface(ac2d1, surface1, aL))
        {
          found = true;
          if (!hasRepr)
          {
            aOldPar = pr->Parameter();
          }
        }
      }
      if (found)
        continue;
    }
    if (pr->IsPointOnCurve())
    {
      occ::handle<BRep_PointOnCurve> aPRep =
        new BRep_PointOnCurve(pr->Parameter(), pr->Curve(), pr->Location());
      alistrep.Append(aPRep);
    }
    else if (pr->IsPointOnCurveOnSurface())
    {
      occ::handle<BRep_PointOnCurveOnSurface> aPonCS = new BRep_PointOnCurveOnSurface(pr->Parameter(),
                                                                                 pr->PCurve(),
                                                                                 pr->Surface(),
                                                                                 pr->Location());
      alistrep.Append(aPonCS);
    }
  }
  double apar = aOldPar;
  double aTol = BRep_Tool::Tolerance(theV);
  if (!hasRepr
      || (fabs(f1 - f2) > Precision::PConfusion() || fabs(l1 - l2) > Precision::PConfusion()))
  {
    gp_Pnt              projP;
    ShapeAnalysis_Curve sae;
    double       adist = sae.Project(C2, apv, Precision::Confusion(), projP, apar);
    if (aTol < adist)
      aTol = adist;
  }
  BRep_Builder aB;
  aB.UpdateVertex(anewV, apar, toedge, aTol);

  // update tolerance
  bool       needUpdate = false;
  gp_Pnt                 aPV        = (*((occ::handle<BRep_TVertex>*)&anewV.TShape()))->Pnt();
  const TopLoc_Location& toLoc      = toedge.Location();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator toitcr(
    (*((occ::handle<BRep_TEdge>*)&toedge.TShape()))->ChangeCurves());

  for (; toitcr.More(); toitcr.Next())
  {
    occ::handle<BRep_GCurve> toGC = occ::down_cast<BRep_GCurve>(toitcr.Value());
    if (toGC.IsNull() || !toGC->IsCurveOnSurface())
      continue;

    TopLoc_Location aL = (toLoc * toGC->Location()).Predivided(theV.Location());
    // aL.Predivided(theV.Location());
    occ::handle<Geom_Surface> surface1 = toGC->Surface();
    occ::handle<Geom2d_Curve> ac2d1    = toGC->PCurve();
    gp_Pnt2d             aP2d     = ac2d1->Value(apar);
    gp_Pnt               aP3d     = surface1->Value(aP2d.X(), aP2d.Y());
    aP3d.Transform(aL.Transformation());
    double adist = aPV.Distance(aP3d);
    if (adist > aTol)
    {
      aTol       = adist;
      needUpdate = true;
    }
  }
  if (needUpdate)
    aB.UpdateVertex(anewV, aTol);
  return anewV;
}

//=================================================================================================

TopoDS_Vertex ShapeAnalysis_TransferParametersProj::CopyNMVertex(const TopoDS_Vertex& theV,
                                                                 const TopoDS_Face&   toFace,
                                                                 const TopoDS_Face&   fromFace)
{
  TopoDS_Vertex anewV;
  if (theV.Orientation() != TopAbs_INTERNAL && theV.Orientation() != TopAbs_EXTERNAL)
    return anewV;

  TopLoc_Location      fromLoc;
  TopLoc_Location      toLoc;
  occ::handle<Geom_Surface> fromSurf = BRep_Tool::Surface(fromFace, fromLoc);
  occ::handle<Geom_Surface> toSurf   = BRep_Tool::Surface(toFace, toLoc);
  fromLoc                       = fromLoc.Predivided(theV.Location());

  anewV      = TopoDS::Vertex(theV.EmptyCopied());
  gp_Pnt apv = BRep_Tool::Pnt(anewV);

  NCollection_List<occ::handle<BRep_PointRepresentation>>& alistrep =
    (*((occ::handle<BRep_TVertex>*)&anewV.TShape()))->ChangePoints();

  NCollection_List<occ::handle<BRep_PointRepresentation>>::Iterator itpr(
    (*((occ::handle<BRep_TVertex>*)&theV.TShape()))->Points());

  bool hasRepr = false;
  double    apar1 = 0., apar2 = 0.;
  for (; itpr.More(); itpr.Next())
  {
    const occ::handle<BRep_PointRepresentation>& pr = itpr.Value();
    if (pr.IsNull())
      continue;
    TopLoc_Location aLoc = pr->Location();
    if (pr->IsPointOnCurveOnSurface())
    {
      occ::handle<BRep_PointOnCurveOnSurface> aPonCS =
        new BRep_PointOnCurveOnSurface(pr->Parameter(), pr->PCurve(), pr->Surface(), aLoc);
      alistrep.Append(aPonCS);
    }
    else if (pr->IsPointOnCurve())
    {
      occ::handle<BRep_PointOnCurve> aPRep = new BRep_PointOnCurve(pr->Parameter(), pr->Curve(), aLoc);
      alistrep.Append(aPRep);
    }
    else if (pr->IsPointOnSurface())
    {
      occ::handle<BRep_PointOnSurface> aOld = occ::down_cast<BRep_PointOnSurface>(pr);

      if (pr->IsPointOnSurface(fromSurf, fromLoc))
      {
        apar1   = aOld->Parameter();
        apar2   = aOld->Parameter2();
        hasRepr = true;
      }
      else
      {
        occ::handle<BRep_PointOnSurface> aPS = new BRep_PointOnSurface(aOld->Parameter(),
                                                                  aOld->Parameter2(),
                                                                  aOld->Surface(),
                                                                  aOld->Location());
        alistrep.Append(aPS);
      }
    }
  }
  double aTol = BRep_Tool::Tolerance(anewV);
  if (!hasRepr || (fromSurf != toSurf || fromLoc != toLoc))
  {
    occ::handle<Geom_Surface>          aS        = BRep_Tool::Surface(toFace);
    occ::handle<ShapeAnalysis_Surface> aSurfTool = new ShapeAnalysis_Surface(aS);
    gp_Pnt2d                      aP2d      = aSurfTool->ValueOfUV(apv, Precision::Confusion());
    apar1                                   = aP2d.X();
    apar2                                   = aP2d.Y();

    if (aTol < aSurfTool->Gap())
      aTol = aSurfTool->Gap() + 0.1 * Precision::Confusion();
    // occ::handle<BRep_PointOnSurface> aPS = new BRep_PointOnSurface(aP2d.X(),aP2d.Y(),toSurf,toLoc);
    // alistrep.Append(aPS);
  }

  BRep_Builder aB;
  aB.UpdateVertex(anewV, apar1, apar2, toFace, aTol);
  return anewV;
}
