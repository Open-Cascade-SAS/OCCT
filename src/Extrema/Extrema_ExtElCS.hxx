// Created on: 1992-07-22
// Created by: Laurent PAINNOT
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Extrema_ExtElCS_HeaderFile
#define _Extrema_ExtElCS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Extrema_HArray1OfPOnCurv.hxx>
#include <Extrema_HArray1OfPOnSurf.hxx>
#include <Standard_Real.hxx>
class StdFail_InfiniteSolutions;
class StdFail_NotDone;
class Standard_OutOfRange;
class gp_Lin;
class gp_Pln;
class gp_Cylinder;
class gp_Cone;
class gp_Sphere;
class gp_Torus;
class gp_Circ;
class gp_Hypr;
class Extrema_POnCurv;
class Extrema_POnSurf;


//! It calculates all the distances between a curve and
//! a surface.
//! These distances can be maximum or minimum.
class Extrema_ExtElCS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_ExtElCS();
  
  //! Calculates the distances between a line and a
  //! plane. The line can be on the plane or on a parallel
  //! plane.
  Standard_EXPORT Extrema_ExtElCS(const gp_Lin& C, const gp_Pln& S);
  
  Standard_EXPORT void Perform (const gp_Lin& C, const gp_Pln& S);
  
  //! Calculates the distances between a line and a
  //! cylinder.
  Standard_EXPORT Extrema_ExtElCS(const gp_Lin& C, const gp_Cylinder& S);
  
  Standard_EXPORT void Perform (const gp_Lin& C, const gp_Cylinder& S);
  
  //! Calculates the distances between a line and a cone.
  Standard_EXPORT Extrema_ExtElCS(const gp_Lin& C, const gp_Cone& S);
  
  Standard_EXPORT void Perform (const gp_Lin& C, const gp_Cone& S);
  
  //! Calculates the distances between a line and a
  //! sphere.
  Standard_EXPORT Extrema_ExtElCS(const gp_Lin& C, const gp_Sphere& S);
  
  Standard_EXPORT void Perform (const gp_Lin& C, const gp_Sphere& S);
  
  //! Calculates the distances between a line and a
  //! torus.
  Standard_EXPORT Extrema_ExtElCS(const gp_Lin& C, const gp_Torus& S);
  
  Standard_EXPORT void Perform (const gp_Lin& C, const gp_Torus& S);
  
  //! Calculates the distances between a circle and a
  //! plane.
  Standard_EXPORT Extrema_ExtElCS(const gp_Circ& C, const gp_Pln& S);
  
  Standard_EXPORT void Perform (const gp_Circ& C, const gp_Pln& S);
  
  //! Calculates the distances between a circle and a
  //! cylinder.
  Standard_EXPORT Extrema_ExtElCS(const gp_Circ& C, const gp_Cylinder& S);
  
  Standard_EXPORT void Perform (const gp_Circ& C, const gp_Cylinder& S);
  
  //! Calculates the distances between a circle and a
  //! cone.
  Standard_EXPORT Extrema_ExtElCS(const gp_Circ& C, const gp_Cone& S);
  
  Standard_EXPORT void Perform (const gp_Circ& C, const gp_Cone& S);
  
  //! Calculates the distances between a circle and a
  //! sphere.
  Standard_EXPORT Extrema_ExtElCS(const gp_Circ& C, const gp_Sphere& S);
  
  Standard_EXPORT void Perform (const gp_Circ& C, const gp_Sphere& S);
  
  //! Calculates the distances between a circle and a
  //! torus.
  Standard_EXPORT Extrema_ExtElCS(const gp_Circ& C, const gp_Torus& S);
  
  Standard_EXPORT void Perform (const gp_Circ& C, const gp_Torus& S);
  
  //! Calculates the distances between a hyperbola and a
  //! plane.
  Standard_EXPORT Extrema_ExtElCS(const gp_Hypr& C, const gp_Pln& S);
  
  Standard_EXPORT void Perform (const gp_Hypr& C, const gp_Pln& S);
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns True if the curve is on a parallel surface.
  Standard_EXPORT Standard_Boolean IsParallel() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N = 1) const;
  
  //! Returns the points of the Nth extremum distance.
  //! P1 is on the curve, P2 on the surface.
  Standard_EXPORT void Points (const Standard_Integer N, Extrema_POnCurv& P1, Extrema_POnSurf& P2) const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Integer myNbExt;
  Standard_Boolean myIsPar;
  Handle(TColStd_HArray1OfReal) mySqDist;
  Handle(Extrema_HArray1OfPOnCurv) myPoint1;
  Handle(Extrema_HArray1OfPOnSurf) myPoint2;


};







#endif // _Extrema_ExtElCS_HeaderFile
