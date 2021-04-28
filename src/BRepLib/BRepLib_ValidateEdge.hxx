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

#include<Standard_TypeDef.hxx>
#include<Standard_Handle.hxx>

class Adaptor3d_Curve;
class Adaptor3d_CurveOnSurface;

//! Computes the max distance between 3D-curve and curve on 
//! surface in fixed points number
class BRepLib_ValidateEdge
{
public:
  //! Initialization constructor
  Standard_EXPORT BRepLib_ValidateEdge(const Handle(Adaptor3d_Curve) theReferenceCurve,
                                       const Handle(Adaptor3d_CurveOnSurface) theOtherCurve,
                                       Standard_Boolean theSameParameter);

  //! Set control points number (if you need a value other than 22)
  void SetControlPointsNumber(Standard_Integer theControlPointsNumber)
  {
    myControlPointsNumber = theControlPointsNumber;
  }

  //! Sets the maximal allowed distance in the Process() function. If the distance greater than 
  //! theToleranceForChecking the Process() function stops. Use this for best performance 
  //! in case of checking of tolerance.
  Standard_EXPORT void SetExitIfToleranceExceeded(Standard_Real theToleranceForChecking);

  //! Computes the max distance for the 3d curve <myReferenceCurve>
  //! and curve on surface <myOtherCurve>. If the SetExitIfToleranceExceeded()
  //!  function was called before <myCalculatedDistance> contains first 
  //! greater than SetExitIfToleranceExceeded() parameter value
  Standard_EXPORT void Process();

  //! Returns true if the distance has been found for all points
  Standard_Boolean IsDone()
  {
    return myIsDone;
  }

  //! Returns true if computed distance is less than <theToleranceToCheck>
  Standard_EXPORT Standard_Boolean CheckTolerance(Standard_Real theToleranceToCheck);

  //! Returns max distance
  Standard_EXPORT Standard_Real GetMaxDistance();

  //! Increase <theToleranceToUpdate> if max distance is greater than <theToleranceToUpdate>
  Standard_EXPORT void UpdateTolerance(Standard_Real& theToleranceToUpdate);

private:
  //! Adds some margin for distance checking
  Standard_Real correctTolerance(Standard_Real theTolerance);

private:
  Handle(Adaptor3d_Curve) myReferenceCurve;
  Handle(Adaptor3d_CurveOnSurface) myOtherCurve;
  Standard_Boolean mySameParameter;
  Standard_Integer myControlPointsNumber; 
  Standard_Real myToleranceForChecking;
  Standard_Real myCalculatedDistance;
  Standard_Boolean myExitIfToleranceExceeded;
  Standard_Boolean myIsDone;
};

#endif  // _BRepLib_ValidateEdge_HeaderFile