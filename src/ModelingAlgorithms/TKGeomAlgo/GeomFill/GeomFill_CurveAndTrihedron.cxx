// Created on: 1997-12-05
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

#include <GeomFill_CurveAndTrihedron.hxx>

#include <Adaptor3d_Curve.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <GeomLib.hxx>
#include <gp_Circ.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Sequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_CurveAndTrihedron, GeomFill_LocationLaw)

//=================================================================================================

GeomFill_CurveAndTrihedron::GeomFill_CurveAndTrihedron(
  const occ::handle<GeomFill_TrihedronLaw>& Trihedron)
{
  myLaw = Trihedron;
  myCurve.Nullify();
  Trans.SetIdentity();
  WithTrans = false;
}

//=================================================================================================

occ::handle<GeomFill_LocationLaw> GeomFill_CurveAndTrihedron::Copy() const
{
  occ::handle<GeomFill_TrihedronLaw> law;
  law                                     = myLaw->Copy();
  occ::handle<GeomFill_CurveAndTrihedron> copy = new (GeomFill_CurveAndTrihedron)(myLaw->Copy());
  copy->SetCurve(myCurve);
  copy->SetTrsf(Trans);
  return copy;
}

//=================================================================================================

bool GeomFill_CurveAndTrihedron::SetCurve(const occ::handle<Adaptor3d_Curve>& C)
{
  myCurve   = C;
  myTrimmed = C;
  return myLaw->SetCurve(C);
}

const occ::handle<Adaptor3d_Curve>& GeomFill_CurveAndTrihedron::GetCurve() const
{
  return myCurve;
}

//=================================================================================================

void GeomFill_CurveAndTrihedron::SetTrsf(const gp_Mat& Transfo)
{
  Trans = Transfo;
  gp_Mat Aux;
  Aux.SetIdentity();
  Aux -= Trans;
  WithTrans = false; // Au cas ou Trans = I
  for (int ii = 1; ii <= 3 && !WithTrans; ii++)
    for (int jj = 1; jj <= 3 && !WithTrans; jj++)
      if (std::abs(Aux.Value(ii, jj)) > 1.e-14)
        WithTrans = true;
}

//=================================================================================================

bool GeomFill_CurveAndTrihedron::D0(const double Param, gp_Mat& M, gp_Vec& V)
{
  bool Ok;
  myTrimmed->D0(Param, Point);
  V.SetXYZ(Point.XYZ());

  Ok = myLaw->D0(Param, V1, V2, V3);
  M.SetCols(V2.XYZ(), V3.XYZ(), V1.XYZ());

  if (WithTrans)
  {
    M *= Trans;
  }
  return Ok;
}

//=================================================================================================

bool GeomFill_CurveAndTrihedron::D0(const double Param,
                                                gp_Mat&             M,
                                                gp_Vec&             V,
                                                NCollection_Array1<gp_Pnt2d>&)
{
  bool Ok;
  myTrimmed->D0(Param, Point);
  V.SetXYZ(Point.XYZ());

  Ok = myLaw->D0(Param, V1, V2, V3);
  M.SetCols(V2.XYZ(), V3.XYZ(), V1.XYZ());

  if (WithTrans)
  {
    M *= Trans;
  }
  return Ok;
}

//=================================================================================================

bool GeomFill_CurveAndTrihedron::D1(const double Param,
                                                gp_Mat&             M,
                                                gp_Vec&             V,
                                                gp_Mat&             DM,
                                                gp_Vec&             DV,
                                                NCollection_Array1<gp_Pnt2d>&,
                                                NCollection_Array1<gp_Vec2d>&)
{
  bool Ok;
  myTrimmed->D1(Param, Point, DV);
  V.SetXYZ(Point.XYZ());

  gp_Vec DV1, DV2, DV3;
  Ok = myLaw->D1(Param, V1, DV1, V2, DV2, V3, DV3);
  M.SetCols(V2.XYZ(), V3.XYZ(), V1.XYZ());
  DM.SetCols(DV2.XYZ(), DV3.XYZ(), DV1.XYZ());

  if (WithTrans)
  {
    M *= Trans;
    DM *= Trans;
  }

  return Ok;
}

//=================================================================================================

bool GeomFill_CurveAndTrihedron::D2(const double Param,
                                                gp_Mat&             M,
                                                gp_Vec&             V,
                                                gp_Mat&             DM,
                                                gp_Vec&             DV,
                                                gp_Mat&             D2M,
                                                gp_Vec&             D2V,
                                                NCollection_Array1<gp_Pnt2d>&,
                                                NCollection_Array1<gp_Vec2d>&,
                                                NCollection_Array1<gp_Vec2d>&)
{
  bool Ok;
  myTrimmed->D2(Param, Point, DV, D2V);
  V.SetXYZ(Point.XYZ());

  gp_Vec DV1, DV2, DV3;
  gp_Vec D2V1, D2V2, D2V3;
  Ok = myLaw->D2(Param, V1, DV1, D2V1, V2, DV2, D2V2, V3, DV3, D2V3);

  M.SetCols(V2.XYZ(), V3.XYZ(), V1.XYZ());
  DM.SetCols(DV2.XYZ(), DV3.XYZ(), DV1.XYZ());
  D2M.SetCols(D2V2.XYZ(), D2V3.XYZ(), D2V1.XYZ());

  if (WithTrans)
  {
    M *= Trans;
    DM *= Trans;
    D2M *= Trans;
  }

  return Ok;
}

//=================================================================================================

int GeomFill_CurveAndTrihedron::NbIntervals(const GeomAbs_Shape S) const
{
  int Nb_Sec, Nb_Law;
  Nb_Sec = myTrimmed->NbIntervals(S);
  Nb_Law = myLaw->NbIntervals(S);

  if (Nb_Sec == 1)
  {
    return Nb_Law;
  }
  else if (Nb_Law == 1)
  {
    return Nb_Sec;
  }

  NCollection_Array1<double>   IntC(1, Nb_Sec + 1);
  NCollection_Array1<double>   IntL(1, Nb_Law + 1);
  NCollection_Sequence<double> Inter;
  myTrimmed->Intervals(IntC, S);
  myLaw->Intervals(IntL, S);

  GeomLib::FuseIntervals(IntC, IntL, Inter, Precision::PConfusion() * 0.99);
  return Inter.Length() - 1;
}

//=================================================================================================

void GeomFill_CurveAndTrihedron::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  int Nb_Sec, Nb_Law;
  Nb_Sec = myTrimmed->NbIntervals(S);
  Nb_Law = myLaw->NbIntervals(S);

  if (Nb_Sec == 1)
  {
    myLaw->Intervals(T, S);
    return;
  }
  else if (Nb_Law == 1)
  {
    myTrimmed->Intervals(T, S);
    return;
  }

  NCollection_Array1<double>   IntC(1, Nb_Sec + 1);
  NCollection_Array1<double>   IntL(1, Nb_Law + 1);
  NCollection_Sequence<double> Inter;
  myTrimmed->Intervals(IntC, S);
  myLaw->Intervals(IntL, S);

  GeomLib::FuseIntervals(IntC, IntL, Inter, Precision::PConfusion() * 0.99);
  for (int ii = 1; ii <= Inter.Length(); ii++)
    T(ii) = Inter(ii);
}

//=================================================================================================

void GeomFill_CurveAndTrihedron::SetInterval(const double First, const double Last)
{
  myLaw->SetInterval(First, Last);
  myTrimmed = myCurve->Trim(First, Last, 0);
}

//=================================================================================================

void GeomFill_CurveAndTrihedron::GetInterval(double& First, double& Last) const
{
  First = myTrimmed->FirstParameter();
  Last  = myTrimmed->LastParameter();
}

//=================================================================================================

void GeomFill_CurveAndTrihedron::GetDomain(double& First, double& Last) const
{
  First = myCurve->FirstParameter();
  Last  = myCurve->LastParameter();
}

//==================================================================
// Function:
// Purpose : GetMaximalNorm
//          On suppose les triedre normee => return 1
//==================================================================
double GeomFill_CurveAndTrihedron::GetMaximalNorm()
{
  return 1.;
}

//=================================================================================================

void GeomFill_CurveAndTrihedron::GetAverageLaw(gp_Mat& AM, gp_Vec& AV)
{
  int ii;
  double    U, delta;
  gp_Vec           V;

  myLaw->GetAverageLaw(V1, V2, V3);
  AM.SetCols(V1.XYZ(), V2.XYZ(), V3.XYZ());

  AV.SetCoord(0., 0., 0.);
  delta = (myTrimmed->LastParameter() - myTrimmed->FirstParameter()) / 10;
  U     = myTrimmed->FirstParameter();
  for (ii = 0; ii <= 10; ii++, U += delta)
  {
    V.SetXYZ(myTrimmed->Value(U).XYZ());
    AV += V;
  }
  AV /= 11;
}

//=================================================================================================

bool GeomFill_CurveAndTrihedron::IsTranslation(double& Error) const
{
  GeomAbs_CurveType Type;
  Error = 0;
  Type  = myCurve->GetType();
  if (Type == GeomAbs_Line)
  {
    return (myLaw->IsConstant() || myLaw->IsOnlyBy3dCurve());
  }
  return false;
}

//=================================================================================================

bool GeomFill_CurveAndTrihedron::IsRotation(double& Error) const
{
  GeomAbs_CurveType Type;
  Error = 0;
  Type  = myCurve->GetType();
  if (Type == GeomAbs_Circle)
  {
    return myLaw->IsOnlyBy3dCurve();
  }
  return false;
}

//=================================================================================================

void GeomFill_CurveAndTrihedron::Rotation(gp_Pnt& Centre) const
{
  //  GeomAbs_CurveType Type;
  //  Type = myCurve->GetType();
  Centre = myCurve->Circle().Location();
}
