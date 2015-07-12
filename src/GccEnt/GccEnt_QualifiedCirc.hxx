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

#ifndef _GccEnt_QualifiedCirc_HeaderFile
#define _GccEnt_QualifiedCirc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Circ2d.hxx>
#include <GccEnt_Position.hxx>
#include <Standard_Boolean.hxx>
class gp_Circ2d;


//! Creates a qualified 2d Circle.
//! A qualified 2D circle is a circle (gp_Circ2d circle) with a
//! qualifier which specifies whether the solution of a
//! construction algorithm using the qualified circle (as an argument):
//! -   encloses the circle, or
//! -   is enclosed by the circle, or
//! -   is built so that both the circle and it are external to   one another, or
//! -   is undefined (all solutions apply).
class GccEnt_QualifiedCirc 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a qualified circle by assigning the qualifier
  //! Qualifier to the circle Qualified. Qualifier may be:
  //! -   GccEnt_enclosing if the solution computed by a
  //! construction algorithm using the qualified circle
  //! encloses the circle, or
  //! -   GccEnt_enclosed if the solution is enclosed by the circle, or
  //! -   GccEnt_outside if both the solution and the circle
  //! are external to one another, or
  //! -   GccEnt_unqualified if all solutions apply.
  Standard_EXPORT GccEnt_QualifiedCirc(const gp_Circ2d& Qualified, const GccEnt_Position Qualifier);
  
  //! Returns a 2D circle to which the qualifier is assigned.
  Standard_EXPORT gp_Circ2d Qualified() const;
  
  //! Returns
  //! -   the qualifier of this qualified circle, if it is enclosing,
  //! enclosed or outside, or
  //! -   GccEnt_noqualifier if it is unqualified.
  Standard_EXPORT GccEnt_Position Qualifier() const;
  
  //! Returns true if the Circ2d is Unqualified and false in
  //! the other cases.
  Standard_EXPORT Standard_Boolean IsUnqualified() const;
  
  //! Returns true if the solution computed by a construction
  //! algorithm using this qualified circle encloses the circle.
  Standard_EXPORT Standard_Boolean IsEnclosing() const;
  
  //! Returns true if the solution computed by a construction
  //! algorithm using this qualified circle is enclosed by the circle.
  Standard_EXPORT Standard_Boolean IsEnclosed() const;
  
  //! Returns true if both the solution computed by a
  //! construction algorithm using this qualified circle and the
  //! circle are external to one another.
  Standard_EXPORT Standard_Boolean IsOutside() const;




protected:





private:



  gp_Circ2d TheQualified;
  GccEnt_Position TheQualifier;


};







#endif // _GccEnt_QualifiedCirc_HeaderFile
