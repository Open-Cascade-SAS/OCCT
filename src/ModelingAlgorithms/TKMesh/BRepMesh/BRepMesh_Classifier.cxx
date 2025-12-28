// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <BRepMesh_Classifier.hxx>

#include <Precision.hxx>
#include <gp_Pnt2d.hxx>
#include <CSLib_Class2d.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepMesh_Classifier, Standard_Transient)

//=================================================================================================

BRepMesh_Classifier::BRepMesh_Classifier() {}

//=================================================================================================

BRepMesh_Classifier::~BRepMesh_Classifier() {}

//=================================================================================================

TopAbs_State BRepMesh_Classifier::Perform(const gp_Pnt2d& thePoint) const
{
  bool isOut = false;
  int aNb   = myTabClass.Length();

  for (int i = 0; i < aNb; i++)
  {
    const int aCur = myTabClass(i)->SiDans(thePoint);
    if (aCur == 0)
    {
      // Point is ON, but mark it as OUT
      isOut = true;
    }
    else
    {
      isOut = myTabOrient(i) ? (aCur == -1) : (aCur == 1);
    }

    if (isOut)
    {
      return TopAbs_OUT;
    }
  }

  return TopAbs_IN;
}

//=================================================================================================

void BRepMesh_Classifier::RegisterWire(const NCollection_Sequence<const gp_Pnt2d*>&   theWire,
                                       const std::pair<double, double>& theTolUV,
                                       const std::pair<double, double>& theRangeU,
                                       const std::pair<double, double>& theRangeV)
{
  const int aNbPnts = theWire.Length();
  if (aNbPnts < 2)
  {
    return;
  }

  // Accumulate angle
  NCollection_Array1<gp_Pnt2d> aPClass(1, aNbPnts);
  double        anAngle = 0.0;
  const gp_Pnt2d *     p1 = theWire(1), *p2 = theWire(2), *p3;
  aPClass(1) = *p1;
  aPClass(2) = *p2;

  constexpr double aAngTol      = Precision::Angular();
  constexpr double aSqConfusion = Precision::PConfusion() * Precision::PConfusion();

  for (int i = 1; i <= aNbPnts; i++)
  {
    int ii = i + 2;
    if (ii > aNbPnts)
    {
      p3 = &aPClass(ii - aNbPnts);
    }
    else
    {
      p3          = theWire.Value(ii);
      aPClass(ii) = *p3;
    }

    const gp_Vec2d A(*p1, *p2), B(*p2, *p3);
    if (A.SquareMagnitude() > aSqConfusion && B.SquareMagnitude() > aSqConfusion)
    {
      const double aCurAngle    = A.Angle(B);
      const double aCurAngleAbs = std::abs(aCurAngle);
      // Check if vectors are opposite
      if (aCurAngleAbs > aAngTol && (M_PI - aCurAngleAbs) > aAngTol)
      {
        anAngle += aCurAngle;
        p1 = p2;
      }
    }
    p2 = p3;
  }
  // Check for zero angle - treat self intersecting wire as outer
  if (std::abs(anAngle) < aAngTol)
    anAngle = 0.0;

  myTabClass.Append(new CSLib_Class2d(aPClass,
                                      theTolUV.first,
                                      theTolUV.second,
                                      theRangeU.first,
                                      theRangeV.first,
                                      theRangeU.second,
                                      theRangeV.second));

  myTabOrient.Append(!(anAngle < 0.0));
}
