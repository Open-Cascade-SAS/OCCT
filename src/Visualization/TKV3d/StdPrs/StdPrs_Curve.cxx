// Created on: 1995-08-04
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

// Great zoom leads to non-coincidence of
// a point and non-infinite lines passing through this point:

#include <Adaptor3d_Curve.hxx>
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
#include <StdPrs_Curve.hxx>
#include <NCollection_Sequence.hxx>

//=================================================================================================

static void FindLimits(const Adaptor3d_Curve& aCurve,
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

static void DrawCurve(const Adaptor3d_Curve&              aCurve,
                      const occ::handle<Graphic3d_Group>& aGroup,
                      const int                           NbP,
                      const double                        U1,
                      const double                        U2,
                      NCollection_Sequence<gp_Pnt>&       Points,
                      const bool                          drawCurve)
{
  int nbintervals = 1;

  if (aCurve.GetType() == GeomAbs_BSplineCurve)
  {
    nbintervals = aCurve.NbKnots() - 1;
    nbintervals = std::max(1, nbintervals / 3);
  }

  switch (aCurve.GetType())
  {
    case GeomAbs_Line: {
      gp_Pnt p1 = aCurve.Value(U1);
      gp_Pnt p2 = aCurve.Value(U2);
      Points.Append(p1);
      Points.Append(p2);
      if (drawCurve)
      {
        occ::handle<Graphic3d_ArrayOfSegments> aPrims = new Graphic3d_ArrayOfSegments(2);
        aPrims->AddVertex(p1);
        aPrims->AddVertex(p2);
        aGroup->AddPrimitiveArray(aPrims);
      }
    }
    break;
    default: {
      const int    N  = std::max(2, NbP * nbintervals);
      const double DU = (U2 - U1) / (N - 1);
      gp_Pnt       p;

      occ::handle<Graphic3d_ArrayOfPolylines> aPrims;
      if (drawCurve)
        aPrims = new Graphic3d_ArrayOfPolylines(N);

      for (int i = 1; i <= N; i++)
      {
        p = aCurve.Value(U1 + (i - 1) * DU);
        Points.Append(p);
        if (drawCurve)
          aPrims->AddVertex(p);
      }
      if (drawCurve)
        aGroup->AddPrimitiveArray(aPrims);
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
                       const int              NbP,
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
      const double DU     = std::sqrt(8.0 * TheDeflection / Radius);
      const double Er     = std::abs(U2 - U1) / DU;
      const int    N      = std::max(2, (int)std::trunc(Er));
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
      break;
    }
    default: {
      const double DU = (U2 - U1) / (NbP - 1);
      gp_Pnt       p1, p2;
      for (int i = 1; i <= NbP; i++)
      {
        p2 = aCurve.Value(U1 + (i - 1) * DU);
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
  return false;
}

//=================================================================================================

void StdPrs_Curve::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                       const Adaptor3d_Curve&                 aCurve,
                       const occ::handle<Prs3d_Drawer>&       aDrawer,
                       const bool                             drawCurve)
{
  aPresentation->CurrentGroup()->SetPrimitivesAspect(aDrawer->LineAspect()->Aspect());

  double V1, V2;
  FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2);

  const int                    NbPoints = aDrawer->Discretisation();
  NCollection_Sequence<gp_Pnt> Pnts;
  DrawCurve(aCurve, aPresentation->CurrentGroup(), NbPoints, V1, V2, Pnts, drawCurve);

  if (aDrawer->LineArrowDraw())
  {
    gp_Pnt Location;
    gp_Vec Direction;
    aCurve.D1(aCurve.LastParameter(), Location, Direction);
    Prs3d_Arrow::Draw(aPresentation->CurrentGroup(),
                      Location,
                      gp_Dir(Direction),
                      aDrawer->ArrowAspect()->Angle(),
                      aDrawer->ArrowAspect()->Length());
  }
}

//=================================================================================================

void StdPrs_Curve::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                       const Adaptor3d_Curve&                 aCurve,
                       const occ::handle<Prs3d_Drawer>&       aDrawer,
                       NCollection_Sequence<gp_Pnt>&          Points,
                       const bool                             drawCurve)
{
  double V1, V2;
  FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2);

  const int NbPoints = aDrawer->Discretisation();
  DrawCurve(aCurve, aPresentation->CurrentGroup(), NbPoints, V1, V2, Points, drawCurve);
}

//=================================================================================================

void StdPrs_Curve::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                       const Adaptor3d_Curve&                 aCurve,
                       const double                           U1,
                       const double                           U2,
                       NCollection_Sequence<gp_Pnt>&          Points,
                       const int                              NbPoints,
                       const bool                             drawCurve)
{
  DrawCurve(aCurve, aPresentation->CurrentGroup(), NbPoints, U1, U2, Points, drawCurve);
}

//=================================================================================================

void StdPrs_Curve::Add(const occ::handle<Prs3d_Presentation>& aPresentation,
                       const Adaptor3d_Curve&                 aCurve,
                       const double                           U1,
                       const double                           U2,
                       const occ::handle<Prs3d_Drawer>&       aDrawer,
                       const bool                             drawCurve)
{
  aPresentation->CurrentGroup()->SetPrimitivesAspect(aDrawer->LineAspect()->Aspect());

  double V1 = U1;
  double V2 = U2;

  if (Precision::IsNegativeInfinite(V1))
    V1 = -aDrawer->MaximalParameterValue();
  if (Precision::IsPositiveInfinite(V2))
    V2 = aDrawer->MaximalParameterValue();

  const int                    NbPoints = aDrawer->Discretisation();
  NCollection_Sequence<gp_Pnt> Pnts;
  DrawCurve(aCurve, aPresentation->CurrentGroup(), NbPoints, V1, V2, Pnts, drawCurve);

  if (aDrawer->LineArrowDraw())
  {
    gp_Pnt Location;
    gp_Vec Direction;
    aCurve.D1(aCurve.LastParameter(), Location, Direction);
    Prs3d_Arrow::Draw(aPresentation->CurrentGroup(),
                      Location,
                      gp_Dir(Direction),
                      aDrawer->ArrowAspect()->Angle(),
                      aDrawer->ArrowAspect()->Length());
  }
}

//=================================================================================================

bool StdPrs_Curve::Match(const double                     X,
                         const double                     Y,
                         const double                     Z,
                         const double                     aDistance,
                         const Adaptor3d_Curve&           aCurve,
                         const occ::handle<Prs3d_Drawer>& aDrawer)
{
  double V1, V2;
  FindLimits(aCurve, aDrawer->MaximalParameterValue(), V1, V2);

  const int NbPoints = aDrawer->Discretisation();
  return MatchCurve(X,
                    Y,
                    Z,
                    aDistance,
                    aCurve,
                    aDrawer->MaximalChordialDeviation(),
                    NbPoints,
                    V1,
                    V2);
}

//=================================================================================================

bool StdPrs_Curve::Match(const double           X,
                         const double           Y,
                         const double           Z,
                         const double           aDistance,
                         const Adaptor3d_Curve& aCurve,
                         const double           aDeflection,
                         const double           aLimit,
                         const int              NbPoints)
{
  double V1, V2;
  FindLimits(aCurve, aLimit, V1, V2);

  return MatchCurve(X, Y, Z, aDistance, aCurve, aDeflection, NbPoints, V1, V2);
}

//=================================================================================================

bool StdPrs_Curve::Match(const double                     X,
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
                    aDrawer->MaximalChordialDeviation(),
                    aDrawer->Discretisation(),
                    V1,
                    V2);
}

//=================================================================================================

bool StdPrs_Curve::Match(const double           X,
                         const double           Y,
                         const double           Z,
                         const double           aDistance,
                         const Adaptor3d_Curve& aCurve,
                         const double           U1,
                         const double           U2,
                         const double           aDeflection,
                         const int              aNbPoints)
{
  return MatchCurve(X, Y, Z, aDistance, aCurve, aDeflection, aNbPoints, U1, U2);
}
