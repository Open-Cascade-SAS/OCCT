// Created on: 1996-07-12
// Created by: Stagiaire Mary FABIEN
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BRepTools_NurbsConvertModification.hxx>

#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRepTools.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Extrema_LocateExtPC2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <ProjLib_ComputeApprox.hxx>
#include <ProjLib_ComputeApproxOnPolarSurface.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Builder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepTools_NurbsConvertModification, BRepTools_CopyModification)

//
namespace
{
static void GeomLib_ChangeUBounds(occ::handle<Geom_BSplineSurface>& aSurface,
                                  const double                      newU1,
                                  const double                      newU2)
{
  NCollection_Array1<double> knots(aSurface->UKnots());
  BSplCLib::Reparametrize(newU1, newU2, knots);
  aSurface->SetUKnots(knots);
}

static void GeomLib_ChangeVBounds(occ::handle<Geom_BSplineSurface>& aSurface,
                                  const double                      newV1,
                                  const double                      newV2)
{
  NCollection_Array1<double> knots(aSurface->VKnots());
  BSplCLib::Reparametrize(newV1, newV2, knots);
  aSurface->SetVKnots(knots);
}

// find 3D curve from theEdge in theMap, and return the transformed curve or NULL
static occ::handle<Geom_Curve> newCurve(
  const NCollection_IndexedDataMap<occ::handle<Standard_Transient>,
                                   occ::handle<Standard_Transient>>& theMap,
  const TopoDS_Edge&                                                 theEdge,
  double&                                                            theFirst,
  double&                                                            theLast)
{
  occ::handle<Geom_Curve> aNewCurve;

  TopLoc_Location         aLoc;
  occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(theEdge, aLoc, theFirst, theLast);
  if (!aCurve.IsNull() && theMap.Contains(aCurve))
  {
    aNewCurve = occ::down_cast<Geom_Curve>(theMap.FindFromKey(aCurve));
    aNewCurve = occ::down_cast<Geom_Curve>(aNewCurve->Transformed(aLoc.Transformation()));
  }
  return aNewCurve;
}

// find 2D curve from theEdge on theFace in theMap, and return the transformed curve or NULL
static occ::handle<Geom2d_Curve> newCurve(
  const NCollection_IndexedDataMap<occ::handle<Standard_Transient>,
                                   occ::handle<Standard_Transient>>& theMap,
  const TopoDS_Edge&                                                 theEdge,
  const TopoDS_Face&                                                 theFace,
  double&                                                            theFirst,
  double&                                                            theLast)
{
  occ::handle<Geom2d_Curve> aC2d = BRep_Tool::CurveOnSurface(theEdge, theFace, theFirst, theLast);
  return (!aC2d.IsNull() && theMap.Contains(aC2d))
           ? occ::down_cast<Geom2d_Curve>(theMap.FindFromKey(aC2d))
           : occ::handle<Geom2d_Curve>();
}

// find surface from theFace in theMap, and return the transformed surface or NULL
static occ::handle<Geom_Surface> newSurface(
  const NCollection_IndexedDataMap<occ::handle<Standard_Transient>,
                                   occ::handle<Standard_Transient>>& theMap,
  const TopoDS_Face&                                                 theFace)
{
  occ::handle<Geom_Surface> aNewSurf;

  TopLoc_Location           aLoc;
  occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(theFace, aLoc);
  if (!aSurf.IsNull() && theMap.Contains(aSurf))
  {
    aNewSurf = occ::down_cast<Geom_Surface>(theMap.FindFromKey(aSurf));
    aNewSurf = occ::down_cast<Geom_Surface>(aNewSurf->Transformed(aLoc.Transformation()));
  }
  return aNewSurf;
}

static bool newParameter(const gp_Pnt&                  thePoint,
                         const occ::handle<Geom_Curve>& theCurve,
                         const double                   theFirst,
                         const double                   theLast,
                         const double                   theTol,
                         double&                        theParam)
{
  GeomAdaptor_Curve   anAdaptor(theCurve);
  Extrema_LocateExtPC proj(thePoint,
                           anAdaptor,
                           theParam,
                           theFirst,
                           theLast,
                           Precision::PConfusion());
  if (proj.IsDone())
  {
    double aDist2Min = proj.SquareDistance();
    if (aDist2Min < theTol * theTol)
    {
      theParam = proj.Point().Parameter();
      return true;
    }
  }
  return false;
}

static bool newParameter(const gp_Pnt2d&                  theUV,
                         const occ::handle<Geom2d_Curve>& theCurve2d,
                         const double                     theFirst,
                         const double                     theLast,
                         const double                     theTol,
                         double&                          theParam)
{
  Geom2dAdaptor_Curve   anAdaptor(theCurve2d);
  Extrema_LocateExtPC2d aProj(theUV, anAdaptor, theParam, Precision::PConfusion());
  if (aProj.IsDone())
  {
    double aDist2Min = aProj.SquareDistance();
    if (aDist2Min < theTol * theTol)
    {
      theParam = aProj.Point().Parameter();
      return true;
    }
  }
  else
  {
    // Try to use general extrema to find the parameter, because Extrema_LocateExtPC2d
    // sometimes could not find a solution if the parameter's first approach is several
    // spans away from the expected solution (test bugs/modalg_7/bug28722).
    Extrema_ExtPC2d anExt(theUV, anAdaptor, theFirst, theLast);
    if (anExt.IsDone())
    {
      int    aMinInd    = 0;
      double aMinSqDist = Precision::Infinite();
      for (int anIndex = 1; anIndex <= anExt.NbExt(); ++anIndex)
        if (anExt.SquareDistance(anIndex) < aMinSqDist)
        {
          aMinSqDist = anExt.SquareDistance(anIndex);
          aMinInd    = anIndex;
        }
      if (aMinSqDist < theTol * theTol)
      {
        theParam = anExt.Point(aMinInd).Parameter();
        return true;
      }
    }
  }
  return false;
}

static bool newUV(const gp_Pnt&                    thePoint,
                  const occ::handle<Geom_Surface>& theSurf,
                  const double                     theTol,
                  gp_Pnt2d&                        theUV)
{
  GeomAdaptor_Surface    anAdaptor(theSurf);
  Extrema_GenLocateExtPS aProj(anAdaptor);
  aProj.Perform(thePoint, theUV.X(), theUV.Y());
  if (aProj.IsDone())
  {
    double aDist2Min = aProj.SquareDistance();
    if (aDist2Min < theTol * theTol)
    {
      gp_XY& aUV = theUV.ChangeCoord();
      aProj.Point().Parameter(aUV.ChangeCoord(1), aUV.ChangeCoord(2));
      return true;
    }
  }
  return false;
}
} // namespace

//=================================================================================================

BRepTools_NurbsConvertModification::BRepTools_NurbsConvertModification() = default;

//=================================================================================================

bool BRepTools_NurbsConvertModification::NewSurface(const TopoDS_Face&         F,
                                                    occ::handle<Geom_Surface>& S,
                                                    TopLoc_Location&           L,
                                                    double&                    Tol,
                                                    bool&                      RevWires,
                                                    bool&                      RevFace)
{
  double U1, U2, curvU1, curvU2, surfU1, surfU2, UTol;
  double V1, V2, curvV1, curvV2, surfV1, surfV2, VTol;
  RevWires                     = false;
  RevFace                      = false;
  occ::handle<Geom_Surface> SS = BRep_Tool::Surface(F, L);
  if (SS.IsNull())
  {
    // processing the case when there is no geometry
    return false;
  }

  occ::handle<Standard_Type> TheTypeSS = SS->DynamicType();
  if ((TheTypeSS == STANDARD_TYPE(Geom_BSplineSurface))
      || (TheTypeSS == STANDARD_TYPE(Geom_BezierSurface)))
  {
    return false;
  }
  S = SS;
  BRepTools::UVBounds(F, curvU1, curvU2, curvV1, curvV2);
  Tol           = BRep_Tool::Tolerance(F);
  double TolPar = 0.1 * Tol;
  bool   IsUp = S->IsUPeriodic(), IsVp = S->IsVPeriodic();
  // OCC466(apo)->
  U1 = curvU1;
  U2 = curvU2;
  V1 = curvV1;
  V2 = curvV2;
  S->Bounds(surfU1, surfU2, surfV1, surfV2);

  if (std::abs(U1 - surfU1) <= TolPar)
    U1 = surfU1;
  if (std::abs(U2 - surfU2) <= TolPar)
    U2 = surfU2;
  if (std::abs(V1 - surfV1) <= TolPar)
    V1 = surfV1;
  if (std::abs(V2 - surfV2) <= TolPar)
    V2 = surfV2;

  if (!IsUp)
  {
    U1 = std::max(surfU1, curvU1);
    U2 = std::min(surfU2, curvU2);
  }
  if (!IsVp)
  {
    V1 = std::max(surfV1, curvV1);
    V2 = std::min(surfV2, curvV2);
  }
  //<-OCC466(apo)

  if (IsUp)
  {
    double Up = S->UPeriod();
    if (U2 - U1 > Up)
      U2 = U1 + Up;
  }
  if (IsVp)
  {
    double Vp = S->VPeriod();
    if (V2 - V1 > Vp)
      V2 = V1 + Vp;
  }

  if (std::abs(surfU1 - U1) > Tol || std::abs(surfU2 - U2) > Tol || std::abs(surfV1 - V1) > Tol
      || std::abs(surfV2 - V2) > Tol)
    S = new Geom_RectangularTrimmedSurface(S, U1, U2, V1, V2);
  S->Bounds(surfU1, surfU2, surfV1, surfV2);

  S                                   = GeomConvert::SurfaceToBSplineSurface(S);
  occ::handle<Geom_BSplineSurface> BS = occ::down_cast<Geom_BSplineSurface>(S);
  BS->Resolution(Tol, UTol, VTol);

  //
  // on recadre les bornes de S  sinon les anciennes PCurves sont aux fraises
  //

  if (std::abs(curvU1 - surfU1) > UTol && !BS->IsUPeriodic())
  {
    GeomLib_ChangeUBounds(BS, U1, U2);
  }
  if (std::abs(curvV1 - surfV1) > VTol && !BS->IsVPeriodic())
  {
    GeomLib_ChangeVBounds(BS, V1, V2);
  }

  if (!myMap.Contains(SS))
  {
    myMap.Add(SS, S);
  }
  return true;
}

static bool IsConvert(const TopoDS_Edge& E)
{
  bool                     isConvert = false;
  occ::handle<BRep_TEdge>& TE        = *((occ::handle<BRep_TEdge>*)&E.TShape());
  // iterate on pcurves
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
  for (; itcr.More() && !isConvert; itcr.Next())
  {
    occ::handle<BRep_GCurve> GC = occ::down_cast<BRep_GCurve>(itcr.Value());
    if (GC.IsNull() || !GC->IsCurveOnSurface())
      continue;
    occ::handle<Geom_Surface> aSurface = GC->Surface();
    occ::handle<Geom2d_Curve> aCurve2d = GC->PCurve();
    isConvert                          = ((!aSurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface))
                  && !aSurface->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
                 || (!aCurve2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))
                     && !aCurve2d->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))));
  }
  return isConvert;
}

//=================================================================================================

bool BRepTools_NurbsConvertModification::NewTriangulation(const TopoDS_Face&               theFace,
                                                          occ::handle<Poly_Triangulation>& theTri)
{
  if (!BRepTools_CopyModification::NewTriangulation(theFace, theTri))
  {
    return false;
  }

  // convert UV nodes of the mesh
  if (theTri->HasUVNodes())
  {
    TopLoc_Location           aLoc;
    occ::handle<Geom_Surface> aSurf    = BRep_Tool::Surface(theFace, aLoc);
    occ::handle<Geom_Surface> aNewSurf = newSurface(myMap, theFace);
    if (!aSurf.IsNull() && !aNewSurf.IsNull())
    {
      double aTol = BRep_Tool::Tolerance(theFace);
      for (int anInd = 1; anInd <= theTri->NbNodes(); ++anInd)
      {
        gp_Pnt2d aUV    = theTri->UVNode(anInd);
        gp_Pnt   aPoint = aSurf->Value(aUV.X(), aUV.Y());
        if (newUV(aPoint, aNewSurf, aTol, aUV))
          theTri->SetUVNode(anInd, aUV);
      }
    }
  }

  return true;
}

//=================================================================================================

bool BRepTools_NurbsConvertModification::NewCurve(const TopoDS_Edge&       E,
                                                  occ::handle<Geom_Curve>& C,
                                                  TopLoc_Location&         L,
                                                  double&                  Tol)
{

  Tol = BRep_Tool::Tolerance(E);
  if (BRep_Tool::Degenerated(E))
  {
    C.Nullify();
    L.Identity();
    return true;
  }
  double f, l;

  occ::handle<Geom_Curve> Caux = BRep_Tool::Curve(E, L, f, l);

  if (Caux.IsNull())
  {
    L.Identity();
    return false;
  }
  occ::handle<Standard_Type> TheType = Caux->DynamicType();
  if ((TheType == STANDARD_TYPE(Geom_BSplineCurve)) || (TheType == STANDARD_TYPE(Geom_BezierCurve)))
  {
    if (IsConvert(E))
    {
      C = occ::down_cast<Geom_Curve>(Caux->Copy());
      return true;
    }
    return false;
  }

  C = Caux;

  double TolPar = Tol * .1;

  if (C->IsPeriodic())
  {
    double p = C->Period();
    double d = std::abs(l - f);
    if (std::abs(d - p) <= TolPar && l <= p)
    {
    }
    else
      C = new Geom_TrimmedCurve(C, f, l);
  }
  else
    C = new Geom_TrimmedCurve(C, f, l);

  // modif WOK++ portage hp (fbi du 14/03/97)
  //   gp_Trsf trsf(L);
  //   gp_Trsf trsf = L.Transformation();

  //  C = GeomConvert::CurveToBSplineCurve(C,Convert_QuasiAngular);

  C = GeomConvert::CurveToBSplineCurve(C);

  double fnew = C->FirstParameter(), lnew = C->LastParameter(), UTol;

  occ::handle<Geom_BSplineCurve> BC = occ::down_cast<Geom_BSplineCurve>(C);

  if (!BC->IsPeriodic())
  {
    BC->Resolution(Tol, UTol);
    if (std::abs(f - fnew) > UTol || std::abs(l - lnew) > UTol)
    {
      NCollection_Array1<double> knots(BC->Knots());
      BSplCLib::Reparametrize(f, l, knots);
      BC->SetKnots(knots);
    }
  }

  if (!myMap.Contains(Caux))
  {
    myMap.Add(Caux, C);
  }
  return true;
}

//=================================================================================================

bool BRepTools_NurbsConvertModification::NewPolygon(const TopoDS_Edge&           theEdge,
                                                    occ::handle<Poly_Polygon3D>& thePoly)
{
  if (!BRepTools_CopyModification::NewPolygon(theEdge, thePoly))
  {
    return false;
  }

  if (!thePoly->HasParameters())
  {
    return false;
  }
  // update parameters of polygon
  double                  aTol = BRep_Tool::Tolerance(theEdge);
  double                  aFirst, aLast;
  occ::handle<Geom_Curve> aCurve    = BRep_Tool::Curve(theEdge, aFirst, aLast);
  occ::handle<Geom_Curve> aNewCurve = newCurve(myMap, theEdge, aFirst, aLast);
  if (aCurve.IsNull() || aNewCurve.IsNull()) // skip processing degenerated edges
  {
    return false;
  }
  NCollection_Array1<double>& aParams = thePoly->ChangeParameters();
  for (int anInd = aParams.Lower(); anInd <= aParams.Upper(); ++anInd)
  {
    double& aParam = aParams(anInd);
    gp_Pnt  aPoint = aCurve->Value(aParam);
    newParameter(aPoint, aNewCurve, aFirst, aLast, aTol, aParam);
  }
  return true;
}

//=================================================================================================

bool BRepTools_NurbsConvertModification::NewPoint(const TopoDS_Vertex&, gp_Pnt&, double&)
{
  return false;
}

//=================================================================================================

bool BRepTools_NurbsConvertModification::NewCurve2d(const TopoDS_Edge&         E,
                                                    const TopoDS_Face&         F,
                                                    const TopoDS_Edge&         newE,
                                                    const TopoDS_Face&         newF,
                                                    occ::handle<Geom2d_Curve>& Curve2d,
                                                    double&                    Tol)
{

  Tol = BRep_Tool::Tolerance(E);
  double                    f2d, l2d;
  occ::handle<Geom2d_Curve> aBaseC2d = BRep_Tool::CurveOnSurface(E, F, f2d, l2d);
  double                    f3d, l3d;
  TopLoc_Location           Loc;
  occ::handle<Geom_Curve>   C3d = BRep_Tool::Curve(E, Loc, f3d, l3d);
  bool isConvert2d              = ((!C3d.IsNull() && !C3d->IsKind(STANDARD_TYPE(Geom_BSplineCurve))
                       && !C3d->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
                      || IsConvert(E));

  occ::handle<Geom2d_Curve> C2d = aBaseC2d;
  if (BRep_Tool::Degenerated(E))
  {
    // Curve2d = C2d;
    if (!C2d->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      occ::handle<Geom2d_TrimmedCurve> aTrimC = new Geom2d_TrimmedCurve(C2d, f2d, l2d);
      C2d                                     = aTrimC;
    }
    Curve2d = Geom2dConvert::CurveToBSplineCurve(C2d);
    myMap.Add(aBaseC2d, Curve2d);
    return true;
  }
  if (!BRepTools::IsReallyClosed(E, F))
  {
    occ::handle<Standard_Type> TheTypeC2d = C2d->DynamicType();

    if (TheTypeC2d == STANDARD_TYPE(Geom2d_TrimmedCurve))
    {
      occ::handle<Geom2d_TrimmedCurve> TC = occ::down_cast<Geom2d_TrimmedCurve>(C2d);
      C2d                                 = TC->BasisCurve();
    }

    double fc = C2d->FirstParameter(), lc = C2d->LastParameter();

    if (!C2d->IsPeriodic())
    {
      if (fc - f2d > Precision::PConfusion())
        f2d = fc;
      if (l2d - lc > Precision::PConfusion())
        l2d = lc;
    }

    C2d = new Geom2d_TrimmedCurve(C2d, f2d, l2d);

    Geom2dAdaptor_Curve              G2dAC(C2d, f2d, l2d);
    occ::handle<Geom2dAdaptor_Curve> G2dAHC = new Geom2dAdaptor_Curve(G2dAC);

    if (!newE.IsNull())
    {
      C3d = BRep_Tool::Curve(newE, f3d, l3d);
    }
    if (C3d.IsNull())
    {
      C3d = BRep_Tool::Curve(E, f3d, l3d);
    }
    if (C3d.IsNull())
      return false;
    GeomAdaptor_Curve              G3dAC(C3d, f3d, l3d);
    occ::handle<GeomAdaptor_Curve> G3dAHC = new GeomAdaptor_Curve(G3dAC);

    double                     Uinf, Usup, Vinf, Vsup, u = 0, v = 0;
    occ::handle<Geom_Surface>  S   = BRep_Tool::Surface(F);
    occ::handle<Standard_Type> myT = S->DynamicType();
    if (myT != STANDARD_TYPE(Geom_Plane))
    {
      if (newF.IsNull())
      {
        occ::handle<Standard_Type> st = C2d->DynamicType();
        if ((st == STANDARD_TYPE(Geom2d_BSplineCurve)) || (st == STANDARD_TYPE(Geom2d_BezierCurve)))
        {
          if (isConvert2d)
          {
            Curve2d       = occ::down_cast<Geom2d_Curve>(C2d->Copy());
            double newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
            if (newTol > Tol)
            {
              Tol = newTol;
              myUpdatedEdges.Append(newE);
            }
            myMap.Add(aBaseC2d, Curve2d);
            return true;
          }
          return false;
        }
      }
      else
      {
        occ::handle<Geom_Surface> aNewS = BRep_Tool::Surface(newF);
        if (!aNewS.IsNull())
          S = aNewS;
      }
      S->Bounds(Uinf, Usup, Vinf, Vsup);
      // Uinf -= 1e-9; Usup += 1e-9; Vinf -= 1e-9; Vsup += 1e-9;
      u = (Usup - Uinf) * 0.1;
      v = (Vsup - Vinf) * 0.1;
      if (S->IsUPeriodic())
      {
        double uperiod = S->UPeriod();
        if (uperiod < (Usup + 2 * u - Uinf))
        {
          if (uperiod <= (Usup - Uinf))
          {
            u = 0;
          }
          else
          {
            u = (uperiod - (Usup - Uinf)) * 0.5;
          }
        }
      }
      if (S->IsVPeriodic())
      {
        double vperiod = S->VPeriod();
        if (vperiod < (Vsup + 2 * v - Vinf))
        {
          if (vperiod <= (Vsup - Vinf))
          {
            v = 0;
          }
          else
          {
            v = (vperiod - (Vsup - Vinf)) * 0.5;
          }
        }
      }
    }
    else
    {
      S = BRep_Tool::Surface(F); // Si S est un plan, pas de changement de parametrisation
      GeomAdaptor_Surface              GAS(S);
      occ::handle<GeomAdaptor_Surface> GAHS = new GeomAdaptor_Surface(GAS);
      ProjLib_ComputeApprox            ProjOnCurve(G3dAHC, GAHS, Tol);
      if (ProjOnCurve.BSpline().IsNull())
      {
        Curve2d       = Geom2dConvert::CurveToBSplineCurve(ProjOnCurve.Bezier());
        double newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
        if (newTol > Tol)
        {
          Tol = newTol;
          myUpdatedEdges.Append(newE);
        }
        myMap.Add(aBaseC2d, Curve2d);
        return true;
      }
      Curve2d       = ProjOnCurve.BSpline();
      double newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
      if (newTol > Tol)
      {
        Tol = newTol;
        myUpdatedEdges.Append(newE);
      }
      myMap.Add(aBaseC2d, Curve2d);
      return true;
    }

    //
    GeomAdaptor_Surface GAS(S, Uinf - u, Usup + u, Vinf - v, Vsup + v);

    occ::handle<GeomAdaptor_Surface> GAHS = new GeomAdaptor_Surface(GAS);

    ProjLib_ComputeApproxOnPolarSurface ProjOnCurve(G2dAHC, G3dAHC, GAHS, Tol);

    if (ProjOnCurve.IsDone())
    {
      Curve2d = ProjOnCurve.BSpline();
      if (S->IsUPeriodic() || S->IsVPeriodic())
      {
        // Surface is periodic, checking curve2d domain
        // Old domain
        double aMinDist = Precision::Infinite();
        if (S->IsUPeriodic())
        {
          aMinDist = std::min(0.5 * S->UPeriod(), aMinDist);
        }
        if (S->IsVPeriodic())
        {
          aMinDist = std::min(0.5 * S->VPeriod(), aMinDist);
        }
        aMinDist *= aMinDist;
        // Old domain
        gp_Pnt2d aPf = C2d->Value(f2d);
        // New domain
        gp_Pnt2d aNewPf = Curve2d->Value(f2d);
        gp_Vec2d aT(aNewPf, aPf);
        if (aT.SquareMagnitude() > aMinDist)
        {
          Curve2d = occ::down_cast<Geom2d_Curve>(Curve2d->Translated(aT));
        }
      }
      double newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
      if (newTol > Tol)
      {
        Tol = newTol;
        myUpdatedEdges.Append(newE);
      }
      myMap.Add(aBaseC2d, Curve2d);
      return true;
    }
    else
    {
      Curve2d       = Geom2dConvert::CurveToBSplineCurve(C2d);
      double newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
      if (newTol > Tol)
      {
        Tol = newTol;
        myUpdatedEdges.Append(newE);
      }
      myMap.Add(aBaseC2d, Curve2d);
      return true;
    }
  }
  else
  {
    NCollection_List<TopoDS_Shape>::Iterator                    itled;
    NCollection_List<occ::handle<Standard_Transient>>::Iterator itlcu;

    for (itled.Initialize(myled), itlcu.Initialize(mylcu); itled.More(); // itlcu.More()
         itled.Next(), itlcu.Next())
    {
      if (itled.Value().IsSame(E))
      {
        // deja traitee
        break;
      }
    }
    if (!itled.More())
    { // on stocke l`edge et la curve2d
      occ::handle<Geom2d_Curve> C2dBis;
      double                    f2dBis, l2dBis;
      C2d = new Geom2d_TrimmedCurve(C2d, f2d, l2d);
      Geom2dAdaptor_Curve              G2dAC(C2d, f2d, l2d);
      occ::handle<Geom2dAdaptor_Curve> G2dAHC  = new Geom2dAdaptor_Curve(G2dAC);
      TopoDS_Edge                      ERevers = E;
      ERevers.Reverse();
      C2dBis = BRep_Tool::CurveOnSurface(ERevers, F, f2dBis, l2dBis);
      occ::handle<Standard_Type> TheTypeC2dBis = C2dBis->DynamicType();
      C2dBis                                   = new Geom2d_TrimmedCurve(C2dBis, f2dBis, l2dBis);
      Geom2dAdaptor_Curve              G2dACBis(C2dBis, f2dBis, l2dBis);
      occ::handle<Geom2dAdaptor_Curve> G2dAHCBis = new Geom2dAdaptor_Curve(G2dACBis);

      if (C3d.IsNull())
      {
        if (isConvert2d)
        {
          Curve2d = occ::down_cast<Geom2d_Curve>(C2d->Copy());

          occ::handle<Geom_Surface> S;
          if (newF.IsNull())
            S = BRep_Tool::Surface(F);
          else
            S = BRep_Tool::Surface(newF);
          //
          double newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
          if (newTol > Tol)
          {
            Tol = newTol;
            myUpdatedEdges.Append(newE);
          }
          myMap.Add(aBaseC2d, Curve2d);
          return true;
        }
        return false;
      }
      if (!newE.IsNull())
      {
        C3d = BRep_Tool::Curve(newE, f3d, l3d);
      }
      GeomAdaptor_Curve              G3dAC(C3d, f3d, l3d);
      occ::handle<GeomAdaptor_Curve> G3dAHC = new GeomAdaptor_Curve(G3dAC);

      occ::handle<Geom_Surface>  S   = BRep_Tool::Surface(F);
      occ::handle<Standard_Type> myT = S->DynamicType();
      if (newF.IsNull())
      {
        mylcu.Append(C2dBis);
        occ::handle<Standard_Type> st = C2d->DynamicType();
        if ((st == STANDARD_TYPE(Geom2d_BSplineCurve)) || (st == STANDARD_TYPE(Geom2d_BezierCurve)))
        {
          if (isConvert2d)
          {
            Curve2d       = occ::down_cast<Geom2d_Curve>(C2d->Copy());
            double newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
            if (newTol > Tol)
            {
              Tol = newTol;
              myUpdatedEdges.Append(newE);
            }
            myMap.Add(aBaseC2d, Curve2d);
            return true;
          }
          return false;
        }
      }
      else
      {
        S = BRep_Tool::Surface(newF); // S est une BSplineSurface : pas besoin de la trimmed
      }
      double Uinf, Usup, Vinf, Vsup, u = 0, v = 0;
      S->Bounds(Uinf, Usup, Vinf, Vsup);
      // Uinf -= 1e-9; Usup += 1e-9; Vinf -= 1e-9; Vsup += 1e-9;
      u = (Usup - Uinf) * 0.1;
      v = (Vsup - Vinf) * 0.1;
      if (S->IsUPeriodic())
      {
        double uperiod = S->UPeriod();
        if (uperiod < (Usup + 2 * u - Uinf))
        {
          if (uperiod <= (Usup - Uinf))
            u = 0;
          else
            u = (uperiod - (Usup - Uinf)) * 0.5;
        }
      }
      if (S->IsVPeriodic())
      {
        double vperiod = S->VPeriod();
        if (vperiod < (Vsup + 2 * v - Vinf))
        {
          if (vperiod <= (Vsup - Vinf))
            v = 0;
          else
            v = (vperiod - (Vsup - Vinf)) * 0.5;
        }
      }
      GeomAdaptor_Surface              GAS(S, Uinf - u, Usup + u, Vinf - v, Vsup + v);
      occ::handle<GeomAdaptor_Surface> GAHS = new GeomAdaptor_Surface(GAS);
      myled.Append(E);

      ProjLib_ComputeApproxOnPolarSurface ProjOnCurve(G2dAHC, G2dAHCBis, G3dAHC, GAHS, Tol);

      if (ProjOnCurve.IsDone())
      {
        Curve2d = ProjOnCurve.BSpline();
        mylcu.Append(ProjOnCurve.Curve2d());
        double newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
        if (newTol > Tol)
        {
          Tol = newTol;
          myUpdatedEdges.Append(newE);
        }
        myMap.Add(aBaseC2d, Curve2d);
        return true;
      }
      else
      {
        Curve2d       = Geom2dConvert::CurveToBSplineCurve(C2d);
        double newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
        if (newTol > Tol)
        {
          Tol = newTol;
          myUpdatedEdges.Append(newE);
        }
        mylcu.Append(C2dBis);
        myMap.Add(aBaseC2d, Curve2d);
        return true;
      }
    }
    else
    { // on est au 2ieme tour
      C2d                           = occ::down_cast<Geom2d_Curve>(itlcu.Value());
      occ::handle<Standard_Type> st = C2d->DynamicType();
      if (!(st == STANDARD_TYPE(Geom2d_BSplineCurve)) && !(st == STANDARD_TYPE(Geom2d_BezierCurve)))
      {
        return false;
      }
      Curve2d = Geom2dConvert::CurveToBSplineCurve(C2d);
      myMap.Add(aBaseC2d, Curve2d);
      return true;
    }
  }
}

//=================================================================================================

bool BRepTools_NurbsConvertModification::NewPolygonOnTriangulation(
  const TopoDS_Edge&                        theEdge,
  const TopoDS_Face&                        theFace,
  occ::handle<Poly_PolygonOnTriangulation>& thePoly)
{
  if (!BRepTools_CopyModification::NewPolygonOnTriangulation(theEdge, theFace, thePoly))
  {
    return false;
  }

  // update parameters of 2D polygon
  if (thePoly->HasParameters())
  {
    double                    aTol = std::max(BRep_Tool::Tolerance(theEdge), thePoly->Deflection());
    TopLoc_Location           aLoc;
    occ::handle<Geom_Surface> aSurf    = BRep_Tool::Surface(theFace, aLoc);
    occ::handle<Geom_Surface> aNewSurf = newSurface(myMap, theFace);
    double                    aFirst, aLast;
    occ::handle<Geom2d_Curve> aC2d    = BRep_Tool::CurveOnSurface(theEdge, theFace, aFirst, aLast);
    occ::handle<Geom2d_Curve> aNewC2d = newCurve(myMap, theEdge, theFace, aFirst, aLast);
    if (!aSurf.IsNull() && !aC2d.IsNull() && !aNewSurf.IsNull() && !aNewC2d.IsNull())
    {
      // compute 2D tolerance
      GeomAdaptor_Surface aSurfAdapt(aSurf);
      double aTol2D = std::max(aSurfAdapt.UResolution(aTol), aSurfAdapt.VResolution(aTol));

      for (int anInd = 1; anInd <= thePoly->NbNodes(); ++anInd)
      {
        double   aParam = thePoly->Parameter(anInd);
        gp_Pnt2d aUV    = aC2d->Value(aParam);
        gp_Pnt   aPoint = aSurf->Value(aUV.X(), aUV.Y());
        if (newUV(aPoint, aNewSurf, aTol, aUV)
            && newParameter(aUV, aNewC2d, aFirst, aLast, aTol2D, aParam))
        {
          thePoly->SetParameter(anInd, aParam);
        }
      }
    }
  }
  return true;
}

//=================================================================================================

bool BRepTools_NurbsConvertModification::NewParameter(const TopoDS_Vertex& V,
                                                      const TopoDS_Edge&   E,
                                                      double&              P,
                                                      double&              Tol)
{
  Tol = BRep_Tool::Tolerance(V);
  if (BRep_Tool::Degenerated(E))
    return false;

  gp_Pnt pnt = BRep_Tool::Pnt(V);
  P          = BRep_Tool::Parameter(V, E);
  double                  aFirst, aLast;
  occ::handle<Geom_Curve> aNewCurve = newCurve(myMap, E, aFirst, aLast);
  return !aNewCurve.IsNull() && newParameter(pnt, aNewCurve, aFirst, aLast, Tol, P);
}

//=================================================================================================

GeomAbs_Shape BRepTools_NurbsConvertModification::Continuity(const TopoDS_Edge& E,
                                                             const TopoDS_Face& F1,
                                                             const TopoDS_Face& F2,
                                                             const TopoDS_Edge&,
                                                             const TopoDS_Face&,
                                                             const TopoDS_Face&)
{
  return BRep_Tool::Continuity(E, F1, F2);
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepTools_NurbsConvertModification::GetUpdatedEdges() const
{
  return myUpdatedEdges;
}
