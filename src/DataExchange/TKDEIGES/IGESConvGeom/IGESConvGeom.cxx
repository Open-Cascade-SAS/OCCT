// Created on: 1994-09-01
// Created by: Christian CAILLET
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

#include <IGESConvGeom.hxx>

#include <BSplCLib.hxx>
#include <BSplSLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Trsf.hxx>
#include <IGESData_ToolLocation.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESGeom_SplineSurface.hxx>
#include <PLib.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

int IGESConvGeom::SplineCurveFromIGES(const occ::handle<IGESGeom_SplineCurve>& st,
                                                   const double /*epscoef*/,
                                                   const double        epsgeom,
                                                   occ::handle<Geom_BSplineCurve>& res)
{
  int returned = 0;

  // we retrieve the degree
  int degree = st->SplineType();
  if (degree > 3)
    degree = 3;

  // we retrieve the number of segments.
  int nbSegs = st->NbSegments();
  if (nbSegs < 1)
    return 5; // FAIL : no segment

  int nbKnots = nbSegs + 1;

  // Array of multiplicities.
  NCollection_Array1<int> multi(1, nbKnots);
  multi.Init(degree);
  multi.SetValue(multi.Lower(), degree + 1);
  multi.SetValue(multi.Upper(), degree + 1);

  // Array of knots.
  NCollection_Array1<double> knots(1, nbKnots);
  NCollection_Array1<double> delta(1, nbSegs);
  int     i; // svv Jan 10 2000 : porting on DEC
  for (i = 1; i <= nbKnots; i++)
    knots.SetValue(i, st->BreakPoint(i));

  for (i = 1; i <= nbSegs; i++)
    delta.SetValue(i, st->BreakPoint(i + 1) - st->BreakPoint(i));

  NCollection_Array1<gp_Pnt> bspoles(1, nbSegs * degree + 1);
  int   ibspole = bspoles.Lower() - 1; // Bspole Index.
  // we need to reparameterize before passing to PLib.
  // we are between [0, T(i+1)-T(i)] and we want [0,1]

  for (i = 1; i <= nbSegs; i++)
  {
    double AX, BX, CX, DX, AY, BY, CY, DY, AZ, BZ, CZ, DZ;
    st->XCoordPolynomial(i, AX, BX, CX, DX);
    st->YCoordPolynomial(i, AY, BY, CY, DY);
    st->ZCoordPolynomial(i, AZ, BZ, CZ, DZ);
    if (st->NbDimensions() == 2)
      BZ = 0., CZ = 0., DZ = 0.;
    double Di  = delta(i);
    double Di2 = delta(i) * delta(i);
    double Di3 = delta(i) * delta(i) * delta(i);

    NCollection_Array1<gp_Pnt> coeff(0, degree);
    switch (degree)
    {
      case 3:
        coeff.SetValue(coeff.Lower() + 3, gp_Pnt(DX * Di3, DY * Di3, DZ * Di3));
        [[fallthrough]];
      case 2:
        coeff.SetValue(coeff.Lower() + 2, gp_Pnt(CX * Di2, CY * Di2, CZ * Di2));
        [[fallthrough]];
      case 1:
        coeff.SetValue(coeff.Lower() + 1, gp_Pnt(BX * Di, BY * Di, BZ * Di));
        coeff.SetValue(coeff.Lower() + 0, gp_Pnt(AX, AY, AZ));
        break;
      default:
        break;
    }

    NCollection_Array1<gp_Pnt> bzpoles(0, degree);
    PLib::CoefficientsPoles(coeff, PLib::NoWeights(), bzpoles, PLib::NoWeights());

    // C0 test.
    // Not to check the first pole of the first segment.
    if (ibspole > bspoles.Lower())
    {
      int bzlow = bzpoles.Lower();
      if (!(bspoles.Value(ibspole).IsEqual(bzpoles.Value(bzlow), epsgeom)))
      {
        returned = 1;
        // Medium point computing.
        bspoles.SetValue(ibspole,
                         gp_Pnt((bspoles.Value(ibspole).X() + bzpoles.Value(bzlow).X()) / 2.,
                                (bspoles.Value(ibspole).Y() + bzpoles.Value(bzlow).Y()) / 2.,
                                (bspoles.Value(ibspole).Z() + bzpoles.Value(bzlow).Z()) / 2.));
      }
    }
    if (i == 1)
      bspoles.SetValue(++ibspole, bzpoles.Value(bzpoles.Lower()));

    for (int j = bzpoles.Lower() + 1; j <= bzpoles.Upper(); j++)
      bspoles.SetValue(++ibspole, bzpoles.Value(j));
  }
  if (ibspole != bspoles.Upper())
  {
    // Just to be sure.
    return 3; // FAIL : Error during creation of control points
  }

  //  Building result taking into account transformation if any :
  //  ===========================================================

  //%13 pdn 12.02.99 USA60293
  //  if (st->HasTransf()) {
  //    gp_Trsf trsf;
  //    double epsilon = 1.E-04;
  //    if (IGESData_ToolLocation::ConvertLocation
  //	(epsilon,st->CompoundLocation(),trsf)) {
  //      for (int i = bspoles.Lower(); i <= bspoles.Upper(); i++)
  //	bspoles.SetValue(i, bspoles.Value(i).Transformed(trsf));
  //    }
  //    else
  //      AddFail(st, "Transformation : not a similarity");
  //  }
  res = new Geom_BSplineCurve(bspoles, knots, multi, degree);
  //  GeomConvert_CompCurveToBSplineCurve CompCurve =
  //    GeomConvert_CompCurveToBSplineCurve(res);
  //  res = CompCurve.BSplineCurve();
  return returned;
}

//=================================================================================================

int IGESConvGeom::IncreaseCurveContinuity(const occ::handle<Geom_BSplineCurve>& res,
                                                       const double              epsgeom,
                                                       const int           continuity)
{
  if (continuity < 1)
    return continuity;
  bool isC1 = true, isC2 = true;
  int degree = res->Degree();

  bool isModified;
  do
  {
    isModified = false;
    for (int i = res->FirstUKnotIndex() + 1; i < res->LastUKnotIndex(); i++)
      if (degree - res->Multiplicity(i) < continuity)
      {
        if (continuity >= 2)
        {
          if (!res->RemoveKnot(i, degree - 2, epsgeom))
          {
            isC2                   = false;
            bool locOK = res->RemoveKnot(i, degree - 1, epsgeom); // is C1 ?
            isC1 &= locOK;
            isModified |= locOK;
          }
          else
            isModified = true;
        }
        else
        {
          bool locOK = res->RemoveKnot(i, degree - 1, epsgeom); // is C1 ?
          isC1 &= locOK;
          isModified |= locOK;
        }
      }
  } while (isModified);

  if (!isC1)
    return 0;
  if (continuity >= 2 && !isC2)
    return 1;
  return continuity;
}

//=================================================================================================

int IGESConvGeom::IncreaseCurveContinuity(const occ::handle<Geom2d_BSplineCurve>& res,
                                                       const double                epsgeom,
                                                       const int continuity)
{
  if (continuity < 1)
    return continuity;
  bool isC1 = true, isC2 = true;
  int degree = res->Degree();

  bool isModified;
  do
  {
    isModified = false;
    for (int i = res->FirstUKnotIndex() + 1; i < res->LastUKnotIndex(); i++)
      if (degree - res->Multiplicity(i) < continuity)
      {
        if (continuity >= 2)
        {
          if (!res->RemoveKnot(i, degree - 2, epsgeom))
          {
            isC2                   = false;
            bool locOK = res->RemoveKnot(i, degree - 1, epsgeom); // is C1 ?
            isC1 &= locOK;
            isModified |= locOK;
          }
          else
            isModified = true;
        }
        else
        {
          bool locOK = res->RemoveKnot(i, degree - 1, epsgeom); // is C1 ?
          isC1 &= locOK;
          isModified |= locOK;
        }
      }
  } while (isModified);

  if (!isC1)
    return 0;
  if (continuity >= 2 && !isC2)
    return 1;
  return continuity;
}

//=================================================================================================

int IGESConvGeom::SplineSurfaceFromIGES(const occ::handle<IGESGeom_SplineSurface>& st,
                                                     const double /*epscoef*/,
                                                     const double          epsgeom,
                                                     occ::handle<Geom_BSplineSurface>& res)
{
  int returned = 0;
  int degree   = st->BoundaryType();
  if (degree > 3)
    degree = 3;
  int DegreeU = degree;
  int DegreeV = degree;

  int NbUSeg = st->NbUSegments();
  int NbVSeg = st->NbVSegments();

  if ((NbUSeg < 1) || (NbVSeg < 1))
    return 5;

  //  Output BSpline knots & multiplicities arrays for U & V :
  //  =========================================================

  NCollection_Array1<double> UKnot(1, NbUSeg + 1);
  NCollection_Array1<double> VKnot(1, NbVSeg + 1);
  NCollection_Array1<double> deltaU(1, NbUSeg);
  NCollection_Array1<double> deltaV(1, NbVSeg);

  int i; // svv Jan 10 2000 : porting on DEC
  for (i = 1; i <= NbUSeg + 1; i++)
    UKnot.SetValue(i, st->UBreakPoint(i));

  for (i = 1; i <= NbUSeg; i++)
    deltaU.SetValue(i, st->UBreakPoint(i + 1) - st->UBreakPoint(i));

  for (i = 1; i <= NbVSeg + 1; i++)
    VKnot.SetValue(i, st->VBreakPoint(i));

  for (i = 1; i <= NbVSeg; i++)
    deltaV.SetValue(i, st->VBreakPoint(i + 1) - st->VBreakPoint(i));

  NCollection_Array1<int> UMult(1, NbUSeg + 1);
  UMult.Init(DegreeU);
  UMult.SetValue(UMult.Lower(), DegreeU + 1);
  UMult.SetValue(UMult.Upper(), DegreeU + 1);

  NCollection_Array1<int> VMult(1, NbVSeg + 1);
  VMult.Init(DegreeV);
  VMult.SetValue(VMult.Lower(), DegreeV + 1);
  VMult.SetValue(VMult.Upper(), DegreeV + 1);

  //  Poles computing
  //  ===============

  int NbUPoles = NbUSeg * DegreeU + 1;
  int NbVPoles = NbVSeg * DegreeV + 1;

  NCollection_Array2<gp_Pnt> BsPole(1, NbUPoles, 1, NbVPoles);

  int iBs, jBs, iBz, jBz;
  bool wasC0 = true;

  //  Patch (1,1)
  //  ===========
  int USeg, VSeg, j;
  USeg = 1;
  VSeg = 1;

  occ::handle<NCollection_HArray1<double>> XPoly = st->XPolynomial(USeg, VSeg);
  occ::handle<NCollection_HArray1<double>> YPoly = st->YPolynomial(USeg, VSeg);
  occ::handle<NCollection_HArray1<double>> ZPoly = st->ZPolynomial(USeg, VSeg);

  NCollection_Array2<gp_Pnt> Coef(1, DegreeU + 1, 1, DegreeV + 1);
  double      ParamU, ParamV;
  ParamU = 1.;
  for (i = 1; i <= DegreeU + 1; i++)
  {
    ParamV = 1.;
    for (j = 1; j <= DegreeV + 1; j++)
    {
      int PolyIndex = i + 4 * (j - 1);
      gp_Pnt           aPoint(XPoly->Value(PolyIndex) * ParamU * ParamV,
                    YPoly->Value(PolyIndex) * ParamU * ParamV,
                    ZPoly->Value(PolyIndex) * ParamU * ParamV);
      Coef.SetValue(i, j, aPoint);
      ParamV = ParamV * deltaV(VSeg);
    }
    ParamU = ParamU * deltaU(USeg);
  }
  NCollection_Array2<gp_Pnt> BzPole(1, DegreeU + 1, 1, DegreeV + 1);
  PLib::CoefficientsPoles(Coef, PLib::NoWeights2(), BzPole, PLib::NoWeights2());

  iBs = BsPole.LowerRow();
  jBs = BsPole.LowerCol();

  //  Making output BSpline poles array :
  for (iBz = BzPole.LowerRow(); iBz <= BzPole.UpperRow(); iBz++)
  {
    for (jBz = BzPole.LowerCol(); jBz <= BzPole.UpperCol(); jBz++)
      BsPole.SetValue(iBs, jBs++, BzPole.Value(iBz, jBz));
    jBs = BsPole.LowerCol();
    iBs++;
  }

  //  Patches (1<USeg<NbUSeg, 1)
  //  ==========================

  VSeg = 1;
  for (USeg = 2; USeg <= NbUSeg; USeg++)
  {
    XPoly  = st->XPolynomial(USeg, VSeg);
    YPoly  = st->YPolynomial(USeg, VSeg);
    ZPoly  = st->ZPolynomial(USeg, VSeg);
    ParamU = 1.;
    for (i = Coef.LowerRow(); i <= Coef.UpperRow(); i++)
    {
      ParamV = 1.;
      for (j = Coef.LowerCol(); j <= Coef.UpperCol(); j++)
      {
        int PolyIndex = i + 4 * (j - 1);
        gp_Pnt           aPoint;
        aPoint.SetCoord(XPoly->Value(PolyIndex) * ParamU * ParamV,
                        YPoly->Value(PolyIndex) * ParamU * ParamV,
                        ZPoly->Value(PolyIndex) * ParamU * ParamV);
        Coef.SetValue(i, j, aPoint);
        ParamV = ParamV * deltaV(VSeg);
      }
      ParamU = ParamU * deltaU(USeg);
    }
    PLib::CoefficientsPoles(Coef, PLib::NoWeights2(), BzPole, PLib::NoWeights2());

    //  C0 check and correction for poles lying on isoparametrics U=0 & V=0
    int iBsPole = BsPole.LowerRow() + (USeg - 1) * DegreeU;
    int jBsPole = BsPole.LowerCol();
    iBz                      = BzPole.LowerRow();
    for (jBz = BzPole.LowerCol(); jBz <= BzPole.UpperCol(); jBz++)
    {
      if (!BzPole.Value(iBz, jBz).IsEqual(BsPole.Value(iBsPole, jBsPole), epsgeom))
      {
        wasC0 = false;
        gp_Pnt        MidPoint;
        double XCoord =
          0.5 * (BzPole.Value(iBz, jBz).X() + BsPole.Value(iBsPole, jBsPole).X());
        double YCoord =
          0.5 * (BzPole.Value(iBz, jBz).Y() + BsPole.Value(iBsPole, jBsPole).Y());
        double ZCoord =
          0.5 * (BzPole.Value(iBz, jBz).Z() + BsPole.Value(iBsPole, jBsPole).Z());
        MidPoint.SetCoord(XCoord, YCoord, ZCoord);
        BsPole.SetValue(iBsPole, jBsPole++, MidPoint);
      }
      else
      {
        BsPole.SetValue(iBsPole, jBsPole++, BzPole.Value(iBz, jBz));
      }
    }

    //  Other poles (no check about C0) :
    iBsPole++;
    jBsPole = BsPole.LowerCol();
    for (iBz = BzPole.LowerRow() + 1; iBz <= BzPole.UpperRow(); iBz++)
    {
      for (jBz = BzPole.LowerCol(); jBz <= BzPole.UpperCol(); jBz++)
        BsPole.SetValue(iBsPole, jBsPole++, BzPole.Value(iBz, jBz));
      iBsPole++;
      jBsPole = BsPole.LowerCol();
    }
  }

  //  Patches (1, 1<VSeg<NbVSeg)
  //  ==========================

  USeg = 1;
  for (VSeg = 2; VSeg <= NbVSeg; VSeg++)
  {
    XPoly  = st->XPolynomial(USeg, VSeg);
    YPoly  = st->YPolynomial(USeg, VSeg);
    ZPoly  = st->ZPolynomial(USeg, VSeg);
    ParamU = 1.;
    for (i = Coef.LowerRow(); i <= Coef.UpperRow(); i++)
    {
      ParamV = 1.;
      for (j = Coef.LowerCol(); j <= Coef.UpperCol(); j++)
      {
        int PolyIndex = i + 4 * (j - 1);
        gp_Pnt           aPoint;
        aPoint.SetCoord(XPoly->Value(PolyIndex) * ParamU * ParamV,
                        YPoly->Value(PolyIndex) * ParamU * ParamV,
                        ZPoly->Value(PolyIndex) * ParamU * ParamV);
        Coef.SetValue(i, j, aPoint);
        ParamV = ParamV * deltaV(VSeg);
      }
      ParamU = ParamU * deltaU(USeg);
    }
    PLib::CoefficientsPoles(Coef, PLib::NoWeights2(), BzPole, PLib::NoWeights2());

    //  C0 check and correction for poles lying on isoparametrics U=0 & V=0
    iBs = BsPole.LowerRow();
    jBs = BsPole.LowerCol() + (VSeg - 1) * DegreeV;
    jBz = BzPole.LowerCol();
    for (iBz = BzPole.LowerRow(); iBz <= BzPole.UpperRow(); iBz++)
    {
      if (!BzPole.Value(iBz, jBz).IsEqual(BsPole.Value(iBs, jBs), epsgeom))
      {
        wasC0 = false;
        gp_Pnt        MidPoint;
        double XCoord = 0.5 * (BzPole.Value(iBz, jBz).X() + BsPole.Value(iBs, jBs).X());
        double YCoord = 0.5 * (BzPole.Value(iBz, jBz).Y() + BsPole.Value(iBs, jBs).Y());
        double ZCoord = 0.5 * (BzPole.Value(iBz, jBz).Z() + BsPole.Value(iBs, jBs).Z());
        MidPoint.SetCoord(XCoord, YCoord, ZCoord);
        BsPole.SetValue(iBs++, jBs, MidPoint);
      }
      else
      {
        BsPole.SetValue(iBs++, jBs, BzPole.Value(iBz, jBz));
      }
    }

    jBs++;
    iBs = BsPole.LowerRow();
    for (jBz = BzPole.LowerCol() + 1; jBz <= BzPole.UpperCol(); jBz++)
    {
      for (iBz = BzPole.LowerRow(); iBz <= BzPole.UpperRow(); iBz++)
        BsPole.SetValue(iBs++, jBs, BzPole.Value(iBz, jBz));
      iBs = BsPole.LowerRow();
      jBs++;
    }
  }

  //  Patches (1<USeg<NbUSeg, 1<VSeg<NbVSeg)
  //  ======================================

  for (VSeg = 2; VSeg <= NbVSeg; VSeg++)
  {
    for (USeg = 2; USeg <= NbUSeg; USeg++)
    {
      XPoly  = st->XPolynomial(USeg, VSeg);
      YPoly  = st->YPolynomial(USeg, VSeg);
      ZPoly  = st->ZPolynomial(USeg, VSeg);
      ParamU = 1.;
      for (i = Coef.LowerRow(); i <= Coef.UpperRow(); i++)
      {
        ParamV = 1.;
        for (j = Coef.LowerCol(); j <= Coef.UpperCol(); j++)
        {
          int PolyIndex = i + 4 * (j - 1);
          gp_Pnt           aPoint;
          aPoint.SetCoord(XPoly->Value(PolyIndex) * ParamU * ParamV,
                          YPoly->Value(PolyIndex) * ParamU * ParamV,
                          ZPoly->Value(PolyIndex) * ParamU * ParamV);
          Coef.SetValue(i, j, aPoint);
          ParamV = ParamV * deltaV(VSeg);
        }
        ParamU = ParamU * deltaU(USeg);
      }
      PLib::CoefficientsPoles(Coef, PLib::NoWeights2(), BzPole, PLib::NoWeights2());

      //  C0 check and correction for poles lying on isoparametrics U=0 & V=0
      iBs = (USeg - 1) * DegreeU + BsPole.LowerRow();
      jBs = (VSeg - 1) * DegreeV + BsPole.LowerCol();
      jBz = BzPole.LowerCol();
      for (iBz = BzPole.LowerRow(); iBz <= BzPole.UpperRow(); iBz++)
      {
        if (!BzPole.Value(iBz, jBz).IsEqual(BsPole.Value(iBs, jBs), epsgeom))
        {
          wasC0 = false;
          gp_Pnt        MidPoint;
          double XCoord = 0.5 * (BzPole.Value(iBz, jBz).X() + BsPole.Value(iBs, jBs).X());
          double YCoord = 0.5 * (BzPole.Value(iBz, jBz).Y() + BsPole.Value(iBs, jBs).Y());
          double ZCoord = 0.5 * (BzPole.Value(iBz, jBz).Z() + BsPole.Value(iBs, jBs).Z());
          MidPoint.SetCoord(XCoord, YCoord, ZCoord);
          BsPole.SetValue(iBs++, jBs, MidPoint);
        }
        else
          BsPole.SetValue(iBs++, jBs, BzPole.Value(iBz, jBz));
      }

      iBs = (USeg - 1) * DegreeU + BsPole.LowerRow();
      iBz = BzPole.LowerRow();
      for (jBz = BzPole.LowerCol(); jBz <= BzPole.UpperCol(); jBz++)
      {
        //  C0 check and correction for poles lying on isoparametrics U=0 & V=0
        if (!BzPole.Value(iBz, jBz).IsEqual(BsPole.Value(iBs, jBs), epsgeom))
        {
          wasC0 = false;
          gp_Pnt        MidPoint;
          double XCoord = 0.5 * (BzPole.Value(iBz, jBz).X() + BsPole.Value(iBs, jBs).X());
          double YCoord = 0.5 * (BzPole.Value(iBz, jBz).Y() + BsPole.Value(iBs, jBs).Y());
          double ZCoord = 0.5 * (BzPole.Value(iBz, jBz).Z() + BsPole.Value(iBs, jBs).Z());
          MidPoint.SetCoord(XCoord, YCoord, ZCoord);
          BsPole.SetValue(iBs, jBs++, MidPoint);
        }
        else
          BsPole.SetValue(iBs, jBs++, BzPole.Value(iBz, jBz));
      }

      iBs = BsPole.LowerRow() + (USeg - 1) * DegreeU + 1;
      jBs = BsPole.LowerCol() + (VSeg - 1) * DegreeV + 1;
      for (iBz = BzPole.LowerRow() + 1; iBz <= BzPole.UpperRow(); iBz++)
      {
        for (jBz = BzPole.LowerCol() + 1; jBz <= BzPole.UpperCol(); jBz++)
          BsPole.SetValue(iBs, jBs++, BzPole.Value(iBz, jBz));
        jBs = BsPole.LowerCol() + (VSeg - 1) * DegreeV + 1;
        iBs++;
      }
    }
  }

  //  Building result taking into account transformation if any :
  //  ===========================================================

  if (st->HasTransf())
  {
    gp_GTrsf      GSplTrsf(st->CompoundLocation());
    gp_Trsf       SplTrsf;
    double epsilon = 1.E-04;
    if (IGESData_ToolLocation::ConvertLocation(epsilon, GSplTrsf, SplTrsf))
      for (iBs = BsPole.LowerRow(); iBs <= BsPole.UpperRow(); iBs++)
        for (jBs = BsPole.LowerCol(); jBs <= BsPole.UpperCol(); jBs++)
          BsPole.SetValue(iBs, jBs, BsPole.Value(iBs, jBs).Transformed(SplTrsf));
    //    else
    //      AddWarning(start, "Transformation skipped : Not a similarity");
  }

  res = new Geom_BSplineSurface(BsPole, UKnot, VKnot, UMult, VMult, DegreeU, DegreeV);
  if (wasC0)
    returned += 1;
  return returned;
}

//=================================================================================================

int IGESConvGeom::IncreaseSurfaceContinuity(const occ::handle<Geom_BSplineSurface>& res,
                                                         const double                epsgeom,
                                                         const int continuity)
{
  if (continuity < 1)
    return continuity;
  bool isC1 = true, isC2 = true;
  int DegreeU = res->UDegree();

  bool isModified;
  do
  {
    isModified = false;
    for (int i = res->FirstUKnotIndex() + 1; i < res->LastUKnotIndex(); i++)
      if (DegreeU - res->UMultiplicity(i) < continuity)
      {
        if (continuity >= 2)
        {
          if (!res->RemoveUKnot(i, DegreeU - 2, epsgeom))
          {
            isC2                   = false;
            bool locOK = res->RemoveUKnot(i, DegreeU - 1, epsgeom); // is C1 ?
            isC1 &= locOK;
            isModified |= locOK;
          }
          else
            isModified = true;
        }
        else
        {
          bool locOK = res->RemoveUKnot(i, DegreeU - 1, epsgeom); // is C1 ?
          isC1 &= locOK;
          isModified |= locOK;
        }
      }
  } while (isModified);

  int DegreeV = res->VDegree();
  do
  {
    isModified = false;
    for (int i = res->FirstVKnotIndex() + 1; i < res->LastVKnotIndex(); i++)
      if (DegreeV - res->VMultiplicity(i) < continuity)
      {
        if (continuity >= 2)
        {
          if (!res->RemoveVKnot(i, DegreeV - 2, epsgeom))
          {
            isC2                   = false;
            bool locOK = res->RemoveVKnot(i, DegreeV - 1, epsgeom); // is C1 ?
            isC1 &= locOK;
            isModified |= locOK;
          }
          else
            isModified = true;
        }
        else
        {
          bool locOK = res->RemoveVKnot(i, DegreeV - 1, epsgeom); // is C1 ?
          isC1 &= locOK;
          isModified |= locOK;
        }
      }
  } while (isModified);

  /*
  while (--i > j) {                                      // from 2 to NbKnots-1
    if (continuity >= 2) {
      if (!res->RemoveUKnot(i, DegreeU-2, epsgeom)) {    // is C2 ?
    isC2 = false;
    isC1 &= res->RemoveUKnot(i, DegreeU-1, epsgeom); // is C1 ?
      }
    }
    else {
      isC1 &= res->RemoveUKnot(i, DegreeU-1, epsgeom); // is C1 ?
    }
  }

  i = res->LastVKnotIndex();   //knots.Upper();
  j = res->FirstVKnotIndex();  //knots.Lower();
  int DegreeV = res->VDegree();
  while (--i > j) {                                      // from 2 to NbKnots-1
    if (continuity >= 2) {
      if (!res->RemoveVKnot(i, DegreeV-2, epsgeom)) {    // is C2 ?
    isC2 = false;
    isC1 &= res->RemoveVKnot(i, DegreeV-1, epsgeom); // is C1 ?
      }
    }
    else {
      isC1 &= res->RemoveVKnot(i, DegreeV-1, epsgeom); // is C1 ?
    }
  }*/

  if (!isC1)
    return 0;
  if (continuity >= 2 && !isC2)
    return 1;
  return continuity;
}
