// Created on: 1997-12-15
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

#include <Adaptor3d_Curve.hxx>
#include <Bnd_Box.hxx>
#include <BndLib_Add3dCurve.hxx>
#include <Extrema_ExtCC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomFill_LocationLaw.hxx>
#include <GeomFill_SectionPlacement.hxx>
#include <GeomLib.hxx>
#include <GeomLProp_CLProps.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <Precision.hxx>
#include <StdFail_NotDone.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

//=================================================================================================

static void Tangente(const Adaptor3d_Curve& Path, const double Param, gp_Pnt& P, gp_Vec& Tang)
{
  Path.D1(Param, P, Tang);
  double Norm = Tang.Magnitude();

  for (int ii = 2; (ii < 12) && (Norm < Precision::Confusion()); ii++)
  {
    Tang = Path.DN(Param, ii);
    Norm = Tang.Magnitude();
  }

  if (Norm > 100 * gp::Resolution())
    Tang /= Norm;
}

static double Penalite(const double angle, const double dist)
{
  double penal;

  if (dist < 1)
    penal = std::sqrt(dist);
  else if (dist < 2)
    penal = std::pow(dist, 2);
  else
    penal = dist + 2;

  if (angle > 1.e-3)
  {
    penal += 1. / angle - 2. / M_PI;
  }
  else
  {
    penal += 1.e3;
  }

  return penal;
}

static double EvalAngle(const gp_Vec& V1, const gp_Vec& V2)
{
  double angle;
  angle = V1.Angle(V2);
  if (angle > M_PI / 2)
    angle = M_PI - angle;
  return angle;
}

//===============================================================
// Function :DistMini
// Purpose : Examine un extrema pour updater <Dist> & <Param>
//===============================================================
static void DistMini(const Extrema_ExtPC&   Ext,
                     const Adaptor3d_Curve& C,
                     double&                Dist,
                     double&                Param)
{
  double dist1, dist2;
  int    ii;
  gp_Pnt P1, P2;
  double Dist2 = RealLast();

  Ext.TrimmedSquareDistances(dist1, dist2, P1, P2);
  if ((dist1 < Dist2) || (dist2 < Dist2))
  {
    if (dist1 < dist2)
    {
      Dist2 = dist1;
      Param = C.FirstParameter();
    }
    else
    {
      Dist2 = dist2;
      Param = C.LastParameter();
    }
  }

  if (Ext.IsDone())
  {
    for (ii = 1; ii <= Ext.NbExt(); ii++)
    {
      if (Ext.SquareDistance(ii) < Dist2)
      {
        Dist2 = Ext.SquareDistance(ii);
        Param = Ext.Point(ii).Parameter();
      }
    }
  }
  Dist = sqrt(Dist2);
}

//=================================================================================================

GeomFill_SectionPlacement::GeomFill_SectionPlacement(const occ::handle<GeomFill_LocationLaw>& L,
                                                     const occ::handle<Geom_Geometry>& Section)
    : myLaw(L), /* myAdpSection(Section),  mySection(Section), */
      SecParam(0.0),
      PathParam(0.0),
      Dist(RealLast()),
      AngleMax(0.)
{

  done      = false;
  isplan    = false;
  myIsPoint = false;

  if (Section->IsInstance(STANDARD_TYPE(Geom_CartesianPoint)))
  {
    myIsPoint = true;
    myPoint   = occ::down_cast<Geom_CartesianPoint>(Section)->Pnt();
    isplan    = true;
  }
  else
  {
    occ::handle<Geom_Curve> CurveSection = occ::down_cast<Geom_Curve>(Section);
    myAdpSection.Load(CurveSection);
    mySection = CurveSection;
  }

  int    i, j, NbPoles = 0;
  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;

  // Boite d'encombrement de la section pour en deduire le gabarit
  Bnd_Box box;
  if (myIsPoint)
    box.Add(myPoint);
  else
    BndLib_Add3dCurve::Add(myAdpSection, 1.e-4, box);
  box.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  double DX = aXmax - aXmin;
  double DY = aYmax - aYmin;
  double DZ = aZmax - aZmin;
  Gabarit   = std::sqrt(DX * DX + DY * DY + DZ * DZ) / 2.;

  Gabarit += Precision::Confusion(); // Cas des toute petite

  // Initialisation de TheAxe pour les cas singulier
  if (!myIsPoint)
  {
    gp_Pnt P;
    gp_Vec V;
    Tangente(myAdpSection,
             (myAdpSection.FirstParameter() + myAdpSection.LastParameter()) / 2,
             P,
             V);
    TheAxe.SetLocation(P);
    TheAxe.SetDirection(V);

    // y a t'il un Plan moyen ?
    GeomAbs_CurveType TheType = myAdpSection.GetType();
    switch (TheType)
    {
      case GeomAbs_Circle: {
        isplan = true;
        TheAxe = myAdpSection.Circle().Axis();
        break;
      }
      case GeomAbs_Ellipse: {
        isplan = true;
        TheAxe = myAdpSection.Ellipse().Axis();
        break;
      }
      case GeomAbs_Hyperbola: {
        isplan = true;
        TheAxe = myAdpSection.Hyperbola().Axis();
        break;
      }
      case GeomAbs_Parabola: {
        isplan = true;
        TheAxe = myAdpSection.Parabola().Axis();
        break;
      }
      case GeomAbs_Line: {
        NbPoles = 0; // Pas de Plan !!
        break;
      }
      case GeomAbs_BezierCurve:
      case GeomAbs_BSplineCurve: {
        NbPoles = myAdpSection.NbPoles();
        break;
      }
      default:
        NbPoles = 21;
    }

    if (!isplan && NbPoles > 2)
    {
      // Calcul d'un plan moyen.
      occ::handle<NCollection_HArray1<gp_Pnt>> Pnts;
      double                                   first = myAdpSection.FirstParameter();
      double                                   last  = myAdpSection.LastParameter();
      if (myAdpSection.IsPeriodic())
      {
        // Correct boundaries to avoid mistake of LocateU
        occ::handle<Geom_Curve> aCurve = myAdpSection.Curve();
        if (aCurve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
          aCurve = (occ::down_cast<Geom_TrimmedCurve>(aCurve))->BasisCurve();
        double Ufirst  = aCurve->FirstParameter();
        double aPeriod = aCurve->Period();
        double U1      = Ufirst + std::floor((first - Ufirst) / aPeriod) * aPeriod;
        double U2      = U1 + aPeriod;
        if (std::abs(first - U1) <= Precision::PConfusion())
          first = U1;
        if (std::abs(last - U2) <= Precision::PConfusion())
          last = U2;
      }
      double t, delta;
      if (myAdpSection.GetType() == GeomAbs_BSplineCurve)
      {
        occ::handle<Geom_BSplineCurve> BC = occ::down_cast<Geom_BSplineCurve>(myAdpSection.Curve());
        int                            I1, I2, I3, I4;
        BC->LocateU(first, Precision::Confusion(), I1, I2);
        BC->LocateU(last, Precision::Confusion(), I3, I4);
        int NbKnots = I3 - I2 + 1;

        int NbLocalPnts = 10;
        int NbPnts      = (NbKnots - 1) * NbLocalPnts;
        if (NbPnts < 0)
          NbPnts = 0;
        if (I1 != I2)
          NbPnts += NbLocalPnts;
        if (I3 != I4 && first < BC->Knot(I3))
          NbPnts += NbLocalPnts;
        if (!myAdpSection.IsClosed())
          NbPnts++;
        Pnts   = new NCollection_HArray1<gp_Pnt>(1, NbPnts);
        int nb = 1;
        if (I1 != I2)
        {
          double locallast = (BC->Knot(I2) < last) ? BC->Knot(I2) : last;
          delta            = (locallast - first) / NbLocalPnts;
          for (j = 0; j < NbLocalPnts; j++)
          {
            t = first + j * delta;
            Pnts->SetValue(nb++, myAdpSection.Value(t));
          }
        }
        for (i = I2; i < I3; i++)
        {
          t     = BC->Knot(i);
          delta = (BC->Knot(i + 1) - t) / NbLocalPnts;
          for (j = 0; j < NbLocalPnts; j++)
          {
            Pnts->SetValue(nb++, myAdpSection.Value(t));
            t += delta;
          }
        }
        if (I3 != I4 && first < BC->Knot(I3))
        {
          t     = BC->Knot(I3);
          delta = (last - t) / NbLocalPnts;
          for (j = 0; j < NbLocalPnts; j++)
          {
            Pnts->SetValue(nb++, myAdpSection.Value(t));
            t += delta;
          }
        }
        if (!myAdpSection.IsClosed())
          Pnts->SetValue(nb, myAdpSection.Value(last));
      }
      else // other type
      {
        int NbPnts = NbPoles - 1;
        if (!myAdpSection.IsClosed())
          NbPnts++;
        Pnts  = new NCollection_HArray1<gp_Pnt>(1, NbPnts);
        delta = (last - first) / (NbPoles - 1);
        for (i = 0; i < NbPoles - 1; i++)
        {
          t = first + i * delta;
          Pnts->SetValue(i + 1, myAdpSection.Value(t));
        }
        if (!myAdpSection.IsClosed())
          Pnts->SetValue(NbPnts, myAdpSection.Value(last));
      }

      bool   issing;
      gp_Ax2 axe;
      GeomLib::AxeOfInertia(Pnts->Array1(), axe, issing, Precision::Confusion());
      if (!issing)
      {
        isplan = true;
        TheAxe.SetLocation(axe.Location());
        TheAxe.SetDirection(axe.Direction());
      }
    }

    myExt.Initialize(myAdpSection,
                     myAdpSection.FirstParameter(),
                     myAdpSection.LastParameter(),
                     Precision::Confusion());
  }
}

//=================================================================================================

void GeomFill_SectionPlacement::SetLocation(const occ::handle<GeomFill_LocationLaw>& L)
{
  myLaw = L;
}

//=================================================================================================

void GeomFill_SectionPlacement::Perform(const double Tol)
{
  occ::handle<Adaptor3d_Curve> Path;
  Path = myLaw->GetCurve();
  Perform(Path, Tol);
}

//=================================================================================================

void GeomFill_SectionPlacement::Perform(const occ::handle<Adaptor3d_Curve>& Path, const double Tol)
{
  double IntTol     = 1.e-5;
  double DistCenter = Precision::Infinite();

  if (myIsPoint)
  {
    Extrema_ExtPC Projector(myPoint, *Path, Precision::Confusion());
    DistMini(Projector, *Path, Dist, PathParam);
    AngleMax = M_PI / 2;
  }
  else
  {
    PathParam = Path->FirstParameter();
    SecParam  = myAdpSection.FirstParameter();

    double distaux, taux = 0.0, alpha;
    gp_Pnt PonPath, PonSec, P;
    gp_Vec VRef, dp1;
    VRef.SetXYZ(TheAxe.Direction().XYZ());

    Tangente(*Path, PathParam, PonPath, dp1);
    PonSec = myAdpSection.Value(SecParam);
    Dist   = PonPath.Distance(PonSec);
    if (Dist > Tol)
    { // On Cherche un meilleur point sur la section
      myExt.Perform(PonPath);
      if (myExt.IsDone())
      {
        DistMini(myExt, myAdpSection, Dist, SecParam);
        PonSec = myAdpSection.Value(SecParam);
      }
    }
    AngleMax = EvalAngle(VRef, dp1);
    if (isplan)
      AngleMax = M_PI / 2 - AngleMax;

    bool Trouve = false;
    int  ii;

    if (isplan)
    {
      // (1.1) Distances Point-Plan
      double DistPlan;
      gp_Vec V1(PonPath, TheAxe.Location());
      DistPlan = std::abs(V1.Dot(VRef));
      if (DistPlan <= IntTol)
        DistCenter = V1.Magnitude();

      gp_Pnt Plast = Path->Value(Path->LastParameter());
      V1.SetXYZ(TheAxe.Location().XYZ() - Plast.XYZ());
      DistPlan = std::abs(V1.Dot(VRef));
      if (DistPlan <= IntTol)
      {
        double aDist = V1.Magnitude();
        if (aDist < DistCenter)
        {
          DistCenter = aDist;
          PonPath    = Plast;
          PathParam  = Path->LastParameter();
        }
      }

      // (1.2) Intersection Plan-courbe
      gp_Ax3                           axe(TheAxe.Location(), TheAxe.Direction());
      occ::handle<Geom_Plane>          plan   = new (Geom_Plane)(axe);
      occ::handle<GeomAdaptor_Surface> adplan = new (GeomAdaptor_Surface)(plan);
      IntCurveSurface_HInter           Intersector;
      Intersector.Perform(Path, adplan);
      if (Intersector.IsDone())
      {
        double w;
        double aDist;
        for (ii = 1; ii <= Intersector.NbPoints(); ii++)
        {
          w     = Intersector.Point(ii).W();
          P     = Path->Value(w);
          aDist = P.Distance(TheAxe.Location());
          if (aDist < DistCenter)
          {
            DistCenter = aDist;
            PonPath    = P;
            PathParam  = w;
          }
        }
      }
      if (!Intersector.IsDone() || Intersector.NbPoints() == 0)
      {
        // Comparing the distances from the path's endpoints to the best matching plane of the
        // profile.
        const gp_Pnt firstPoint    = Path->Value(Path->FirstParameter());
        const gp_Pnt lastPoint     = Path->Value(Path->LastParameter());
        const gp_Pln plane         = plan->Pln();
        double       firstDistance = plane.SquareDistance(firstPoint);
        double       lastDistance  = plane.SquareDistance(lastPoint);

        if (((std::abs(firstDistance) < Precision::SquareConfusion())
             && std::abs(lastDistance) < Precision::SquareConfusion())
            || firstDistance < lastDistance)
        {
          PathParam = Path->FirstParameter();
        }
        else
        {
          PathParam = Path->LastParameter();
          Tangente(*Path, PathParam, PonPath, dp1);
          PonSec = myAdpSection.Value(SecParam);
          Dist   = PonPath.Distance(PonSec);
          if (Dist > Tol)
          { // On Cherche un meilleur point sur la section
            myExt.Perform(PonPath);
            if (myExt.IsDone())
            {
              DistMini(myExt, myAdpSection, Dist, SecParam);
              PonSec = myAdpSection.Value(SecParam);
            }
          }
          AngleMax = EvalAngle(VRef, dp1);
          AngleMax = M_PI / 2 - AngleMax;
        }
      }

      /*
         // (1.1) Distances Point-Plan
         double DistPlan;
         gp_Vec V1 (PonPath, TheAxe.Location());
         DistPlan = std::abs(V1.Dot(VRef));

         // On examine l'autre extremite
         gp_Pnt P;
         Tangente(Path->Curve(), Path->LastParameter(), P, dp1);
         V1.SetXYZ(TheAxe.Location().XYZ()-P.XYZ());
         if  (std::abs(V1.Dot(VRef)) <= DistPlan ) { // On prend l'autre extremite
         alpha =  M_PI/2 - EvalAngle(VRef, dp1);
         distaux =  PonPath.Distance(PonSec);
         if (distaux > Tol) {
         myExt.Perform(P);
         if ( myExt.IsDone() )
         DistMini(myExt, myAdpSection, distaux, taux);
         }
         else
         taux = SecParam;

         if (Choix(distaux, alpha)) {
         Dist = distaux;
         AngleMax = alpha;
         PonPath = P;
         PathParam = Path->LastParameter();
         }
         }

         // (1.2) Intersection Plan-courbe
         gp_Ax3 axe (TheAxe.Location(), TheAxe.Direction());
         occ::handle<Geom_Plane> plan = new (Geom_Plane)(axe);
         occ::handle<GeomAdaptor_Surface> adplan =
         new (GeomAdaptor_Surface)(plan);
         IntCurveSurface_HInter Intersector;
         Intersector.Perform(Path, adplan);
         if (Intersector.IsDone()) {
         double w;
         gp_Vec V;
         for (ii=1; ii<=Intersector.NbPoints(); ii++) {
         w = Intersector.Point(ii).W();
         //(1.3) test d'angle
         Tangente( Path->Curve(), w, P, V);
         alpha = M_PI/2 - EvalAngle(V, VRef);
         //(1.4) Test de distance Point-Courbe
         myExt.Perform(P);
         if ( myExt.IsDone() ) {
         DistMini(myExt, myAdpSection, distaux, taux);
         if (Choix(distaux, alpha)) {
         Dist = distaux;
         SecParam  = taux;
         AngleMax = alpha;
         PonPath = P;
         PathParam = w;
         PonSec = myAdpSection.Value(SecParam);
         }
         }
         else {
         distaux = P.Distance(PonSec);
         if (Choix(distaux, alpha)) {
         Dist = distaux;
         AngleMax = alpha;
         PonPath = P;
         PathParam = w;
         }
         }
         }
         }
      */
#ifdef OCCT_DEBUG
      if (Intersector.NbPoints() == 0)
      {
        Intersector.Dump();
      }
#endif
    }

    // Cas General
    if (!isplan)
    {
      // (2.1) Distance avec les extremites ...
      myExt.Perform(PonPath);
      if (myExt.IsDone())
      {
        DistMini(myExt, myAdpSection, distaux, taux);
        if (distaux < Dist)
        {
          Dist     = distaux;
          SecParam = taux;
        }
      }
      Trouve = (Dist <= Tol);
      if (!Trouve)
      {
        Tangente(*Path, Path->LastParameter(), P, dp1);
        alpha = EvalAngle(VRef, dp1);
        myExt.Perform(P);
        if (myExt.IsDone())
        {
          if (myExt.IsDone())
          {
            DistMini(myExt, myAdpSection, distaux, taux);
            if (Choix(distaux, alpha))
            {
              Dist      = distaux;
              SecParam  = taux;
              AngleMax  = alpha;
              PonPath   = P;
              PathParam = Path->LastParameter();
            }
          }
        }
        Trouve = (Dist <= Tol);
      }

      // (2.2) Distance courbe-courbe
      if (!Trouve)
      {
        Extrema_ExtCC Ext(*Path,
                          myAdpSection,
                          Path->FirstParameter(),
                          Path->LastParameter(),
                          myAdpSection.FirstParameter(),
                          myAdpSection.LastParameter(),
                          Path->Resolution(Tol / 100),
                          myAdpSection.Resolution(Tol / 100));
        if (Ext.IsDone() && !Ext.IsParallel())
        {
          Extrema_POnCurv P1, P2;
          for (ii = 1; ii <= Ext.NbExt(); ii++)
          {
            distaux = sqrt(Ext.SquareDistance(ii));
            Ext.Points(ii, P1, P2);
            Tangente(*Path, P1.Parameter(), P, dp1);
            alpha = EvalAngle(VRef, dp1);
            if (Choix(distaux, alpha))
            {
              Trouve    = true;
              Dist      = distaux;
              PathParam = P1.Parameter();
              SecParam  = P2.Parameter();
              PonSec    = P2.Value();
              PonPath   = P;
              AngleMax  = alpha;
            }
          }
        }
        if (!Trouve)
        {
          // Si l'on a toujours rien, on essai une distance point/path
          // c'est la derniere chance.
          Extrema_ExtPC PExt;
          PExt.Initialize(*Path,
                          Path->FirstParameter(),
                          Path->LastParameter(),
                          Precision::Confusion());
          PExt.Perform(PonSec);
          if (PExt.IsDone())
          {
            // modified for OCC13595  Tue Oct 17 15:00:08 2006.BEGIN
            // 	      DistMini(PExt, myAdpSection, distaux, taux);
            DistMini(PExt, *Path, distaux, taux);
            // modified for OCC13595  Tue Oct 17 15:00:11 2006.END
            Tangente(*Path, taux, P, dp1);
            alpha = EvalAngle(VRef, dp1);
            if (Choix(distaux, alpha))
            {
              Dist      = distaux;
              PonPath   = P;
              AngleMax  = alpha;
              PathParam = taux;
            }
          }
        }
      }
    }
  }

  done = true;
}

//===============================================================
// Function : Perform
// Purpose : Calcul le placement pour un parametre donne.
//===============================================================
void GeomFill_SectionPlacement::Perform(const double Param, const double Tol)
{
  done = true;
  occ::handle<Adaptor3d_Curve> Path;
  Path = myLaw->GetCurve();

  PathParam = Param;
  if (myIsPoint)
  {
    gp_Pnt PonPath = Path->Value(PathParam);
    Dist           = PonPath.Distance(myPoint);
    AngleMax       = M_PI / 2;
  }
  else
  {
    SecParam = myAdpSection.FirstParameter();

    //  double distaux, taux, alpha;
    //  gp_Pnt PonPath, PonSec, P;
    gp_Pnt PonPath, PonSec;
    gp_Vec VRef, dp1;
    VRef.SetXYZ(TheAxe.Direction().XYZ());

    Tangente(*Path, PathParam, PonPath, dp1);
    PonSec = myAdpSection.Value(SecParam);
    Dist   = PonPath.Distance(PonSec);
    if (Dist > Tol)
    { // On Cherche un meilleur point sur la section
      myExt.Perform(PonPath);
      if (myExt.IsDone())
      {
        DistMini(myExt, myAdpSection, Dist, SecParam);
        PonSec = myAdpSection.Value(SecParam);
      }
    }
    AngleMax = EvalAngle(VRef, dp1);
    if (isplan)
      AngleMax = M_PI / 2 - AngleMax;
  }

  done = true;
}

//=================================================================================================

bool GeomFill_SectionPlacement::IsDone() const
{
  return done;
}

//=================================================================================================

double GeomFill_SectionPlacement::ParameterOnPath() const
{
  return PathParam;
}

//=================================================================================================

double GeomFill_SectionPlacement::ParameterOnSection() const
{
  return SecParam;
}

//=================================================================================================

double GeomFill_SectionPlacement::Distance() const
{
  return Dist;
}

//=================================================================================================

double GeomFill_SectionPlacement::Angle() const
{
  return AngleMax;
}

//=================================================================================================

gp_Trsf GeomFill_SectionPlacement::Transformation(const bool WithTranslation,
                                                  const bool WithCorrection) const
{
  gp_Vec V;
  gp_Mat M;
  gp_Dir DN, D;
  // modified by NIZHNY-MKK  Fri Oct 17 15:27:07 2003
  gp_Pnt P(0., 0., 0.), PSection(0., 0., 0.);

  // Calcul des reperes
  myLaw->D0(PathParam, M, V);

  P.SetXYZ(V.XYZ());
  D.SetXYZ(M.Column(3));
  DN.SetXYZ(M.Column(1));
  gp_Ax3 Paxe(P, D, DN);

  if (WithTranslation || WithCorrection)
  {
    if (myIsPoint)
      PSection = myPoint;
    else
      PSection = mySection->Value(SecParam);
  }
  // modified by NIZHNY-MKK  Wed Oct  8 15:03:19 2003.BEGIN
  gp_Trsf Rot;

  if (WithCorrection && !myIsPoint)
  {
    double angle;

    if (!isplan)
      throw Standard_Failure("Illegal usage: can't rotate non-planar profile");

    gp_Dir ProfileNormal = TheAxe.Direction();
    gp_Dir SpineStartDir = Paxe.Direction();

    if (!ProfileNormal.IsParallel(SpineStartDir, Precision::Angular()))
    {
      gp_Dir DirAxeOfRotation = ProfileNormal ^ SpineStartDir;
      angle                   = ProfileNormal.AngleWithRef(SpineStartDir, DirAxeOfRotation);
      gp_Ax1 AxeOfRotation(TheAxe.Location(), DirAxeOfRotation);
      Rot.SetRotation(AxeOfRotation, angle);
    }
    PSection.Transform(Rot);
  }
  // modified by NIZHNY-MKK  Wed Oct  8 15:03:21 2003.END

  if (WithTranslation)
  {
    P.ChangeCoord().SetLinearForm(-1, PSection.XYZ(), V.XYZ());
  }
  else
  {
    P.SetCoord(0., 0., 0.);
  }

  gp_Ax3 Saxe(P, gp::DZ(), gp::DX());

  // Transfo
  gp_Trsf Tf;
  Tf.SetTransformation(Saxe, Paxe);

  if (WithCorrection)
  {
    // modified by NIZHNY-MKK  Fri Oct 17 15:26:36 2003.BEGIN
    //     double angle;
    //     gp_Trsf Rot;

    //     if (!isplan)
    //       throw Standard_Failure("Illegal usage: can't rotate non-planar profile");

    //     gp_Dir ProfileNormal = TheAxe.Direction();
    //     gp_Dir SpineStartDir = Paxe.Direction();
    //     if (! ProfileNormal.IsParallel( SpineStartDir, Precision::Angular() ))
    //       {
    // 	gp_Dir DirAxeOfRotation = ProfileNormal ^ SpineStartDir;
    // 	angle = ProfileNormal.AngleWithRef( SpineStartDir, DirAxeOfRotation );
    // 	gp_Ax1 AxeOfRotation( TheAxe.Location(), DirAxeOfRotation );
    // 	Rot.SetRotation( AxeOfRotation, angle );
    //       }
    // modified by NIZHNY-MKK  Fri Oct 17 15:26:42 2003.END

    Tf *= Rot;
  }

  return Tf;
}

//=================================================================================================

occ::handle<Geom_Curve> GeomFill_SectionPlacement::Section(const bool WithTranslation) const
{
  occ::handle<Geom_Curve> TheSection = occ::down_cast<Geom_Curve>(mySection->Copy());
  TheSection->Transform(Transformation(WithTranslation, false));
  return TheSection;
}

//=================================================================================================

occ::handle<Geom_Curve> GeomFill_SectionPlacement::ModifiedSection(const bool WithTranslation) const
{
  occ::handle<Geom_Curve> TheSection = occ::down_cast<Geom_Curve>(mySection->Copy());
  TheSection->Transform(Transformation(WithTranslation, true));
  return TheSection;
}

//=================================================================================================

void GeomFill_SectionPlacement::SectionAxis(const gp_Mat& M, gp_Vec& T, gp_Vec& N, gp_Vec& BN) const
{
  double            Eps = 1.e-10;
  gp_Dir            D;
  gp_Vec            PathNormal;
  GeomLProp_CLProps CP(mySection, SecParam, 2, Eps);
  if (CP.IsTangentDefined())
  {
    CP.Tangent(D);
    T.SetXYZ(D.XYZ());
    T.Normalize();
    if (CP.Curvature() > Eps)
    {
      CP.Normal(D);
      N.SetXYZ(D.XYZ());
    }
    else
    {
      // Cas ambigu, on essai de recuperer la normal a la trajectoire
      // Reste le probleme des points d'inflexions, qui n'est pas
      // bien traiter par LProp (pas de recuperation de la normal) :
      // A voir...
      PathNormal.SetXYZ(M.Column(1));
      PathNormal.Normalize();
      BN = T ^ PathNormal;
      if (BN.Magnitude() > Eps)
      {
        BN.Normalize();
        // N = BN ^ T;
      }
      N = BN ^ T;
    }
  }
  else
  { // Cas indefinie, on prend le triedre
    // complet sur la trajectoire
    T.SetXYZ(M.Column(3));
    N.SetXYZ(M.Column(2));
  }
  BN = T ^ N;
}

//===============================================================
// Function :Choix
// Purpose : Decide si le couple (dist, angle) est "meilleur" que
// couple courrant.
//===============================================================
bool GeomFill_SectionPlacement::Choix(const double dist, const double angle) const
{
  double evoldist, evolangle;
  evoldist  = dist - Dist;
  evolangle = angle - AngleMax;
  // (1) Si la gain en distance est > que le gabarit, on prend
  if (evoldist < -Gabarit)
    return true;

  //  (2) si l'ecart en distance est de l'ordre du gabarit
  if (std::abs(evoldist) < Gabarit)
  {
    //  (2.1) si le gain en angle est important on garde
    if (evolangle > 0.5)
      return true;
    //  (2.2) si la variation d'angle est moderee on evalue
    //  une fonction de penalite
    if (Penalite(angle, dist / Gabarit) < Penalite(AngleMax, Dist / Gabarit))
      return true;
  }

  return false;
}
