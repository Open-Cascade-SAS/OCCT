// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2024 OPEN CASCADE SAS
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

#include <IntCurveSurface_TheHCurveTool.hxx>

#include <Adaptor3d_Curve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAbs_Shape.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <gce_MakeLin.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#define myMinPnts 5

Standard_Integer IntCurveSurface_TheHCurveTool::NbSamples(const Handle(Adaptor3d_Curve)& C,
                                                          const Standard_Real            U0,
                                                          const Standard_Real            U1)
{
  GeomAbs_CurveType   typC     = C->GetType();
  const Standard_Real nbsOther = 10.0;
  Standard_Real       nbs      = nbsOther;

  if (typC == GeomAbs_Line)
    nbs = 2;
  else if (typC == GeomAbs_BezierCurve)
    nbs = 3 + C->NbPoles();
  else if (typC == GeomAbs_BSplineCurve)
  {
    nbs = C->NbKnots();
    nbs *= C->Degree();
    nbs *= C->LastParameter() - C->FirstParameter();
    nbs /= U1 - U0;
    if (nbs < 2.0)
      nbs = 2;
  }
  if (nbs > 50)
    nbs = 50;
  return ((Standard_Integer)nbs);
}

void IntCurveSurface_TheHCurveTool::SamplePars(const Handle(Adaptor3d_Curve)& C,
                                               const Standard_Real            U0,
                                               const Standard_Real            U1,
                                               const Standard_Real            Defl,
                                               const Standard_Integer         NbMin,
                                               Handle(TColStd_HArray1OfReal)& Pars)
{
  GeomAbs_CurveType   typC     = C->GetType();
  const Standard_Real nbsOther = 10.0;
  Standard_Real       nbs      = nbsOther;

  if (typC == GeomAbs_Line)
    nbs = 2;
  else if (typC == GeomAbs_BezierCurve)
  {
    nbs = 3 + C->NbPoles();
  }

  if (typC != GeomAbs_BSplineCurve)
  {
    if (nbs > 50)
      nbs = 50;
    Standard_Integer nnbs = (Standard_Integer)nbs;

    Pars             = new TColStd_HArray1OfReal(1, nnbs);
    Standard_Real du = (U1 - U0) / (nnbs - 1);

    Pars->SetValue(1, U0);
    Pars->SetValue(nnbs, U1);
    Standard_Integer i;
    Standard_Real    u;
    for (i = 2, u = U0 + du; i < nnbs; ++i, u += du)
    {
      Pars->SetValue(i, u);
    }
    return;
  }

  const Handle(Geom_BSplineCurve)& aBC = C->BSpline();

  Standard_Integer i, j, k, nbi;
  Standard_Real    t1, t2, dt;
  Standard_Integer ui1 = aBC->FirstUKnotIndex();
  Standard_Integer ui2 = aBC->LastUKnotIndex();

  for (i = ui1; i < ui2; ++i)
  {
    if (U0 >= aBC->Knot(i) && U0 < aBC->Knot(i + 1))
    {
      ui1 = i;
      break;
    }
  }

  for (i = ui2; i > ui1; --i)
  {
    if (U1 <= aBC->Knot(i) && U1 > aBC->Knot(i - 1))
    {
      ui2 = i;
      break;
    }
  }

  Standard_Integer nbsu = ui2 - ui1 + 1;
  nbsu += (nbsu - 1) * (aBC->Degree() - 1);
  Standard_Boolean bUniform = Standard_False;
  if (nbsu < NbMin)
  {
    nbsu     = NbMin;
    bUniform = Standard_True;
  }

  TColStd_Array1OfReal    aPars(1, nbsu);
  TColStd_Array1OfBoolean aFlg(1, nbsu);
  // Filling of sample parameters
  if (bUniform)
  {
    t1          = U0;
    t2          = U1;
    dt          = (t2 - t1) / (nbsu - 1);
    aPars(1)    = t1;
    aFlg(1)     = Standard_False;
    aPars(nbsu) = t2;
    aFlg(nbsu)  = Standard_False;
    for (i = 2, t1 += dt; i < nbsu; ++i, t1 += dt)
    {
      aPars(i) = t1;
      aFlg(i)  = Standard_False;
    }
  }
  else
  {
    nbi = aBC->Degree();
    k   = 0;
    t1  = U0;
    for (i = ui1 + 1; i <= ui2; ++i)
    {
      if (i == ui2)
        t2 = U1;
      else
        t2 = aBC->Knot(i);
      dt = (t2 - t1) / nbi;
      j  = 1;
      do
      {
        ++k;
        aPars(k) = t1;
        aFlg(k)  = Standard_False;
        t1 += dt;
      } while (++j <= nbi);
      t1 = t2;
    }
    ++k;
    aPars(k) = t1;
  }
  // Analysis of deflection

  Standard_Real    aDefl2 = Max(Defl * Defl, 1.e-9);
  Standard_Real    tol    = Max(0.01 * aDefl2, 1.e-9);
  Standard_Integer l;

  Standard_Integer NbSamples = 2;
  aFlg(1)                    = Standard_True;
  aFlg(nbsu)                 = Standard_True;
  j                          = 1;
  Standard_Boolean bCont     = Standard_True;
  while (j < nbsu - 1 && bCont)
  {
    if (aFlg(j + 1))
    {
      ++j;
      continue;
    }

    t2        = aPars(j);
    gp_Pnt p1 = aBC->Value(t2);
    for (k = j + 2; k <= nbsu; ++k)
    {
      t2        = aPars(k);
      gp_Pnt p2 = aBC->Value(t2);

      if (p1.SquareDistance(p2) <= tol)
        continue;

      gce_MakeLin      MkLin(p1, p2);
      const gp_Lin&    lin = MkLin.Value();
      Standard_Boolean ok  = Standard_True;
      for (l = j + 1; l < k; ++l)
      {
        if (aFlg(l))
        {
          ok = Standard_False;
          break;
        }

        gp_Pnt        pp = aBC->Value(aPars(l));
        Standard_Real d  = lin.SquareDistance(pp);

        if (d <= aDefl2)
          continue;

        ok = Standard_False;
        break;
      }

      if (!ok)
      {
        j       = k - 1;
        aFlg(j) = Standard_True;
        ++NbSamples;
        break;
      }

      if (aFlg(k))
      {
        j = k;
        break;
      }
    }

    if (k >= nbsu)
      bCont = Standard_False;
  }

  if (NbSamples < myMinPnts)
  {
    // uniform distribution
    NbSamples = myMinPnts;
    Pars      = new TColStd_HArray1OfReal(1, NbSamples);
    t1        = U0;
    t2        = U1;
    dt        = (t2 - t1) / (NbSamples - 1);
    Pars->SetValue(1, t1);
    Pars->SetValue(NbSamples, t2);
    for (i = 2, t1 += dt; i < NbSamples; ++i, t1 += dt)
    {
      Pars->SetValue(i, t1);
    }
    return;
  }

  Pars = new TColStd_HArray1OfReal(1, NbSamples);
  j    = 0;
  for (i = 1; i <= nbsu; ++i)
  {
    if (aFlg(i))
    {
      ++j;
      Pars->SetValue(j, aPars(i));
    }
  }
}
