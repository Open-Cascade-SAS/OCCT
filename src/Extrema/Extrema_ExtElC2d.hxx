// Created on: 1994-01-04
// Created by: Christophe MARION
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Extrema_ExtElC2d_HeaderFile
#define _Extrema_ExtElC2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <Extrema_POnCurv2d.hxx>

class gp_Lin2d;
class gp_Circ2d;
class gp_Elips2d;
class gp_Hypr2d;
class gp_Parab2d;


//! It calculates all the distance between two elementary
//! curves.
//! These distances can be maximum or minimum.
class Extrema_ExtElC2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_ExtElC2d();
  
  //! Calculates the distance between two lines.
  //! AngTol is used to test if the lines are parallel:
  //! Angle(C1,C2) < AngTol.
  Standard_EXPORT Extrema_ExtElC2d(const gp_Lin2d& C1, const gp_Lin2d& C2, const Standard_Real AngTol);
  
  //! Calculates the distance between a line and a
  //! circle.
  Standard_EXPORT Extrema_ExtElC2d(const gp_Lin2d& C1, const gp_Circ2d& C2, const Standard_Real Tol);
  
  //! Calculates the distance between a line and an
  //! ellipse.
  Standard_EXPORT Extrema_ExtElC2d(const gp_Lin2d& C1, const gp_Elips2d& C2);
  
  //! Calculates the distance between a line and a
  //! hyperbola.
  Standard_EXPORT Extrema_ExtElC2d(const gp_Lin2d& C1, const gp_Hypr2d& C2);
  
  //! Calculates the distance between a line and a
  //! parabola.
  Standard_EXPORT Extrema_ExtElC2d(const gp_Lin2d& C1, const gp_Parab2d& C2);
  
  //! Calculates the distance between two circles.
  //! The circles can be parallel or identical.
  Standard_EXPORT Extrema_ExtElC2d(const gp_Circ2d& C1, const gp_Circ2d& C2);
  
  //! Calculates the distance between a circle and an
  //! ellipse.
  Standard_EXPORT Extrema_ExtElC2d(const gp_Circ2d& C1, const gp_Elips2d& C2);
  
  //! Calculates the distance between a circle and a
  //! hyperbola.
  Standard_EXPORT Extrema_ExtElC2d(const gp_Circ2d& C1, const gp_Hypr2d& C2);
  
  //! Calculates the distance between a circle and a
  //! parabola.
  Standard_EXPORT Extrema_ExtElC2d(const gp_Circ2d& C1, const gp_Parab2d& C2);
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns True if the two curves are parallel.
  Standard_EXPORT Standard_Boolean IsParallel() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N = 1) const;
  
  //! Returns the points of the Nth extremum distance.
  //! P1 is on the first curve, P2 on the second one.
  Standard_EXPORT void Points (const Standard_Integer N, Extrema_POnCurv2d& P1, Extrema_POnCurv2d& P2) const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Boolean myIsPar;
  Standard_Integer myNbExt;
  Standard_Real mySqDist[8];
  Extrema_POnCurv2d myPoint[8][2];


};







#endif // _Extrema_ExtElC2d_HeaderFile
