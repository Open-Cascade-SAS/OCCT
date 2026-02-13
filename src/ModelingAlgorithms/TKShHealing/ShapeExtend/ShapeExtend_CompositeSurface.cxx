// Created on: 1999-04-27
// Created by: Pavel DURANDIN
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

#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeExtend_CompositeSurface, Geom_Surface)

//=================================================================================================

ShapeExtend_CompositeSurface::ShapeExtend_CompositeSurface() = default;

//=================================================================================================

ShapeExtend_CompositeSurface::ShapeExtend_CompositeSurface(
  const occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>>& GridSurf,
  const ShapeExtend_Parametrisation                                  param)
{
  Init(GridSurf, param);
}

//=================================================================================================

ShapeExtend_CompositeSurface::ShapeExtend_CompositeSurface(
  const occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>>& GridSurf,
  const NCollection_Array1<double>&                                  UJoints,
  const NCollection_Array1<double>&                                  VJoints)
{
  Init(GridSurf, UJoints, VJoints);
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::Init(
  const occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>>& GridSurf,
  const ShapeExtend_Parametrisation                                  param)
{
  if (GridSurf.IsNull())
    return false;
  myPatches = GridSurf;
  ComputeJointValues(param);
  return CheckConnectivity(Precision::Confusion());
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::Init(
  const occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>>& GridSurf,
  const NCollection_Array1<double>&                                  UJoints,
  const NCollection_Array1<double>&                                  VJoints)
{
  if (GridSurf.IsNull())
    return false;
  myPatches = GridSurf;

  bool ok = true;
  if (!SetUJointValues(UJoints) || !SetVJointValues(VJoints))
  {
    ok = false;
    ComputeJointValues(ShapeExtend_Natural);
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeExtend_CompositeSurface::Init: bad joint values" << std::endl;
#endif
  }

  return (CheckConnectivity(Precision::Confusion()) ? ok : false);
}

//=================================================================================================

int ShapeExtend_CompositeSurface::NbUPatches() const
{
  return myPatches->ColLength();
}

//=================================================================================================

int ShapeExtend_CompositeSurface::NbVPatches() const
{
  return myPatches->RowLength();
}

//=================================================================================================

const occ::handle<Geom_Surface>& ShapeExtend_CompositeSurface::Patch(const int i, const int j) const
{
  return myPatches->Value(i, j);
}

//=================================================================================================

const occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>>& ShapeExtend_CompositeSurface::
  Patches() const
{
  return myPatches;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> ShapeExtend_CompositeSurface::UJointValues() const
{
  return myUJointValues;
}

//=================================================================================================

occ::handle<NCollection_HArray1<double>> ShapeExtend_CompositeSurface::VJointValues() const
{
  return myVJointValues;
}

//=================================================================================================

double ShapeExtend_CompositeSurface::UJointValue(const int i) const
{
  return myUJointValues->Value(i);
}

//=================================================================================================

double ShapeExtend_CompositeSurface::VJointValue(const int i) const
{
  return myVJointValues->Value(i);
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::SetUJointValues(const NCollection_Array1<double>& UJoints)
{
  int NbU = NbUPatches();
  if (UJoints.Length() != NbU + 1)
    return false;

  occ::handle<NCollection_HArray1<double>> UJointValues =
    new NCollection_HArray1<double>(1, NbU + 1);
  for (int i = 1, j = UJoints.Lower(); i <= NbU + 1; i++, j++)
  {
    UJointValues->SetValue(i, UJoints(j));
    if (i > 1 && UJoints(j) - UJoints(j - 1) < Precision::PConfusion())
      return false;
  }
  myUJointValues = UJointValues;
  return true;
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::SetVJointValues(const NCollection_Array1<double>& VJoints)
{
  int NbV = NbVPatches();
  if (VJoints.Length() != NbV + 1)
    return false;

  occ::handle<NCollection_HArray1<double>> VJointValues =
    new NCollection_HArray1<double>(1, NbV + 1);
  for (int i = 1, j = VJoints.Lower(); i <= NbV + 1; i++, j++)
  {
    VJointValues->SetValue(i, VJoints(j));
    if (i > 1 && VJoints(j) - VJoints(j - 1) < Precision::PConfusion())
      return false;
  }
  myVJointValues = VJointValues;
  return true;
}

//=================================================================================================

void ShapeExtend_CompositeSurface::SetUFirstValue(const double UFirst)
{
  if (myUJointValues.IsNull())
    return;

  double shift = UFirst - myUJointValues->Value(1);
  int    NbU   = myUJointValues->Length();
  for (int i = 1; i <= NbU; i++)
  {
    myUJointValues->SetValue(i, myUJointValues->Value(i) + shift);
  }
}

//=================================================================================================

void ShapeExtend_CompositeSurface::SetVFirstValue(const double VFirst)
{
  if (myVJointValues.IsNull())
    return;

  double shift = VFirst - myVJointValues->Value(1);
  int    NbV   = myVJointValues->Length();
  for (int i = 1; i <= NbV; i++)
  {
    myVJointValues->SetValue(i, myVJointValues->Value(i) + shift);
  }
}

//=================================================================================================

int ShapeExtend_CompositeSurface::LocateUParameter(const double U) const
{
  int nbPatch = NbUPatches();
  for (int i = 2; i <= nbPatch; i++)
    if (U < myUJointValues->Value(i))
      return i - 1;
  return nbPatch;
}

//=================================================================================================

int ShapeExtend_CompositeSurface::LocateVParameter(const double V) const
{
  int nbPatch = NbVPatches();
  for (int i = 2; i <= nbPatch; i++)
    if (V < myVJointValues->Value(i))
      return i - 1;
  return nbPatch;
}

//=================================================================================================

void ShapeExtend_CompositeSurface::LocateUVPoint(const gp_Pnt2d& pnt, int& i, int& j) const
{
  i = LocateUParameter(pnt.X());
  j = LocateVParameter(pnt.Y());
}

//=================================================================================================

const occ::handle<Geom_Surface>& ShapeExtend_CompositeSurface::Patch(const double U,
                                                                     const double V) const
{
  return myPatches->Value(LocateUParameter(U), LocateVParameter(V));
}

//=================================================================================================

const occ::handle<Geom_Surface>& ShapeExtend_CompositeSurface::Patch(const gp_Pnt2d& pnt) const
{
  return myPatches->Value(LocateUParameter(pnt.X()), LocateVParameter(pnt.Y()));
}

//=================================================================================================

double ShapeExtend_CompositeSurface::ULocalToGlobal(const int i, const int j, const double u) const
{
  double u1, u2, v1, v2;
  myPatches->Value(i, j)->Bounds(u1, u2, v1, v2);
  double scale = (myUJointValues->Value(i + 1) - myUJointValues->Value(i)) / (u2 - u1);
  // clang-format off
  return u * scale + ( myUJointValues->Value(i) - u1 * scale ); // ! this formula is stable if u1 is infinite
  // clang-format on
}

//=================================================================================================

double ShapeExtend_CompositeSurface::VLocalToGlobal(const int i, const int j, const double v) const
{
  double u1, u2, v1, v2;
  myPatches->Value(i, j)->Bounds(u1, u2, v1, v2);
  double scale = (myVJointValues->Value(j + 1) - myVJointValues->Value(j)) / (v2 - v1);
  // clang-format off
  return v * scale + ( myVJointValues->Value(j) - v1 * scale ); // ! this formula is stable if v1 is infinite
  // clang-format on
}

//=================================================================================================

gp_Pnt2d ShapeExtend_CompositeSurface::LocalToGlobal(const int       i,
                                                     const int       j,
                                                     const gp_Pnt2d& uv) const
{
  double u1, u2, v1, v2;
  myPatches->Value(i, j)->Bounds(u1, u2, v1, v2);
  double scaleu = (myUJointValues->Value(i + 1) - myUJointValues->Value(i)) / (u2 - u1);
  double scalev = (myVJointValues->Value(j + 1) - myVJointValues->Value(j)) / (v2 - v1);
  return gp_Pnt2d(uv.X() * scaleu
                    + (myUJointValues->Value(i)
                       - u1 * scaleu), // ! this formula is stable if u1 or v1 is infinite
                  uv.Y() * scalev + (myVJointValues->Value(j) - v1 * scalev));
}

//=================================================================================================

double ShapeExtend_CompositeSurface::UGlobalToLocal(const int i, const int j, const double U) const
{
  double u1, u2, v1, v2;
  myPatches->Value(i, j)->Bounds(u1, u2, v1, v2);
  double scale = (u2 - u1) / (myUJointValues->Value(i + 1) - myUJointValues->Value(i));
  // clang-format off
  return U * scale + ( u1 - myUJointValues->Value(i) * scale ); // ! this formula is stable if u1 is infinite
  // clang-format on
}

//=================================================================================================

double ShapeExtend_CompositeSurface::VGlobalToLocal(const int i, const int j, const double V) const
{
  double u1, u2, v1, v2;
  myPatches->Value(i, j)->Bounds(u1, u2, v1, v2);
  double scale = (v2 - v1) / (myVJointValues->Value(j + 1) - myVJointValues->Value(j));
  // clang-format off
  return V * scale + ( v1 - myVJointValues->Value(j) * scale ); // ! this formula is stable if v1 is infinite
  // clang-format on
}

//=================================================================================================

gp_Pnt2d ShapeExtend_CompositeSurface::GlobalToLocal(const int       i,
                                                     const int       j,
                                                     const gp_Pnt2d& UV) const
{
  double u1, u2, v1, v2;
  myPatches->Value(i, j)->Bounds(u1, u2, v1, v2);
  double scaleu = (u2 - u1) / (myUJointValues->Value(i + 1) - myUJointValues->Value(i));
  double scalev = (v2 - v1) / (myVJointValues->Value(j + 1) - myVJointValues->Value(j));
  return gp_Pnt2d(
    UV.X() * scaleu
      + (u1
         - myUJointValues->Value(i) * scaleu), // ! this formula is stable if u1 or v1 is infinite
    UV.Y() * scalev + (v1 - myVJointValues->Value(j) * scalev));
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::GlobalToLocalTransformation(const int  i,
                                                               const int  j,
                                                               double&    uFact,
                                                               gp_Trsf2d& Trsf) const
{
  double u1, u2, v1, v2;
  myPatches->Value(i, j)->Bounds(u1, u2, v1, v2);

  double   scaleu = (u2 - u1) / (myUJointValues->Value(i + 1) - myUJointValues->Value(i));
  double   scalev = (v2 - v1) / (myVJointValues->Value(j + 1) - myVJointValues->Value(j));
  gp_Vec2d shift(u1 / scaleu - myUJointValues->Value(i), v1 / scalev - myVJointValues->Value(j));

  uFact = scaleu / scalev;
  gp_Trsf2d Shift, Scale;
  if (shift.X() != 0. || shift.Y() != 0.)
    Shift.SetTranslation(shift);
  if (scalev != 1.)
    Scale.SetScale(gp_Pnt2d(0, 0), scalev);
  Trsf = Scale * Shift;
  return uFact != 1. || Trsf.Form() != gp_Identity;
}

//=======================================================================
// Inherited methods (from Geom_Geometry and Geom_Surface)
//=======================================================================

//=================================================================================================

void ShapeExtend_CompositeSurface::Transform(const gp_Trsf& T)
{
  if (myPatches.IsNull())
    return;
  for (int i = 1; i <= NbUPatches(); i++)
    for (int j = 1; j <= NbVPatches(); j++)
      Patch(i, j)->Transform(T);
}

//=================================================================================================

occ::handle<Geom_Geometry> ShapeExtend_CompositeSurface::Copy() const
{
  occ::handle<ShapeExtend_CompositeSurface> surf = new ShapeExtend_CompositeSurface;
  if (myPatches.IsNull())
    return surf;

  occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> patches =
    new NCollection_HArray2<occ::handle<Geom_Surface>>(1, NbUPatches(), 1, NbVPatches());
  for (int i = 1; i <= NbUPatches(); i++)
    for (int j = 1; j <= NbVPatches(); j++)
      patches->SetValue(i, j, occ::down_cast<Geom_Surface>(Patch(i, j)->Copy()));
  surf->Init(patches);
  return surf;
}

//=================================================================================================

void ShapeExtend_CompositeSurface::UReverse() {}

//=================================================================================================

double ShapeExtend_CompositeSurface::UReversedParameter(const double U) const
{
  return U;
}

//=================================================================================================

void ShapeExtend_CompositeSurface::VReverse() {}

//=================================================================================================

double ShapeExtend_CompositeSurface::VReversedParameter(const double V) const
{
  return V;
}

//=================================================================================================

void ShapeExtend_CompositeSurface::Bounds(double& U1, double& U2, double& V1, double& V2) const
{
  U1 = UJointValue(1);
  V1 = VJointValue(1);
  U2 = UJointValue(NbUPatches() + 1);
  V2 = VJointValue(NbVPatches() + 1);
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::IsUPeriodic() const
{
  return false;
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::IsVPeriodic() const
{
  return false;
}

//=================================================================================================

occ::handle<Geom_Curve> ShapeExtend_CompositeSurface::UIso(const double) const
{
  occ::handle<Geom_Curve> dummy;
  return dummy;
}

//=================================================================================================

occ::handle<Geom_Curve> ShapeExtend_CompositeSurface::VIso(const double) const
{
  occ::handle<Geom_Curve> dummy;
  return dummy;
}

//=================================================================================================

GeomAbs_Shape ShapeExtend_CompositeSurface::Continuity() const
{
  return GeomAbs_C0;
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::IsCNu(const int N) const
{
  return N <= 0;
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::IsCNv(const int N) const
{
  return N <= 0;
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::IsUClosed() const
{
  return myUClosed;
}

//=================================================================================================

bool ShapeExtend_CompositeSurface::IsVClosed() const
{
  return myVClosed;
}

//=================================================================================================

std::optional<gp_Pnt> ShapeExtend_CompositeSurface::EvalD0(const double U, const double V) const
{
  int      i  = LocateUParameter(U);
  int      j  = LocateVParameter(V);
  gp_Pnt2d uv = GlobalToLocal(i, j, gp_Pnt2d(U, V));
  return myPatches->Value(i, j)->EvalD0(uv.X(), uv.Y());
}

//=================================================================================================

std::optional<Geom_SurfD1> ShapeExtend_CompositeSurface::EvalD1(const double U,
                                                                const double V) const
{
  int      i  = LocateUParameter(U);
  int      j  = LocateVParameter(V);
  gp_Pnt2d uv = GlobalToLocal(i, j, gp_Pnt2d(U, V));
  return myPatches->Value(i, j)->EvalD1(uv.X(), uv.Y());
}

//=================================================================================================

std::optional<Geom_SurfD2> ShapeExtend_CompositeSurface::EvalD2(const double U,
                                                                const double V) const
{
  int      i  = LocateUParameter(U);
  int      j  = LocateVParameter(V);
  gp_Pnt2d uv = GlobalToLocal(i, j, gp_Pnt2d(U, V));
  return myPatches->Value(i, j)->EvalD2(uv.X(), uv.Y());
}

//=================================================================================================

std::optional<Geom_SurfD3> ShapeExtend_CompositeSurface::EvalD3(const double U,
                                                                const double V) const
{
  int      i  = LocateUParameter(U);
  int      j  = LocateVParameter(V);
  gp_Pnt2d uv = GlobalToLocal(i, j, gp_Pnt2d(U, V));
  return myPatches->Value(i, j)->EvalD3(uv.X(), uv.Y());
}

//=================================================================================================

std::optional<gp_Vec> ShapeExtend_CompositeSurface::EvalDN(const double U,
                                                           const double V,
                                                           const int    Nu,
                                                           const int    Nv) const
{
  int      i  = LocateUParameter(U);
  int      j  = LocateVParameter(V);
  gp_Pnt2d uv = GlobalToLocal(i, j, gp_Pnt2d(U, V));
  return myPatches->Value(i, j)->EvalDN(uv.X(), uv.Y(), Nu, Nv);
}

//=================================================================================================

gp_Pnt ShapeExtend_CompositeSurface::Value(const gp_Pnt2d& pnt) const
{
  int      i  = LocateUParameter(pnt.X());
  int      j  = LocateVParameter(pnt.Y());
  gp_Pnt2d uv = GlobalToLocal(i, j, pnt);
  gp_Pnt   point;
  myPatches->Value(i, j)->D0(uv.X(), uv.Y(), point);
  return point;
}

//=================================================================================================

void ShapeExtend_CompositeSurface::ComputeJointValues(const ShapeExtend_Parametrisation param)
{
  int NbU        = NbUPatches();
  int NbV        = NbVPatches();
  myUJointValues = new NCollection_HArray1<double>(1, NbU + 1);
  myVJointValues = new NCollection_HArray1<double>(1, NbV + 1);

  if (param == ShapeExtend_Natural)
  {
    double U1, U2, V1, V2, U = 0, V = 0;
    int    i; // svv Jan 10 2000 : porting on DEC
    for (i = 1; i <= NbU; i++)
    {
      myPatches->Value(i, 1)->Bounds(U1, U2, V1, V2);
      if (i == 1)
        myUJointValues->SetValue(1, U = U1);
      U += (U2 - U1);
      myUJointValues->SetValue(i + 1, U);
    }
    for (i = 1; i <= NbV; i++)
    {
      myPatches->Value(1, i)->Bounds(U1, U2, V1, V2);
      if (i == 1)
        myVJointValues->SetValue(1, V = V1);
      V += (V2 - V1);
      myVJointValues->SetValue(i + 1, V);
    }
  }
  else
  {
    double stepu = 1., stepv = 1.; // suppose param == ShapeExtend_Uniform
    if (param == ShapeExtend_Unitary)
    {
      stepu /= NbU;
      stepv /= NbV;
    }
    int i; // svv Jan 10 2000 : porting on DEC
    for (i = 0; i <= NbU; i++)
      myUJointValues->SetValue(i + 1, i * stepu);
    for (i = 0; i <= NbV; i++)
      myVJointValues->SetValue(i + 1, i * stepv);
  }
}

//=================================================================================================

static inline double LimitValue(const double& par)
{
  return Precision::IsInfinite(par) ? (par < 0 ? -10000. : 10000.) : par;
}

static void GetLimitedBounds(const occ::handle<Geom_Surface>& surf,
                             double&                          U1,
                             double&                          U2,
                             double&                          V1,
                             double&                          V2)
{
  surf->Bounds(U1, U2, V1, V2);
  U1 = LimitValue(U1);
  U2 = LimitValue(U2);
  V1 = LimitValue(V1);
  V2 = LimitValue(V2);
}

bool ShapeExtend_CompositeSurface::CheckConnectivity(const double Prec)
{
  const int NPOINTS = 23;
  bool      ok      = true;
  int       NbU     = NbUPatches();
  int       NbV     = NbVPatches();

  // check in u direction
  int i, j; // svv Jan 10 2000 : porting on DEC
  for (i = 1, j = NbU; i <= NbU; j = i++)
  {
    double maxdist2 = 0.;
    for (int k = 1; k <= NbV; k++)
    {
      occ::handle<Geom_Surface> sj = myPatches->Value(j, k);
      occ::handle<Geom_Surface> si = myPatches->Value(i, k);
      double                    Uj1, Uj2, Vj1, Vj2;
      GetLimitedBounds(sj, Uj1, Uj2, Vj1, Vj2);
      double Ui1, Ui2, Vi1, Vi2;
      GetLimitedBounds(si, Ui1, Ui2, Vi1, Vi2);
      double stepj = (Vj2 - Vj1) / (NPOINTS - 1);
      double stepi = (Vi2 - Vi1) / (NPOINTS - 1);
      for (int isample = 0; isample < NPOINTS; isample++)
      {
        double parj  = Vj1 + stepj * isample;
        double pari  = Vi1 + stepi * isample;
        double dist2 = sj->Value(Uj2, parj).SquareDistance(si->Value(Ui1, pari));
        if (maxdist2 < dist2)
          maxdist2 = dist2;
      }
    }
    if (i == 1)
      myUClosed = (maxdist2 <= Prec * Prec);
    else if (maxdist2 > Prec * Prec)
      ok = false;
  }

  // check in v direction
  for (i = 1, j = NbV; i <= NbV; j = i++)
  {
    double maxdist2 = 0.;
    for (int k = 1; k <= NbU; k++)
    {
      occ::handle<Geom_Surface> sj = myPatches->Value(k, j);
      occ::handle<Geom_Surface> si = myPatches->Value(k, i);
      double                    Uj1, Uj2, Vj1, Vj2;
      GetLimitedBounds(sj, Uj1, Uj2, Vj1, Vj2);
      double Ui1, Ui2, Vi1, Vi2;
      GetLimitedBounds(si, Ui1, Ui2, Vi1, Vi2);
      double stepj = (Uj2 - Uj1) / (NPOINTS - 1);
      double stepi = (Ui2 - Ui1) / (NPOINTS - 1);
      for (int isample = 0; isample < NPOINTS; isample++)
      {
        double parj  = Uj1 + stepj * isample;
        double pari  = Ui1 + stepi * isample;
        double dist2 = sj->Value(parj, Vj2).SquareDistance(si->Value(pari, Vi1));
        if (maxdist2 < dist2)
          maxdist2 = dist2;
      }
    }
    if (i == 1)
      myVClosed = (maxdist2 <= Prec * Prec);
    else if (maxdist2 > Prec * Prec)
      ok = false;
  }

#ifdef OCCT_DEBUG
  if (!ok)
    std::cout << "Warning: ShapeExtend_CompositeSurface: not connected in 3d" << std::endl;
#endif
  return ok;
}
