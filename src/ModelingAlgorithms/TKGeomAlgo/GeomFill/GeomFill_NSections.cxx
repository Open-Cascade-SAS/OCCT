// Created on: 1998-12-14
// Created by: Joelle CHAUVET
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

// Modified:    Fri Jan  8 15:47:20 1999
//              enfin un calcul exact pour D1 et D2
//              le calcul par differ. finies est garde dans verifD1 et verifD2
// Modified:    Mon Jan 18 11:06:46 1999
//              mise au point de D1, D2 et IsConstant

#include <GeomFill_NSections.hxx>

#include <GCPnts_AbscissaPoint.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(GeomFill_NSections, GeomFill_SectionLaw)

#ifdef OCCT_DEBUG
static bool Affich = 0;
static int  NbSurf = 0;
#endif

#ifdef OCCT_DEBUG
// verification des fonctions de derivation D1 et D2 par differences finies
static bool verifD1(const NCollection_Array1<gp_Pnt>& P1,
                    const NCollection_Array1<double>& W1,
                    const NCollection_Array1<gp_Pnt>& P2,
                    const NCollection_Array1<double>& W2,
                    const NCollection_Array1<gp_Vec>& DPoles,
                    const NCollection_Array1<double>& DWeights,
                    const double                      pTol,
                    const double                      wTol,
                    const double                      pas)
{
  bool   ok = true;
  int    ii, L = P1.Length();
  double dw;
  gp_Vec dP;
  for (ii = 1; ii <= L; ii++)
  {
    dw = (W2(ii) - W1(ii)) / pas;
    if (std::abs(dw - DWeights(ii)) > wTol)
    {
      if (Affich)
      {
        std::cout << "erreur dans la derivee 1ere du poids pour l'indice " << ii << std::endl;
        std::cout << "par diff finies : " << dw << std::endl;
        std::cout << "resultat obtenu : " << DWeights(ii) << std::endl;
      }
      ok = false;
    }
    dP.SetXYZ((P2(ii).XYZ() - P1(ii).XYZ()) / pas);
    gp_Vec diff = dP - DPoles(ii);
    if (diff.Magnitude() > pTol)
    {
      if (Affich)
      {
        std::cout << "erreur dans la derivee 1ere du pole pour l'indice " << ii << std::endl;
        std::cout << "par diff finies : (" << dP.X() << " " << dP.Y() << " " << dP.Z() << ")"
                  << std::endl;
        std::cout << "resultat obtenu : (" << DPoles(ii).X() << " " << DPoles(ii).Y() << " "
                  << DPoles(ii).Z() << ")" << std::endl;
      }
      ok = false;
    }
  }
  return ok;
}

static bool verifD2(const NCollection_Array1<gp_Vec>& DP1,
                    const NCollection_Array1<double>& DW1,
                    const NCollection_Array1<gp_Vec>& DP2,
                    const NCollection_Array1<double>& DW2,
                    const NCollection_Array1<gp_Vec>& D2Poles,
                    const NCollection_Array1<double>& D2Weights,
                    const double                      pTol,
                    const double                      wTol,
                    const double                      pas)
{
  bool   ok = true;
  int    ii, L = DP1.Length();
  double d2w;
  gp_Vec d2P;
  for (ii = 1; ii <= L; ii++)
  {
    double dw1 = DW1(ii), dw2 = DW2(ii);
    d2w = (dw2 - dw1) / pas;
    if (std::abs(d2w - D2Weights(ii)) > wTol)
    {
      if (Affich)
      {
        std::cout << "erreur dans la derivee 2nde du poids pour l'indice " << ii << std::endl;
        std::cout << "par diff finies : " << d2w << std::endl;
        std::cout << "resultat obtenu : " << D2Weights(ii) << std::endl;
      }
      ok = false;
    }
    d2P.SetXYZ((DP2(ii).XYZ() - DP1(ii).XYZ()) / pas);
    gp_Vec diff = d2P - D2Poles(ii);
    if (diff.Magnitude() > pTol)
    {
      if (Affich)
      {
        std::cout << "erreur dans la derivee 2nde du pole pour l'indice " << ii << std::endl;
        std::cout << "par diff finies : (" << d2P.X() << " " << d2P.Y() << " " << d2P.Z() << ")"
                  << std::endl;
        std::cout << "resultat obtenu : (" << D2Poles(ii).X() << " " << D2Poles(ii).Y() << " "
                  << D2Poles(ii).Z() << ")" << std::endl;
      }
      ok = false;
    }
  }
  return ok;
}
#endif

// fonction d'evaluation des poles et des poids de mySurface pour D1 et D2
static void ResultEval(const occ::handle<Geom_BSplineSurface>& surf,
                       const double                            V,
                       const int                               deriv,
                       NCollection_Array1<double>&             Result)
{
  bool rational = surf->IsVRational();
  int  gap      = 3;
  if (rational)
    gap++;
  int Cdeg = surf->VDegree(), Cdim = surf->NbUPoles() * gap, NbP = surf->NbVPoles();

  //  les noeuds plats
  const NCollection_Array1<double>& FKnots = surf->VKnotSequence();

  //  les poles
  int                        Psize = Cdim * NbP;
  NCollection_Array1<double> SPoles(1, Psize);
  int                        ii, jj, ipole = 1;
  for (jj = 1; jj <= NbP; jj++)
  {
    for (ii = 1; ii <= surf->NbUPoles(); ii++)
    {
      SPoles(ipole)     = surf->Pole(ii, jj).X();
      SPoles(ipole + 1) = surf->Pole(ii, jj).Y();
      SPoles(ipole + 2) = surf->Pole(ii, jj).Z();
      if (rational)
      {
        SPoles(ipole + 3) = surf->Weight(ii, jj);
        SPoles(ipole) *= SPoles(ipole + 3);
        SPoles(ipole + 1) *= SPoles(ipole + 3);
        SPoles(ipole + 2) *= SPoles(ipole + 3);
      }
      ipole += gap;
    }
  }
  double* Padr = (double*)&SPoles(1);

  bool periodic_flag = false;
  int  extrap_mode[2];
  extrap_mode[0] = extrap_mode[1] = Cdeg;
  NCollection_Array1<double> EvalBS(1, Cdim * (deriv + 1));
  double*                    Eadr = (double*)&EvalBS(1);
  BSplCLib::Eval(V, periodic_flag, deriv, extrap_mode[0], Cdeg, FKnots, Cdim, *Padr, *Eadr);

  for (ii = 1; ii <= Cdim; ii++)
  {
    Result(ii) = EvalBS(ii + deriv * Cdim);
  }
}

//=================================================================================================

GeomFill_NSections::GeomFill_NSections(const NCollection_Sequence<occ::handle<Geom_Curve>>& NC)
{
  mySections = NC;
  UFirst     = 0.;
  ULast      = 1.;
  VFirst     = 0.;
  VLast      = 1.;
  myRefSurf.Nullify();
  ComputeSurface();
}

//=================================================================================================

GeomFill_NSections::GeomFill_NSections(const NCollection_Sequence<occ::handle<Geom_Curve>>& NC,
                                       const NCollection_Sequence<double>&                  NP)
{
  mySections = NC;
  myParams   = NP;
  UFirst     = 0.;
  ULast      = 1.;
  VFirst     = 0.;
  VLast      = 1.;
  myRefSurf.Nullify();
  ComputeSurface();
}

//=================================================================================================

GeomFill_NSections::GeomFill_NSections(const NCollection_Sequence<occ::handle<Geom_Curve>>& theNC,
                                       const NCollection_Sequence<double>&                  theNP,
                                       const double                                         theUF,
                                       const double                                         theUL)
{
  mySections = theNC;
  myParams   = theNP;
  UFirst     = theUF;
  ULast      = theUL;
  VFirst     = 0.0;
  VLast      = 1.0;
  myRefSurf.Nullify();
  ComputeSurface();
}

//=================================================================================================

GeomFill_NSections::GeomFill_NSections(const NCollection_Sequence<occ::handle<Geom_Curve>>& NC,
                                       const NCollection_Sequence<double>&                  NP,
                                       const double                                         UF,
                                       const double                                         UL,
                                       const double                                         VF,
                                       const double                                         VL)
{
  mySections = NC;
  myParams   = NP;
  UFirst     = UF;
  ULast      = UL;
  VFirst     = VF;
  VLast      = VL;
  myRefSurf.Nullify();
  ComputeSurface();
}

//=================================================================================================

GeomFill_NSections::GeomFill_NSections(const NCollection_Sequence<occ::handle<Geom_Curve>>& NC,
                                       const NCollection_Sequence<gp_Trsf>&                 Trsfs,
                                       const NCollection_Sequence<double>&                  NP,
                                       const double                                         UF,
                                       const double                                         UL,
                                       const double                                         VF,
                                       const double                                         VL,
                                       const occ::handle<Geom_BSplineSurface>&              Surf)
{
  mySections = NC;
  myTrsfs    = Trsfs;
  myParams   = NP;
  UFirst     = UF;
  ULast      = UL;
  VFirst     = VF;
  VLast      = VL;
  myRefSurf  = Surf;
  ComputeSurface();
}

//=======================================================
// Purpose :D0
//=======================================================
bool GeomFill_NSections::D0(const double                V,
                            NCollection_Array1<gp_Pnt>& Poles,
                            NCollection_Array1<double>& Weights)
{
  if (mySurface.IsNull())
  {
    return false;
  }
  else
  {
    occ::handle<Geom_BSplineCurve> Curve =
      occ::down_cast<Geom_BSplineCurve>(mySurface->VIso(V, false));
    const NCollection_Array1<gp_Pnt>& poles = Curve->Poles();
    const NCollection_Array1<double>* aWPtr = Curve->Weights();
    NCollection_Array1<double>        weights(1, mySurface->NbUPoles());
    if (aWPtr != nullptr)
      weights = *aWPtr;
    else
      weights.Init(1.0);
    int ii, L = Poles.Length();
    for (ii = 1; ii <= L; ii++)
    {
      Poles(ii).SetXYZ(poles(ii).XYZ());
      Weights(ii) = weights(ii);
    }
  }
  return true;
}

//=======================================================
// Purpose :D1
//=======================================================
bool GeomFill_NSections::D1(const double                V,
                            NCollection_Array1<gp_Pnt>& Poles,
                            NCollection_Array1<gp_Vec>& DPoles,
                            NCollection_Array1<double>& Weights,
                            NCollection_Array1<double>& DWeights)
{
  if (mySurface.IsNull())
    return false;

  bool ok = D0(V, Poles, Weights);
  if (!ok)
    return false;

  int  L = Poles.Length(), derivative_request = 1;
  bool rational = mySurface->IsVRational();
  int  gap      = 3;
  if (rational)
    gap++;

  int                              dimResult = mySurface->NbUPoles() * gap;
  occ::handle<Geom_BSplineSurface> surf_deper;
  if (mySurface->IsVPeriodic())
  {
    surf_deper = occ::down_cast<Geom_BSplineSurface>(mySurface->Copy());
    surf_deper->SetVNotPeriodic();
    dimResult = surf_deper->NbUPoles() * gap;
  }
  NCollection_Array1<double> Result(1, dimResult);
  if (mySurface->IsVPeriodic())
  {
    ResultEval(surf_deper, V, derivative_request, Result);
  }
  else
  {
    ResultEval(mySurface, V, derivative_request, Result);
  }

  double           ww;
  constexpr double EpsW       = 10 * Precision::PConfusion();
  bool             NullWeight = false;
  if (!rational)
    DWeights.Init(0.);
  int indice = 1, ii;

  //  recopie des poles du resultat sous forme de points 3D et de poids
  for (ii = 1; ii <= L && (!NullWeight); ii++)
  {
    DPoles(ii).SetX(Result(indice));
    DPoles(ii).SetY(Result(indice + 1));
    DPoles(ii).SetZ(Result(indice + 2));
    if (rational)
    {
      ww = Weights(ii);
      if (ww < EpsW)
      {
        NullWeight = true;
      }
      else
      {
        DWeights(ii) = Result(indice + 3);
        DPoles(ii).SetXYZ((DPoles(ii).XYZ() - DWeights(ii) * Poles(ii).Coord()) / ww);
      }
    }
    indice += gap;
  }
  return !NullWeight;
}

//=======================================================
// Purpose :D2
//=======================================================
bool GeomFill_NSections::D2(const double                V,
                            NCollection_Array1<gp_Pnt>& Poles,
                            NCollection_Array1<gp_Vec>& DPoles,
                            NCollection_Array1<gp_Vec>& D2Poles,
                            NCollection_Array1<double>& Weights,
                            NCollection_Array1<double>& DWeights,
                            NCollection_Array1<double>& D2Weights)
{
  if (mySurface.IsNull())
    return false;

  // pb dans BSplCLib::Eval() pour les surfaces rationnelles de degre 1
  // si l'ordre de derivation est egal a 2.
  if (mySurface->VDegree() < 2)
    return false;

  bool ok = D1(V, Poles, DPoles, Weights, DWeights);
  if (!ok)
    return false;

  int  L = Poles.Length(), derivative_request = 2;
  bool rational = mySurface->IsVRational();
  int  gap      = 3;
  if (rational)
    gap++;

  int                              dimResult = mySurface->NbUPoles() * gap;
  occ::handle<Geom_BSplineSurface> surf_deper;
  if (mySurface->IsVPeriodic())
  {
    surf_deper = occ::down_cast<Geom_BSplineSurface>(mySurface->Copy());
    surf_deper->SetVNotPeriodic();
    dimResult = surf_deper->NbUPoles() * gap;
  }
  NCollection_Array1<double> Result(1, dimResult);
  if (mySurface->IsVPeriodic())
  {
    ResultEval(surf_deper, V, derivative_request, Result);
  }
  else
  {
    ResultEval(mySurface, V, derivative_request, Result);
  }

  double           ww;
  constexpr double EpsW       = 10 * Precision::PConfusion();
  bool             NullWeight = false;
  if (!rational)
    D2Weights.Init(0.);
  int indice = 1, ii;

  //  recopie des poles du resultat sous forme de points 3D et de poids
  for (ii = 1; ii <= L && (!NullWeight); ii++)
  {
    D2Poles(ii).SetX(Result(indice));
    D2Poles(ii).SetY(Result(indice + 1));
    D2Poles(ii).SetZ(Result(indice + 2));
    if (rational)
    {
      ww = Weights(ii);
      if (ww < EpsW)
      {
        NullWeight = true;
      }
      else
      {
        D2Weights(ii) = Result(indice + 3);
        D2Poles(ii).SetXYZ((D2Poles(ii).XYZ() - D2Weights(ii) * Poles(ii).Coord()
                            - 2 * DWeights(ii) * DPoles(ii).XYZ())
                           / ww);
      }
    }
    indice += gap;
  }
  return !NullWeight;
}

//=======================================================
// Purpose :BSplineSurface()
//=======================================================
occ::handle<Geom_BSplineSurface> GeomFill_NSections::BSplineSurface() const
{
  return mySurface;
}

//=======================================================
// Purpose :SetSurface()
//=======================================================
void GeomFill_NSections::SetSurface(const occ::handle<Geom_BSplineSurface>& RefSurf)
{
  myRefSurf = RefSurf;
}

//=======================================================
// Purpose :ComputeSurface()
//=======================================================
void GeomFill_NSections::ComputeSurface()
{

  occ::handle<Geom_BSplineSurface> BS;
  if (myRefSurf.IsNull())
  {

    double myPres3d = 1.e-06;
    int    i, j, jdeb = 1, jfin = mySections.Length();

    if (jfin <= jdeb)
    {
      // We will not be able to create surface from single curve.
      return;
    }

    GeomFill_SectionGenerator        section;
    occ::handle<Geom_BSplineSurface> surface;

    for (j = jdeb; j <= jfin; j++)
    {

      // read the j-th curve
      occ::handle<Geom_Curve> curv = mySections(j);

      // transformation to BSpline reparametrized to [UFirst,ULast]
      occ::handle<Geom_BSplineCurve> curvBS = occ::down_cast<Geom_BSplineCurve>(curv);
      if (curvBS.IsNull())
      {
        curvBS = GeomConvert::CurveToBSplineCurve(curv, Convert_QuasiAngular);
      }

      NCollection_Array1<double> BSK(curvBS->Knots());
      BSplCLib::Reparametrize(UFirst, ULast, BSK);
      curvBS->SetKnots(BSK);

      section.AddCurve(curvBS);
    }

    /*
    if (s2Point) {
      curv =  mySections(jfin+1);
      first =  curv->FirstParameter();
      last = curv->LastParameter();
      NCollection_Array1<gp_Pnt> Extremities(1,2);
      Extremities(1) = curv->Value(first);
      Extremities(2) = curv->Value(last);
      NCollection_Array1<double> Bounds(1,2);
      Bounds(1) = UFirst;
      Bounds(2) = ULast;
      double Deg = 1;
      NCollection_Array1<int> Mult(1,2);
      Mult(1) = (int ) Deg+1;
      Mult(2) = (int ) Deg+1;
      occ::handle<Geom_BSplineCurve> BSPoint
        = new Geom_BSplineCurve(Extremities,Bounds,Mult,(int ) Deg);
      section.AddCurve(BSPoint);
    }*/

    int Nbcurves = mySections.Length();
    int Nbpar    = myParams.Length();
    if (Nbpar > 0)
    {
      occ::handle<NCollection_HArray1<double>> HPar = new NCollection_HArray1<double>(1, Nbpar);
      for (i = 1; i <= Nbpar; i++)
      {
        HPar->SetValue(i, myParams(i));
      }
      section.SetParam(HPar);
    }
    section.Perform(Precision::PConfusion());

    occ::handle<GeomFill_Line> line = new GeomFill_Line(Nbcurves);
    int                        nbIt = 0, degmin = 2, degmax = 6;
    bool                       knownP = Nbpar > 0;
    GeomFill_AppSurf           anApprox(degmin, degmax, myPres3d, myPres3d, nbIt, knownP);
    anApprox.SetContinuity(GeomAbs_C1);
    bool SpApprox = true;
    anApprox.Perform(line, section, SpApprox);

    BS = new Geom_BSplineSurface(anApprox.SurfPoles(),
                                 anApprox.SurfWeights(),
                                 anApprox.SurfUKnots(),
                                 anApprox.SurfVKnots(),
                                 anApprox.SurfUMults(),
                                 anApprox.SurfVMults(),
                                 anApprox.UDegree(),
                                 anApprox.VDegree(),
                                 section.IsPeriodic());
  }

  else
  {

    // segmentation de myRefSurf
    double Ui1, Ui2, V0, V1;
    BS  = occ::down_cast<Geom_BSplineSurface>(myRefSurf->Copy());
    Ui1 = UFirst;
    Ui2 = ULast;
    int i1, i2;
    myRefSurf->LocateU(Ui1, Precision::PConfusion(), i1, i2);
    if (std::abs(Ui1 - myRefSurf->UKnot(i1)) <= Precision::PConfusion())
      Ui1 = myRefSurf->UKnot(i1);
    if (std::abs(Ui1 - myRefSurf->UKnot(i2)) <= Precision::PConfusion())
      Ui1 = myRefSurf->UKnot(i2);
    myRefSurf->LocateU(Ui2, Precision::PConfusion(), i1, i2);
    if (std::abs(Ui2 - myRefSurf->UKnot(i1)) <= Precision::PConfusion())
      Ui2 = myRefSurf->UKnot(i1);
    if (std::abs(Ui2 - myRefSurf->UKnot(i2)) <= Precision::PConfusion())
      Ui2 = myRefSurf->UKnot(i2);
    V0 = myRefSurf->VKnot(myRefSurf->FirstVKnotIndex());
    V1 = myRefSurf->VKnot(myRefSurf->LastVKnotIndex());
    BS->CheckAndSegment(Ui1, Ui2, V0, V1);
  }
  mySurface = BS;
  // On augmente le degre pour que le positionnement D2 soit correct
  if (mySurface->VDegree() < 2)
  {
    mySurface->IncreaseDegree(mySurface->UDegree(), 2);
  }
#ifdef OCCT_DEBUG
  NbSurf++;
  if (Affich)
  {
  }
#endif
}

//=======================================================
// Purpose :SectionShape
//=======================================================
void GeomFill_NSections::SectionShape(int& NbPoles, int& NbKnots, int& Degree) const
{
  if (mySurface.IsNull())
    return;

  NbPoles = mySurface->NbUPoles();
  NbKnots = mySurface->NbUKnots();
  Degree  = mySurface->UDegree();
}

//=======================================================
// Purpose :Knots
//=======================================================
void GeomFill_NSections::Knots(NCollection_Array1<double>& TKnots) const
{
  if (!mySurface.IsNull())
    TKnots = mySurface->UKnots();
}

//=======================================================
// Purpose :Mults
//=======================================================
void GeomFill_NSections::Mults(NCollection_Array1<int>& TMults) const
{
  if (!mySurface.IsNull())
    TMults = mySurface->UMultiplicities();
}

//=======================================================
// Purpose :IsRational
//=======================================================
bool GeomFill_NSections::IsRational() const
{
  if (!mySurface.IsNull())
    return mySurface->IsURational();

  return false;
}

//=======================================================
// Purpose :IsUPeriodic
//=======================================================
bool GeomFill_NSections::IsUPeriodic() const
{
  if (!mySurface.IsNull())
    return mySurface->IsUPeriodic();

  return false;
}

//=======================================================
// Purpose :IsVPeriodic
//=======================================================
bool GeomFill_NSections::IsVPeriodic() const
{
  if (!mySurface.IsNull())
    return mySurface->IsVPeriodic();

  return false;
}

//=======================================================
// Purpose :NbIntervals
//=======================================================
int GeomFill_NSections::NbIntervals(const GeomAbs_Shape S) const
{
  if (mySurface.IsNull())
    return 0;

  GeomAdaptor_Surface AdS(mySurface);
  return AdS.NbVIntervals(S);
}

//=======================================================
// Purpose :Intervals
//=======================================================
void GeomFill_NSections::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  if (mySurface.IsNull())
    return;

  GeomAdaptor_Surface AdS(mySurface);
  AdS.VIntervals(T, S);
}

//=======================================================
// Purpose : SetInterval
//=======================================================
// void GeomFill_NSections::SetInterval(const double F,
void GeomFill_NSections::SetInterval(const double,
                                     //                                           const double L)
                                     const double)
{
  // rien a faire : mySurface est supposee Cn en V
}

//=======================================================
// Purpose : GetInterval
//=======================================================
void GeomFill_NSections::GetInterval(double& F, double& L) const
{
  F = VFirst;
  L = VLast;
}

//=======================================================
// Purpose : GetDomain
//=======================================================
void GeomFill_NSections::GetDomain(double& F, double& L) const
{
  F = VFirst;
  L = VLast;
}

//=======================================================
// Purpose : GetTolerance
//=======================================================
void GeomFill_NSections::GetTolerance(
  const double BoundTol,
  const double SurfTol,
  //                                            const double AngleTol,
  const double,
  NCollection_Array1<double>& Tol3d) const
{
  Tol3d.Init(SurfTol);
  if (BoundTol < SurfTol)
  {
    Tol3d(Tol3d.Lower()) = BoundTol;
    Tol3d(Tol3d.Upper()) = BoundTol;
  }
}

//=======================================================
// Purpose : BarycentreOfSurf
//=======================================================
gp_Pnt GeomFill_NSections::BarycentreOfSurf() const
{
  gp_Pnt P, Bary;
  Bary.SetCoord(0., 0., 0.);

  if (mySurface.IsNull())
    return Bary;

  int    ii, jj;
  double U0, U1, V0, V1;
  mySurface->Bounds(U0, U1, V0, V1);
  double V = V0, DeltaV = (V1 - V0) / 20;
  double U = U0, DeltaU = (U1 - U0) / 20;
  for (jj = 0; jj <= 20; jj++, V += DeltaV)
  {
    for (ii = 0; ii <= 20; ii++, U += DeltaU)
    {
      P = mySurface->Value(U, V);
      Bary.ChangeCoord() += P.XYZ();
    }
  }

  Bary.ChangeCoord() /= (21 * 21);
  return Bary;
}

//=======================================================
// Purpose : MaximalSection
//=======================================================
double GeomFill_NSections::MaximalSection() const
{
  double L, Lmax = 0.;
  int    ii;
  for (ii = 1; ii <= mySections.Length(); ii++)
  {
    GeomAdaptor_Curve AC(mySections(ii));
    L = GCPnts_AbscissaPoint::Length(AC);
    if (L > Lmax)
      Lmax = L;
  }
  return Lmax;
}

//=======================================================
// Purpose : GetMinimalWeight
//=======================================================
void GeomFill_NSections::GetMinimalWeight(NCollection_Array1<double>& Weights) const
{
  if (mySurface.IsNull())
    return;

  if (mySurface->IsURational())
  {
    int                               NbU = mySurface->NbUPoles(), NbV = mySurface->NbVPoles();
    const NCollection_Array2<double>* aWSurfPtr = mySurface->Weights();
    if (aWSurfPtr == nullptr)
    {
      Weights.Init(1);
      return;
    }
    const NCollection_Array2<double>& WSurf = *aWSurfPtr;
    int                               i, j;
    for (i = 1; i <= NbU; i++)
    {
      double min = WSurf(i, 1);
      for (j = 2; j <= NbV; j++)
      {
        if (min > WSurf(i, j))
          min = WSurf(i, j);
      }
      Weights.SetValue(i, min);
    }
  }
  else
  {
    Weights.Init(1);
  }
}

//=======================================================
// Purpose : IsConstant
//=======================================================
bool GeomFill_NSections::IsConstant(double& Error) const
{
  // on se limite a 2 sections
  bool   isconst = (mySections.Length() == 2);
  double Err     = 0.;

  if (isconst)
  {
    GeomAdaptor_Curve AC1(mySections(1));
    GeomAbs_CurveType CType = AC1.GetType();
    GeomAdaptor_Curve AC2(mySections(2));
    // les sections doivent avoir le meme type
    isconst = (AC2.GetType() == CType);

    if (isconst)
    {
      if (CType == GeomAbs_Circle)
      {
        gp_Circ C1  = AC1.Circle();
        gp_Circ C2  = AC2.Circle();
        double  Tol = 1.e-7;
        bool    samedir, samerad, samepos;
        samedir = (C1.Axis().IsParallel(C2.Axis(), 1.e-4));
        samerad = (std::abs(C1.Radius() - C2.Radius()) < Tol);
        samepos = (C1.Location().Distance(C2.Location()) < Tol);
        if (!samepos)
        {
          gp_Ax1 D(C1.Location(), gp_Vec(C1.Location(), C2.Location()));
          samepos = (C1.Axis().IsParallel(D, 1.e-4));
        }
        isconst = samedir && samerad && samepos;
      }
      else if (CType == GeomAbs_Line)
      {
        gp_Lin L1  = AC1.Line();
        gp_Lin L2  = AC2.Line();
        double Tol = 1.e-7;
        bool   samedir, samelength, samepos;
        samedir    = (L1.Direction().IsParallel(L2.Direction(), 1.e-4));
        gp_Pnt P11 = AC1.Value(AC1.FirstParameter()), P12 = AC1.Value(AC1.LastParameter()),
               P21 = AC2.Value(AC2.FirstParameter()), P22 = AC2.Value(AC2.LastParameter());
        samelength = (std::abs(P11.Distance(P12) - P21.Distance(P22)) < Tol);
        // l'ecart entre les 2 sections ne compte pas
        samepos = ((P11.Distance(P21) < Tol && P12.Distance(P22) < Tol)
                   || (P12.Distance(P21) < Tol && P11.Distance(P22) < Tol));
        // samepos = true;
        isconst = samedir && samelength && samepos;
      }
      else
      {
        isconst = false;
      }
    }
  }

  Error = Err;
  return isconst;
}

//=======================================================
// Purpose : ConstantSection
//=======================================================
occ::handle<Geom_Curve> GeomFill_NSections::ConstantSection() const
{
  //  double Err;
  //  if (!IsConstant(Err)) throw StdFail_NotDone("The Law is not Constant!");
  occ::handle<Geom_Curve> C;
  C = occ::down_cast<Geom_Curve>(mySections(1)->Copy());
  return C;
}

//=======================================================
// Purpose : IsConicalLaw
//=======================================================
bool GeomFill_NSections::IsConicalLaw(double& Error) const
{
  bool   isconic = (mySections.Length() == 2);
  double Err     = 0.;
  if (isconic)
  {
    GeomAdaptor_Curve AC1(mySections(1));
    GeomAdaptor_Curve AC2(mySections(2));
    isconic = (AC1.GetType() == GeomAbs_Circle) && (AC2.GetType() == GeomAbs_Circle);
    if (isconic)
    {
      gp_Circ C1 = AC1.Circle();
      if (!myTrsfs.IsEmpty())
        C1.Transform(myTrsfs(1).Inverted());
      gp_Circ C2 = AC2.Circle();
      if (!myTrsfs.IsEmpty())
        C2.Transform(myTrsfs(2).Inverted());
      double Tol = 1.e-7;
      // bool samedir, linearrad, sameaxis;
      isconic = (C1.Axis().IsParallel(C2.Axis(), 1.e-4));
      //  pour 2 sections, la variation du rayon est forcement lineaire
      // linearrad = true;
      //  formule plus generale pour 3 sections au moins
      //  double param0 = C2.Radius()*myParams(1) - C1.Radius()*myParams(2);
      //  param0 = param0 / (C2.Radius()-C1.Radius()) ;
      //  linearrad = ( std::abs( C3.Radius()*myParams(1)-C1.Radius()*myParams(3)
      //                          - param0*(C3.Radius()-C1.Radius()) ) < Tol);
      if (isconic)
      {
        gp_Lin Line1(C1.Axis());
        isconic = (Line1.Distance(C2.Location()) < Tol);
        /*
        sameaxis = (C1.Location().Distance(C2.Location())<Tol);
        if (!sameaxis) {
        gp_Ax1 D(C1.Location(),gp_Vec(C1.Location(),C2.Location()));
        sameaxis = (C1.Axis().IsParallel(D,1.e-4));
        }
        isconic = samedir && linearrad && sameaxis;
        */
        if (isconic)
        {
          //// Modified by jgv, 18.02.2009 for OCC20866 ////
          double first1 = AC1.FirstParameter(), last1 = AC1.LastParameter();
          double first2 = AC2.FirstParameter(), last2 = AC2.LastParameter();
          isconic = (std::abs(first1 - first2) <= Precision::PConfusion()
                     && std::abs(last1 - last2) <= Precision::PConfusion());
          //////////////////////////////////////////////////
        }
      }
    }
  }

  Error = Err;
  return isconic;
}

//=======================================================
// Purpose : CirclSection
//=======================================================
occ::handle<Geom_Curve> GeomFill_NSections::CirclSection(const double V) const
{
  double Err;
  if (!IsConicalLaw(Err))
    throw StdFail_NotDone("The Law is not Conical!");

  GeomAdaptor_Curve AC1(mySections(1));
  GeomAdaptor_Curve AC2(mySections(mySections.Length()));
  gp_Circ           C1 = AC1.Circle();
  gp_Circ           C2 = AC2.Circle();

  double p1 = myParams(1), p2 = myParams(myParams.Length());
  double radius = (C2.Radius() - C1.Radius()) * (V - p1) / (p2 - p1) + C1.Radius();

  C1.SetRadius(radius);
  occ::handle<Geom_Curve> C = new (Geom_Circle)(C1);

  const double aParF   = AC1.FirstParameter();
  const double aParL   = AC1.LastParameter();
  const double aPeriod = AC1.IsPeriodic() ? AC1.Period() : 0.0;

  if ((aPeriod == 0.0) || (std::abs(aParL - aParF - aPeriod) > Precision::PConfusion()))
  {
    occ::handle<Geom_Curve> Cbis = new Geom_TrimmedCurve(C, aParF, aParL);
    C                            = Cbis;
  }
  return C;
}
