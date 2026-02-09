// Created on: 1995-10-04
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

// 09/09/1996 PMN Ajout des methodes GetCircle et GetTolerance.
// 30/12/1996 PMN Ajout de GetMinimalWeight
// 23/09/1997 PMN Supprimme GetCircle et GetTol (passe dans GeomFill)

#include <BlendFunc.hxx>

#include <Adaptor3d_Surface.hxx>
#include <CSLib.hxx>
#include <CSLib_NormalStatus.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <NCollection_Array2.hxx>

//=================================================================================================

void BlendFunc::GetShape(const BlendFunc_SectionShape  SShape,
                         const double                  MaxAng,
                         int&                          NbPoles,
                         int&                          NbKnots,
                         int&                          Degree,
                         Convert_ParameterisationType& TConv)
{
  switch (SShape)
  {
    case BlendFunc_Rational: {
      int NbSpan = (int)(std::ceil(3. * std::abs(MaxAng) / 2. / M_PI));
      NbPoles    = 2 * NbSpan + 1;
      NbKnots    = NbSpan + 1;
      Degree     = 2;
      if (NbSpan == 1)
      {
        TConv = Convert_TgtThetaOver2_1;
      }
      else
      { // QuasiAngular affin d'etre C1 (et meme beaucoup plus)
        NbPoles = 7;
        NbKnots = 2;
        Degree  = 6;
        TConv   = Convert_QuasiAngular;
      }
    }
    break;
    case BlendFunc_QuasiAngular: {
      NbPoles = 7;
      NbKnots = 2;
      Degree  = 6;
      TConv   = Convert_QuasiAngular;
    }
    break;
    case BlendFunc_Polynomial: {
      NbPoles = 8;
      NbKnots = 2;
      Degree  = 7;
      TConv   = Convert_Polynomial;
    }
    break;
    case BlendFunc_Linear: {
      NbPoles = 2;
      NbKnots = 2;
      Degree  = 1;
    }
    break;
  }
}

//=======================================================================
// function : GetMinimalWeights
// purpose  : On suppose les extremum de poids sont obtenus pour les
//           extremums d'angles (A verifier eventuelement pour Quasi-Angular)
//=======================================================================

void BlendFunc::GetMinimalWeights(const BlendFunc_SectionShape       SShape,
                                  const Convert_ParameterisationType TConv,
                                  const double                       MinAng,
                                  const double                       MaxAng,
                                  NCollection_Array1<double>&        Weights)

{
  switch (SShape)
  {
    case BlendFunc_Polynomial:
    case BlendFunc_Linear: {
      Weights.Init(1);
    }
    break;
    case BlendFunc_Rational:
    case BlendFunc_QuasiAngular: {
      gp_Ax2                         popAx2(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z));
      gp_Circ                        C(popAx2, 1);
      occ::handle<Geom_TrimmedCurve> Sect1 = new Geom_TrimmedCurve(new Geom_Circle(C), 0., MaxAng);
      occ::handle<Geom_BSplineCurve> CtoBspl   = GeomConvert::CurveToBSplineCurve(Sect1, TConv);
      const NCollection_Array1<double>* aWPtr1 = CtoBspl->Weights();
      if (aWPtr1 != nullptr)
        for (int ii = Weights.Lower(); ii <= Weights.Upper(); ii++)
          Weights(ii) = (*aWPtr1)(ii);

      NCollection_Array1<double> poids(Weights.Lower(), Weights.Upper());
      double                     angle_min = std::max(Precision::PConfusion(), MinAng);

      occ::handle<Geom_TrimmedCurve> Sect2 =
        new Geom_TrimmedCurve(new Geom_Circle(C), 0., angle_min);
      CtoBspl                                  = GeomConvert::CurveToBSplineCurve(Sect2, TConv);
      const NCollection_Array1<double>* aWPtr2 = CtoBspl->Weights();
      if (aWPtr2 != nullptr)
        for (int ii = poids.Lower(); ii <= poids.Upper(); ii++)
          poids(ii) = (*aWPtr2)(ii);

      for (int ii = Weights.Lower(); ii <= Weights.Upper(); ii++)
      {
        if (poids(ii) < Weights(ii))
        {
          Weights(ii) = poids(ii);
        }
      }
    }
    break;
  }
}

//=================================================================================================

GeomAbs_Shape BlendFunc::NextShape(const GeomAbs_Shape S)
{
  switch (S)
  {
    case GeomAbs_C0:
      return GeomAbs_C1;
    case GeomAbs_C1:
      return GeomAbs_C2;
    case GeomAbs_C2:
      return GeomAbs_C3;
    default:
      break;
  }
  return GeomAbs_CN;
}

//=================================================================================================

bool BlendFunc::ComputeNormal(const occ::handle<Adaptor3d_Surface>& Surf,
                              const gp_Pnt2d&                       p2d,
                              gp_Vec&                               Normal)
{
  const int    MaxOrder = 3;
  const double U        = p2d.X();
  const double V        = p2d.Y();

  int i, j;

  NCollection_Array2<gp_Vec> DerSurf(0, MaxOrder + 1, 0, MaxOrder + 1);
  for (i = 1; i <= MaxOrder + 1; i++)
    DerSurf.SetValue(i, 0, Surf->DN(U, V, i, 0));
  for (i = 0; i <= MaxOrder + 1; i++)
    for (j = 1; j <= MaxOrder + 1; j++)
      DerSurf.SetValue(i, j, Surf->DN(U, V, i, j));

  NCollection_Array2<gp_Vec> DerNUV(0, MaxOrder, 0, MaxOrder);
  for (i = 0; i <= MaxOrder; i++)
    for (j = 0; j <= MaxOrder; j++)
      DerNUV.SetValue(i, j, CSLib::DNNUV(i, j, DerSurf));

  gp_Dir             thenormal;
  CSLib_NormalStatus stat;
  int                OrderU, OrderV;
  const double       Umin = Surf->FirstUParameter();
  const double       Umax = Surf->LastUParameter();
  const double       Vmin = Surf->FirstVParameter();
  const double       Vmax = Surf->LastVParameter(); // szv: was FirstVParameter!
  CSLib::Normal(MaxOrder,
                DerNUV,
                double(1.e-9),
                U,
                V,
                Umin,
                Umax,
                Vmin,
                Vmax,
                stat,
                thenormal,
                OrderU,
                OrderV);
  if (stat == CSLib_Defined)
  {
    Normal.SetXYZ(thenormal.XYZ());
    return true;
  }
  return false;
}

//=================================================================================================

bool BlendFunc::ComputeDNormal(const occ::handle<Adaptor3d_Surface>& Surf,
                               const gp_Pnt2d&                       p2d,
                               gp_Vec&                               Normal,
                               gp_Vec&                               DNu,
                               gp_Vec&                               DNv)
{
  const int    MaxOrder = 3;
  const double U        = p2d.X();
  const double V        = p2d.Y();

  int i, j;

  NCollection_Array2<gp_Vec> DerSurf(0, MaxOrder + 1, 0, MaxOrder + 1);
  for (i = 1; i <= MaxOrder + 1; i++)
    DerSurf.SetValue(i, 0, Surf->DN(U, V, i, 0));
  for (i = 0; i <= MaxOrder + 1; i++)
    for (j = 1; j <= MaxOrder + 1; j++)
      DerSurf.SetValue(i, j, Surf->DN(U, V, i, j));

  NCollection_Array2<gp_Vec> DerNUV(0, MaxOrder, 0, MaxOrder);
  for (i = 0; i <= MaxOrder; i++)
    for (j = 0; j <= MaxOrder; j++)
      DerNUV.SetValue(i, j, CSLib::DNNUV(i, j, DerSurf));

  gp_Dir             thenormal;
  CSLib_NormalStatus stat;
  int                OrderU, OrderV;
  const double       Umin = Surf->FirstUParameter();
  const double       Umax = Surf->LastUParameter();
  const double       Vmin = Surf->FirstVParameter();
  const double       Vmax = Surf->LastVParameter(); // szv: was FirstVParameter!
  CSLib::Normal(MaxOrder,
                DerNUV,
                double(1.e-9),
                U,
                V,
                Umin,
                Umax,
                Vmin,
                Vmax,
                stat,
                thenormal,
                OrderU,
                OrderV);
  if (stat == CSLib_Defined)
  {
    Normal.SetXYZ(thenormal.XYZ());
    DNu = CSLib::DNNormal(1, 0, DerNUV, OrderU, OrderV);
    DNv = CSLib::DNNormal(0, 1, DerNUV, OrderU, OrderV);
    return true;
  }
  return false;
}
