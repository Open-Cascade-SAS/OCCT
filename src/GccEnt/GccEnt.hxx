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

#ifndef _GccEnt_HeaderFile
#define _GccEnt_HeaderFile

#include <GccEnt_Position.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class GccEnt_QualifiedLin;
class gp_Lin2d;
class GccEnt_QualifiedCirc;
class gp_Circ2d;
class GccEnt_QualifiedLin;
class GccEnt_QualifiedCirc;


//! This package provides an implementation of the qualified
//! entities useful to create 2d entities with geometric
//! constraints. The qualifier explains which subfamily of
//! solutions we want to obtain. It uses the following law: the
//! matter/the interior side is at the left of the line, if we go
//! from the beginning to the end.
//! The qualifiers are:
//! Enclosing   : the solution(s) must enclose the argument.
//! Enclosed    : the solution(s) must be enclosed in the
//! argument.
//! Outside     : both the solution(s) and the argument must be
//! outside to each other.
//! Unqualified : the position is undefined, so give all the
//! solutions.
//! The use of a qualifier is always required if such
//! subfamilies exist. For example, it is not used for a point.
//! Note:    the interior of a curve is defined as the left-hand
//! side of the curve in relation to its orientation.
class GccEnt 
{
public:

  DEFINE_STANDARD_ALLOC


  //! Prints the name of Position type as a String on the Stream.
  static Standard_OStream& Print (const GccEnt_Position thePosition, Standard_OStream& theStream)
  {
    return (theStream << PositionToString (thePosition));
  }

  //! Returns the string name for a given position.
  //! @param thePosition position type
  //! @return string identifier from the list UNQUALIFIED ENCLOSING ENCLOSED OUTSIDE NOQUALIFIER
  Standard_EXPORT static Standard_CString PositionToString (GccEnt_Position thePosition);

  //! Returns the position from the given string identifier (using case-insensitive comparison).
  //! @param thePositionString string identifier
  //! @return position or GccEnt_unqualified if string identifier is invalid
  static GccEnt_Position PositionFromString (Standard_CString thePositionString)
  {
    GccEnt_Position aPosition = GccEnt_unqualified;
    PositionFromString (thePositionString, aPosition);
    return aPosition;
  }

  //! Determines the position from the given string identifier (using case-insensitive comparison).
  //! @param thePositionString string identifier
  //! @param thePosition detected shape type
  //! @return TRUE if string identifier is known
  Standard_EXPORT static Standard_Boolean PositionFromString (Standard_CString thePositionString,
                                                              GccEnt_Position& thePosition);
  
  //! Constructs a qualified line,
  //! so that the relative position to the circle or line of the
  //! solution computed by a construction algorithm using the
  //! qualified circle or line is not qualified, i.e. all solutions apply.
  Standard_EXPORT static GccEnt_QualifiedLin Unqualified (const gp_Lin2d& Obj);
  
  //! Constructs a qualified circle
  //! so that the relative position to the circle or line of the
  //! solution computed by a construction algorithm using the
  //! qualified circle or line is not qualified, i.e. all solutions apply.
  Standard_EXPORT static GccEnt_QualifiedCirc Unqualified (const gp_Circ2d& Obj);
  

  //! Constructs such a qualified circle that the solution
  //! computed by a construction algorithm using the qualified
  //! circle encloses the circle.
  Standard_EXPORT static GccEnt_QualifiedCirc Enclosing (const gp_Circ2d& Obj);
  
  //! Constructs a qualified line,
  //! so that the solution computed by a construction
  //! algorithm using the qualified circle or line is enclosed by
  //! the circle or line.
  Standard_EXPORT static GccEnt_QualifiedLin Enclosed (const gp_Lin2d& Obj);
  
  //! Constructs a qualified circle
  //! so that the solution computed by a construction
  //! algorithm using the qualified circle or line is enclosed by
  //! the circle or line.
  Standard_EXPORT static GccEnt_QualifiedCirc Enclosed (const gp_Circ2d& Obj);
  
  //! Constructs a qualified line,
  //! so that the solution computed by a construction
  //! algorithm using the qualified circle or line and the circle
  //! or line are external to one another.
  Standard_EXPORT static GccEnt_QualifiedLin Outside (const gp_Lin2d& Obj);
  
  //! Constructs a qualified circle
  //! so that the solution computed by a construction
  //! algorithm using the qualified circle or line and the circle
  //! or line are external to one another.
  Standard_EXPORT static GccEnt_QualifiedCirc Outside (const gp_Circ2d& Obj);




protected:





private:




friend class GccEnt_QualifiedLin;
friend class GccEnt_QualifiedCirc;

};







#endif // _GccEnt_HeaderFile
