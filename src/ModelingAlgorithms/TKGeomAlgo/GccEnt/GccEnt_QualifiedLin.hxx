// Created on: 1991-04-15
// Created by: Philippe DAUTRY
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

#ifndef _GccEnt_QualifiedLin_HeaderFile
#define _GccEnt_QualifiedLin_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <GccEnt_Position.hxx>
#include <gp_Lin2d.hxx>
#include <Standard_Boolean.hxx>

//! Describes a qualified 2D line.
//! A qualified 2D line is a line (gp_Lin2d line) with a
//! qualifier which specifies whether the solution of a
//! construction algorithm using the qualified line (as an argument):
//! -   is 'enclosed' by the line, or
//! -   is built so that both the line and it are external to one another, or
//! -   is undefined (all solutions apply).
//! Note: the interior of a line is defined as the left-hand
//! side of the line in relation to its orientation (i.e. when
//! moving from the start to the end of the curve).
class GccEnt_QualifiedLin
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs a qualified line by assigning the qualifier
  //! Qualifier to the line Qualified.
  //! Qualifier may be:
  //! -   GccEnt_enclosed if the solution is enclosed by the line, or
  //! -   GccEnt_outside if both the solution and the line are external to one another, or
  //! -   GccEnt_unqualified if all solutions apply.
  //! Note : the interior of a line is defined as the left-hand
  //! side of the line in relation to its orientation.
  Standard_EXPORT GccEnt_QualifiedLin(const gp_Lin2d& Qualified, const GccEnt_Position Qualifier);

  //! Returns a 2D line to which the qualifier is assigned.
  Standard_EXPORT gp_Lin2d Qualified() const;

  //! Returns the qualifier of this qualified line, if it is "enclosed" or
  //! "outside", or
  //! -   GccEnt_noqualifier if it is unqualified.
  Standard_EXPORT GccEnt_Position Qualifier() const;

  //! Returns true if the solution is unqualified and false in
  //! the other cases.
  Standard_EXPORT Standard_Boolean IsUnqualified() const;

  //! Returns true if the solution is Enclosed in the Lin2d and false in
  //! the other cases.
  Standard_EXPORT Standard_Boolean IsEnclosed() const;

  //! Returns true if the solution is Outside the Lin2d and false in
  //! the other cases.
  Standard_EXPORT Standard_Boolean IsOutside() const;

protected:
private:
  GccEnt_Position TheQualifier;
  gp_Lin2d        TheQualified;
};

#endif // _GccEnt_QualifiedLin_HeaderFile
