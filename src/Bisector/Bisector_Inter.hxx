// Created on: 1994-06-24
// Created by: Yves FRICAUD
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

#ifndef _Bisector_Inter_HeaderFile
#define _Bisector_Inter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntRes2d_Intersection.hxx>
class Bisector_Bisec;
class IntRes2d_Domain;
class Geom2d_Curve;
class Bisector_BisecCC;
class Geom2d_Line;


//! Intersection between two <Bisec> from Bisector.
class Bisector_Inter  : public IntRes2d_Intersection
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Bisector_Inter();
  
  //! Intersection between 2 curves.
  //! C1 separates the element A and B.
  //! C2 separates the elements C et D.
  //! If B an C have the same geometry. <ComunElement>
  //! Has to be True.
  //! It Permits an optimiztion of the computation.
  Standard_EXPORT Bisector_Inter(const Bisector_Bisec& C1, const IntRes2d_Domain& D1, const Bisector_Bisec& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean ComunElement);
  
  //! Intersection between 2 curves.
  //! C1 separates the element A and B.
  //! C2 separates the elements C et D.
  //! If B an C have the same geometry. <ComunElement>
  //! Has to be True.
  //! It Permits an optimiztion of the computation.
  Standard_EXPORT void Perform (const Bisector_Bisec& C1, const IntRes2d_Domain& D1, const Bisector_Bisec& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean ComunElement);




protected:





private:

  
  //! Intersection between 2 curves.
  Standard_EXPORT void SinglePerform (const Handle(Geom2d_Curve)& C1, const IntRes2d_Domain& D1, const Handle(Geom2d_Curve)& C2, const IntRes2d_Domain& D2, const Standard_Real TolConf, const Standard_Real Tol, const Standard_Boolean ComunElement);
  
  Standard_EXPORT void NeighbourPerform (const Handle(Bisector_BisecCC)& C1, const IntRes2d_Domain& D1, const Handle(Bisector_BisecCC)& C2, const IntRes2d_Domain& D2, const Standard_Real Tol);
  
  Standard_EXPORT void TestBound (const Handle(Geom2d_Line)& C1, const IntRes2d_Domain& D1, const Handle(Geom2d_Curve)& C2, const IntRes2d_Domain& D2, const Standard_Real Tol, const Standard_Boolean Reverse);




};







#endif // _Bisector_Inter_HeaderFile
