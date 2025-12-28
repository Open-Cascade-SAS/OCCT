// Created on: 1995-08-07
// Created by: Modelistation
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

#include <Adaptor3d_IsoCurve.hxx>
#include <Bnd_Box2d.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Graphic3d_Group.hxx>
#include <Hatch_Hatcher.hxx>
#include <Precision.hxx>
#include <Prs3d_IsoAspect.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <StdPrs_ToolRFace.hxx>
#include <StdPrs_WFDeflectionRestrictedFace.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Sequence.hxx>

#ifdef OCCT_DEBUG_MESH
  #include <OSD_Chronometer.hxx>
extern OSD_Chronometer FFaceTimer1, FFaceTimer2, FFaceTimer3, FFaceTimer4;
#endif

//=================================================================================================

static void FindLimits(const Adaptor3d_Curve& aCurve,
                       const double    aLimit,
                       double&         First,
                       double&         Last)
{
  First                     = std::max(aCurve.FirstParameter(), First);
  Last                      = std::min(aCurve.LastParameter(), Last);
  bool firstInf = Precision::IsNegativeInfinite(First);
  bool lastInf  = Precision::IsPositiveInfinite(Last);

  if (firstInf || lastInf)
  {
    gp_Pnt        P1, P2;
    double delta = 1;
    if (firstInf && lastInf)
    {
      do
      {
        delta *= 2;
        First = -delta;
        Last  = delta;
        aCurve.D0(First, P1);
        aCurve.D0(Last, P2);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (firstInf)
    {
      aCurve.D0(Last, P2);
      do
      {
        delta *= 2;
        First = Last - delta;
        aCurve.D0(First, P1);
      } while (P1.Distance(P2) < aLimit);
    }
    else if (lastInf)
    {
      aCurve.D0(First, P1);
      do
      {
        delta *= 2;
        Last = First + delta;
        aCurve.D0(Last, P2);
      } while (P1.Distance(P2) < aLimit);
    }
  }
}

//=================================================================================================

void StdPrs_WFDeflectionRestrictedFace::Add(const occ::handle<Prs3d_Presentation>&  aPresentation,
                                            const occ::handle<BRepAdaptor_Surface>& aFace,
                                            const bool             DrawUIso,
                                            const bool             DrawVIso,
                                            const double                Deflection,
                                            const int             NBUiso,
                                            const int             NBViso,
                                            const occ::handle<Prs3d_Drawer>&        aDrawer,
                                            NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>&        Curves)
{

#ifdef OCCT_DEBUG_MESH
  FFaceTimer1.Start();
#endif

  StdPrs_ToolRFace    ToolRst(aFace);
  const double UF = aFace->FirstUParameter();
  const double UL = aFace->LastUParameter();
  const double VF = aFace->FirstVParameter();
  const double VL = aFace->LastVParameter();

  const double aLimit = aDrawer->MaximalParameterValue();

  // compute bounds of the restriction
  double UMin = std::max(UF, -aLimit);
  double UMax = std::min(UL, aLimit);
  double VMin = std::max(VF, -aLimit);
  double VMax = std::min(VL, aLimit);

  // update min max for the hatcher.
  gp_Pnt2d      P1, P2;
  double U1, U2;
  gp_Pnt        dummypnt;
  double ddefle = std::max(UMax - UMin, VMax - VMin) * aDrawer->DeviationCoefficient();
  NCollection_Sequence<gp_Pnt2d> tabP;
  double          aHatchingTol = 1.e100;

  UMin = VMin = 1.e100;
  UMax = VMax = -1.e100;

  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next())
  {
    const TopAbs_Orientation anOrient  = ToolRst.Orientation();
    const Adaptor2d_Curve2d* TheRCurve = &ToolRst.Value();
    if (TheRCurve->GetType() != GeomAbs_Line)
    {
      GCPnts_QuasiUniformDeflection UDP(*TheRCurve, ddefle);
      if (UDP.IsDone())
      {
        const int aNumberOfPoints = UDP.NbPoints();
        if (aNumberOfPoints >= 2)
        {
          dummypnt = UDP.Value(1);
          P2.SetCoord(dummypnt.X(), dummypnt.Y());
          UMin = std::min(P2.X(), UMin);
          UMax = std::max(P2.X(), UMax);
          VMin = std::min(P2.Y(), VMin);
          VMax = std::max(P2.Y(), VMax);
          for (int i = 2; i <= aNumberOfPoints; i++)
          {
            P1       = P2;
            dummypnt = UDP.Value(i);
            P2.SetCoord(dummypnt.X(), dummypnt.Y());
            UMin         = std::min(P2.X(), UMin);
            UMax         = std::max(P2.X(), UMax);
            VMin         = std::min(P2.Y(), VMin);
            VMax         = std::max(P2.Y(), VMax);
            aHatchingTol = std::min(P1.SquareDistance(P2), aHatchingTol);

            if (anOrient == TopAbs_FORWARD)
            {
              // isobuild.Trim (P1, P2);
              tabP.Append(P1);
              tabP.Append(P2);
            }
            else
            {
              // isobuild.Trim (P2, P1);
              tabP.Append(P2);
              tabP.Append(P1);
            }
          }
        }
      }
      else
      {
#ifdef OCCT_DEBUG
        std::cout << "Cannot evaluate curve on surface" << std::endl;
#endif
      }
    }
    else
    {
      U1 = TheRCurve->FirstParameter();
      U2 = TheRCurve->LastParameter();
      // MSV 17.08.06 OCC13144: U2 occurred less than U1,
      // to overcome it ensure that distance U2-U1 is not greater than aLimit*2,
      // if greater then choose an origin and use aLimit to define U1 and U2 anew
      double aOrigin = 0.;
      if (!Precision::IsNegativeInfinite(U1) || !Precision::IsPositiveInfinite(U2))
      {
        if (Precision::IsNegativeInfinite(U1))
        {
          aOrigin = U2 - aLimit;
        }
        else if (Precision::IsPositiveInfinite(U2))
        {
          aOrigin = U1 + aLimit;
        }
        else
        {
          aOrigin = (U1 + U2) * 0.5;
        }
      }
      U1           = std::max(aOrigin - aLimit, U1);
      U2           = std::min(aOrigin + aLimit, U2);
      P1           = TheRCurve->Value(U1);
      P2           = TheRCurve->Value(U2);
      UMin         = std::min(P1.X(), UMin);
      UMax         = std::max(P1.X(), UMax);
      VMin         = std::min(P1.Y(), VMin);
      VMax         = std::max(P1.Y(), VMax);
      UMin         = std::min(P2.X(), UMin);
      UMax         = std::max(P2.X(), UMax);
      VMin         = std::min(P2.Y(), VMin);
      VMax         = std::max(P2.Y(), VMax);
      aHatchingTol = std::min(P1.SquareDistance(P2), aHatchingTol);

      if (anOrient == TopAbs_FORWARD)
      {
        // isobuild.Trim (P1, P2);
        tabP.Append(P1);
        tabP.Append(P2);
      }
      else
      {
        // isobuild.Trim (P2, P1);
        tabP.Append(P2);
        tabP.Append(P1);
      }
    }
  }

#ifdef OCCT_DEBUG_MESH
  FFaceTimer1.Stop();
  FFaceTimer2.Start();
#endif

  // Compute the hatching tolerance.
  aHatchingTol *= 0.1;
  aHatchingTol = std::max(Precision::Confusion(), aHatchingTol);
  aHatchingTol = std::min(1.e-5, aHatchingTol);

  // load the isos
  Hatch_Hatcher    isobuild(aHatchingTol, ToolRst.IsOriented());
  bool isUClosed = aFace->IsUClosed();
  bool isVClosed = aFace->IsVClosed();
  if (!isUClosed)
  {
    UMin = UMin + (UMax - UMin) / 1000.;
    UMax = UMax - (UMax - UMin) / 1000.;
  }
  if (!isVClosed)
  {
    VMin = VMin + (VMax - VMin) / 1000.;
    VMax = VMax - (VMax - VMin) / 1000.;
  }

  if (DrawUIso)
  {
    if (NBUiso > 0)
    {
      isUClosed        = false; // En attendant un hatcher de course.
      double du = isUClosed ? (UMax - UMin) / NBUiso : (UMax - UMin) / (1 + NBUiso);
      for (int i = 1; i <= NBUiso; i++)
      {
        isobuild.AddXLine(UMin + du * i);
      }
    }
  }
  if (DrawVIso)
  {
    if (NBViso > 0)
    {
      isVClosed        = false;
      double dv = isVClosed ? (VMax - VMin) / NBViso : (VMax - VMin) / (1 + NBViso);
      for (int i = 1; i <= NBViso; i++)
      {
        isobuild.AddYLine(VMin + dv * i);
      }
    }
  }

#ifdef OCCT_DEBUG_MESH
  FFaceTimer2.Stop();
  FFaceTimer3.Start();
#endif

  int ll = tabP.Length();
  for (int i = 1; i <= ll; i += 2)
  {
    isobuild.Trim(tabP(i), tabP(i + 1));
  }

#ifdef OCCT_DEBUG_MESH
  FFaceTimer3.Stop();
  FFaceTimer4.Start();
#endif

  // draw the isos

  Adaptor3d_IsoCurve anIso;
  anIso.Load(aFace);
  occ::handle<Geom_Curve>         BC;
  const BRepAdaptor_Surface& BS      = *aFace;
  GeomAbs_SurfaceType        thetype = aFace->GetType();

  occ::handle<Geom_Surface> GB;
  if (thetype == GeomAbs_BezierSurface)
  {
    GB = BS.Bezier();
  }
  else if (thetype == GeomAbs_BSplineSurface)
  {
    GB = BS.BSpline();
  }

  const double    anAngle        = aDrawer->DeviationAngle();
  const int aNumberOfLines = isobuild.NbLines();
  for (int i = 1; i <= aNumberOfLines; i++)
  {
    int NumberOfIntervals = isobuild.NbIntervals(i);
    double    Coord             = isobuild.Coordinate(i);
    for (int j = 1; j <= NumberOfIntervals; j++)
    {
      double b1 = isobuild.Start(i, j), b2 = isobuild.End(i, j);
      if (!GB.IsNull())
      {
        if (isobuild.IsXLine(i))
        {
          BC = GB->UIso(Coord);
        }
        else
        {
          BC = GB->VIso(Coord);
        }

        GeomAdaptor_Curve GC(BC);
        FindLimits(GC, aLimit, b1, b2);
        if (b2 - b1 > Precision::Confusion())
        {
          occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
          StdPrs_DeflectionCurve::Add(aPresentation,
                                      GC,
                                      b1,
                                      b2,
                                      Deflection,
                                      aPoints->ChangeSequence(),
                                      anAngle,
                                      false);
          Curves.Append(aPoints);
        }
      }
      else
      {
        if (isobuild.IsXLine(i))
        {
          anIso.Load(GeomAbs_IsoU, Coord, b1, b2);
        }
        else
        {
          anIso.Load(GeomAbs_IsoV, Coord, b1, b2);
        }

        FindLimits(anIso, aLimit, b1, b2);
        if (b2 - b1 > Precision::Confusion())
        {
          occ::handle<NCollection_HSequence<gp_Pnt>> aPoints = new NCollection_HSequence<gp_Pnt>();
          StdPrs_DeflectionCurve::Add(aPresentation,
                                      anIso,
                                      b1,
                                      b2,
                                      Deflection,
                                      aPoints->ChangeSequence(),
                                      anAngle,
                                      false);
          Curves.Append(aPoints);
        }
      }
    }
  }
#ifdef OCCT_DEBUG_MESH
  FFaceTimer4.Stop();
#endif
}

//=================================================================================================

bool StdPrs_WFDeflectionRestrictedFace::Match(const double X,
                                                          const double Y,
                                                          const double Z,
                                                          const double aDistance,
                                                          const occ::handle<BRepAdaptor_Surface>& aFace,
                                                          const occ::handle<Prs3d_Drawer>& aDrawer,
                                                          const bool      DrawUIso,
                                                          const bool      DrawVIso,
                                                          const double         Deflection,
                                                          const int      NBUiso,
                                                          const int      NBViso)
{

  StdPrs_ToolRFace    ToolRst(aFace);
  const double aLimit = aDrawer->MaximalParameterValue();

  // compute bounds of the restriction
  double    UMin, UMax, VMin, VMax;
  double    u, v, step;
  int i, nbPoints = 10;
  UMin = VMin = RealLast();
  UMax = VMax = RealFirst();

  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next())
  {
    const Adaptor2d_Curve2d* TheRCurve = &ToolRst.Value();
    u                                  = TheRCurve->FirstParameter();
    v                                  = TheRCurve->LastParameter();
    step                               = (v - u) / nbPoints;
    for (i = 0; i <= nbPoints; i++)
    {
      gp_Pnt2d P = TheRCurve->Value(u);
      if (P.X() < UMin)
        UMin = P.X();
      if (P.X() > UMax)
        UMax = P.X();
      if (P.Y() < VMin)
        VMin = P.Y();
      if (P.Y() > VMax)
        VMax = P.Y();
      u += step;
    }
  }

  // load the isos
  Hatch_Hatcher    isobuild(1.e-5, ToolRst.IsOriented());
  bool UClosed = aFace->IsUClosed();
  bool VClosed = aFace->IsVClosed();

  if (!UClosed)
  {
    UMin = UMin + (UMax - UMin) / 1000.;
    UMax = UMax - (UMax - UMin) / 1000.;
  }

  if (!VClosed)
  {
    VMin = VMin + (VMax - VMin) / 1000.;
    VMax = VMax - (VMax - VMin) / 1000.;
  }

  if (DrawUIso)
  {
    if (NBUiso > 0)
    {
      UClosed          = false; // En attendant un hatcher de course.
      double du = UClosed ? (UMax - UMin) / NBUiso : (UMax - UMin) / (1 + NBUiso);
      for (i = 1; i <= NBUiso; i++)
      {
        isobuild.AddXLine(UMin + du * i);
      }
    }
  }
  if (DrawVIso)
  {
    if (NBViso > 0)
    {
      VClosed          = false;
      double dv = VClosed ? (VMax - VMin) / NBViso : (VMax - VMin) / (1 + NBViso);
      for (i = 1; i <= NBViso; i++)
      {
        isobuild.AddYLine(VMin + dv * i);
      }
    }
  }

  // trim the isos
  gp_Pnt2d P1, P2;
  gp_Pnt   dummypnt;
  for (ToolRst.Init(); ToolRst.More(); ToolRst.Next())
  {
    TopAbs_Orientation            Orient    = ToolRst.Orientation();
    const Adaptor2d_Curve2d*      TheRCurve = &ToolRst.Value();
    GCPnts_QuasiUniformDeflection UDP(*TheRCurve, Deflection);
    if (UDP.IsDone())
    {
      int NumberOfPoints = UDP.NbPoints();
      if (NumberOfPoints >= 2)
      {
        dummypnt = UDP.Value(1);
        P2.SetCoord(dummypnt.X(), dummypnt.Y());
        for (i = 2; i <= NumberOfPoints; i++)
        {
          P1       = P2;
          dummypnt = UDP.Value(i);
          P2.SetCoord(dummypnt.X(), dummypnt.Y());
          if (Orient == TopAbs_FORWARD)
            isobuild.Trim(P1, P2);
          else
            isobuild.Trim(P2, P1);
        }
      }
    }
#ifdef OCCT_DEBUG
    else
    {
      std::cout << "Cannot evaluate curve on surface" << std::endl;
    }
#endif
  }

  // draw the isos

  Adaptor3d_IsoCurve anIso;
  anIso.Load(aFace);
  int NumberOfLines = isobuild.NbLines();
  double    anAngle       = aDrawer->DeviationAngle();

  for (i = 1; i <= NumberOfLines; i++)
  {
    int NumberOfIntervals = isobuild.NbIntervals(i);
    double    Coord             = isobuild.Coordinate(i);
    for (int j = 1; j <= NumberOfIntervals; j++)
    {
      double b1 = isobuild.Start(i, j), b2 = isobuild.End(i, j);

      b1 = b1 == RealFirst() ? -aLimit : b1;
      b2 = b2 == RealLast() ? aLimit : b2;

      if (isobuild.IsXLine(i))
        anIso.Load(GeomAbs_IsoU, Coord, b1, b2);
      else
        anIso.Load(GeomAbs_IsoV, Coord, b1, b2);

      if (StdPrs_DeflectionCurve::Match(X, Y, Z, aDistance, anIso, b1, b2, Deflection, anAngle))
        return true;
    }
  }
  return false;
}

//=================================================================================================

void StdPrs_WFDeflectionRestrictedFace::Add(const occ::handle<Prs3d_Presentation>&  aPresentation,
                                            const occ::handle<BRepAdaptor_Surface>& aFace,
                                            const occ::handle<Prs3d_Drawer>&        aDrawer)
{
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>> Curves;
  StdPrs_WFDeflectionRestrictedFace::Add(aPresentation,
                                         aFace,
                                         true,
                                         true,
                                         aDrawer->MaximalChordialDeviation(),
                                         aDrawer->UIsoAspect()->Number(),
                                         aDrawer->VIsoAspect()->Number(),
                                         aDrawer,
                                         Curves);
}

//=================================================================================================

void StdPrs_WFDeflectionRestrictedFace::AddUIso(const occ::handle<Prs3d_Presentation>&  aPresentation,
                                                const occ::handle<BRepAdaptor_Surface>& aFace,
                                                const occ::handle<Prs3d_Drawer>&        aDrawer)
{
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>> Curves;
  StdPrs_WFDeflectionRestrictedFace::Add(aPresentation,
                                         aFace,
                                         true,
                                         false,
                                         aDrawer->MaximalChordialDeviation(),
                                         aDrawer->UIsoAspect()->Number(),
                                         aDrawer->VIsoAspect()->Number(),
                                         aDrawer,
                                         Curves);
}

//=================================================================================================

void StdPrs_WFDeflectionRestrictedFace::AddVIso(const occ::handle<Prs3d_Presentation>&  aPresentation,
                                                const occ::handle<BRepAdaptor_Surface>& aFace,
                                                const occ::handle<Prs3d_Drawer>&        aDrawer)
{
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>> Curves;
  StdPrs_WFDeflectionRestrictedFace::Add(aPresentation,
                                         aFace,
                                         false,
                                         true,
                                         aDrawer->MaximalChordialDeviation(),
                                         aDrawer->UIsoAspect()->Number(),
                                         aDrawer->VIsoAspect()->Number(),
                                         aDrawer,
                                         Curves);
}

//=================================================================================================

bool StdPrs_WFDeflectionRestrictedFace::Match(const double X,
                                                          const double Y,
                                                          const double Z,
                                                          const double aDistance,
                                                          const occ::handle<BRepAdaptor_Surface>& aFace,
                                                          const occ::handle<Prs3d_Drawer>& aDrawer)
{
  return StdPrs_WFDeflectionRestrictedFace::Match(X,
                                                  Y,
                                                  Z,
                                                  aDistance,
                                                  aFace,
                                                  aDrawer,
                                                  true,
                                                  true,
                                                  aDrawer->MaximalChordialDeviation(),
                                                  aDrawer->UIsoAspect()->Number(),
                                                  aDrawer->VIsoAspect()->Number());
}

//=================================================================================================

bool StdPrs_WFDeflectionRestrictedFace::MatchUIso(
  const double                X,
  const double                Y,
  const double                Z,
  const double                aDistance,
  const occ::handle<BRepAdaptor_Surface>& aFace,
  const occ::handle<Prs3d_Drawer>&        aDrawer)
{
  return StdPrs_WFDeflectionRestrictedFace::Match(X,
                                                  Y,
                                                  Z,
                                                  aDistance,
                                                  aFace,
                                                  aDrawer,
                                                  true,
                                                  false,
                                                  aDrawer->MaximalChordialDeviation(),
                                                  aDrawer->UIsoAspect()->Number(),
                                                  aDrawer->VIsoAspect()->Number());
}

//=================================================================================================

bool StdPrs_WFDeflectionRestrictedFace::MatchVIso(
  const double                X,
  const double                Y,
  const double                Z,
  const double                aDistance,
  const occ::handle<BRepAdaptor_Surface>& aFace,
  const occ::handle<Prs3d_Drawer>&        aDrawer)
{
  return StdPrs_WFDeflectionRestrictedFace::Match(X,
                                                  Y,
                                                  Z,
                                                  aDistance,
                                                  aFace,
                                                  aDrawer,
                                                  false,
                                                  true,
                                                  aDrawer->MaximalChordialDeviation(),
                                                  aDrawer->UIsoAspect()->Number(),
                                                  aDrawer->VIsoAspect()->Number());
}
