// Created by: Nikolai BUKHALOV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _GeomLib_CheckCurveOnSurface_HeaderFile
#define _GeomLib_CheckCurveOnSurface_HeaderFile

#include <Geom_Curve.hxx>
#include <Precision.hxx>
#include <Standard.hxx>

class Geom_Surface;
class Geom2d_Curve;

//! Computes the max distance between 3D-curve and 2D-curve
//! in some surface.
class GeomLib_CheckCurveOnSurface 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Default contructor
  Standard_EXPORT GeomLib_CheckCurveOnSurface(void);
  
  //! Contructor
  Standard_EXPORT
    GeomLib_CheckCurveOnSurface(const Handle(Geom_Curve)& theCurve, 
                                const Handle(Geom_Surface)& theSurface, 
                                const Standard_Real theFirst, 
                                const Standard_Real theLast,
                                const Standard_Real theTolRange = 
                                                      Precision::PConfusion());
  
  //! Sets the data for the algorithm
  Standard_EXPORT void Init (const Handle(Geom_Curve)& theCurve, 
                             const Handle(Geom_Surface)& theSurface, 
                             const Standard_Real theFirst, 
                             const Standard_Real theLast,
                             const Standard_Real theTolRange = Precision::PConfusion());

  //! Initializes all members by dafault values
  Standard_EXPORT void Init();

  //! Computes the max distance for the 3d curve <myCurve>
  //! and 2d curve <thePCurve>
  //! If isTheMultyTheadDisabled == TRUE then computation will be made
  //! without any parallelization.
  Standard_EXPORT void Perform(const Handle(Geom2d_Curve)& thePCurve, 
                               const Standard_Boolean isTheMultyTheradDisabled = Standard_False);

  //! Returns my3DCurve
  const Handle(Geom_Curve)& Curve() const
  {
    return myCurve;
  }  

  //! Returns mySurface
  const Handle(Geom_Surface)& Surface() const
  {
    return mySurface;
  }
  
  //! Returns first and last parameter of the curves
  //! (2D- and 3D-curves are considered to have same range)
  void Range (Standard_Real& theFirst, Standard_Real& theLast)
  {
    theFirst = myFirst;
    theLast  = myLast;
  }

  //! Returns true if the max distance has been found
  Standard_Boolean IsDone() const
  {
    return (myErrorStatus == 0);
  }
  
  //! Returns error status
  //! The possible values are:
  //! 0 - OK;
  //! 1 - null curve or surface or 2d curve;
  //! 2 - invalid parametric range;
  //! 3 - error in calculations.
  Standard_Integer ErrorStatus() const
  {
    return myErrorStatus;
  }
  
  //! Returns max distance
  Standard_Real MaxDistance() const
  {
    return myMaxDistance;
  }
  
  //! Returns parameter in which the distance is maximal
  Standard_Real MaxParameter() const
  {
    return myMaxParameter;
  }

private:

  Handle(Geom_Curve) myCurve;
  Handle(Geom_Surface) mySurface;
  Standard_Real myFirst;
  Standard_Real myLast;
  Standard_Integer myErrorStatus;
  Standard_Real myMaxDistance;
  Standard_Real myMaxParameter;
  Standard_Real myTolRange;
};

#endif // _BRepLib_CheckCurveOnSurface_HeaderFile
