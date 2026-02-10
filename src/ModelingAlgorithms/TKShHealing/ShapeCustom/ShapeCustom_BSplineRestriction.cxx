// Created on: 1999-06-18
// Created by: Galina Koulikova
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
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Modifier.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Conic.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <gp_Pnt.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeConstruct.hxx>
#include <ShapeCustom_BSplineRestriction.hxx>
#include <ShapeCustom_RestrictionParameters.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_BSplineRestriction, ShapeCustom_Modification)

static GeomAbs_Shape IntegerToGeomAbsShape(const int i)
{
  GeomAbs_Shape result = GeomAbs_C0;
  switch (i)
  {
    case 0:
      result = GeomAbs_C0;
      break;
    case 1:
      result = GeomAbs_C1;
      break;
    case 2:
      result = GeomAbs_C2;
      break;
    case 3:
      result = GeomAbs_C3;
      break;
    default:
      result = GeomAbs_CN;
      break;
  }
  return result;
}

static int ContToInteger(const GeomAbs_Shape Cont)
{
  int result = 0;
  switch (Cont)
  {
    case GeomAbs_C0:
    case GeomAbs_G1:
      result = 0;
      break;
    case GeomAbs_C1:
    case GeomAbs_G2:
      result = 1;
      break;
    case GeomAbs_C2:
      result = 2;
      break;
    case GeomAbs_C3:
      result = 3;
      break;
    default:
      result = 4;
      break;
  }
  return result;
}

static bool IsConvertCurve3d(const occ::handle<Geom_Curve>&                        aCurve,
                             int                                                   Degree,
                             int                                                   NbSeg,
                             bool                                                  myRational,
                             const occ::handle<ShapeCustom_RestrictionParameters>& aParameters)
{
  if (aCurve.IsNull())
    return false;
  if (aParameters->ConvertCurve3d())
    return true;
  if (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    occ::handle<Geom_TrimmedCurve> tmp      = occ::down_cast<Geom_TrimmedCurve>(aCurve);
    occ::handle<Geom_Curve>        BasCurve = tmp->BasisCurve();
    return IsConvertCurve3d(BasCurve, Degree, NbSeg, myRational, aParameters);
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
  {
    if (aParameters->ConvertOffsetCurv3d())
      return true;
    occ::handle<Geom_OffsetCurve> tmp      = occ::down_cast<Geom_OffsetCurve>(aCurve);
    occ::handle<Geom_Curve>       BasCurve = tmp->BasisCurve();
    return IsConvertCurve3d(BasCurve, Degree, NbSeg, myRational, aParameters);
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    occ::handle<Geom_BSplineCurve> BsC = occ::down_cast<Geom_BSplineCurve>(aCurve);
    if (BsC->Degree() > Degree || ((BsC->NbKnots() - 1) >= NbSeg))
      return true;
    return myRational && BsC->IsRational();
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve))
      && (occ::down_cast<Geom_BezierCurve>(aCurve)->Degree() > Degree
          || (myRational && occ::down_cast<Geom_BezierCurve>(aCurve)->IsRational())))
    return true;
  // else return false;
  return false;
}

static bool IsConvertSurface(const occ::handle<Geom_Surface>&                      aSurface,
                             const int                                             Degree,
                             const int                                             NbSeg,
                             const bool                                            myRational,
                             const occ::handle<ShapeCustom_RestrictionParameters>& aParameters)
{
  if (aSurface.IsNull())
    return false;
  if (aSurface->IsKind(STANDARD_TYPE(Geom_Plane)))
  {
    return aParameters->ConvertPlane();
  }
  else if (aSurface->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
    return aParameters->ConvertConicalSurf();
  else if (aSurface->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
    return aParameters->ConvertSphericalSurf();
  else if (aSurface->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
    return aParameters->ConvertCylindricalSurf();
  else if (aSurface->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
    return aParameters->ConvertToroidalSurf();

  // else if(aSurface->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))    {
  //   return aParameters->ConvertElementarySurf();
  //  }
  if (aSurface->IsKind(STANDARD_TYPE(Geom_SweptSurface)))
  {
    if (aSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))
        && aParameters->ConvertRevolutionSurf())
      return true;
    if (aSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
        && aParameters->ConvertExtrusionSurf())
      return true;
    occ::handle<Geom_SweptSurface> aSurf    = occ::down_cast<Geom_SweptSurface>(aSurface);
    occ::handle<Geom_Curve>        BasCurve = aSurf->BasisCurve();
    return IsConvertCurve3d(BasCurve, Degree, NbSeg, myRational, aParameters);
  }
  if (aSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> aSurf =
      occ::down_cast<Geom_RectangularTrimmedSurface>(aSurface);
    occ::handle<Geom_Surface> theSurf = aSurf->BasisSurface();
    return IsConvertSurface(theSurf, Degree, NbSeg, myRational, aParameters);
  }
  if (aSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    if (aParameters->ConvertOffsetSurf())
      return true;
    occ::handle<Geom_OffsetSurface> aSurf   = occ::down_cast<Geom_OffsetSurface>(aSurface);
    occ::handle<Geom_Surface>       theSurf = aSurf->BasisSurface();
    return IsConvertSurface(theSurf, Degree, NbSeg, myRational, aParameters);
  }
  if (aSurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
  {

    occ::handle<Geom_BSplineSurface> theSurf = occ::down_cast<Geom_BSplineSurface>(aSurface);
    if (theSurf->UDegree() > Degree || theSurf->VDegree() > Degree)
      return true;
    if ((theSurf->NbUKnots() - 1) * (theSurf->NbVKnots() - 1) > NbSeg)
      return true;
    if (myRational && (theSurf->IsURational() || theSurf->IsVRational()))
      return true;
    return false;
  }

  if (aSurface->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
  {
    if (aParameters->ConvertBezierSurf())
      return true;
    occ::handle<Geom_BezierSurface> theSurf = occ::down_cast<Geom_BezierSurface>(aSurface);
    if (theSurf->UDegree() > Degree || theSurf->VDegree() > Degree)
      return true;
    if (myRational && (theSurf->IsURational() || theSurf->IsVRational()))
      return true;
    return false;
  }
  return false;
}

static bool IsConvertCurve2d(const occ::handle<Geom2d_Curve>&                      aCurve,
                             int                                                   Degree,
                             int                                                   NbSeg,
                             bool                                                  myRational,
                             const occ::handle<ShapeCustom_RestrictionParameters>& aParameters)
{
  if (aCurve.IsNull())
    return false;
  if (aParameters->ConvertCurve2d())
    return true;
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    occ::handle<Geom2d_TrimmedCurve> tmp      = occ::down_cast<Geom2d_TrimmedCurve>(aCurve);
    occ::handle<Geom2d_Curve>        BasCurve = tmp->BasisCurve();
    return IsConvertCurve2d(BasCurve, Degree, NbSeg, myRational, aParameters);
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
  {
    if (aParameters->ConvertOffsetCurv2d())
      return true;
    occ::handle<Geom2d_OffsetCurve> tmp      = occ::down_cast<Geom2d_OffsetCurve>(aCurve);
    occ::handle<Geom2d_Curve>       BasCurve = tmp->BasisCurve();
    return IsConvertCurve2d(BasCurve, Degree, NbSeg, myRational, aParameters);
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))
      && ((occ::down_cast<Geom2d_BSplineCurve>(aCurve)->Degree() > Degree
           || ((occ::down_cast<Geom2d_BSplineCurve>(aCurve)->NbKnots() - 1) > NbSeg))
          || (myRational && occ::down_cast<Geom2d_BSplineCurve>(aCurve)->IsRational())))
    return true;
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))
      && ((occ::down_cast<Geom2d_BezierCurve>(aCurve)->Degree() > Degree)
          || (myRational && occ::down_cast<Geom2d_BezierCurve>(aCurve)->IsRational())))
    return true;
  // else return false;
  return false;
}

//=================================================================================================

ShapeCustom_BSplineRestriction::ShapeCustom_BSplineRestriction()
{
  myApproxSurfaceFlag = true;
  myApproxCurve3dFlag = true;
  myApproxCurve2dFlag = true;
  myTol3d             = 0.01;
  myTol2d             = 1E-6;
  myContinuity3d      = GeomAbs_C1;
  myContinuity2d      = GeomAbs_C2;
  myMaxDegree         = 9;
  myNbMaxSeg          = 10000;
  mySurfaceError      = Precision::Confusion();
  myCurve3dError      = Precision::Confusion();
  myCurve2dError      = Precision::PConfusion();
  myNbOfSpan          = 0;
  myConvert           = false;
  myDeg               = true;
  myRational          = false;
  myParameters        = new ShapeCustom_RestrictionParameters;
}

ShapeCustom_BSplineRestriction::ShapeCustom_BSplineRestriction(const bool   anApproxSurfaceFlag,
                                                               const bool   anApproxCurve3dFlag,
                                                               const bool   anApproxCurve2dFlag,
                                                               const double aTol3d,
                                                               const double aTol2d,
                                                               const GeomAbs_Shape aContinuity3d,
                                                               const GeomAbs_Shape aContinuity2d,
                                                               const int           aMaxDegree,
                                                               const int           aNbMaxSeg,
                                                               const bool          Deg,
                                                               const bool          Rational)
{
  myApproxSurfaceFlag = anApproxSurfaceFlag;
  myApproxCurve3dFlag = anApproxCurve3dFlag;
  myApproxCurve2dFlag = anApproxCurve2dFlag;
  myTol3d             = aTol3d;
  myTol2d             = aTol2d;
  myMaxDegree         = aMaxDegree;
  myContinuity3d      = aContinuity3d;
  myContinuity2d      = aContinuity2d;
  myNbMaxSeg          = aNbMaxSeg;
  mySurfaceError      = Precision::Confusion();
  myCurve3dError      = Precision::Confusion();
  myCurve2dError      = Precision::PConfusion();
  myNbOfSpan          = 0;
  myConvert           = false;
  myDeg               = Deg;
  myRational          = Rational;
  myParameters        = new ShapeCustom_RestrictionParameters;
}

ShapeCustom_BSplineRestriction::ShapeCustom_BSplineRestriction(
  const bool                                            anApproxSurfaceFlag,
  const bool                                            anApproxCurve3dFlag,
  const bool                                            anApproxCurve2dFlag,
  const double                                          aTol3d,
  const double                                          aTol2d,
  const GeomAbs_Shape                                   aContinuity3d,
  const GeomAbs_Shape                                   aContinuity2d,
  const int                                             aMaxDegree,
  const int                                             aNbMaxSeg,
  const bool                                            Deg,
  const bool                                            Rational,
  const occ::handle<ShapeCustom_RestrictionParameters>& aModes)
{
  myApproxSurfaceFlag = anApproxSurfaceFlag;
  myApproxCurve3dFlag = anApproxCurve3dFlag;
  myApproxCurve2dFlag = anApproxCurve2dFlag;
  myTol3d             = aTol3d;
  myTol2d             = aTol2d;
  myMaxDegree         = aMaxDegree;
  myContinuity3d      = aContinuity3d;
  myContinuity2d      = aContinuity2d;
  myNbMaxSeg          = aNbMaxSeg;
  mySurfaceError      = Precision::Confusion();
  myCurve3dError      = Precision::Confusion();
  myCurve2dError      = Precision::PConfusion();
  myNbOfSpan          = 0;
  myConvert           = false;
  myDeg               = Deg;
  myRational          = Rational;
  myParameters        = aModes;
}

//=================================================================================================

bool ShapeCustom_BSplineRestriction::NewSurface(const TopoDS_Face&         F,
                                                occ::handle<Geom_Surface>& S,
                                                TopLoc_Location&           L,
                                                double&                    Tol,
                                                bool&                      RevWires,
                                                bool&                      RevFace)
{
  if (!myApproxSurfaceFlag)
    return false;
  RevWires                           = false;
  RevFace                            = false;
  myConvert                          = false;
  occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(F, L);
  if (aSurface.IsNull())
    return false;
  bool IsOf = true;
  if (myParameters->ConvertOffsetSurf())
    IsOf = false;
  double UF, UL, VF, VL;
  aSurface->Bounds(UF, UL, VF, VL);
  double Umin, Umax, Vmin, Vmax;
  BRepTools::UVBounds(F, Umin, Umax, Vmin, Vmax);
  if (myParameters->SegmentSurfaceMode())
  {
    UF = Umin;
    UL = Umax;
    VF = Vmin;
    VL = Vmax;
  }
  else
  {
    if (Precision::IsInfinite(UF) || Precision::IsInfinite(UL))
    {
      UF = Umin;
      UL = Umax;
    }
    if (Precision::IsInfinite(VF) || Precision::IsInfinite(VL))
    {
      VF = Vmin;
      VL = Vmax;
    }
  }

  bool IsConv = ConvertSurface(aSurface, S, UF, UL, VF, VL, IsOf);
  Tol         = Precision::Confusion(); // mySurfaceError;

  if (IsConv)
  {
    bool wasBSpline = aSurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface));
    occ::handle<Geom_RectangularTrimmedSurface> rts =
      occ::down_cast<Geom_RectangularTrimmedSurface>(aSurface);
    if (!rts.IsNull())
      wasBSpline = rts->BasisSurface()->IsKind(STANDARD_TYPE(Geom_BSplineSurface));

    if (wasBSpline)
      SendMsg(F, Message_Msg("BSplineRestriction.NewSurface.MSG1"));
    else
      SendMsg(F, Message_Msg("BSplineRestriction.NewSurface.MSG0"));
  }

  return IsConv;
}

//=================================================================================================

static void ConvertExtrusion(const occ::handle<Geom_Curve>& C, /*const gp_Dir& direction,*/
                             gp_Trsf&                       shiftF,
                             gp_Trsf&                       shiftL,
                             const double                   VF,
                             const double                   VL,
                             occ::handle<Geom_Surface>&     bspline)
{
  occ::handle<Geom_BSplineCurve>    bspl    = occ::down_cast<Geom_BSplineCurve>(C);
  int                               nbPoles = bspl->NbPoles();
  const NCollection_Array1<gp_Pnt>& poles   = bspl->Poles();
  const NCollection_Array1<double>& weights = bspl->WeightsArray();
  const NCollection_Array1<double>& knots   = bspl->Knots();
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

  bspline = new Geom_BSplineSurface(resPoles,
                                    resWeigth,
                                    knots,
                                    vknots,
                                    mults,
                                    vmults,
                                    bspl->Degree(),
                                    1,
                                    bspl->IsPeriodic(),
                                    false);
}

bool ShapeCustom_BSplineRestriction::ConvertSurface(const occ::handle<Geom_Surface>& aSurface,
                                                    occ::handle<Geom_Surface>&       S,
                                                    const double                     UF,
                                                    const double                     UL,
                                                    const double                     VF,
                                                    const double                     VL,
                                                    const bool                       IsOf)
{
  if (!IsConvertSurface(aSurface, myMaxDegree, myNbMaxSeg, myRational, myParameters))
    return false;

  occ::handle<Geom_Surface> aSurf = aSurface;
  if (aSurf->IsKind(STANDARD_TYPE(Geom_Plane)) && myParameters->ConvertPlane())
  {
    occ::handle<Geom_Plane>    pln = occ::down_cast<Geom_Plane>(aSurf);
    NCollection_Array2<gp_Pnt> poles(1, 2, 1, 2);
    NCollection_Array1<double> uknots(1, 2);
    NCollection_Array1<int>    umults(1, 2);
    NCollection_Array1<double> vknots(1, 2);
    NCollection_Array1<int>    vmults(1, 2);

    poles(1, 1) = pln->Value(UF, VF);
    poles(1, 2) = pln->Value(UF, VL);
    poles(2, 1) = pln->Value(UL, VF);
    poles(2, 2) = pln->Value(UL, VL);
    uknots(1)   = UF;
    uknots(2)   = UL;
    vknots(1)   = VF;
    vknots(2)   = VL;
    umults(1) = umults(2) = vmults(1) = vmults(2) = 2;
    S = new Geom_BSplineSurface(poles, uknots, vknots, umults, vmults, 1, 1, false, false);
    return true;
  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
  {
    occ::handle<Geom_SurfaceOfRevolution> Surface = occ::down_cast<Geom_SurfaceOfRevolution>(aSurf);
    occ::handle<Geom_Curve>               BasCurve = Surface->BasisCurve();
    occ::handle<Geom_Curve>               ResCurve;
    double                                TolS = Precision::Confusion();
    if (myParameters->ConvertRevolutionSurf())
    {
      if (BasCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
      {
        GeomAbs_Shape cnt = BasCurve->Continuity();
        cnt               = (cnt > GeomAbs_C2 ? GeomAbs_C2 : cnt);
        if (ConvertCurve(BasCurve,
                         ResCurve,
                         false,
                         std::max(VF, BasCurve->FirstParameter()),
                         std::min(VL, BasCurve->LastParameter()),
                         TolS,
                         false))
        {
          occ::handle<Geom_SurfaceOfRevolution> newRevol =
            new Geom_SurfaceOfRevolution(ResCurve, Surface->Axis());
          aSurf = newRevol;
#ifdef OCCT_DEBUG
          std::cout << " Revolution on offset converted" << std::endl;
#endif
        }
      }
    }
    else
    {
      if (ConvertCurve(BasCurve,
                       ResCurve,
                       false,
                       std::max(VF, BasCurve->FirstParameter()),
                       std::min(VL, BasCurve->LastParameter()),
                       TolS,
                       IsOf))
      {
        S = new Geom_SurfaceOfRevolution(ResCurve, Surface->Axis());
        return true;
      }
      else
        return false;
    }
  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
  {
    occ::handle<Geom_SurfaceOfLinearExtrusion> Surface =
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(aSurf);
    occ::handle<Geom_Curve> BasCurve = Surface->BasisCurve();
    occ::handle<Geom_Curve> ResCurve;
    double                  TolS = Precision::Confusion();
    if (myParameters->ConvertExtrusionSurf())
    {
      GeomAbs_Shape cnt = Surface->Continuity();
      cnt               = (cnt > GeomAbs_C2 ? GeomAbs_C2 : cnt);
      occ::handle<Geom_BSplineCurve> bspl =
        ShapeConstruct::ConvertCurveToBSpline(BasCurve, UF, UL, TolS, cnt, myNbMaxSeg, myMaxDegree);
      BasCurve = bspl;
      ConvertCurve(BasCurve,
                   ResCurve,
                   true,
                   std::max(UF, BasCurve->FirstParameter()),
                   std::min(UL, BasCurve->LastParameter()),
                   TolS,
                   IsOf);
      gp_Trsf shiftF, shiftL;
      shiftF.SetTranslation(Surface->Value(UF, 0), Surface->Value(UF, VF));
      shiftL.SetTranslation(Surface->Value(UF, 0), Surface->Value(UF, VL));
      ConvertExtrusion(ResCurve, /*Surface->Direction(),*/ shiftF, shiftL, VF, VL, S);
      return true;
    }
    else
    {
      if (ConvertCurve(BasCurve,
                       ResCurve,
                       false,
                       std::max(UF, BasCurve->FirstParameter()),
                       std::min(UL, BasCurve->LastParameter()),
                       TolS,
                       IsOf))
      {
        S = new Geom_SurfaceOfLinearExtrusion(ResCurve, Surface->Direction());
        return true;
      }
      else
        return false;
    }
  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> tmp =
      occ::down_cast<Geom_RectangularTrimmedSurface>(aSurf);
    double U1, U2, V1, V2;
    tmp->Bounds(U1, U2, V1, V2);
    occ::handle<Geom_Surface> theSurf = tmp->BasisSurface();
    occ::handle<Geom_Surface> ResSurface;
    if (ConvertSurface(theSurf, ResSurface, U1, U2, V1, V2, IsOf))
    {
      // S = new Geom_RectangularTrimmedSurface(ResSurface,U1,U2,V1,V2);
      S = ResSurface;
      return true;
    }
    else
      return false;
  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)) && IsOf)
  {
    occ::handle<Geom_OffsetSurface> tmp     = occ::down_cast<Geom_OffsetSurface>(aSurf);
    occ::handle<Geom_Surface>       theSurf = tmp->BasisSurface();
    occ::handle<Geom_Surface>       ResSurface;
    if (ConvertSurface(theSurf, ResSurface, UF, UL, VF, VL))
    {
      if (ResSurface->Continuity() != GeomAbs_C0)
      {
        S = new Geom_OffsetSurface(ResSurface, tmp->Offset());
        return true;
      }
      else if (ConvertSurface(aSurf, S, UF, UL, VF, VL, false))
        return true;
      else
        return false;
    }
    else
      return false;
  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_BezierSurface)) && myParameters->ConvertBezierSurf())
  {
    occ::handle<Geom_BezierSurface>   bezier  = occ::down_cast<Geom_BezierSurface>(aSurf);
    int                               uDegree = bezier->UDegree();
    int                               vDegree = bezier->VDegree();
    const NCollection_Array2<gp_Pnt>& aPoles  = bezier->Poles();
    const NCollection_Array2<double>& aWeights = bezier->WeightsArray();
    NCollection_Array1<double> uKnots(1, 2), vKnots(1, 2);
    uKnots(1) = 0;
    uKnots(2) = 1;
    vKnots(1) = 0;
    vKnots(2) = 1;
    NCollection_Array1<int> uMults(1, 2), vMults(1, 2);
    uMults.Init(uDegree + 1);
    vMults.Init(vDegree + 1);
    occ::handle<Geom_BSplineSurface> bspline =
      new Geom_BSplineSurface(aPoles, aWeights, uKnots, vKnots, uMults, vMults, uDegree, vDegree);

    if (!ConvertSurface(bspline, S, UF, UL, VF, VL, IsOf))
      S = bspline;
    return true;
  }

  int  NbSeg = 1;
  bool URat  = false;
  bool VRat  = false;
  // if (aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)) ||
  //     aSurf->IsKind(STANDARD_TYPE(Geom_BezierSurface)) ||
  //     (aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)) && !IsOf) ||
  //     aSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)) ||
  //     aSurface->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))) {
  int UDeg = 1, VDeg = 1;
  if (aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
  {
    occ::handle<Geom_BSplineSurface> BsS = occ::down_cast<Geom_BSplineSurface>(aSurf);
    UDeg                                 = BsS->UDegree();
    VDeg                                 = BsS->VDegree();
    NbSeg                                = (BsS->NbUKnots() - 1) * (BsS->NbVKnots() - 1);
    URat                                 = BsS->IsURational();
    VRat                                 = BsS->IsVRational();
    bool IsR                             = (myRational && (URat || VRat));
    if (UDeg <= myMaxDegree && VDeg <= myMaxDegree && NbSeg <= myNbMaxSeg && !IsR)
      return false;
  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
  {
    occ::handle<Geom_BezierSurface> BsZ = occ::down_cast<Geom_BezierSurface>(aSurf);
    UDeg                                = BsZ->UDegree();
    VDeg                                = BsZ->VDegree();
    NbSeg                               = 1;
    URat                                = BsZ->IsURational();
    VRat                                = BsZ->IsVRational();
    bool IsR                            = (myRational && (URat || VRat));
    if (UDeg <= myMaxDegree && VDeg <= myMaxDegree && NbSeg <= myNbMaxSeg && !IsR)
      return false;
  }
  GeomAbs_Shape Cont = myContinuity3d;
  if (aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    Cont = GeomAbs_C0;
  /*    bool IsR = (myRational && (URat || VRat));
  if( UDeg <= myMaxDegree &&  VDeg <= myMaxDegree && NbSeg <=  myNbMaxSeg && !IsR ) {
  return false;

  }*/

  double aTol3d;
  int    nbOfSpan, imax = 10;
  int    MaxSeg = myNbMaxSeg;
  int    MaxDeg = myMaxDegree;
  double u1, u2, v1, v2;
  aSurf->Bounds(u1, u2, v1, v2);
  double ShiftU = 0, ShiftV = 0;
  if (std::abs(u1 - UF) > Precision::PConfusion() || std::abs(u2 - UL) > Precision::PConfusion()
      || std::abs(v1 - VF) > Precision::PConfusion() || std::abs(v2 - VL) > Precision::PConfusion())
  {
    /*if(aSurf->IsUPeriodic() ) {
    double aDelta = (UL > UF ? UL - UF : UF - UL );
    u1 = (aDelta > 2.*M_PI ? 0. : UF + ShapeAnalysis::AdjustByPeriod(UF,0.5*(UL+UF),2*M_PI));
    u2 = (aDelta > 2.*M_PI ? 2.*M_PI : u1 + aDelta);
    }*/
    bool isTrim = false;
    if (!aSurf->IsUPeriodic())
    { // else {
      u1     = std::max(u1, UF);
      u2     = std::min(u2, UL);
      isTrim = true;
    }
    /*if(aSurf->IsVPeriodic()) {

    double aDelta = (VL > VF ? VL - VF : VF - VL );
    v1 = (aDelta > 2.*M_PI ? 0. : VF + ShapeAnalysis::AdjustByPeriod(VF,0.5*(UL+UF),2*M_PI));
    v2 = (aDelta > 2.*M_PI ? 2.* M_PI : v1 + aDelta);
    }*/
    if (!aSurf->IsVPeriodic())
    { // else
      v1     = std::max(v1, VF);
      v2     = std::min(v2, VL);
      isTrim = true;
    }

    if (isTrim && (u1 != u2) && (v1 != v2))
    {
      occ::handle<Geom_RectangularTrimmedSurface> trSurface =
        new Geom_RectangularTrimmedSurface(aSurf, u1, u2, v1, v2);
      double ur1, ur2, vr1, vr2;
      trSurface->Bounds(ur1, ur2, vr1, vr2);
      ShiftU = u1 - ur1;
      ShiftV = v1 - vr1;
      aSurf  = trSurface;
    }
  }
  int aCU = std::min(ContToInteger(Cont), ContToInteger(aSurf->Continuity()));
  int aCV = std::min(ContToInteger(Cont), ContToInteger(aSurf->Continuity()));
  if (!aCU)
    aCU = ContToInteger(Cont);
  if (!aCV)
    aCV = ContToInteger(Cont);

  for (;;)
  {
    double prevTol = RealLast(), newTol = 0;
    for (int i = 1; i <= imax; i++)
    {
      aTol3d = myTol3d * i / 2;
      while (aCU >= 0 || aCV >= 0)
      {
        try
        {
          OCC_CATCH_SIGNALS
          GeomAbs_Shape aContV = IntegerToGeomAbsShape(aCV);
          GeomAbs_Shape aContU = IntegerToGeomAbsShape(aCU);

          GeomConvert_ApproxSurface
               anApprox(aSurf, aTol3d, aContU, aContV, MaxDeg, MaxDeg, MaxSeg, 0);
          bool Done = anApprox.IsDone();
          newTol    = anApprox.MaxError();
          if (anApprox.MaxError() <= myTol3d && Done)
          {

            nbOfSpan = (anApprox.Surface()->NbUKnots() - 1) * (anApprox.Surface()->NbVKnots() - 1);
#ifdef OCCT_DEBUG
            if ((imax - i + 1) != 1)
            {
              std::cout << " iteration = " << i << "\terror = " << anApprox.MaxError()
                        << "\tspans = " << nbOfSpan << std::endl;
              std::cout << " Surface is approximated with continuity "
                        << IntegerToGeomAbsShape(std::min(aCU, aCV)) << std::endl;
            }
#endif
            S                                    = anApprox.Surface();
            occ::handle<Geom_BSplineSurface> Bsc = occ::down_cast<Geom_BSplineSurface>(S);
            if (aSurface->IsUPeriodic())
              Bsc->SetUPeriodic();
            if (aSurface->IsVPeriodic())
              Bsc->SetVPeriodic();
            // int DegU = Bsc->UDegree(); // DegU not used (skl)
            // int DegV = Bsc->VDegree(); // DegV not used (skl)
            // int nbVK = Bsc->NbVKnots(); // nbVK not used (skl)
            // int nbUK = Bsc->NbUKnots(); // nbUK not used (skl)
            myConvert      = true;
            myNbOfSpan     = myNbOfSpan + nbOfSpan;
            mySurfaceError = std::max(mySurfaceError, anApprox.MaxError());
            if (std::abs(ShiftU) > Precision::PConfusion())
            {
              NCollection_Array1<double> uknots(Bsc->UKnots());
              for (int j = 1; j <= uknots.Length(); j++)
                uknots(j) += ShiftU;
              Bsc->SetUKnots(uknots);
            }
            if (std::abs(ShiftV) > Precision::PConfusion())
            {
              NCollection_Array1<double> vknots(Bsc->VKnots());
              for (int j = 1; j <= vknots.Length(); j++)
                vknots(j) += ShiftV;
              Bsc->SetVKnots(vknots);
            }

            return true;
          }
          else
          {
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
          // szv: protection against loop
          if (aCU == 0 && aCV == 0)
            break;
          if (aCU > 0)
            aCU--;
          if (aCV > 0)
            aCV--;
        }
      }
      if (prevTol <= newTol)
        break;
      else
        prevTol = newTol;
    }
    // int GMaxDegree = 15;//Geom_BSplineSurface::MaxDegree();

    if (myDeg)
    {
      if (MaxSeg < myParameters->GMaxSeg())
      {
        if (aCV != 0 || aCU != 0)
        {
          if (aCV > 0)
            aCV--;
          if (aCU > 0)
            aCU--;
        }
        else
          MaxSeg = 2 * MaxSeg; // myGMaxSeg;
        if (MaxSeg > myParameters->GMaxSeg())
          MaxSeg = myParameters->GMaxSeg();
        else
          continue;
      }
      else
      {
#ifdef OCCT_DEBUG
        std::cout << " Approximation iteration out. Surface is not approximated." << std::endl;
#endif
        return false;
      }
    }
    else
    {
      if (MaxDeg < myParameters->GMaxDegree())
      {
        MaxDeg = myParameters->GMaxDegree();
        continue;
      }
      else
      {
#ifdef OCCT_DEBUG
        std::cout << " Approximation iteration out. Surface is not approximated." << std::endl;
#endif
        return false;
      }
    }
  }
  //}
  // else
  // Surface is not BSpline or Bezier
  //  return false;
}

//=================================================================================================

bool ShapeCustom_BSplineRestriction::NewCurve(const TopoDS_Edge&       E,
                                              occ::handle<Geom_Curve>& C,
                                              TopLoc_Location&         L,
                                              double&                  Tol)
{
  if (!myApproxCurve3dFlag)
    return false;
  double                  First, Last;
  occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(E, L, First, Last);
  double                  TolCur = BRep_Tool::Tolerance(E);
  // if(aCurve.IsNull()) return false;
  bool                     IsConvert = false;
  occ::handle<BRep_TEdge>& TE        = *((occ::handle<BRep_TEdge>*)&E.TShape());
  // iterate on pcurves
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
  for (; itcr.More(); itcr.Next())
  {
    occ::handle<BRep_GCurve> GC = occ::down_cast<BRep_GCurve>(itcr.Value());
    if (GC.IsNull() || !GC->IsCurveOnSurface())
      continue;
    occ::handle<Geom_Surface> aSurface = GC->Surface();
    occ::handle<Geom2d_Curve> aCurve2d = GC->PCurve();
    if ((myApproxSurfaceFlag
         && IsConvertSurface(aSurface, myMaxDegree, myNbMaxSeg, myRational, myParameters))
        || (myApproxCurve2dFlag
            && IsConvertCurve2d(aCurve2d, myMaxDegree, myNbMaxSeg, myRational, myParameters)))
    {
      IsConvert = true;
      break;
    }
  }
  if (aCurve.IsNull())
  {
    if (IsConvert)
    {
      C   = aCurve;
      Tol = TolCur;
      return true;
    }
    else
      return false;
  }
  bool IsOf = true;
  if (myParameters->ConvertOffsetCurv3d())
    IsOf = false;
  bool IsConv = ConvertCurve(aCurve, C, IsConvert, First, Last, TolCur, IsOf);
  Tol         = BRep_Tool::Tolerance(E); // TolCur;

  if (IsConv)
  {
    bool                           wasBSpline = aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve));
    occ::handle<Geom_TrimmedCurve> tc         = occ::down_cast<Geom_TrimmedCurve>(aCurve);
    if (!tc.IsNull())
      wasBSpline = tc->BasisCurve()->IsKind(STANDARD_TYPE(Geom_BSplineCurve));

    if (wasBSpline)
      SendMsg(E, Message_Msg("BSplineRestriction.NewCurve.MSG1"));
    else
      SendMsg(E, Message_Msg("BSplineRestriction.NewCurve.MSG0"));
  }
  return IsConv;
}

//=================================================================================================

bool ShapeCustom_BSplineRestriction::ConvertCurve(const occ::handle<Geom_Curve>& aCurve,
                                                  occ::handle<Geom_Curve>&       C,
                                                  const bool                     IsConvert,
                                                  const double                   First,
                                                  const double                   Last,
                                                  double&                        TolCur,
                                                  const bool                     IsOf)
{
  //  TolCur =  Precision::Confusion();
  if (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    occ::handle<Geom_TrimmedCurve> tmp = occ::down_cast<Geom_TrimmedCurve>(aCurve);
    // double pf =tmp->FirstParameter(), pl =  tmp->LastParameter(); // pf,pl not used - see
    // below (skl)
    occ::handle<Geom_Curve> BasCurve = tmp->BasisCurve();
    occ::handle<Geom_Curve> ResCurve;
    if (ConvertCurve(BasCurve, ResCurve, IsConvert, First, Last, TolCur, IsOf))
    {
      //      Stanadrd_Real F = std::max(pf,First), L = std::min(pl,Last);
      //      if(First != Last)
      //	C = new
      // Geom_TrimmedCurve(ResCurve,Max(First,ResCurve->FirstParameter()),Min(Last,ResCurve->LastParameter()));
      // else
      C = ResCurve;
      return true;
    }
    else
    {
      if (IsConvert)
      {
        C      = occ::down_cast<Geom_Curve>(aCurve->Copy());
        TolCur = Precision::Confusion();
        return true;
      }

      return false;
    }
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom_Line)) && myParameters->ConvertCurve3d())
  {
    occ::handle<Geom_Line>     aLine = occ::down_cast<Geom_Line>(aCurve);
    NCollection_Array1<gp_Pnt> poles(1, 2);
    poles(1) = aLine->Value(First);
    poles(2) = aLine->Value(Last);
    NCollection_Array1<double> knots(1, 2);
    knots(1) = First;
    knots(2) = Last;
    NCollection_Array1<int> mults(1, 2);
    mults.Init(2);
    occ::handle<Geom_BSplineCurve> res = new Geom_BSplineCurve(poles, knots, mults, 1);
    C                                  = res;
    return true;
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom_Conic)) && myParameters->ConvertCurve3d())
  {
    occ::handle<Geom_BSplineCurve> aBSpline;
    // clang-format off
    occ::handle<Geom_Curve> tcurve = new Geom_TrimmedCurve(aCurve,First,Last); //protection against parabols ets
    // clang-format on
    GeomConvert_ApproxCurve approx(tcurve,
                                   myTol3d /*Precision::Approximation()*/,
                                   myContinuity2d,
                                   myNbMaxSeg,
                                   6);
    if (approx.HasResult())
      aBSpline = approx.Curve();
    else
      aBSpline = GeomConvert::CurveToBSplineCurve(tcurve, Convert_QuasiAngular);

    double Shift = First - aBSpline->FirstParameter();
    if (std::abs(Shift) > Precision::PConfusion())
    {
      NCollection_Array1<double> newKnots(aBSpline->Knots());
      for (int i = 1; i <= newKnots.Length(); i++)
        newKnots(i) += Shift;
      aBSpline->SetKnots(newKnots);
    }
    occ::handle<Geom_Curve> ResCurve;
    if (ConvertCurve(aBSpline, ResCurve, IsConvert, First, Last, TolCur, false))
    {
      C = ResCurve;
      return true;
    }
    else
    {
      C      = aBSpline;
      TolCur = Precision::PConfusion();
      return true;
    }
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve)) && myParameters->ConvertCurve3d())
  {
    occ::handle<Geom_Curve> aBSpline =
      GeomConvert::CurveToBSplineCurve(aCurve, Convert_QuasiAngular);
    occ::handle<Geom_Curve> ResCurve;
    if (ConvertCurve(aBSpline, ResCurve, IsConvert, First, Last, TolCur, false))
    {
      C = ResCurve;
      return true;
    }
    else
    {
      C      = aBSpline;
      TolCur = Precision::PConfusion();
      return true;
    }
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)) && IsOf)
  {
    occ::handle<Geom_OffsetCurve> tmp      = occ::down_cast<Geom_OffsetCurve>(aCurve);
    occ::handle<Geom_Curve>       BasCurve = tmp->BasisCurve();
    occ::handle<Geom_Curve>       ResCurve;
    if (ConvertCurve(BasCurve, ResCurve, IsConvert, First, Last, TolCur))
    {
      if (ResCurve->Continuity() != GeomAbs_C0)
      {
        C = new Geom_OffsetCurve(ResCurve, tmp->Offset(), tmp->Direction());
        return true;
      }
      else if (ConvertCurve(aCurve, C, IsConvert, First, Last, TolCur, false))
        return true;
      else
      {
        if (IsConvert)
        {
          C      = occ::down_cast<Geom_Curve>(aCurve->Copy());
          TolCur = Precision::Confusion();
          return true;
        }
        return false;
      }
    }
    else
    {
      if (IsConvert)
      {
        C      = occ::down_cast<Geom_Curve>(aCurve->Copy());
        TolCur = Precision::Confusion();
        return true;
      }
      return false;
    }
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))
      || aCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve))
      || (aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)) && !IsOf))
  {
    int Deg = 1;

    if (aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
    {
      occ::handle<Geom_BSplineCurve> BsC = occ::down_cast<Geom_BSplineCurve>(aCurve);
      Deg                                = BsC->Degree();
      bool IsR                           = (myRational && BsC->IsRational());
      if (!IsR && Deg <= myMaxDegree && (BsC->NbKnots() - 1) <= myNbMaxSeg)
      {
        if (IsConvert)
        {
          C      = occ::down_cast<Geom_Curve>(aCurve->Copy());
          TolCur = Precision::Confusion();
          return true;
        }
        else
          return false;
      }
    }
    if (aCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
    {
      occ::handle<Geom_BezierCurve> BzC = occ::down_cast<Geom_BezierCurve>(aCurve);
      Deg                               = BzC->Degree();
      bool IsR                          = (myRational && BzC->IsRational());
      if (!IsR && Deg <= myMaxDegree)
      {
        if (IsConvert)
        {
          C      = occ::down_cast<Geom_Curve>(aCurve->Copy());
          TolCur = Precision::Confusion();
          return true;
        }
        else
          return false;
      }
    }
    occ::handle<Geom_Curve> aCurve1;
    double                  pf = aCurve->FirstParameter(), pl = aCurve->LastParameter();
    // 15.11.2002 PTV OCC966
    if (ShapeAnalysis_Curve::IsPeriodic(aCurve) && (First != Last))
      aCurve1 = new Geom_TrimmedCurve(aCurve, First, Last);
    else if (pf < (First - Precision::PConfusion()) || pl > (Last + Precision::PConfusion()))
    {
      double F = std::max(First, pf), L = std::min(Last, pl);
      if (F != L)
        aCurve1 = new Geom_TrimmedCurve(aCurve, F, L);
      else
        aCurve1 = aCurve;
    }
    else
      aCurve1 = aCurve;
    int aC = std::min(ContToInteger(myContinuity3d), ContToInteger(aCurve->Continuity()));
    if (!aC)
      aC = ContToInteger(myContinuity3d);
    // aC = std::min(aC,(Deg -1));
    int MaxSeg = myNbMaxSeg;
    int MaxDeg = myMaxDegree;
    // GeomAbs_Shape aCont = IntegerToGeomAbsShape(aC);
    int aC1 = aC;
    // int GMaxDegree = 15; //Geom_BSplineCurve::MaxDegree();
    for (; aC >= 0; aC--)
    {
      try
      {
        OCC_CATCH_SIGNALS
        for (int j = 1; j <= 2; j++)
        {
          GeomAbs_Shape           aCont = IntegerToGeomAbsShape(aC);
          GeomConvert_ApproxCurve anApprox(aCurve1, myTol3d, aCont, MaxSeg, MaxDeg);
          bool                    Done = anApprox.IsDone();
          C                            = anApprox.Curve();
          int Nbseg                    = occ::down_cast<Geom_BSplineCurve>(C)->NbKnots() - 1;
          int DegC                     = occ::down_cast<Geom_BSplineCurve>(C)->Degree();
          if (myDeg
              && ((DegC > MaxDeg) || !Done || (anApprox.MaxError() >= std::max(TolCur, myTol3d))))
          {
            if (MaxSeg < myParameters->GMaxSeg())
            {
              MaxSeg = myParameters->GMaxSeg();
              aC     = aC1;
              continue;
            }
            else
            {
#ifdef OCCT_DEBUG
              std::cout << "Curve is not aproxed with continuity  " << aCont << std::endl;
#endif
              if (IsConvert)
              {
                C      = occ::down_cast<Geom_Curve>(aCurve->Copy());
                TolCur = Precision::Confusion();
                return true;
              }
            }
          }
          if (!myDeg
              && ((Nbseg > myParameters->GMaxSeg()) || !Done
                  || (anApprox.MaxError() >= std::max(TolCur, myTol3d))))
          {
            if (MaxDeg < myParameters->GMaxDegree())
            {
              MaxDeg = myParameters->GMaxDegree();
              aC     = aC1;
              continue;
            }
            else
            {
#ifdef OCCT_DEBUG
              std::cout << "Curve is not aproxed with continuity  " << aCont << std::endl;
#endif
              if (IsConvert)
              {
                C      = occ::down_cast<Geom_Curve>(aCurve->Copy());
                TolCur = Precision::Confusion();
                return true;
              }
            }
          }
          myConvert      = true;
          TolCur         = anApprox.MaxError();
          myCurve3dError = std::max(myCurve3dError, anApprox.MaxError());
          return true;
        }
      }
      catch (Standard_Failure const& anException)
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning: GeomConvert_ApproxCurve Exception: Wrong Coefficient : Decrease "
                     "continuity    ";
        anException.Print(std::cout);
        std::cout << std::endl;
#endif
        (void)anException;
        continue;
      }
    }
    return false;
  }
  else
  {
    if (IsConvert)
    {
      C      = occ::down_cast<Geom_Curve>(aCurve->Copy());
      TolCur = Precision::Confusion();
      return true;
    }
    return false;
  }
}

//=================================================================================================

bool ShapeCustom_BSplineRestriction::NewCurve2d(const TopoDS_Edge& E,
                                                const TopoDS_Face& F,
                                                const TopoDS_Edge& NewE,
                                                const TopoDS_Face& /*NewF*/,
                                                occ::handle<Geom2d_Curve>& C,
                                                double&                    Tol)
{
  if (!myApproxCurve2dFlag && !myApproxSurfaceFlag)
    return false;
  double                    First, Last, F1, L1;
  TopLoc_Location           L, Loc1;
  occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(F, L);
  GeomAdaptor_Surface       AdS(aSurface);
  double                    TolCur =
    std::min(AdS.UResolution(BRep_Tool::Tolerance(E)), AdS.VResolution(BRep_Tool::Tolerance(E)));
  occ::handle<Geom2d_Curve> aCurve = BRep_Tool::CurveOnSurface(E, F, First, Last);
  if (aCurve.IsNull())
    return false;
  occ::handle<Geom_Curve> aCur3d = BRep_Tool::Curve(E, Loc1, F1, L1);
  //  bool IsConvert = (IsConvertSurface(aSurface,myMaxDegree,myNbMaxSeg) ||
  //  !E.IsSame(NewE));

  bool IsConvert =
    ((myApproxSurfaceFlag
      && IsConvertSurface(aSurface, myMaxDegree, myNbMaxSeg, myRational, myParameters))
     || (myApproxCurve3dFlag
         && IsConvertCurve3d(aCur3d, myMaxDegree, myNbMaxSeg, myRational, myParameters)));

  if (!IsConvert)
  {
    occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&E.TShape());
    // iterate on pcurves
    NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
    for (; itcr.More(); itcr.Next())
    {
      occ::handle<BRep_GCurve> GC = occ::down_cast<BRep_GCurve>(itcr.Value());
      if (GC.IsNull() || !GC->IsCurveOnSurface())
        continue;
      occ::handle<Geom_Surface> aSurf  = GC->Surface();
      occ::handle<Geom2d_Curve> aCur2d = GC->PCurve();
      if ((myApproxSurfaceFlag
           && IsConvertSurface(aSurf, myMaxDegree, myNbMaxSeg, myRational, myParameters))
          || (myApproxCurve2dFlag
              && IsConvertCurve2d(aCur2d, myMaxDegree, myNbMaxSeg, myRational, myParameters)))
      {
        IsConvert = true;
        break;
      }
    }
  }
  if (!myApproxCurve2dFlag)
  {
    if (IsConvert)
    {
      C = occ::down_cast<Geom2d_Curve>(aCurve->Copy());
      return true;
    }
    else
      return false;
  }
  bool IsOf = true;
  if (myParameters->ConvertOffsetCurv2d())
    IsOf = false;
  bool IsConv = ConvertCurve2d(aCurve, C, IsConvert, First, Last, TolCur, IsOf);

  Tol = BRep_Tool::Tolerance(E); // TolCur;
  BRep_Builder B;
  if (!IsConv && !NewE.IsSame(E))
    B.Range(NewE, First, Last);
  return IsConv;
}

//=================================================================================================

bool ShapeCustom_BSplineRestriction::ConvertCurve2d(const occ::handle<Geom2d_Curve>& aCurve,
                                                    occ::handle<Geom2d_Curve>&       C,
                                                    const bool                       IsConvert,
                                                    const double                     First,
                                                    const double                     Last,
                                                    double&                          TolCur,
                                                    const bool                       IsOf)
{
  // TolCur = Precision::PConfusion();
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    occ::handle<Geom2d_TrimmedCurve> tmp = occ::down_cast<Geom2d_TrimmedCurve>(aCurve);
    // double pf =tmp->FirstParameter(), pl =  tmp->LastParameter();
    occ::handle<Geom2d_Curve> BasCurve = tmp->BasisCurve();
    occ::handle<Geom2d_Curve> ResCurve;
    if (ConvertCurve2d(BasCurve, ResCurve, IsConvert, First, Last, TolCur, IsOf))
    {
      //      double F = std::max(ResCurve->FirstParameter(),First), L =
      //      std::min(ResCurve->LastParameter(),Last); if(F != Last)
      // C = new
      // Geom2d_TrimmedCurve(ResCurve,Max(First,ResCurve->FirstParameter()),Min(Last,ResCurve->LastParameter()));
      // else
      C = ResCurve;
      return true;
    }
    else
    {
      if (IsConvert)
      {
        C      = occ::down_cast<Geom2d_Curve>(aCurve->Copy());
        TolCur = Precision::PConfusion();
        return true;
      }
      else
        return false;
    }
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_Line)) && myParameters->ConvertCurve2d())
  {
    occ::handle<Geom2d_Line>     aLine2d = occ::down_cast<Geom2d_Line>(aCurve);
    NCollection_Array1<gp_Pnt2d> poles(1, 2);
    poles(1) = aLine2d->Value(First);
    poles(2) = aLine2d->Value(Last);
    NCollection_Array1<double> knots(1, 2);
    knots(1) = First;
    knots(2) = Last;
    NCollection_Array1<int> mults(1, 2);
    mults.Init(2);
    occ::handle<Geom2d_BSplineCurve> res = new Geom2d_BSplineCurve(poles, knots, mults, 1);
    C                                    = res;
    return true;
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_Conic)) && myParameters->ConvertCurve2d())
  {
    occ::handle<Geom2d_BSplineCurve> aBSpline2d;
    // clang-format off
    occ::handle<Geom2d_Curve> tcurve = new Geom2d_TrimmedCurve(aCurve,First,Last); //protection against parabols ets
    // clang-format on
    Geom2dConvert_ApproxCurve approx(tcurve, myTol2d, myContinuity2d, myNbMaxSeg, 6);
    if (approx.HasResult())
      aBSpline2d = approx.Curve();
    else
      aBSpline2d = Geom2dConvert::CurveToBSplineCurve(tcurve, Convert_QuasiAngular);

    double Shift = First - aBSpline2d->FirstParameter();
    if (std::abs(Shift) > Precision::PConfusion())
    {
      NCollection_Array1<double> newKnots(aBSpline2d->Knots());
      for (int i = 1; i <= newKnots.Length(); i++)
        newKnots(i) += Shift;
      aBSpline2d->SetKnots(newKnots);
    }
    occ::handle<Geom2d_Curve> ResCurve;
    if (ConvertCurve2d(aBSpline2d, ResCurve, IsConvert, First, Last, TolCur, false))
    {
      C = ResCurve;
      return true;
    }
    else
    {
      C      = aBSpline2d;
      TolCur = Precision::PConfusion();
      return true;
    }
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)) && myParameters->ConvertCurve2d())
  {
    occ::handle<Geom2d_Curve> aBSpline2d =
      Geom2dConvert::CurveToBSplineCurve(aCurve, Convert_QuasiAngular);
    occ::handle<Geom2d_Curve> ResCurve;
    if (ConvertCurve2d(aBSpline2d, ResCurve, IsConvert, First, Last, TolCur, false))
    {
      C = ResCurve;
      return true;
    }
    else
    {
      C      = aBSpline2d;
      TolCur = Precision::PConfusion();
      return true;
    }
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)) && IsOf)
  {
    occ::handle<Geom2d_OffsetCurve> tmp      = occ::down_cast<Geom2d_OffsetCurve>(aCurve);
    occ::handle<Geom2d_Curve>       BasCurve = tmp->BasisCurve();
    occ::handle<Geom2d_Curve>       ResCurve;
    if (ConvertCurve2d(BasCurve, ResCurve, IsConvert, First, Last, TolCur))
    {
      if (ResCurve->Continuity() != GeomAbs_C0)
      {
        C = new Geom2d_OffsetCurve(ResCurve, tmp->Offset());
        return true;
      }
      else if (ConvertCurve2d(aCurve, ResCurve, IsConvert, First, Last, TolCur, false))
        return true;
      else
      {
        if (IsConvert)
        {
          C      = occ::down_cast<Geom2d_Curve>(aCurve->Copy());
          TolCur = Precision::PConfusion();
          return true;
        }
        else
          return false;
      }
    }
    else
    {
      if (IsConvert)
      {
        C      = occ::down_cast<Geom2d_Curve>(aCurve->Copy());
        TolCur = Precision::PConfusion();
        return true;
      }
      else
        return false;
    }
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))
      || aCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))
      || ((aCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))) && !IsOf))
  {
    int Deg = 1;

    if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
    {
      occ::handle<Geom2d_BSplineCurve> BsC = occ::down_cast<Geom2d_BSplineCurve>(aCurve);
      Deg                                  = BsC->Degree();
      bool IsR                             = (myRational && BsC->IsRational());
      if (!IsR && Deg <= myMaxDegree && (BsC->NbKnots() - 1) <= myNbMaxSeg)
      {
        if (IsConvert)
        {
          C      = occ::down_cast<Geom2d_Curve>(aCurve->Copy());
          TolCur = Precision::PConfusion();
          return true;
        }
        else
          return false;
      }
    }
    if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
    {
      occ::handle<Geom2d_BezierCurve> BzC = occ::down_cast<Geom2d_BezierCurve>(aCurve);
      Deg                                 = BzC->Degree();
      bool IsR                            = (myRational && BzC->IsRational());
      if (!IsR && Deg <= myMaxDegree)
      {
        if (IsConvert)
        {
          C      = occ::down_cast<Geom2d_Curve>(aCurve->Copy());
          TolCur = Precision::PConfusion();
          return true;
        }
        else
          return false;
      }
    }
    occ::handle<Geom2d_Curve> aCurve1;
    double                    pf = aCurve->FirstParameter(), pl = aCurve->LastParameter();
    // 15.11.2002 PTV OCC966
    if (ShapeAnalysis_Curve::IsPeriodic(aCurve) && (First != Last))
      aCurve1 = new Geom2d_TrimmedCurve(aCurve, First, Last);
    else if (aCurve->FirstParameter() < (First - Precision::PConfusion())
             || aCurve->LastParameter() > (Last + Precision::PConfusion()))
    {
      double F = std::max(First, pf), L = std::min(Last, pl);
      if (F != L)
        aCurve1 = new Geom2d_TrimmedCurve(aCurve, F, L);
      else
        aCurve1 = aCurve;
    }
    else
      aCurve1 = aCurve;
    int aC = std::min(ContToInteger(myContinuity2d), ContToInteger(aCurve->Continuity()));
    if (!aC)
      aC = ContToInteger(myContinuity2d);
    // aC = std::min(aC,(Deg -1));
    int aC1 = aC;
    // GeomAbs_Shape aCont =IntegerToGeomAbsShape(aC);
    int MaxSeg = myNbMaxSeg;
    int MaxDeg = myMaxDegree;
    // int GMaxDegree = 15;//Geom2d_BSplineCurve::MaxDegree();
    for (; aC >= 0; aC--)
    {
      try
      {
        OCC_CATCH_SIGNALS
        GeomAbs_Shape aCont = IntegerToGeomAbsShape(aC);
        for (int j = 1; j <= 2; j++)
        {
          Geom2dConvert_ApproxCurve anApprox(aCurve1, myTol2d, aCont, MaxSeg, MaxDeg);
          bool                      Done = anApprox.IsDone();
          C                              = anApprox.Curve();
          int Nbseg                      = occ::down_cast<Geom2d_BSplineCurve>(C)->NbKnots() - 1;
          int DegC                       = occ::down_cast<Geom2d_BSplineCurve>(C)->Degree();

          if (myDeg
              && ((DegC > MaxDeg) || !Done || (anApprox.MaxError() >= std::max(myTol2d, TolCur))))
          {
            if (MaxSeg < myParameters->GMaxSeg())
            {
              MaxSeg = myParameters->GMaxSeg();
              aC     = aC1;
              continue;
            }
            else
            {
#ifdef OCCT_DEBUG
              std::cout << "Curve is not aproxed with continuity  " << aCont << std::endl;
#endif
              if (IsConvert)
              {
                C      = occ::down_cast<Geom2d_Curve>(aCurve->Copy());
                TolCur = Precision::PConfusion();
                return true;
              }
            }
          }

          if (!myDeg
              && ((Nbseg >= MaxSeg) || !Done || (anApprox.MaxError() >= std::max(myTol2d, TolCur))))
          {
            if (MaxDeg < myParameters->GMaxDegree())
            {
              MaxDeg = myParameters->GMaxDegree();
              aC     = aC1;
              continue;
            }
            else
            {
#ifdef OCCT_DEBUG
              std::cout << "Curve is not aproxed with continuity  " << aCont << std::endl;
#endif
              if (IsConvert)
              {
                C      = occ::down_cast<Geom2d_Curve>(aCurve->Copy());
                TolCur = Precision::PConfusion();
                return true;
              }
            }
          }
          myConvert      = true;
          TolCur         = anApprox.MaxError();
          myCurve2dError = std::max(myCurve2dError, anApprox.MaxError());
          return true;
        }
      }
      catch (Standard_Failure const& anException)
      {
#ifdef OCCT_DEBUG
        std::cout << "Warning: Geom2dConvert_ApproxCurve Exception: Wrong Coefficient :Decrease "
                     "Continuity    ";
        anException.Print(std::cout);
        std::cout << std::endl;
#endif
        (void)anException;
        continue;
      }
    }
    return false;
  }
  else
  {
    if (IsConvert)
    {
      C      = occ::down_cast<Geom2d_Curve>(aCurve->Copy());
      TolCur = Precision::PConfusion();
      return true;
    }
    else
      return false;
  }
}

//=================================================================================================

bool ShapeCustom_BSplineRestriction::NewPoint(const TopoDS_Vertex& V, gp_Pnt& P, double& Tol)
{
  Tol = BRep_Tool::Tolerance(V);
  if (myConvert)
  {
    gp_Pnt p1(BRep_Tool::Pnt(V).XYZ());
    P = p1;
    return true;
  }
  else
    return false;
}

//=================================================================================================

bool ShapeCustom_BSplineRestriction::NewParameter(const TopoDS_Vertex& /*V*/,
                                                  const TopoDS_Edge& /*E*/,
                                                  double& /*P*/,
                                                  double& /*Tol*/)
{
  return false;
}

//=================================================================================================

GeomAbs_Shape ShapeCustom_BSplineRestriction::Continuity(const TopoDS_Edge& E,
                                                         const TopoDS_Face& F1,
                                                         const TopoDS_Face& F2,
                                                         const TopoDS_Edge& /*NewE*/,
                                                         const TopoDS_Face& /*NewF1*/,
                                                         const TopoDS_Face& /*NewF2*/)
{
  return BRep_Tool::Continuity(E, F1, F2);
}

//=================================================================================================

double ShapeCustom_BSplineRestriction::MaxErrors(double& aCurve3dErr, double& aCurve2dErr) const
{
  aCurve3dErr = myCurve3dError;
  aCurve2dErr = myCurve2dError;
  return mySurfaceError;
}

//=================================================================================================

int ShapeCustom_BSplineRestriction::NbOfSpan() const
{
  return myNbOfSpan;
}
