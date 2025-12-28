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

#include <HelixGeom_BuilderHelixGen.hxx>

#include <cmath>

//=================================================================================================

HelixGeom_BuilderHelixGen::HelixGeom_BuilderHelixGen()
    : 
      myT1(0.0),
      myT2(2.0 * M_PI),
      myPitch(1.0),
      myRStart(1.0),
      myTaperAngle(0.0),
      myIsClockWise(true)
{
}

//=================================================================================================

HelixGeom_BuilderHelixGen::~HelixGeom_BuilderHelixGen() = default;

//=================================================================================================

void HelixGeom_BuilderHelixGen::SetCurveParameters(const double aT1,
                                                   const double aT2,
                                                   const double aPitch,
                                                   const double aRStart,
                                                   const double aTaperAngle,
                                                   const bool   aIsCW)
{
  // Input validation would typically be done in derived classes or Load methods
  // Base class just stores the values
  myT1          = aT1;
  myT2          = aT2;
  myPitch       = aPitch;
  myRStart      = aRStart;
  myTaperAngle  = aTaperAngle;
  myIsClockWise = aIsCW;
}

//=================================================================================================

void HelixGeom_BuilderHelixGen::CurveParameters(double& aT1,
                                                double& aT2,
                                                double& aPitch,
                                                double& aRStart,
                                                double& aTaperAngle,
                                                bool&   aIsClockWise) const
{
  aT1          = myT1;
  aT2          = myT2;
  aPitch       = myPitch;
  aRStart      = myRStart;
  aTaperAngle  = myTaperAngle;
  aIsClockWise = myIsClockWise;
}
