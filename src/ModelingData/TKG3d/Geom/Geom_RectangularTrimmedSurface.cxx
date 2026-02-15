// Created on: 1993-03-10
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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

// *******************************************************************
// *******************************************************************

#include <ElCLib.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_RectangularTrimmedSurface, Geom_BoundedSurface)

typedef Geom_RectangularTrimmedSurface RectangularTrimmedSurface;
typedef gp_Ax1                         Ax1;
typedef gp_Ax2                         Ax2;
typedef gp_Pnt                         Pnt;
typedef gp_Trsf                        Trsf;
typedef gp_Vec                         Vec;

//=================================================================================================

occ::handle<Geom_Geometry> Geom_RectangularTrimmedSurface::Copy() const
{

  occ::handle<Geom_RectangularTrimmedSurface> S;

  if (isutrimmed && isvtrimmed)
    S = new RectangularTrimmedSurface(basisSurf, utrim1, utrim2, vtrim1, vtrim2, true, true);
  else if (isutrimmed)
    S = new RectangularTrimmedSurface(basisSurf, utrim1, utrim2, true, true);
  else if (isvtrimmed)
    S = new RectangularTrimmedSurface(basisSurf, vtrim1, vtrim2, false, true);

  return S;
}

//=================================================================================================

Geom_RectangularTrimmedSurface::Geom_RectangularTrimmedSurface(

  const occ::handle<Geom_Surface>& S,
  const double                     U1,
  const double                     U2,
  const double                     V1,
  const double                     V2,
  const bool                       USense,
  const bool                       VSense)

    : utrim1(U1),
      vtrim1(V1),
      utrim2(U2),
      vtrim2(V2),
      isutrimmed(true),
      isvtrimmed(true)
{

  // kill trimmed basis surfaces
  occ::handle<Geom_RectangularTrimmedSurface> T = occ::down_cast<Geom_RectangularTrimmedSurface>(S);
  if (!T.IsNull())
    basisSurf = occ::down_cast<Geom_Surface>(T->BasisSurface()->Copy());
  else
    basisSurf = occ::down_cast<Geom_Surface>(S->Copy());

  occ::handle<Geom_OffsetSurface> O = occ::down_cast<Geom_OffsetSurface>(basisSurf);
  if (!O.IsNull())
  {
    occ::handle<Geom_RectangularTrimmedSurface> S2 =
      new Geom_RectangularTrimmedSurface(O->BasisSurface(), U1, U2, V1, V2, USense, VSense);
    basisSurf = new Geom_OffsetSurface(S2, O->Offset(), true);
  }

  SetTrim(U1, U2, V1, V2, USense, VSense);
}

//=================================================================================================

Geom_RectangularTrimmedSurface::Geom_RectangularTrimmedSurface(

  const occ::handle<Geom_Surface>& S,
  const double                     Param1,
  const double                     Param2,
  const bool                       UTrim,
  const bool                       Sense)
{
  // kill trimmed basis surfaces
  occ::handle<Geom_RectangularTrimmedSurface> T = occ::down_cast<Geom_RectangularTrimmedSurface>(S);
  if (!T.IsNull())
    basisSurf = occ::down_cast<Geom_Surface>(T->BasisSurface()->Copy());
  else
    basisSurf = occ::down_cast<Geom_Surface>(S->Copy());

  occ::handle<Geom_OffsetSurface> O = occ::down_cast<Geom_OffsetSurface>(basisSurf);
  if (!O.IsNull())
  {
    occ::handle<Geom_RectangularTrimmedSurface> S2 =
      new Geom_RectangularTrimmedSurface(O->BasisSurface(), Param1, Param2, UTrim, Sense);
    basisSurf = new Geom_OffsetSurface(S2, O->Offset(), true);
  }

  if (!T.IsNull())
  {
    if (UTrim && T->isvtrimmed)
    {
      SetTrim(Param1, Param2, T->vtrim1, T->vtrim2, Sense, true);
      return;
    }
    else if (!UTrim && T->isutrimmed)
    {
      SetTrim(T->utrim1, T->utrim2, Param1, Param2, true, Sense);
      return;
    }
  }

  SetTrim(Param1, Param2, UTrim, Sense);
}

//=================================================================================================

void Geom_RectangularTrimmedSurface::SetTrim(const double U1,
                                             const double U2,
                                             const double V1,
                                             const double V2,
                                             const bool   USense,
                                             const bool   VSense)
{
  SetTrim(U1, U2, V1, V2, true, true, USense, VSense);
}

//=================================================================================================

void Geom_RectangularTrimmedSurface::SetTrim(const double Param1,
                                             const double Param2,
                                             const bool   UTrim,
                                             const bool   Sense)
{

  // dummy arguments to call general SetTrim
  double dummy_a     = 0.;
  double dummy_b     = 0.;
  bool   dummy_Sense = true;

  if (UTrim)
  {
    if (isvtrimmed)
    {
      SetTrim(Param1, Param2, vtrim1, vtrim2, true, true, Sense, dummy_Sense);
    }
    else
    {
      SetTrim(Param1, Param2, dummy_a, dummy_b, true, false, Sense, dummy_Sense);
    }
  }
  else
  {
    if (isutrimmed)
    {
      SetTrim(utrim1, utrim2, Param1, Param2, true, true, dummy_Sense, Sense);
    }
    else
    {
      SetTrim(dummy_a, dummy_b, Param1, Param2, false, true, dummy_Sense, Sense);
    }
  }
}

//=================================================================================================

void Geom_RectangularTrimmedSurface::SetTrim(const double U1,
                                             const double U2,
                                             const double V1,
                                             const double V2,
                                             const bool   UTrim,
                                             const bool   VTrim,
                                             const bool   USense,
                                             const bool   VSense)
{

  bool   UsameSense = true;
  bool   VsameSense = true;
  double Udeb, Ufin, Vdeb, Vfin;

  basisSurf->Bounds(Udeb, Ufin, Vdeb, Vfin);

  // Trimming the U-Direction
  isutrimmed = UTrim;
  if (!UTrim)
  {
    utrim1 = Udeb;
    utrim2 = Ufin;
  }
  else
  {
    if (U1 == U2)
      throw Standard_ConstructionError("Geom_RectangularTrimmedSurface::U1==U2");

    if (basisSurf->IsUPeriodic())
    {
      UsameSense = USense;

      // set uTrim1 in the range Udeb , Ufin
      // set uTrim2 in the range uTrim1 , uTrim1 + Period()
      utrim1 = U1;
      utrim2 = U2;
      ElCLib::AdjustPeriodic(Udeb,
                             Ufin,
                             std::min(std::abs(utrim2 - utrim1) / 2, Precision::PConfusion()),
                             utrim1,
                             utrim2);
    }
    else
    {
      if (U1 < U2)
      {
        UsameSense = USense;
        utrim1     = U1;
        utrim2     = U2;
      }
      else
      {
        UsameSense = !USense;
        utrim1     = U2;
        utrim2     = U1;
      }

      if ((Udeb - utrim1 > Precision::PConfusion()) || (utrim2 - Ufin > Precision::PConfusion()))
        throw Standard_ConstructionError(
          "Geom_RectangularTrimmedSurface::U parameters out of range");
    }
  }

  // Trimming the V-Direction
  isvtrimmed = VTrim;
  if (!VTrim)
  {
    vtrim1 = Vdeb;
    vtrim2 = Vfin;
  }
  else
  {
    if (V1 == V2)
      throw Standard_ConstructionError("Geom_RectangularTrimmedSurface::V1==V2");

    if (basisSurf->IsVPeriodic())
    {
      VsameSense = VSense;

      // set vTrim1 in the range Vdeb , Vfin
      // set vTrim2 in the range vTrim1 , vTrim1 + Period()
      vtrim1 = V1;
      vtrim2 = V2;
      ElCLib::AdjustPeriodic(Vdeb,
                             Vfin,
                             std::min(std::abs(vtrim2 - vtrim1) / 2, Precision::PConfusion()),
                             vtrim1,
                             vtrim2);
    }
    else
    {
      if (V1 < V2)
      {
        VsameSense = VSense;
        vtrim1     = V1;
        vtrim2     = V2;
      }
      else
      {
        VsameSense = !VSense;
        vtrim1     = V2;
        vtrim2     = V1;
      }

      if ((Vdeb - vtrim1 > Precision::PConfusion()) || (vtrim2 - Vfin > Precision::PConfusion()))
        throw Standard_ConstructionError(
          "Geom_RectangularTrimmedSurface::V parameters out of range");
    }
  }

  if (!UsameSense)
    UReverse();
  if (!VsameSense)
    VReverse();
}

//=================================================================================================

void Geom_RectangularTrimmedSurface::UReverse()
{
  double U1 = basisSurf->UReversedParameter(utrim2);
  double U2 = basisSurf->UReversedParameter(utrim1);
  basisSurf->UReverse();
  SetTrim(U1, U2, vtrim1, vtrim2, isutrimmed, isvtrimmed, true, true);
}

//=================================================================================================

double Geom_RectangularTrimmedSurface::UReversedParameter(const double U) const
{

  return basisSurf->UReversedParameter(U);
}

//=================================================================================================

void Geom_RectangularTrimmedSurface::VReverse()
{
  double V1 = basisSurf->VReversedParameter(vtrim2);
  double V2 = basisSurf->VReversedParameter(vtrim1);
  basisSurf->VReverse();
  SetTrim(utrim1, utrim2, V1, V2, isutrimmed, isvtrimmed, true, true);
}

//=================================================================================================

double Geom_RectangularTrimmedSurface::VReversedParameter(const double V) const
{

  return basisSurf->VReversedParameter(V);
}

//=================================================================================================

occ::handle<Geom_Surface> Geom_RectangularTrimmedSurface::BasisSurface() const
{
  return basisSurf;
}

//=================================================================================================

GeomAbs_Shape Geom_RectangularTrimmedSurface::Continuity() const
{

  return basisSurf->Continuity();
}

//=================================================================================================

gp_Pnt Geom_RectangularTrimmedSurface::EvalD0(const double U, const double V) const
{
  return basisSurf->EvalD0(U, V);
}

//=================================================================================================

Geom_Surface::ResD1 Geom_RectangularTrimmedSurface::EvalD1(const double U, const double V) const
{
  return basisSurf->EvalD1(U, V);
}

//=================================================================================================

Geom_Surface::ResD2 Geom_RectangularTrimmedSurface::EvalD2(const double U, const double V) const
{
  return basisSurf->EvalD2(U, V);
}

//=================================================================================================

Geom_Surface::ResD3 Geom_RectangularTrimmedSurface::EvalD3(const double U, const double V) const
{
  return basisSurf->EvalD3(U, V);
}

//=================================================================================================

gp_Vec Geom_RectangularTrimmedSurface::EvalDN(const double U,
                                              const double V,
                                              const int    Nu,
                                              const int    Nv) const
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0)
    throw Geom_UndefinedDerivative();
  return basisSurf->EvalDN(U, V, Nu, Nv);
}

//=================================================================================================

void Geom_RectangularTrimmedSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{

  U1 = utrim1;
  U2 = utrim2;
  V1 = vtrim1;
  V2 = vtrim2;
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_RectangularTrimmedSurface::UIso(const double U) const
{

  occ::handle<Geom_Curve> C = basisSurf->UIso(U);

  if (isvtrimmed)
  {
    occ::handle<Geom_TrimmedCurve> Ct;
    Ct = new Geom_TrimmedCurve(C, vtrim1, vtrim2, true);
    return Ct;
  }
  else
  {
    return C;
  }
}

//=================================================================================================

occ::handle<Geom_Curve> Geom_RectangularTrimmedSurface::VIso(const double V) const
{

  occ::handle<Geom_Curve> C = basisSurf->VIso(V);

  if (isutrimmed)
  {
    occ::handle<Geom_TrimmedCurve> Ct;
    Ct = new Geom_TrimmedCurve(C, utrim1, utrim2, true);
    return Ct;
  }
  else
  {
    return C;
  }
}

//=================================================================================================

bool Geom_RectangularTrimmedSurface::IsCNu(const int N) const
{

  Standard_RangeError_Raise_if(N < 0, " ");
  return basisSurf->IsCNu(N);
}

//=================================================================================================

bool Geom_RectangularTrimmedSurface::IsCNv(const int N) const
{

  Standard_RangeError_Raise_if(N < 0, " ");
  return basisSurf->IsCNv(N);
}

//=================================================================================================

void Geom_RectangularTrimmedSurface::Transform(const Trsf& T)
{
  basisSurf->Transform(T);
  basisSurf->TransformParameters(utrim1, vtrim1, T);
  basisSurf->TransformParameters(utrim2, vtrim2, T);
}

//=======================================================================
// function : IsUPeriodic
// purpose  :
// 24/11/98: pmn : Compare la periode a la longeur de l'intervalle
//=======================================================================

bool Geom_RectangularTrimmedSurface::IsUPeriodic() const
{
  return basisSurf->IsUPeriodic() && !isutrimmed;
}

//=================================================================================================

double Geom_RectangularTrimmedSurface::UPeriod() const
{
  return basisSurf->UPeriod();
}

//=================================================================================================

bool Geom_RectangularTrimmedSurface::IsVPeriodic() const
{
  return basisSurf->IsVPeriodic() && !isvtrimmed;
}

//=================================================================================================

double Geom_RectangularTrimmedSurface::VPeriod() const
{
  return basisSurf->VPeriod();
}

//=================================================================================================

bool Geom_RectangularTrimmedSurface::IsUClosed() const
{

  if (isutrimmed)
    return false;
  else
    return basisSurf->IsUClosed();
}

//=================================================================================================

bool Geom_RectangularTrimmedSurface::IsVClosed() const
{

  if (isvtrimmed)
    return false;
  else
    return basisSurf->IsVClosed();
}

//=================================================================================================

void Geom_RectangularTrimmedSurface::TransformParameters(double&        U,
                                                         double&        V,
                                                         const gp_Trsf& T) const
{
  basisSurf->TransformParameters(U, V, T);
}

//=================================================================================================

gp_GTrsf2d Geom_RectangularTrimmedSurface::ParametricTransformation(const gp_Trsf& T) const
{
  return basisSurf->ParametricTransformation(T);
}

//=================================================================================================

void Geom_RectangularTrimmedSurface::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedSurface)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, basisSurf.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, utrim1)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vtrim1)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, utrim2)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, vtrim2)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, isutrimmed)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, isvtrimmed)
}
