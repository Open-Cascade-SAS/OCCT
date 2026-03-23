// Created on: 1998-02-26
// Created by: Roman BORISOV
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

#include <Adaptor3d_Curve.hxx>
#include <GeomFill_SnglrFunc.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>

GeomFill_SnglrFunc::GeomFill_SnglrFunc(const occ::handle<Adaptor3d_Curve>& HC)
    : myHCurve(HC),
      ratio(1)
{
}

//=================================================================================================

occ::handle<Adaptor3d_Curve> GeomFill_SnglrFunc::ShallowCopy() const
{
  occ::handle<GeomFill_SnglrFunc> aCopy = new GeomFill_SnglrFunc(myHCurve->ShallowCopy());
  aCopy->ratio                          = ratio;
  return aCopy;
}

void GeomFill_SnglrFunc::SetRatio(const double Ratio)
{
  ratio = Ratio;
}

double GeomFill_SnglrFunc::FirstParameter() const
{
  return myHCurve->FirstParameter();
}

double GeomFill_SnglrFunc::LastParameter() const
{
  return myHCurve->LastParameter();
}

int GeomFill_SnglrFunc::NbIntervals(const GeomAbs_Shape S) const
{
  GeomAbs_Shape HCS = GeomAbs_C0;
  if (S == GeomAbs_C0)
  {
    HCS = GeomAbs_C2;
  }
  else if (S == GeomAbs_C1)
  {
    HCS = GeomAbs_C3;
  }
  else if (S >= GeomAbs_C2)
  {
    HCS = GeomAbs_CN;
  }
  return myHCurve->NbIntervals(HCS);
}

void GeomFill_SnglrFunc::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  GeomAbs_Shape HCS = GeomAbs_C0;
  if (S == GeomAbs_C0)
  {
    HCS = GeomAbs_C2;
  }
  else if (S == GeomAbs_C1)
  {
    HCS = GeomAbs_C3;
  }
  else if (S >= GeomAbs_C2)
  {
    HCS = GeomAbs_CN;
  }
  myHCurve->Intervals(T, HCS);
}

bool GeomFill_SnglrFunc::IsPeriodic() const
{
  return myHCurve->IsPeriodic();
}

double GeomFill_SnglrFunc::Period() const
{
  return myHCurve->Period();
}

gp_Pnt GeomFill_SnglrFunc::EvalD0(const double theU) const
{
  const Geom_Curve::ResD2 aRes = myHCurve->EvalD2(theU);
  return gp_Pnt((aRes.D1 * ratio).Crossed(aRes.D2).XYZ());
}

Geom_Curve::ResD1 GeomFill_SnglrFunc::EvalD1(const double theU) const
{
  const Geom_Curve::ResD3 aRes = myHCurve->EvalD3(theU);
  const gp_Vec            aDC  = aRes.D1 * ratio;
  return {gp_Pnt(aDC.Crossed(aRes.D2).XYZ()), aDC.Crossed(aRes.D3)};
}

Geom_Curve::ResD2 GeomFill_SnglrFunc::EvalD2(const double theU) const
{
  const Geom_Curve::ResD3 aRes = myHCurve->EvalD3(theU);
  const gp_Vec            aD4  = myHCurve->EvalDN(theU, 4);
  return {gp_Pnt((aRes.D1.Crossed(aRes.D2) * ratio).XYZ()),
          aRes.D1.Crossed(aRes.D3) * ratio,
          (aRes.D2.Crossed(aRes.D3) + aRes.D1.Crossed(aD4)) * ratio};
}

Geom_Curve::ResD3 GeomFill_SnglrFunc::EvalD3(const double theU) const
{
  const Geom_Curve::ResD3 aRes = myHCurve->EvalD3(theU);
  const gp_Vec            aD4  = myHCurve->EvalDN(theU, 4);
  const gp_Vec            aD5  = myHCurve->EvalDN(theU, 5);
  return {gp_Pnt((aRes.D1.Crossed(aRes.D2) * ratio).XYZ()),
          aRes.D1.Crossed(aRes.D3) * ratio,
          (aRes.D2.Crossed(aRes.D3) + aRes.D1.Crossed(aD4)) * ratio,
          (aRes.D1.Crossed(aD5) + aRes.D2.Crossed(aD4) * 2) * ratio};
}

gp_Vec GeomFill_SnglrFunc::EvalDN(const double theU, const int theN) const
{
  Standard_RangeError_Raise_if(theN < 1, "Exception: Geom2d_OffsetCurve::DN(). N<1.");

  switch (theN)
  {
    case 1:
      return EvalD1(theU).D1;
    case 2:
      return EvalD2(theU).D2;
    case 3:
      return EvalD3(theU).D3;
    default:
      throw Standard_NotImplemented("Exception: Derivative order is greater than 3. "
                                    "Cannot compute of derivative.");
  }
}

double GeomFill_SnglrFunc::Resolution(const double R3D) const
{
  return Precision::Parametric(R3D);
}

GeomAbs_CurveType GeomFill_SnglrFunc::GetType() const
{
  return GeomAbs_OtherCurve;
}
