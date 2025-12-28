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

// Great zoom leads to non-coincidence of
// a point and non-infinite lines passing through this point:

#include <BndLib_Add3dCurve.hxx>
#include <GCPnts_TangentialDeflection.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <StdPrs_DeflectionCurve.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Array1.hxx>

//=================================================================================================

static double GetDeflection(const Adaptor3d_Curve&           aCurve,
                            const double                     U1,
                            const double                     U2,
                            const occ::handle<Prs3d_Drawer>& aDrawer)
{
  double TheDeflection;

  if (aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE)
  {
    // On calcule la fleche en fonction des min max globaux de la piece:
    Bnd_Box Total;
    BndLib_Add3dCurve::Add(aCurve, U1, U2, 0., Total);
    double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    Total.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    double m = RealLast();
    if (!(Total.IsOpenXmin() || Total.IsOpenXmax()))
      m = std::abs(aXmax - aXmin);
    if (!(Total.IsOpenYmin() || Total.IsOpenYmax()))
      m = std::max(m, std::abs(aYmax - aYmin));
    if (!(Total.IsOpenZmin() || Total.IsOpenZmax()))
      m = std::max(m, std::abs(aZmax - aZmin));

    m = std::min(m, aDrawer->MaximalParameterValue());
    m = std::max(m, Precision::Confusion());

    TheDeflection = m * aDrawer->DeviationCoefficient();
  }
  else
    TheDeflection = aDrawer->MaximalChordialDeviation();

  return TheDeflection;
}

//=================================================================================================

static bool FindLimits(const Adaptor3d_Curve& aCurve,
                       const double           aLimit,
                       double&                First,
                       double&                Last)
{
  First         = aCurve.FirstParameter();
  Last          = aCurve.LastParameter();
  bool firstInf = Precision::IsNegativeInfinite(First);
  bool lastInf  = Precision::IsPositiveInfinite(Last);

  if (firstInf || lastInf)
  {
    gp_Pnt P1, P2;
    double delta = 1;
    int    count = 0;
    if (firstInf && lastInf)
    {
      do
      {
        if (count++ == 100000)
          return false;
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
        if (count++ == 100000)
          return false;
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
        if (count++ == 100000)
          return false;
        delta *= 2;
        Last = First + delta;
        aCurve.D0(Last, P2);
      } while (P1.Distance(P2) < aLimit);
    }
  }
  return true;
}

//=================================================================================================

static void drawCurve(Adaptor3d_Curve&                    aCurve,
                      const occ::handle<Graphic3d_Group>& aGroup,
                      const double                        TheDeflection,
                      const double                        anAngle,
                      const double                        U1,
                      const double                        U2,
                      NCollection_Sequence<gp_Pnt>&       Points)
{
  switch (aCurve.GetType())
  {
    case GeomAbs_Line: {
      gp_Pnt p1 = aCurve.Value(U1);
      gp_Pnt p2 = aCurve.Value(U2);
      Points.Append(p1);
      Points.Append(p2);
      if (!aGroup.IsNull())
      {
        occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(2);
        aPrims->AddVertex(p1);
        aPrims->AddVertex(p2);
        aGroup->AddPrimitiveArray(aPrims);
      }
      break;
    }
    default: {
      const int                  nbinter = aCurve.NbIntervals(GeomAbs_C1);
      NCollection_Array1<double> T(1, nbinter + 1);
      aCurve.Intervals(T, GeomAbs_C1);

      double                       theU1, theU2;
      int                          NumberOfPoints, i, j;
      NCollection_Sequence<gp_Pnt> SeqP;

      for (j = 1; j <= nbinter; j++)
      {
        theU1 = T(j);
        theU2 = T(j + 1);
        if (theU2 > U1 && theU1 < U2)
        {
          theU1 = std::max(theU1, U1);
          theU2 = std::min(theU2, U2);

          GCPnts_TangentialDeflection Algo(aCurve, theU1, theU2, anAngle, TheDeflection);
          NumberOfPoints = Algo.NbPoints();

          if (NumberOfPoints > 0)
          {
            for (i = 1; i <= NumberOfPoints; i++)
              SeqP.Append(Algo.Value(i));
          }
        }
      }

      occ::handle<Graphic3d_ArrayOfPolylines> aPrims;
      if (!aGroup.IsNull())
        aPrims = new Graphic3d_ArrayOfPolylines(SeqP.Length());

      for (i = 1; i <= SeqP.Length(); i++)
      {
        const gp_Pnt& p = SeqP.Value(i);
        Points.Append(p);
        if (!aGroup.IsNull())
        {
          aPrims->AddVertex(p);
        }
      }
      if (!aGroup.IsNull())
      {
        aGroup->AddPrimitiveArray(aPrims);
      }
    }
  }
}

//=================================================================================================

static bool MatchCurve(const double           X,
                       const double           Y,
                       const double           Z,
                       const double           aDistance,
                       const Adaptor3d_Curve& aCurve,
                       const double           TheDeflection,
                       const double           anAngle,
                       const double           U1,
                       const double           U2)
{
  double retdist;
  switch (aCurve.GetType())
  {
    case GeomAbs_Line: {
      gp_Pnt p1 = aCurve.Value(U1);
      if (std::abs(X - p1.X()) + std::abs(Y - p1.Y()) + std::abs(Z - p1.Z()) <= aDistance)
        return true;
      gp_Pnt p2 = aCurve.Value(U2);
      if (std::abs(X - p2.X()) + std::abs(Y - p2.Y()) + std::abs(Z - p2.Z()) <= aDistance)
        return true;
      return Prs3d::MatchSegment(X, Y, Z, aDistance, p1, p2, retdist);
    }
    case GeomAbs_Circle: {
      const double Radius = aCurve.Circle().Radius();
      if (!Precision::IsInfinite(Radius))
      {
        const double DU = std::sqrt(8.0 * TheDeflection / Radius);
        const double Er = std::abs(U2 - U1) / DU;
        const int    N  = std::max(2, (int)std::trunc(Er));
        if (N > 0)
        {
          gp_Pnt p1, p2;
          for (int Index = 1; Index <= N + 1; Index++)
          {
            p2 = aCurve.Value(U1 + (Index - 1) * DU);
            if (std::abs(X - p2.X()) + std::abs(Y - p2.Y()) + std::abs(Z - p2.Z()) <= aDistance)
              return true;

            if (Index > 1)
            {
              if (Prs3d::MatchSegment(X, Y, Z, aDistance, p1, p2, retdist))
                return true;
            }
            p1 = p2;
          }
        }
      }
      break;
    }
    default: {
      GCPnts_TangentialDeflection Algo(aCurve, U1, U2, anAngle, TheDeflection);
      const int                   NumberOfPoints = Algo.NbPoints();
      if (NumberOfPoints > 0)
      {
        gp_Pnt p1, p2;
        for (int i = 1; i <= NumberOfPoints; i++)
        {
          p2 = Algo.Value(i);
          if (std::abs(X - p2.X()) + std::abs(Y - p2.Y()) + std::abs(Z - p2.Z()) <= aDistance)
            return true;
          if (i > 1)
          {
            if (Prs3d::MatchSegment(X, Y, Z, aDistance, p1, p2, retdist))
              return true;
          }
          p1 = p2;
        }
      }
    }
  }
  return false;
}

//=================================================================================================

void StdPrs_DeflectionCurve::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                 Adaptor3d_Curve&                       aCurve,
                                 const occ::handle<Prs3d_Drawer>&       aDrawer,
                                 const bool                             theToDrawCurve)
{
  occ::handle<Graphic3d_Group> aGroup;
  if (theToDrawCurve)
  {
    aGroup = aPresentation->CurrentGroup();
    aGroup->SetPrimitivesAspect(aDrawer->LineAspect()->Aspect());
  }

  double V1, V2;
  if (FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2))
  {
    NCollection_Sequence<gp_Pnt> Points;
    drawCurve(aCurve,
              aGroup,
              GetDeflection(aCurve, V1, V2, aDrawer),
              aDrawer->DeviationAngle(),
              V1,
              V2,
              Points);

    if (aDrawer->LineArrowDraw() && !aGroup.IsNull())
    {
      gp_Pnt Location;
      gp_Vec Direction;
      aCurve.D1(V2, Location, Direction);
      Prs3d_Arrow::Draw(aGroup,
                        Location,
                        gp_Dir(Direction),
                        aDrawer->ArrowAspect()->Angle(),
                        aDrawer->ArrowAspect()->Length());
    }
  }
}

//=================================================================================================

void StdPrs_DeflectionCurve::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                 Adaptor3d_Curve&                       aCurve,
                                 const double                           U1,
                                 const double                           U2,
                                 const occ::handle<Prs3d_Drawer>&       aDrawer,
                                 const bool                             theToDrawCurve)
{
  occ::handle<Graphic3d_Group> aGroup;
  if (theToDrawCurve)
  {
    aGroup = aPresentation->CurrentGroup();
    aGroup->SetPrimitivesAspect(aDrawer->LineAspect()->Aspect());
  }

  double V1 = U1;
  double V2 = U2;

  if (Precision::IsNegativeInfinite(V1))
    V1 = -aDrawer->MaximalParameterValue();
  if (Precision::IsPositiveInfinite(V2))
    V2 = aDrawer->MaximalParameterValue();

  NCollection_Sequence<gp_Pnt> Points;
  drawCurve(aCurve,
            aGroup,
            GetDeflection(aCurve, V1, V2, aDrawer),
            aDrawer->DeviationAngle(),
            V1,
            V2,
            Points);

  if (aDrawer->LineArrowDraw() && !aGroup.IsNull())
  {
    gp_Pnt Location;
    gp_Vec Direction;
    aCurve.D1(V2, Location, Direction);
    Prs3d_Arrow::Draw(aGroup,
                      Location,
                      gp_Dir(Direction),
                      aDrawer->ArrowAspect()->Angle(),
                      aDrawer->ArrowAspect()->Length());
  }
}

//=================================================================================================

void StdPrs_DeflectionCurve::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                 Adaptor3d_Curve&                       aCurve,
                                 const double                           U1,
                                 const double                           U2,
                                 const double                           aDeflection,
                                 NCollection_Sequence<gp_Pnt>&          Points,
                                 const double                           anAngle,
                                 const bool                             theToDrawCurve)
{
  occ::handle<Graphic3d_Group> aGroup;
  if (theToDrawCurve)
  {
    aGroup = aPresentation->CurrentGroup();
  }

  drawCurve(aCurve, aGroup, aDeflection, anAngle, U1, U2, Points);
}

//=================================================================================================

void StdPrs_DeflectionCurve::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                 Adaptor3d_Curve&                       aCurve,
                                 const double                           aDeflection,
                                 const double                           aLimit,
                                 const double                           anAngle,
                                 const bool                             theToDrawCurve)
{
  double V1, V2;
  if (!FindLimits(aCurve, aLimit, V1, V2))
  {
    return;
  }

  occ::handle<Graphic3d_Group> aGroup;
  if (theToDrawCurve)
  {
    aGroup = aPresentation->CurrentGroup();
  }

  NCollection_Sequence<gp_Pnt> Points;
  drawCurve(aCurve, aGroup, aDeflection, anAngle, V1, V2, Points);
}

//=================================================================================================

void StdPrs_DeflectionCurve::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                                 Adaptor3d_Curve&                       aCurve,
                                 const double                           aDeflection,
                                 const occ::handle<Prs3d_Drawer>&       aDrawer,
                                 NCollection_Sequence<gp_Pnt>&          Points,
                                 const bool                             theToDrawCurve)
{
  double V1, V2;
  if (!FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2))
  {
    return;
  }

  occ::handle<Graphic3d_Group> aGroup;
  if (theToDrawCurve)
  {
    aGroup = aPresentation->CurrentGroup();
  }
  drawCurve(aCurve, aGroup, aDeflection, aDrawer->DeviationAngle(), V1, V2, Points);
}

//=================================================================================================

bool StdPrs_DeflectionCurve::Match(const double                     X,
                                   const double                     Y,
                                   const double                     Z,
                                   const double                     aDistance,
                                   const Adaptor3d_Curve&           aCurve,
                                   const occ::handle<Prs3d_Drawer>& aDrawer)
{
  double V1, V2;
  if (FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2))
  {
    return MatchCurve(X,
                      Y,
                      Z,
                      aDistance,
                      aCurve,
                      GetDeflection(aCurve, V1, V2, aDrawer),
                      aDrawer->DeviationAngle(),
                      V1,
                      V2);
  }
  return false;
}

//=================================================================================================

bool StdPrs_DeflectionCurve::Match(const double                     X,
                                   const double                     Y,
                                   const double                     Z,
                                   const double                     aDistance,
                                   const Adaptor3d_Curve&           aCurve,
                                   const double                     U1,
                                   const double                     U2,
                                   const occ::handle<Prs3d_Drawer>& aDrawer)
{
  double V1 = U1;
  double V2 = U2;

  if (Precision::IsNegativeInfinite(V1))
    V1 = -aDrawer->MaximalParameterValue();
  if (Precision::IsPositiveInfinite(V2))
    V2 = aDrawer->MaximalParameterValue();

  return MatchCurve(X,
                    Y,
                    Z,
                    aDistance,
                    aCurve,
                    GetDeflection(aCurve, V1, V2, aDrawer),
                    aDrawer->DeviationAngle(),
                    V1,
                    V2);
}

//=================================================================================================

bool StdPrs_DeflectionCurve::Match(const double           X,
                                   const double           Y,
                                   const double           Z,
                                   const double           aDistance,
                                   const Adaptor3d_Curve& aCurve,
                                   const double           U1,
                                   const double           U2,
                                   const double           aDeflection,
                                   const double           anAngle)
{
  return MatchCurve(X, Y, Z, aDistance, aCurve, aDeflection, anAngle, U1, U2);
}

//=================================================================================================

bool StdPrs_DeflectionCurve::Match(const double           X,
                                   const double           Y,
                                   const double           Z,
                                   const double           aDistance,
                                   const Adaptor3d_Curve& aCurve,
                                   const double           aDeflection,
                                   const double           aLimit,
                                   const double           anAngle)
{
  double V1, V2;
  if (FindLimits(aCurve, aLimit, V1, V2))
  {
    return MatchCurve(X, Y, Z, aDistance, aCurve, aDeflection, anAngle, V1, V2);
  }
  return false;
}
