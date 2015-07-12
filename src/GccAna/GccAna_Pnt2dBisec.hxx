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

#ifndef _GccAna_Pnt2dBisec_HeaderFile
#define _GccAna_Pnt2dBisec_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <gp_Lin2d.hxx>
class StdFail_NotDone;
class gp_Pnt2d;
class gp_Lin2d;


//! This class implements the algorithms used to
//! create the bisecting line between two 2d points
//! Describes functions for building a bisecting line between two 2D points.
//! The bisecting line between two points is the bisector of
//! the segment which joins the two points, if these are not coincident.
//! The algorithm does not find a solution if the two points are coincident.
//! A Pnt2dBisec object provides a framework for:
//! -   defining the construction of the bisecting line,
//! -   implementing the construction algorithm, and consulting the result.
class GccAna_Pnt2dBisec 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a bisecting line between the points Point1 and Point2.
  Standard_EXPORT GccAna_Pnt2dBisec(const gp_Pnt2d& Point1, const gp_Pnt2d& Point2);
  
  //! Returns true (this construction algorithm never fails).
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns true if this algorithm has a solution, i.e. if the
  //! two points are not coincident.
  Standard_EXPORT Standard_Boolean HasSolution() const;
  
  //! Returns a line, representing the solution computed by this algorithm.
  Standard_EXPORT gp_Lin2d ThisSolution() const;




protected:





private:



  Standard_Boolean WellDone;
  Standard_Boolean HasSol;
  gp_Lin2d linsol;


};







#endif // _GccAna_Pnt2dBisec_HeaderFile
