// Created on: 1997-11-21
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef OCCT_DEBUG
  #define No_Standard_RangeError
  #define No_Standard_OutOfRange
  #define No_Standard_DimensionError
#endif

#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_SectionLaw.hxx>
#include <GeomFill_SweepFunction.hxx>
#include <GeomLib.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Sequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_SweepFunction, Approx_SweepFunction)

//=================================================================================================

GeomFill_SweepFunction::GeomFill_SweepFunction(const occ::handle<GeomFill_SectionLaw>&  Section,
                                               const occ::handle<GeomFill_LocationLaw>& Location,
                                               const double                 FirstParameter,
                                               const double FirstParameterOnS,
                                               const double RatioParameterOnS)
{
  myLoc   = Location;
  mySec   = Section;
  myf     = FirstParameter;
  myfOnS  = FirstParameterOnS;
  myRatio = RatioParameterOnS;
}

//=================================================================================================

bool GeomFill_SweepFunction::D0(const double Param,
                                            const double,
                                            const double,
                                            NCollection_Array1<gp_Pnt>&   Poles,
                                            NCollection_Array1<gp_Pnt2d>& Poles2d,
                                            NCollection_Array1<double>& Weigths)
{
  int ii, L;
  bool Ok;
  double    T = myfOnS + (Param - myf) * myRatio;
  L                  = Poles.Length();

  Ok = myLoc->D0(Param, M, V, Poles2d);
  if (!Ok)
    return Ok;
  Ok = mySec->D0(T, Poles, Weigths);
  if (!Ok)
    return Ok;

  for (ii = 1; ii <= L; ii++)
  {
    gp_XYZ& aux = Poles(ii).ChangeCoord();
    aux *= M;
    aux += V.XYZ();
  }
  return true;
}

//=================================================================================================

bool GeomFill_SweepFunction::D1(const double Param,
                                            const double,
                                            const double,
                                            NCollection_Array1<gp_Pnt>&   Poles,
                                            NCollection_Array1<gp_Vec>&   DPoles,
                                            NCollection_Array1<gp_Pnt2d>& Poles2d,
                                            NCollection_Array1<gp_Vec2d>& DPoles2d,
                                            NCollection_Array1<double>& Weigths,
                                            NCollection_Array1<double>& DWeigths)
{
  int ii, L;
  bool Ok;
  double    T = myfOnS + (Param - myf) * myRatio;
  gp_XYZ           PPrim;
  L = Poles.Length();

  Ok = myLoc->D1(Param, M, V, DM, DV, Poles2d, DPoles2d);
  if (!Ok)
    return Ok;
  Ok = mySec->D1(T, Poles, DPoles, Weigths, DWeigths);
  if (!Ok)
    return Ok;

  for (ii = 1; ii <= L; ii++)
  {
    PPrim     = DPoles(ii).XYZ();
    gp_XYZ& P = Poles(ii).ChangeCoord();
    PPrim *= myRatio;
    DWeigths(ii) *= myRatio;
    PPrim *= M;
    PPrim += DM * P;
    PPrim += DV.XYZ();
    DPoles(ii).SetXYZ(PPrim);

    P *= M;
    P += V.XYZ();
  }
  return true;
}

//=================================================================================================

bool GeomFill_SweepFunction::D2(const double Param,
                                            const double,
                                            const double,
                                            NCollection_Array1<gp_Pnt>&   Poles,
                                            NCollection_Array1<gp_Vec>&   DPoles,
                                            NCollection_Array1<gp_Vec>&   D2Poles,
                                            NCollection_Array1<gp_Pnt2d>& Poles2d,
                                            NCollection_Array1<gp_Vec2d>& DPoles2d,
                                            NCollection_Array1<gp_Vec2d>& D2Poles2d,
                                            NCollection_Array1<double>& Weigths,
                                            NCollection_Array1<double>& DWeigths,
                                            NCollection_Array1<double>& D2Weigths)
{
  int ii, L;
  bool Ok;
  double    T           = myfOnS + (Param - myf) * myRatio;
  double    squareratio = myRatio * myRatio;
  L                            = Poles.Length();

  Ok = myLoc->D2(Param, M, V, DM, DV, D2M, D2V, Poles2d, DPoles2d, D2Poles2d);
  if (!Ok)
    return Ok;
  Ok = mySec->D2(T, Poles, DPoles, D2Poles, Weigths, DWeigths, D2Weigths);
  if (!Ok)
    return Ok;

  for (ii = 1; ii <= L; ii++)
  {
    gp_XYZ PSecn = D2Poles(ii).XYZ();
    gp_XYZ PPrim = DPoles(ii).XYZ();
    PPrim *= myRatio;
    DWeigths(ii) *= myRatio;
    PSecn *= squareratio;
    D2Weigths(ii) *= squareratio;
    gp_XYZ& P = Poles(ii).ChangeCoord();

    PSecn *= M;
    PSecn += 2 * (DM * PPrim);
    PSecn += D2M * P;
    PSecn += D2V.XYZ();
    D2Poles(ii).SetXYZ(PSecn);

    PPrim *= M;
    PPrim += DM * P;
    PPrim += DV.XYZ();
    DPoles(ii).SetXYZ(PPrim);

    P *= M;
    P += V.XYZ();
  }
  return true;
}

//=================================================================================================

int GeomFill_SweepFunction::Nb2dCurves() const
{
  return myLoc->Nb2dCurves();
}

//=================================================================================================

void GeomFill_SweepFunction::SectionShape(int& NbPoles,
                                          int& NbKnots,
                                          int& Degree) const
{
  mySec->SectionShape(NbPoles, NbKnots, Degree);
}

//=================================================================================================

void GeomFill_SweepFunction::Knots(NCollection_Array1<double>& TKnots) const
{
  mySec->Knots(TKnots);
}

//=================================================================================================

void GeomFill_SweepFunction::Mults(NCollection_Array1<int>& TMults) const
{
  mySec->Mults(TMults);
}

//=================================================================================================

bool GeomFill_SweepFunction::IsRational() const
{
  return mySec->IsRational();
}

//=================================================================================================

int GeomFill_SweepFunction::NbIntervals(const GeomAbs_Shape S) const
{
  int Nb_Sec, Nb_Loc;
  Nb_Sec = mySec->NbIntervals(S);
  Nb_Loc = myLoc->NbIntervals(S);

  if (Nb_Sec == 1)
  {
    return Nb_Loc;
  }
  else if (Nb_Loc == 1)
  {
    return Nb_Sec;
  }

  NCollection_Array1<double>   IntS(1, Nb_Sec + 1);
  NCollection_Array1<double>   IntL(1, Nb_Loc + 1);
  NCollection_Sequence<double> Inter;
  double          T;
  int       ii;
  mySec->Intervals(IntS, S);
  for (ii = 1; ii <= Nb_Sec + 1; ii++)
  {
    T        = (IntS(ii) - myfOnS) / myRatio + myf;
    IntS(ii) = T;
  }
  myLoc->Intervals(IntL, S);

  GeomLib::FuseIntervals(IntS, IntL, Inter, Precision::PConfusion() * 0.99);
  return Inter.Length() - 1;
}

//=================================================================================================

void GeomFill_SweepFunction::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  int Nb_Sec, Nb_Loc, ii;
  Nb_Sec = mySec->NbIntervals(S);
  Nb_Loc = myLoc->NbIntervals(S);

  if (Nb_Sec == 1)
  {
    myLoc->Intervals(T, S);
    return;
  }
  else if (Nb_Loc == 1)
  {
    double t;
    mySec->Intervals(T, S);
    for (ii = 1; ii <= Nb_Sec + 1; ii++)
    {
      t     = (T(ii) - myfOnS) / myRatio + myf;
      T(ii) = t;
    }
    return;
  }

  NCollection_Array1<double>   IntS(1, Nb_Sec + 1);
  NCollection_Array1<double>   IntL(1, Nb_Loc + 1);
  NCollection_Sequence<double> Inter;
  double          t;

  mySec->Intervals(IntS, S);
  for (ii = 1; ii <= Nb_Sec + 1; ii++)
  {
    t        = (IntS(ii) - myfOnS) / myRatio + myf;
    IntS(ii) = t;
  }
  myLoc->Intervals(IntL, S);

  GeomLib::FuseIntervals(IntS, IntL, Inter, Precision::PConfusion() * 0.99);
  for (ii = 1; ii <= Inter.Length(); ii++)
    T(ii) = Inter(ii);
}

//=================================================================================================

void GeomFill_SweepFunction::SetInterval(const double First, const double Last)
{
  double uf, ul;
  myLoc->SetInterval(First, Last);
  uf = myf + (First - myf) * myRatio;
  ul = myf + (Last - myf) * myRatio;
  mySec->SetInterval(uf, ul);
}

//=================================================================================================

void GeomFill_SweepFunction::GetTolerance(const double   BoundTol,
                                          const double   SurfTol,
                                          const double   AngleTol,
                                          NCollection_Array1<double>& Tol3d) const
{
  mySec->GetTolerance(BoundTol, SurfTol, AngleTol, Tol3d);
}

//=================================================================================================

void GeomFill_SweepFunction::Resolution(const int Index,
                                        const double    Tol,
                                        double&         TolU,
                                        double&         TolV) const
{
  myLoc->Resolution(Index, Tol, TolU, TolV);
}

//=================================================================================================

void GeomFill_SweepFunction::SetTolerance(const double Tol3d, const double Tol2d)
{
  mySec->SetTolerance(Tol3d, Tol2d);
  myLoc->SetTolerance(Tol3d, Tol2d);
}

//=================================================================================================

gp_Pnt GeomFill_SweepFunction::BarycentreOfSurf() const
{
  gp_Pnt Bary;
  gp_Vec Translate;
  gp_Mat aM;

  Bary = mySec->BarycentreOfSurf();
  myLoc->GetAverageLaw(aM, Translate);
  Bary.ChangeCoord() *= aM;
  Bary.ChangeCoord() += Translate.XYZ();

  return Bary;
}

//=================================================================================================

double GeomFill_SweepFunction::MaximalSection() const
{
  double L = mySec->MaximalSection();
  L *= myLoc->GetMaximalNorm();
  return L;
}

//=================================================================================================

void GeomFill_SweepFunction::GetMinimalWeight(NCollection_Array1<double>& Weigths) const
{
  mySec->GetMinimalWeight(Weigths);
}
