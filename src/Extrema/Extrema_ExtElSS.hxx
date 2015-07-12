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

#ifndef _Extrema_ExtElSS_HeaderFile
#define _Extrema_ExtElSS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Extrema_HArray1OfPOnSurf.hxx>
#include <Standard_Real.hxx>
class StdFail_InfiniteSolutions;
class StdFail_NotDone;
class Standard_OutOfRange;
class gp_Pln;
class gp_Sphere;
class gp_Cylinder;
class gp_Cone;
class gp_Torus;
class Extrema_POnSurf;


//! It calculates all the distances between 2 elementary
//! surfaces.
//! These distances can be maximum or minimum.
class Extrema_ExtElSS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_ExtElSS();
  
  //! Calculates the distances between 2 planes.
  //! These planes can be parallel.
  Standard_EXPORT Extrema_ExtElSS(const gp_Pln& S1, const gp_Pln& S2);
  
  Standard_EXPORT void Perform (const gp_Pln& S1, const gp_Pln& S2);
  
  //! Calculates the distances between a plane
  //! and a sphere.
  Standard_EXPORT Extrema_ExtElSS(const gp_Pln& S1, const gp_Sphere& S2);
  
  Standard_EXPORT void Perform (const gp_Pln& S1, const gp_Sphere& S2);
  
  //! Calculates the distances between 2 spheres.
  //! These spheres can be parallel.
  Standard_EXPORT Extrema_ExtElSS(const gp_Sphere& S1, const gp_Sphere& S2);
  
  Standard_EXPORT void Perform (const gp_Sphere& S1, const gp_Sphere& S2);
  
  //! Calculates the distances between a sphere
  //! and a cylinder.
  Standard_EXPORT Extrema_ExtElSS(const gp_Sphere& S1, const gp_Cylinder& S2);
  
  Standard_EXPORT void Perform (const gp_Sphere& S1, const gp_Cylinder& S2);
  
  //! Calculates the distances between a sphere
  //! and a cone.
  Standard_EXPORT Extrema_ExtElSS(const gp_Sphere& S1, const gp_Cone& S2);
  
  Standard_EXPORT void Perform (const gp_Sphere& S1, const gp_Cone& S2);
  
  //! Calculates the distances between a sphere
  //! and a torus.
  Standard_EXPORT Extrema_ExtElSS(const gp_Sphere& S1, const gp_Torus& S2);
  
  Standard_EXPORT void Perform (const gp_Sphere& S1, const gp_Torus& S2);
  
  //! Returns True if the distances are found.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns True if the two surfaces are parallel.
  Standard_EXPORT Standard_Boolean IsParallel() const;
  
  //! Returns the number of extremum distances.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Returns the value of the Nth extremum square distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N = 1) const;
  
  //! Returns the points for the Nth resulting distance.
  //! P1 is on the first surface, P2 on the second one.
  Standard_EXPORT void Points (const Standard_Integer N, Extrema_POnSurf& P1, Extrema_POnSurf& P2) const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Boolean myIsPar;
  Standard_Integer myNbExt;
  Handle(TColStd_HArray1OfReal) mySqDist;
  Handle(Extrema_HArray1OfPOnSurf) myPOnS1;
  Handle(Extrema_HArray1OfPOnSurf) myPOnS2;


};







#endif // _Extrema_ExtElSS_HeaderFile
