// Created on: 1991-04-03
// Created by: Remi GILET
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _GccAna_Lin2dBisec_HeaderFile
#define _GccAna_Lin2dBisec_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColgp_Array1OfLin2d.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
class gp_Lin2d;
class gp_Pnt2d;


//! Describes functions for building bisecting lines between two 2D lines.
//! A bisecting line between two lines is such that each of its
//! points is at the same distance from the two lines.
//! If the two lines are secant, there are two orthogonal
//! bisecting lines which share the angles made by the two
//! straight lines in two equal parts. If D1 and D2 are the
//! unit vectors of the two straight lines, those of the two
//! bisecting lines are collinear with the following vectors:
//! -   D1 + D2 for the "internal" bisecting line,
//! -   D1 - D2 for the "external" bisecting line.
//! If the two lines are parallel, the (unique) bisecting line is
//! the straight line equidistant from the two straight lines. If
//! the two straight lines are coincident, the algorithm
//! returns the first straight line as the solution.
//! A Lin2dTanObl object provides a framework for:
//! -   defining the construction of the bisecting lines,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class GccAna_Lin2dBisec 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs bisecting lines between the two lines Lin1 and Lin2.
  Standard_EXPORT GccAna_Lin2dBisec(const gp_Lin2d& Lin1, const gp_Lin2d& Lin2);
  
  //! Returns True when the algorithm succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of solutions and raise NotDone if
  //! the constructor wasn't called before.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns the solution number Index .
  //! The first solution is the inside one and the second is the
  //! outside one.
  //! For the first solution the direction is D1+D2 (D1 is
  //! the direction of the first argument and D2 the
  //! direction of the second argument).
  //! For the second solution the direction is D1-D2.
  //! Raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT gp_Lin2d ThisSolution (const Standard_Integer Index) const;
  
  //! Returns information about the intersection point between
  //! the result number Index and the first argument.
  //! Raises NotDone if the construction algorithm  didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void Intersection1 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;
  
  //! Returns information about the intersection point between
  //! the result number Index and the second argument.
  //! Raises NotDone if the construction algorithm
  //! didn't succeed.
  //! It raises OutOfRange if Index is greater than the
  //! number of solutions.
  Standard_EXPORT void Intersection2 (const Standard_Integer Index, Standard_Real& ParSol, Standard_Real& ParArg, gp_Pnt2d& PntSol) const;




protected:





private:



  Standard_Boolean WellDone;
  Standard_Integer NbrSol;
  TColgp_Array1OfLin2d linsol;
  TColgp_Array1OfPnt2d pntint1sol;
  TColgp_Array1OfPnt2d pntint2sol;
  TColStd_Array1OfReal par1sol;
  TColStd_Array1OfReal par2sol;
  TColStd_Array1OfReal pararg1;
  TColStd_Array1OfReal pararg2;


};







#endif // _GccAna_Lin2dBisec_HeaderFile
