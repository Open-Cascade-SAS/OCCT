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

#ifndef _HelixGeom_BuilderHelixGen_HeaderFile
#define _HelixGeom_BuilderHelixGen_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <HelixGeom_BuilderApproxCurve.hxx>

//! Base class for helix curve building algorithms with parameter management.
//!
//! This class extends HelixGeom_BuilderApproxCurve by adding helix-specific
//! geometric parameters:
//! - Parameter range (T1, T2) - angular range in radians
//! - Pitch - vertical distance per full turn (2*PI radians)
//! - Start radius (RStart) - radius at parameter T1
//! - Taper angle - angle for radius variation (0 = cylindrical)
//! - Orientation - clockwise or counter-clockwise
//!
//! Concrete implementations include:
//! - HelixGeom_BuilderHelix: Single helix approximation
//! - HelixGeom_BuilderHelixCoil: Multi-coil helix approximation
//!
//! @sa HelixGeom_BuilderApproxCurve, HelixGeom_HelixCurve
class HelixGeom_BuilderHelixGen : public HelixGeom_BuilderApproxCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Sets parameters for building helix curves
  Standard_EXPORT void SetCurveParameters(const Standard_Real    aT1,
                                          const Standard_Real    aT2,
                                          const Standard_Real    aPitch,
                                          const Standard_Real    aRStart,
                                          const Standard_Real    aTaperAngle,
                                          const Standard_Boolean bIsClockwise);

  //! Gets parameters for building helix curves
  Standard_EXPORT void CurveParameters(Standard_Real&    aT1,
                                       Standard_Real&    aT2,
                                       Standard_Real&    aPitch,
                                       Standard_Real&    aRStart,
                                       Standard_Real&    aTaperAngle,
                                       Standard_Boolean& bIsClockwise) const;

protected:
  //! Sets default parameters
  Standard_EXPORT HelixGeom_BuilderHelixGen();
  Standard_EXPORT virtual ~HelixGeom_BuilderHelixGen();

  Standard_Real    myT1;
  Standard_Real    myT2;
  Standard_Real    myPitch;
  Standard_Real    myRStart;
  Standard_Real    myTaperAngle;
  Standard_Boolean myIsClockWise;

private:
};

#endif // _HelixGeom_BuilderHelixGen_HeaderFile
