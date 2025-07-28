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

#include <Geom_Curve.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>
#include <HelixGeom_BuilderHelix.hxx>
#include <HelixGeom_BuilderHelixCoil.hxx>
#include <TColGeom_SequenceOfCurve.hxx>

//=================================================================================================

HelixGeom_BuilderHelix::HelixGeom_BuilderHelix()
    : HelixGeom_BuilderHelixGen()
{
}

//=================================================================================================

HelixGeom_BuilderHelix::~HelixGeom_BuilderHelix() {}

//=================================================================================================

void HelixGeom_BuilderHelix::SetPosition(const gp_Ax2& aAx2)
{
  myPosition = aAx2;
}

//=================================================================================================

const gp_Ax2& HelixGeom_BuilderHelix::Position() const
{
  return myPosition;
}

//=================================================================================================

void HelixGeom_BuilderHelix::Perform()
{
  myErrorStatus   = 0;
  myWarningStatus = 0;

  // Initialize variables for helix segmentation
  Standard_Integer           iErr, aN, i, aNbC;
  Standard_Real              aTwoPI, dT, aT1x, aT2x, aTR;
  HelixGeom_BuilderHelixCoil aBHC;

  // Initialize result containers
  myCurves.Clear();
  myTolReached = -1.;
  aTwoPI       = 2. * M_PI;
  aBHC.SetTolerance(myTolerance);
  aBHC.SetApproxParameters(myCont, myMaxDegree, myMaxSeg);

  // Determine number of full turns for segmentation
  dT = myT2 - myT1;
  aN = (Standard_Integer)(dT / aTwoPI);
  if (!aN)
  {
    aBHC.SetCurveParameters(myT1, myT2, myPitch, myRStart, myTaperAngle, myIsClockWise);
    aBHC.Perform();
    iErr = aBHC.ErrorStatus();
    if (iErr)
    {
      myErrorStatus = 2;
      return;
    }
    const TColGeom_SequenceOfCurve& aSC = aBHC.Curves();
    const Handle(Geom_Curve)&       aC  = aSC(1);
    myCurves.Append(aC);
    myTolReached = aBHC.ToleranceReached();
  }
  else
  {
    // Case: helix spans multiple full turns - process in segments
    Standard_Boolean bIsCylindrical;
    Standard_Real    aTolAngle;

    aTolAngle      = 1.e-4;
    bIsCylindrical = fabs(myTaperAngle) < aTolAngle;
    aT1x           = myT1;
    aT2x           = myT1 + aTwoPI;
    for (i = 1; i <= aN; ++i)
    {
      if (i > 1 && bIsCylindrical)
      {
        // Optimization: for cylindrical helixes, reuse first coil with translation
        Handle(Geom_Curve) aCi;
        gp_Pnt             aP1, aPi;

        const Handle(Geom_Curve)& aC1 = myCurves(1);
        aC1->D0(aC1->FirstParameter(), aP1);
        aPi.SetCoord(aP1.X(), aP1.Y(), aP1.Z() + (i - 1) * myPitch);
        aCi = Handle(Geom_Curve)::DownCast(aC1->Translated(aP1, aPi));

        myCurves.Append(aCi);
        aT1x = aT2x;
        aT2x = aT1x + aTwoPI;
        // Skip to next iteration for optimization
        continue;
      }

      aBHC.SetCurveParameters(aT1x, aT2x, myPitch, myRStart, myTaperAngle, myIsClockWise);
      // Perform approximation for this segment
      aBHC.Perform();
      iErr = aBHC.ErrorStatus();
      if (iErr)
      {
        myErrorStatus = 2;
        return;
      }
      // Extract approximated curves from builder
      const TColGeom_SequenceOfCurve& aSC = aBHC.Curves();
      const Handle(Geom_Curve)&       aC  = aSC(1);
      myCurves.Append(aC);
      aTR = aBHC.ToleranceReached();
      if (aTR > myTolReached)
      {
        myTolReached = aTR;
      }
      // Move to next segment parameters
      aT1x = aT2x;
      aT2x = aT1x + aTwoPI;
    } // for (i=1; i<=aN; ++i) {
    // Handle remaining partial turn if any
    aT2x              = myT2;
    Standard_Real eps = 1.e-7 * aTwoPI;
    if (fabs(aT2x - aT1x) > eps)
    {
      aBHC.SetCurveParameters(aT1x, aT2x, myPitch, myRStart, myTaperAngle, myIsClockWise);
      aBHC.Perform();
      iErr = aBHC.ErrorStatus();
      if (iErr)
      {
        myErrorStatus = 2;
        return;
      }
      // Extract curves from the final partial segment
      const TColGeom_SequenceOfCurve& aSC = aBHC.Curves();
      const Handle(Geom_Curve)&       aC  = aSC(1);
      myCurves.Append(aC);
      aTR = aBHC.ToleranceReached();
      if (aTR > myTolReached)
      {
        myTolReached = aTR;
      }
    }
  }
  // Apply coordinate system transformation to all curves
  // Transformation
  gp_Trsf aTrsf;
  gp_Ax3  aAx3, aAx3x(myPosition);
  // Set transformation from standard coordinate system to helix position
  aTrsf.SetDisplacement(aAx3, aAx3x);
  // Apply transformation to all generated curves
  aNbC = myCurves.Length();
  for (i = 1; i <= aNbC; ++i)
  {
    Handle(Geom_Curve)& aC = myCurves(i);
    aC->Transform(aTrsf);
  }
}
