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

#ifndef _GccAna_Circ2dBisec_HeaderFile
#define _GccAna_Circ2dBisec_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <gp_Circ2d.hxx>
class GccInt_Bisec;

//! This class describes functions for building bisecting curves between two 2D circles.
//! A bisecting curve between two circles is a curve such
//! that each of its points is at the same distance from the
//! two circles. It can be an ellipse, hyperbola, circle or line,
//! depending on the relative position of the two circles.
//! The algorithm computes all the elementary curves which
//! are solutions. There is no solution if the two circles are coincident.
//! A Circ2dBisec object provides a framework for:
//! -   defining the construction of the bisecting curves,
//! -   implementing the construction algorithm, and consulting the result.
class GccAna_Circ2dBisec
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs bisecting curves between the two circles Circ1 and Circ2.
  Standard_EXPORT GccAna_Circ2dBisec(const gp_Circ2d& Circ1, const gp_Circ2d& Circ2);

  //! This method returns True if the construction algorithm succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;

  //! This method returns the number of solutions.
  //! Raises NotDone if the construction algorithm  didn't succeed.
  Standard_EXPORT Standard_Integer NbSolutions() const;

  //! Returns the solution number Index
  //! Raises OutOfRange exception if Index is greater than
  //! the number of solutions.
  //! It raises NotDone if the construction algorithm
  //! didn't succeed.
  Standard_EXPORT Handle(GccInt_Bisec) ThisSolution(const Standard_Integer Index) const;

protected:
private:
  Standard_Boolean WellDone;
  Standard_Integer NbrSol;
  Standard_Integer intersection;
  Standard_Boolean sameradius;
  gp_Circ2d        circle1;
  gp_Circ2d        circle2;
};

#endif // _GccAna_Circ2dBisec_HeaderFile
