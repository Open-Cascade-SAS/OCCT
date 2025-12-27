// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <HelixGeom_BuilderApproxCurve.hxx>
#include <Geom_Curve.hxx>
#include <NCollection_Sequence.hxx>

//=================================================================================================

HelixGeom_BuilderApproxCurve::HelixGeom_BuilderApproxCurve()
    : myErrorStatus(0),
      myWarningStatus(0),
      myTolerance(0.0001),
      myCont(GeomAbs_C2),
      myMaxDegree(8),
      myMaxSeg(150),
      myTolReached(99.0)
{
}

//=================================================================================================

HelixGeom_BuilderApproxCurve::~HelixGeom_BuilderApproxCurve() {}

//=================================================================================================

void HelixGeom_BuilderApproxCurve::SetApproxParameters(const GeomAbs_Shape    aCont,
                                                       const int aMaxDegree,
                                                       const int aMaxSeg)
{
  myCont      = aCont;
  myMaxDegree = aMaxDegree;
  myMaxSeg    = aMaxSeg;
}

//=================================================================================================

void HelixGeom_BuilderApproxCurve::ApproxParameters(GeomAbs_Shape&    aCont,
                                                    int& aMaxDegree,
                                                    int& aMaxSeg) const
{
  aCont      = myCont;
  aMaxDegree = myMaxDegree;
  aMaxSeg    = myMaxSeg;
}

//=================================================================================================

void HelixGeom_BuilderApproxCurve::SetTolerance(const double aTolerance)
{
  myTolerance = aTolerance;
}

//=================================================================================================

double HelixGeom_BuilderApproxCurve::Tolerance() const
{
  return myTolerance;
}

//=================================================================================================

double HelixGeom_BuilderApproxCurve::ToleranceReached() const
{
  return myTolReached;
}

//=================================================================================================

const NCollection_Sequence<occ::handle<Geom_Curve>>& HelixGeom_BuilderApproxCurve::Curves() const
{
  return myCurves;
}

//=================================================================================================

int HelixGeom_BuilderApproxCurve::ErrorStatus() const
{
  return myErrorStatus;
}

//=================================================================================================

int HelixGeom_BuilderApproxCurve::WarningStatus() const
{
  return myWarningStatus;
}
