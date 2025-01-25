// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _BRepLib_ValidateEdge_HeaderFile
#define _BRepLib_ValidateEdge_HeaderFile

#include <Standard_TypeDef.hxx>
#include <Standard_Handle.hxx>

class Adaptor3d_Curve;
class Adaptor3d_CurveOnSurface;

//! Computes the max distance between 3D-curve and curve on surface.
//! This class uses 2 methods: approximate using finite
//! number of points (default) and exact
class BRepLib_ValidateEdge
{
public:
  //! Initialization constructor
  Standard_EXPORT BRepLib_ValidateEdge(const Handle(Adaptor3d_Curve)          theReferenceCurve,
                                       const Handle(Adaptor3d_CurveOnSurface) theOtherCurve,
                                       Standard_Boolean                       theSameParameter);

  //! Sets method to calculate distance: Calculating in finite number of points (if theIsExact
  //! is false, faster, but possible not correct result) or exact calculating by using
  //! BRepLib_CheckCurveOnSurface class (if theIsExact is true, slowly, but more correctly).
  //! Exact method is used only when edge is SameParameter.
  //! Default method is calculating in finite number of points
  void SetExactMethod(Standard_Boolean theIsExact) { myIsExactMethod = theIsExact; }

  //! Returns true if exact method selected
  Standard_Boolean IsExactMethod() { return myIsExactMethod; }

  //! Sets parallel flag
  void SetParallel(Standard_Boolean theIsMultiThread) { myIsMultiThread = theIsMultiThread; }

  //! Returns true if parallel flag is set
  Standard_Boolean IsParallel() { return myIsMultiThread; }

  //! Set control points number (if you need a value other than 22)
  void SetControlPointsNumber(Standard_Integer theControlPointsNumber)
  {
    myControlPointsNumber = theControlPointsNumber;
  }

  //! Sets limit to compute a distance in the Process() function. If the distance greater than
  //! theToleranceForChecking the Process() function stopped. Use this in case checking of
  //! tolerance for best performcnce. Has no effect in case using exact method.
  void SetExitIfToleranceExceeded(Standard_Real theToleranceForChecking);

  //! Computes the max distance for the 3d curve <myReferenceCurve>
  //! and curve on surface <myOtherCurve>. If the SetExitIfToleranceExceeded()
  //!  function was called before <myCalculatedDistance> contains first
  //! greater than SetExitIfToleranceExceeded() parameter value. In case
  //! using exact method always computes real max distance.
  Standard_EXPORT void Process();

  //! Returns true if the distance has been found for all points
  Standard_Boolean IsDone() const { return myIsDone; }

  //! Returns true if computed distance is less than <theToleranceToCheck>
  Standard_EXPORT Standard_Boolean CheckTolerance(Standard_Real theToleranceToCheck);

  //! Returns max distance
  Standard_EXPORT Standard_Real GetMaxDistance();

  //! Increase <theToleranceToUpdate> if max distance is greater than <theToleranceToUpdate>
  Standard_EXPORT void UpdateTolerance(Standard_Real& theToleranceToUpdate);

private:
  //! Adds some margin for distance checking
  Standard_Real correctTolerance(Standard_Real theTolerance);

  //! Calculating in finite number of points
  void processApprox();

  //! Calculating by using BRepLib_CheckCurveOnSurface class
  void processExact();

private:
  Handle(Adaptor3d_Curve)          myReferenceCurve;
  Handle(Adaptor3d_CurveOnSurface) myOtherCurve;
  Standard_Boolean                 mySameParameter;
  Standard_Integer                 myControlPointsNumber;
  Standard_Real                    myToleranceForChecking;
  Standard_Real                    myCalculatedDistance;
  Standard_Boolean                 myExitIfToleranceExceeded;
  Standard_Boolean                 myIsDone;
  Standard_Boolean                 myIsExactMethod;
  Standard_Boolean                 myIsMultiThread;
};

#endif // _BRepLib_ValidateEdge_HeaderFile
