// Created on: 1991-10-04
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

#ifndef _GccInt_Bisec_HeaderFile
#define _GccInt_Bisec_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <GccInt_IType.hxx>
class gp_Pnt2d;
class gp_Lin2d;
class gp_Circ2d;
class gp_Hypr2d;
class gp_Parab2d;
class gp_Elips2d;


class GccInt_Bisec;
DEFINE_STANDARD_HANDLE(GccInt_Bisec, Standard_Transient)

//! The deferred class GccInt_Bisec is the root class for
//! elementary bisecting loci between two simple geometric
//! objects (i.e. circles, lines or points).
//! Bisecting loci between two geometric objects are such
//! that each of their points is at the same distance from the
//! two geometric objects. It is typically a curve, such as a
//! line, circle or conic.
//! Generally there is more than one elementary object
//! which is the solution to a bisecting loci problem: each
//! solution is described with one elementary bisecting
//! locus. For example, the bisectors of two secant straight
//! lines are two perpendicular straight lines.
//! The GccInt package provides concrete implementations
//! of the following elementary derived bisecting loci:
//! -   lines, circles, ellipses, hyperbolas and parabolas, and
//! -   points (not used in this context).
//! The GccAna package provides numerous algorithms for
//! computing the bisecting loci between circles, lines or
//! points, whose solutions are these types of elementary bisecting locus.
class GccInt_Bisec : public Standard_Transient
{

public:

  
  //! Returns the type of bisecting object (line, circle,
  //! parabola, hyperbola, ellipse, point).
  Standard_EXPORT virtual GccInt_IType ArcType() const = 0;
  
  //! Returns the bisecting line when ArcType returns Pnt.
  //! An exception DomainError is raised if ArcType is not a Pnt.
  Standard_EXPORT virtual gp_Pnt2d Point() const;
  
  //! Returns the bisecting line when ArcType returns Lin.
  //! An exception DomainError is raised if ArcType is not a Lin.
  Standard_EXPORT virtual gp_Lin2d Line() const;
  
  //! Returns the bisecting line when ArcType returns Cir.
  //! An exception DomainError is raised if ArcType is not a Cir.
  Standard_EXPORT virtual gp_Circ2d Circle() const;
  
  //! Returns the bisecting line when ArcType returns Hpr.
  //! An exception DomainError is raised if ArcType is not a Hpr.
  Standard_EXPORT virtual gp_Hypr2d Hyperbola() const;
  
  //! Returns the bisecting line when ArcType returns Par.
  //! An exception DomainError is raised if ArcType is not a Par.
  Standard_EXPORT virtual gp_Parab2d Parabola() const;
  
  //! Returns the bisecting line when ArcType returns Ell.
  //! An exception DomainError is raised if ArcType is not an Ell.
  Standard_EXPORT virtual gp_Elips2d Ellipse() const;




  DEFINE_STANDARD_RTTIEXT(GccInt_Bisec,Standard_Transient)

protected:




private:




};







#endif // _GccInt_Bisec_HeaderFile
