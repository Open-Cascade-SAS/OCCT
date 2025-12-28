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
  Standard_EXPORT BRepLib_ValidateEdge(const occ::handle<Adaptor3d_Curve>          theReferenceCurve,
                                       const occ::handle<Adaptor3d_CurveOnSurface> theOtherCurve,
                                       bool                       theSameParameter);

  //! Sets method to calculate distance: Calculating in finite number of points (if theIsExact
  //! is false, faster, but possible not correct result) or exact calculating by using
  //! BRepLib_CheckCurveOnSurface class (if theIsExact is true, slowly, but more correctly).
  //! Exact method is used only when edge is SameParameter.
  //! Default method is calculating in finite number of points
  void SetExactMethod(bool theIsExact) { myIsExactMethod = theIsExact; }

  //! Returns true if exact method selected
  bool IsExactMethod() { return myIsExactMethod; }

  //! Sets parallel flag
  void SetParallel(bool theIsMultiThread) { myIsMultiThread = theIsMultiThread; }

  //! Returns true if parallel flag is set
  bool IsParallel() { return myIsMultiThread; }

  //! Set control points number (if you need a value other than 22)
  void SetControlPointsNumber(int theControlPointsNumber)
  {
    myControlPointsNumber = theControlPointsNumber;
  }

  //! Sets limit to compute a distance in the Process() function. If the distance greater than
  //! theToleranceForChecking the Process() function stopped. Use this in case checking of
  //! tolerance for best performcnce. Has no effect in case using exact method.
  void SetExitIfToleranceExceeded(double theToleranceForChecking);

  //! Computes the max distance for the 3d curve <myReferenceCurve>
  //! and curve on surface <myOtherCurve>. If the SetExitIfToleranceExceeded()
  //!  function was called before <myCalculatedDistance> contains first
  //! greater than SetExitIfToleranceExceeded() parameter value. In case
  //! using exact method always computes real max distance.
  Standard_EXPORT void Process();

  //! Returns true if the distance has been found for all points
  bool IsDone() const { return myIsDone; }

  //! Returns true if computed distance is less than <theToleranceToCheck>
  Standard_EXPORT bool CheckTolerance(double theToleranceToCheck);

  //! Returns max distance
  Standard_EXPORT double GetMaxDistance();

  //! Increase <theToleranceToUpdate> if max distance is greater than <theToleranceToUpdate>
  Standard_EXPORT void UpdateTolerance(double& theToleranceToUpdate);

private:
  //! Adds some margin for distance checking
  double correctTolerance(double theTolerance);

  //! Calculating in finite number of points
  void processApprox();

  //! Calculating by using BRepLib_CheckCurveOnSurface class
  void processExact();

private:
  occ::handle<Adaptor3d_Curve>          myReferenceCurve;
  occ::handle<Adaptor3d_CurveOnSurface> myOtherCurve;
  bool                 mySameParameter;
  int                 myControlPointsNumber;
  double                    myToleranceForChecking;
  double                    myCalculatedDistance;
  bool                 myExitIfToleranceExceeded;
  bool                 myIsDone;
  bool                 myIsExactMethod;
  bool                 myIsMultiThread;
};

#endif // _BRepLib_ValidateEdge_HeaderFile
