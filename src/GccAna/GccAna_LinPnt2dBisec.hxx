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

#ifndef _GccAna_LinPnt2dBisec_HeaderFile
#define _GccAna_LinPnt2dBisec_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class GccInt_Bisec;
class Standard_ConstructionError;
class StdFail_NotDone;
class gp_Lin2d;
class gp_Pnt2d;


//! Describes functions for building bisecting curves between a 2D line and a point.
//! A bisecting curve between a line and a point is such a
//! curve that each of its points is at the same distance from
//! the circle and the point. It can be a parabola or a line,
//! depending on the relative position of the line and the
//! circle. There is always one unique solution.
//! A LinPnt2dBisec object provides a framework for:
//! - defining the construction of the bisecting curve,
//! - implementing the construction algorithm, and
//! - consulting the result.
class GccAna_LinPnt2dBisec 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a bisecting curve between the line Line1 and the point Point2.
  Standard_EXPORT GccAna_LinPnt2dBisec(const gp_Lin2d& Line1, const gp_Pnt2d& Point2);
  
  //! Returns True if the algorithm succeeded.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the number of solutions.
  //! It raises NotDone if the construction algorithm didn't succeed.
  Standard_EXPORT Handle(GccInt_Bisec) ThisSolution() const;




protected:





private:



  Standard_Boolean WellDone;
  Handle(GccInt_Bisec) bissol;


};







#endif // _GccAna_LinPnt2dBisec_HeaderFile
