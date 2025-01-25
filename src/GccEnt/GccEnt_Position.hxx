// Created on: 1991-03-05
// Created by: Remy GILET
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

#ifndef _GccEnt_Position_HeaderFile
#define _GccEnt_Position_HeaderFile

//! Qualifies the position of a solution of a construction
//! algorithm with respect to one of its arguments. This is one of the following:
//! -   GccEnt_unqualified: the position of the solution
//! is undefined with respect to the argument,
//! -   GccEnt_enclosing: the solution encompasses the argument,
//! -   GccEnt_enclosed: the solution is encompassed by the argument,
//! -   GccEnt_outside: the solution and the argument
//! are external to one another,
//! -   GccEnt_noqualifier: the value returned during a
//! consultation of the qualifier when the argument is
//! defined as GccEnt_unqualified.
//! Note: the interior of a line or any open curve is
//! defined as the left-hand side of the line or curve in
//! relation to its orientation.
enum GccEnt_Position
{
  GccEnt_unqualified,
  GccEnt_enclosing,
  GccEnt_enclosed,
  GccEnt_outside,
  GccEnt_noqualifier
};

#endif // _GccEnt_Position_HeaderFile
