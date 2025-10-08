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

#include <Geom_BSplineCurve.hxx>
#include <HelixGeom_BuilderHelixCoil.hxx>
#include <HelixGeom_HelixCurve.hxx>
#include <HelixGeom_Tools.hxx>

//=================================================================================================

HelixGeom_BuilderHelixCoil::HelixGeom_BuilderHelixCoil()
    : HelixGeom_BuilderHelixGen()
{
  myT1          = 0.;
  myT2          = 2. * M_PI;
  myPitch       = 1.;
  myRStart      = 1.;
  myTaperAngle  = 0.;
  myIsClockWise = Standard_True;
}

//=================================================================================================

HelixGeom_BuilderHelixCoil::~HelixGeom_BuilderHelixCoil() {}

//=================================================================================================

void HelixGeom_BuilderHelixCoil::Perform()
{
  myErrorStatus   = 0;
  myWarningStatus = 0;
  // Initialize variables for curve approximation
  Standard_Integer             iErr;
  HelixGeom_HelixCurve         aAdaptor;
  Handle(HelixGeom_HelixCurve) aHAdaptor;
  Handle(Geom_BSplineCurve)    aBC;
  // Clear previous results and setup helix adaptor
  myCurves.Clear();
  // Load helix parameters into the adaptor
  aAdaptor.Load(myT1, myT2, myPitch, myRStart, myTaperAngle, myIsClockWise);
  aHAdaptor = new HelixGeom_HelixCurve(aAdaptor);
  // Perform B-spline approximation of the helix curve
  iErr = HelixGeom_Tools::ApprCurve3D(aHAdaptor,
                                      myTolerance,
                                      myCont,
                                      myMaxSeg,
                                      myMaxDegree,
                                      aBC,
                                      myTolReached);
  if (iErr)
  {
    myErrorStatus = 2;
  }
  else
  {
    myCurves.Append(aBC);
  }
}
