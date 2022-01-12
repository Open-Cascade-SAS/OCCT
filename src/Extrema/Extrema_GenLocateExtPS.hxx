// Created on: 1995-07-18
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Extrema_GenLocateExtPS_HeaderFile
#define _Extrema_GenLocateExtPS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Boolean.hxx>
#include <Extrema_POnSurf.hxx>
class gp_Pnt;
class Adaptor3d_Surface;


//! With a close point, it calculates the distance
//! between a point and a surface.
//! Criteria type is defined in "Perform" method.
class Extrema_GenLocateExtPS 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor.
  Standard_EXPORT Extrema_GenLocateExtPS(const Adaptor3d_Surface& theS,
                                         const Standard_Real theTolU = Precision::PConfusion(),
                                         const Standard_Real theTolV = Precision::PConfusion());
  
  //! Calculates the extrema between the point and the surface using a close point.
  //! The close point is defined by the parameter values theU0 and theV0.
  //! Type of the algorithm depends on the isDistanceCriteria flag.
  //! If flag value is false - normal projection criteria will be used.
  //! If flag value is true - distance criteria will be used.
  Standard_EXPORT void Perform(const gp_Pnt& theP,
                               const Standard_Real theU0,
                               const Standard_Real theV0,
                               const Standard_Boolean isDistanceCriteria = Standard_False);
  
  //! Returns True if the distance is found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the value of the extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance() const;
  
  //! Returns the point of the extremum distance.
  Standard_EXPORT const Extrema_POnSurf& Point() const;

  //! Returns True if UV point theU0, theV0 is point of local minimum of square distance between
  //! point theP and points theS(U, V), U, V are in small area around theU0, theV0
  Standard_EXPORT static Standard_Boolean IsMinDist(const gp_Pnt& theP, const Adaptor3d_Surface& theS,
    const Standard_Real theU0, const Standard_Real theV0);

private:

  const Extrema_GenLocateExtPS& operator=(const Extrema_GenLocateExtPS&);
  Extrema_GenLocateExtPS(const Extrema_GenLocateExtPS&);

  // Input.
  const Adaptor3d_Surface& mySurf;
  Standard_Real myTolU, myTolV;

  // State.
  Standard_Boolean myDone;

  // Result.
  Standard_Real mySqDist;
  Extrema_POnSurf myPoint;


};







#endif // _Extrema_GenLocateExtPS_HeaderFile
