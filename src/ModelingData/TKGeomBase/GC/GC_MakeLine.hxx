// Created on: 1992-09-28
// Created by: Remi GILET
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

#ifndef _GC_MakeLine_HeaderFile
#define _GC_MakeLine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GC_Root.hxx>
#include <Geom_Line.hxx>

class gp_Ax1;
class gp_Lin;
class gp_Pnt;
class gp_Dir;

//! This class implements the following algorithms used
//! to create a Line from Geom.
//! * Create a Line parallel to another and passing
//! through a point.
//! * Create a Line passing through 2 points.
//! A MakeLine object provides a framework for:
//! -   defining the construction of the line,
//! -   implementing the construction algorithm, and
//! -   consulting the results. In particular, the Value
//! function returns the constructed line.
class GC_MakeLine : public GC_Root
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a line located in 3D space with the axis placement A1.
  //! The Location of A1 is the origin of the line.
  Standard_EXPORT GC_MakeLine(const gp_Ax1& A1);

  //! Creates a line from a non persistent line from package gp.
  Standard_EXPORT GC_MakeLine(const gp_Lin& L);

  //! P is the origin and V is the direction of the line.
  Standard_EXPORT GC_MakeLine(const gp_Pnt& P, const gp_Dir& V);

  //! Make a Line from Geom <TheLin> parallel to another
  //! Lin <Lin> and passing through a Pnt <Point>.
  Standard_EXPORT GC_MakeLine(const gp_Lin& Lin, const gp_Pnt& Point);

  //! Make a Line from Geom <TheLin> passing through 2
  //! Pnt <P1>,<P2>.
  //! It returns false if <p1> and <P2> are confused.
  //! Warning
  //! If the points P1 and P2 are coincident (that is, when
  //! IsDone returns false), the Status function returns gce_ConfusedPoints.
  Standard_EXPORT GC_MakeLine(const gp_Pnt& P1, const gp_Pnt& P2);

  //! Returns the constructed line.
  //! Exceptions StdFail_NotDone if no line is constructed.
  Standard_EXPORT const Handle(Geom_Line)& Value() const;

  operator const Handle(Geom_Line) & () const { return Value(); }

private:
  Handle(Geom_Line) TheLine;
};

#endif // _GC_MakeLine_HeaderFile
