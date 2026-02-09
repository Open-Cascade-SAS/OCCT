// Created on: 1994-02-28
// Created by: Bruno DUMORTIER
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

#include <Adaptor3d_Curve.hxx>
#include <ElCLib.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomConvert.hxx>
#include <GeomFill_Profiler.hxx>
#include <GeomFill_SweepSectionGenerator.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_RangeError.hxx>
#include <NCollection_Array1.hxx>

#include <cstdio>

//=================================================================================================

GeomFill_SweepSectionGenerator::GeomFill_SweepSectionGenerator()
    : myRadius(0.0),
      myIsDone(false),
      myNbSections(0),
      myType(-1),
      myPolynomial(false)
{
}

//=================================================================================================

GeomFill_SweepSectionGenerator::GeomFill_SweepSectionGenerator(const occ::handle<Geom_Curve>& Path,
                                                               const double Radius)
{
  Init(Path, Radius);
}

//=================================================================================================

GeomFill_SweepSectionGenerator::GeomFill_SweepSectionGenerator(
  const occ::handle<Geom_Curve>& Path,
  const occ::handle<Geom_Curve>& FirstSect)
{
  Init(Path, FirstSect);
}

//=================================================================================================

GeomFill_SweepSectionGenerator::GeomFill_SweepSectionGenerator(
  const occ::handle<Geom_Curve>& Path,
  const occ::handle<Geom_Curve>& FirstSect,
  const occ::handle<Geom_Curve>& LastSect)
{
  Init(Path, FirstSect, LastSect);
}

//=================================================================================================

GeomFill_SweepSectionGenerator::GeomFill_SweepSectionGenerator(
  const occ::handle<Adaptor3d_Curve>& Path,
  const occ::handle<Adaptor3d_Curve>& Curve1,
  const occ::handle<Adaptor3d_Curve>& Curve2,
  const double                        Radius)
{
  Init(Path, Curve1, Curve2, Radius);
}

//=================================================================================================

void GeomFill_SweepSectionGenerator::Init(const occ::handle<Geom_Curve>& Path, const double Radius)
{
  myIsDone = false;
  myRadius = Radius;
  GeomAdaptor_Curve ThePath(Path);

  if (ThePath.GetType() == GeomAbs_Circle)
  {

    myCircPathAxis = ThePath.Circle().Axis();
    myType         = 4;
  }
  else
    myType = 1;
  if (Path->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    myPath = occ::down_cast<Geom_BSplineCurve>(Path->Copy());
  }
  else
  {
    myPath = GeomConvert::CurveToBSplineCurve(Path);
  }
}

//=================================================================================================

void GeomFill_SweepSectionGenerator::Init(const occ::handle<Geom_Curve>& Path,
                                          const occ::handle<Geom_Curve>& FirstSect)
{
  myIsDone = false;
  myRadius = 0;
  GeomAdaptor_Curve ThePath(Path);

  if (ThePath.GetType() == GeomAbs_Circle)
  {
    myCircPathAxis = ThePath.Circle().Axis();
    myType         = 5;
  }

  else
    myType = 2;

  if (Path->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    myPath = occ::down_cast<Geom_BSplineCurve>(Path->Copy());
  }
  else
  {
    myPath = GeomConvert::CurveToBSplineCurve(Path);
  }
  if (FirstSect->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    myFirstSect = occ::down_cast<Geom_BSplineCurve>(FirstSect->Copy());
  }
  else
  {
    // JAG
    myFirstSect = GeomConvert::CurveToBSplineCurve(FirstSect, Convert_QuasiAngular);
  }
  if (myFirstSect->IsPeriodic())
    myFirstSect->SetNotPeriodic();
}

//=================================================================================================

void GeomFill_SweepSectionGenerator::Init(const occ::handle<Geom_Curve>& Path,
                                          const occ::handle<Geom_Curve>& FirstSect,
                                          const occ::handle<Geom_Curve>& LastSect)
{
  myIsDone = false;
  myRadius = 0;
  GeomAdaptor_Curve ThePath(Path);

  if (ThePath.GetType() == GeomAbs_Circle)
  {

    myCircPathAxis = ThePath.Circle().Axis();
    myType         = 6;
  }
  else
    myType = 3;

  if (Path->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    myPath = occ::down_cast<Geom_BSplineCurve>(Path->Copy());
  }
  else
  {
    myPath = GeomConvert::CurveToBSplineCurve(Path);
  }

  // JAG
  if (FirstSect->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    myFirstSect = occ::down_cast<Geom_BSplineCurve>(FirstSect->Copy());
  }
  else
  {
    myFirstSect = GeomConvert::CurveToBSplineCurve(FirstSect, Convert_QuasiAngular);
  }
  if (LastSect->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    myLastSect = occ::down_cast<Geom_BSplineCurve>(LastSect->Copy());
  }
  else
  {
    myLastSect = GeomConvert::CurveToBSplineCurve(LastSect, Convert_QuasiAngular);
  }

  if (myFirstSect->IsPeriodic())
    myFirstSect->SetNotPeriodic();
  if (myLastSect->IsPeriodic())
    myLastSect->SetNotPeriodic();

  // JAG

  GeomFill_Profiler Profil;
  Profil.AddCurve(myFirstSect);
  Profil.AddCurve(myLastSect);
  Profil.Perform(Precision::Confusion());

  myFirstSect = occ::down_cast<Geom_BSplineCurve>(Profil.Curve(1));
  myLastSect  = occ::down_cast<Geom_BSplineCurve>(Profil.Curve(2));
}

//=================================================================================================

void GeomFill_SweepSectionGenerator::Init(const occ::handle<Adaptor3d_Curve>& Path,
                                          const occ::handle<Adaptor3d_Curve>& Curve1,
                                          const occ::handle<Adaptor3d_Curve>& Curve2,
                                          const double                        Radius)
{
  myIsDone = false;
  myRadius = Radius;
  myType   = 0;

  occ::handle<Geom_Curve> CC = GeomAdaptor::MakeCurve(*Path);
  myPath                     = GeomConvert::CurveToBSplineCurve(CC);
  myAdpPath                  = Path;
  myAdpFirstSect             = Curve1;
  myAdpLastSect              = Curve2;
}

//=================================================================================================

void GeomFill_SweepSectionGenerator::Perform(const bool Polynomial)
{
  myPolynomial = Polynomial;

  // eval myNbSections.
  int NSpans = myPath->NbKnots() - 1;

  myNbSections = 21 * NSpans;

  double U;

  double U1 = myPath->FirstParameter();
  double U2 = myPath->LastParameter();

  GCPnts_QuasiUniformDeflection Samp;
  // Calcul de la longueur approximative de la courbe
  GeomAdaptor_Curve AdpPath(myPath);
  gp_Pnt            P1     = AdpPath.Value(U1);
  gp_Pnt            P2     = AdpPath.Value((U1 + U2) / 2.);
  gp_Pnt            P3     = AdpPath.Value(U2);
  double            Length = P1.Distance(P2) + P2.Distance(P3);
  double            Fleche = 1.e-5 * Length;
  Samp.Initialize(AdpPath, Fleche);

  if (Samp.IsDone() && (Samp.NbPoints() > myNbSections))
  {
    myNbSections = Samp.NbPoints();
  }
  // the transformations are calculate on differents points of <myPath>
  // corresponding to the path parameter uniformly reparted.
  double                     DeltaU = (U2 - U1) / (double)(myNbSections - 1);
  NCollection_Array1<double> Parameters(1, myNbSections);
  //  Parameters(1) = U1;
  //  for (int i = 2; i < myNbSections; i++) {
  //    Parameters(i) = U1 + (i-1) * DeltaU;
  //  }
  //  Parameters(myNbSections) = U2;

  Parameters(1) = 0.;
  for (int i = 2; i < myNbSections; i++)
  {
    Parameters(i) = (i - 1) * DeltaU;
  }
  Parameters(myNbSections) = U2 - U1;

  gp_Vec  D1Ref, D1;
  gp_Pnt  PRef, P;
  gp_Trsf TR, cumulTR, Trans;

  myPath->D1(U1, PRef, D1Ref);

  if ((myType == 1) || (myType == 4))
  {
    // We create a circle with radius <myRadius>. This axis is create with
    // main direction <DRef> (first derivate vector of <myPath> on the first
    // point <PRef> ). This circle is, after transform to BSpline curve,
    // put in <myFirstSect>.

    gp_Ax2 CircleAxis(PRef, D1Ref);
    /*
        occ::handle<Geom_Circle> Circ = new Geom_Circle( CircleAxis, myRadius);

        myFirstSect = GeomConvert::CurveToBSplineCurve(Circ);
        // le cercle est segmente car AppBlend_AppSurf ne gere
        // pas les courbes periodiques.
        myFirstSect->Segment(0., 2.*M_PI);
    */
    occ::handle<Geom_TrimmedCurve> Circ =
      new Geom_TrimmedCurve(new Geom_Circle(CircleAxis, myRadius), 0., 2. * M_PI);

    myFirstSect = GeomConvert::CurveToBSplineCurve(Circ, Convert_QuasiAngular);
  }

  if (myType <= 3 && myType >= 1)
  {

    for (int i = 2; i <= myNbSections; i++)
    {

      U = Parameters(i) + U1;
      if (i == myNbSections)
        U = U2;

      myPath->D1(U, P, D1);

      // Eval the translation between the (i-1) section and the i-th.
      Trans.SetTranslation(PRef, P);

      gp_Trsf Rot;
      if (!D1Ref.IsParallel(D1, Precision::Angular()))
      {
        // Eval the Rotation between (i-1) section and the i-th.
        Rot.SetRotation(gp_Ax1(P, gp_Dir(D1Ref ^ D1)), D1Ref.AngleWithRef(D1, D1Ref ^ D1));
      }
      else if (D1Ref.IsOpposite(D1, Precision::Angular()))
#ifdef OCCT_DEBUG
        std::cout << "Que fais-je ???? " << std::endl;
#endif

      // TR is the transformation between (i-1) section and the i-th.
      TR = Rot * Trans;
      // cumulTR is the transformation between <myFirstSec> and
      // the i-th section.
      cumulTR = TR * cumulTR;

      myTrsfs.Append(cumulTR);

      PRef  = P;
      D1Ref = D1;
    }
  }
  else if (myType != 0)
  {
    for (int i = 2; i <= myNbSections; i++)
    {
      cumulTR.SetRotation(myCircPathAxis, Parameters(i));
      myTrsfs.Append(cumulTR);
    }
  }

  myIsDone = true;
}

//=================================================================================================

void GeomFill_SweepSectionGenerator::GetShape(int& NbPoles,
                                              int& NbKnots,
                                              int& Degree,
                                              int& NbPoles2d) const
{
  /*
   if ( myType == 1) {
      NbPoles   = 7;
      NbKnots   = 4;
      Degree    = 2;
    }
    else {
  */
  if (myType != 0)
  {
    NbPoles = myFirstSect->NbPoles();
    NbKnots = myFirstSect->NbKnots();
    Degree  = myFirstSect->Degree();
  }
  else
  { // myType == 0
    NbPoles = 7;
    NbKnots = 2;
    Degree  = 6;
  }
  NbPoles2d = 0;
}

//=================================================================================================

void GeomFill_SweepSectionGenerator::Knots(NCollection_Array1<double>& TKnots) const
{
  /*
    if (myType == 1) {
      double U = 2.*M_PI/3.;
      for ( int i = 1; i <= 4; i++)
        TKnots(i) = ( i-1) * U;
    }
    else {
  */
  if (myType != 0)
  {
    TKnots = myFirstSect->Knots();
  }
  else
  {
    TKnots(1) = 0.;
    TKnots(2) = 1.;
  }
  //  }
}

//=================================================================================================

void GeomFill_SweepSectionGenerator::Mults(NCollection_Array1<int>& TMults) const
{
  /*
    if ( myType == 1) {
      TMults( 1) = TMults( 4) = 3;
      TMults( 2) = TMults( 3) = 2;
    }
    else {
  */
  if (myType != 0)
  {
    TMults = myFirstSect->Multiplicities();
  }
  else
  {
    TMults(1) = TMults(2) = 7;
  }
  //  }
}

//=================================================================================================

bool GeomFill_SweepSectionGenerator::Section(const int                     P,
                                             NCollection_Array1<gp_Pnt>&   Poles,
                                             NCollection_Array1<gp_Vec>&   DPoles,
                                             NCollection_Array1<gp_Pnt2d>& Poles2d,
                                             NCollection_Array1<gp_Vec2d>&, // DPoles2d,
                                             NCollection_Array1<double>& Weigths,
                                             NCollection_Array1<double>& DWeigths) const
{
  Section(P, Poles, Poles2d, Weigths);

  // pour les tuyaux sur aretes pour l'instant on ne calcule pas les derivees
  if (myType == 0)
    return false; // a voir pour mieux.

  // calcul des derivees sur la surface
  // on calcule les derivees en approximant le path au voisinage du point
  // P(u) par le cercle osculateur au path .

  // calcul du cercle osculateur.

  double U;
  if (P == 1)
  {
    U = myPath->FirstParameter();
  }
  else if (P == myNbSections)
  {
    U = myPath->LastParameter();
  }
  else
    return false;

  gp_Vec D1, D2;
  gp_Pnt Pt;

  myPath->D2(U, Pt, D1, D2);
  double l = D1.Magnitude();

  if (l < Epsilon(1.))
    return false;

  gp_Dir T = D1;
  double m = D2.Dot(T);
  gp_Vec D = D2 - m * T;
  double c = D.Magnitude() / (l * l);

  if (c < Epsilon(1.))
  {
    // null curvature : equivalent to a translation of the section
    for (int i = 1; i <= myFirstSect->NbPoles(); i++)
    {
      DPoles(i) = D1;
    }
  }
  else
  {
    gp_Dir N = D;
    gp_Pnt Q = Pt.Translated((1. / c) * gp_Vec(N));
    double x, y;
    gp_Vec V;
    for (int i = 1; i <= myFirstSect->NbPoles(); i++)
    {
      V         = gp_Vec(Q, Poles(i));
      x         = V * gp_Vec(T);
      y         = V * gp_Vec(N);
      DPoles(i) = x * gp_Vec(N) - y * gp_Vec(T);
      if (DPoles(i).Magnitude() > Epsilon(1.))
      {
        DPoles(i).Normalize();
        DPoles(i) *= std::sqrt(x * x + y * y);
      }
    }
  }

  for (int i = 1; i <= myFirstSect->NbPoles(); i++)
  {
    DWeigths(i) = 0.;
  }

  return true;
}

//=================================================================================================

void GeomFill_SweepSectionGenerator::Section(const int                   P,
                                             NCollection_Array1<gp_Pnt>& Poles,
                                             NCollection_Array1<gp_Pnt2d>&, // Poles2d,
                                             NCollection_Array1<double>& Weigths) const
{
  if (myType != 0)
  {
    Poles = myFirstSect->Poles();
    const NCollection_Array1<double>* aWPtr = myFirstSect->Weights();
    if (aWPtr != nullptr)
      Weigths = *aWPtr;
    else
      Weigths.Init(1.0);
    gp_Trsf cumulTR;
    if (P > 1)
    {
      cumulTR = myTrsfs(P - 1);
      // <cumulTR> transform <myFirstSect> to the P ieme Section. In fact
      // each points of the array <poles> will be transformed.

      if ((myType == 3) || (myType == 6))
      {
        for (int i = 1; i <= myFirstSect->NbPoles(); i++)
        {
          Poles(i).SetXYZ((myNbSections - P) * myFirstSect->Pole(i).XYZ()
                          + (P - 1) * myLastSect->Pole(i).XYZ());
          Poles(i).SetXYZ(Poles(i).XYZ() / (myNbSections - 1));

          Weigths(i) =
            (myNbSections - P) * myFirstSect->Weight(i) + (P - 1) * myLastSect->Weight(i);
          Weigths(i) /= myNbSections - 1;
        }
      }

      for (int i = 1; i <= Poles.Length(); i++)
        Poles(i).Transform(cumulTR);
    }
  }
  else
  {

    double Coef = (P - 1.) / (myNbSections - 1.);
    double U    = (1 - Coef) * myAdpPath->FirstParameter() + Coef * myAdpPath->LastParameter();

    gp_Pnt PPath = myAdpPath->Value(U);

    double Alpha = U - myAdpPath->FirstParameter();
    Alpha /= myAdpPath->LastParameter() - myAdpPath->FirstParameter();

    double U1 =
      (1 - Alpha) * myAdpFirstSect->FirstParameter() + Alpha * myAdpFirstSect->LastParameter();

    if (myAdpFirstSect->GetType() == GeomAbs_Line)
    {
      if (Precision::IsInfinite(myAdpFirstSect->FirstParameter())
          || Precision::IsInfinite(myAdpFirstSect->LastParameter()))
      {
        gp_Lin aLine = myAdpFirstSect->Line();
        U1           = ElCLib::Parameter(aLine, PPath);
      }
    }
    gp_Pnt P1 = myAdpFirstSect->Value(U1);

    double U2 =
      (1 - Alpha) * myAdpLastSect->FirstParameter() + Alpha * myAdpLastSect->LastParameter();

    if (myAdpLastSect->GetType() == GeomAbs_Line)
    {
      if (Precision::IsInfinite(myAdpLastSect->FirstParameter())
          || Precision::IsInfinite(myAdpLastSect->LastParameter()))
      {
        gp_Lin aLine = myAdpLastSect->Line();
        U2           = ElCLib::Parameter(aLine, PPath);
      }
    }
    gp_Pnt P2 = myAdpLastSect->Value(U2);

    gp_Ax2 Axis;
    double Angle;
    if (P1.Distance(P2) < Precision::Confusion())
    {
      Angle = 0.;
    }
    else
    {
      Axis  = gp_Ax2(PPath, gp_Vec(PPath, P1) ^ gp_Vec(PPath, P2), gp_Vec(PPath, P1));
      Angle = ElCLib::CircleParameter(Axis, P2);
    }
#ifdef OCCT_DEBUG
/*
    if (false) {
      gp_Vec dummyD1 = myAdpPath->DN(U,1);
      gp_Vec dummyTg = Axis.Direction();
      double Cos = dummyD1.Dot(dummyTg);
      if ( Cos > 0.) std::cout << "+" ;
      else           std::cout << "-" ;
    }
*/
#endif
    if (Angle < Precision::Angular())
    {
      for (int i = 1; i <= Poles.Upper(); i++)
      {
        Poles(i)   = P1;
        Weigths(i) = 1;
      }
    }
    else
    {
      occ::handle<Geom_Circle>       Circ = new Geom_Circle(Axis, myRadius);
      occ::handle<Geom_TrimmedCurve> CT   = new Geom_TrimmedCurve(Circ, 0., Angle);
      occ::handle<Geom_BSplineCurve> BS;
      if (myPolynomial)
        BS = GeomConvert::CurveToBSplineCurve(CT, Convert_Polynomial);
      else
        BS = GeomConvert::CurveToBSplineCurve(CT, Convert_QuasiAngular);

      Poles = BS->Poles();
      const NCollection_Array1<double>* aBSWPtr = BS->Weights();
      if (aBSWPtr != nullptr)
        Weigths = *aBSWPtr;
      else
        Weigths.Init(1.0);
    }
  }
}

//=================================================================================================

const gp_Trsf& GeomFill_SweepSectionGenerator::Transformation(const int Index) const
{
  if (Index > myTrsfs.Length())
    throw Standard_RangeError("GeomFill_SweepSectionGenerator::Transformation");

  return myTrsfs(Index);
}

//=================================================================================================

double GeomFill_SweepSectionGenerator::Parameter(const int P) const
{
  if (P == 1)
  {
    return myPath->FirstParameter();
  }
  else if (P == myNbSections)
  {
    return myPath->LastParameter();
  }
  else
  {
    double U1  = myPath->FirstParameter();
    double U2  = myPath->LastParameter();
    double prm = ((myNbSections - P) * U1 + (P - 1) * U2) / (double)(myNbSections - 1);
    return prm;
  }
}
