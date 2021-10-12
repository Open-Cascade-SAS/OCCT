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

#ifndef _GccAna_CircLin2dBisec_HeaderFile
#define _GccAna_CircLin2dBisec_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
class GccInt_Bisec;


//! Describes functions for building bisecting curves between a 2D line and a 2D circle.
//! A bisecting curve between a circle and a line is a curve
//! such that each of its points is at the same distance from
//! the circle and the line. It can be a parabola or a line,
//! depending of the relative position of the line and the
//! circle. The algorithm computes all the elementary curves which are solutions.
//! A CircLin2dBisec object provides a framework for:
//! -   defining the construction of the bisecting curves,
//! -   implementing the construction algorithm, and
//! -   consulting the result.
class GccAna_CircLin2dBisec 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs bisecting curves between the circle Circle and the line Line.
  Standard_EXPORT GccAna_CircLin2dBisec(const gp_Circ2d& Circle, const gp_Lin2d& Line);
  
  //! Returns true (this construction algorithm never fails).
  Standard_EXPORT Standard_Boolean IsDone() const;
  

  //! Returns the number of curves, representing solutions computed by this algorithm.
  Standard_EXPORT Standard_Integer NbSolutions() const;
  
  //! Returns the solution number Index and raises OutOfRange
  //! exception if Index is greater than the number of solutions
  //! Exceptions
  //! Standard_OutOfRange if Index is less than zero or
  //! greater than the number of solutions computed by this algorithm.
  Standard_EXPORT Handle(GccInt_Bisec) ThisSolution (const Standard_Integer Index) const;




protected:





private:



  Standard_Boolean WellDone;
  Standard_Integer NbrSol;
  gp_Circ2d circle;
  gp_Lin2d line;


};







#endif // _GccAna_CircLin2dBisec_HeaderFile
