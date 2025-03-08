// Created on: 1991-02-20
// Created by: Jacques GOUSSARD
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

#ifndef _IntAna2d_AnaIntersection_HeaderFile
#define _IntAna2d_AnaIntersection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <IntAna2d_IntPoint.hxx>
class gp_Lin2d;
class gp_Circ2d;
class IntAna2d_Conic;
class gp_Elips2d;
class gp_Parab2d;
class gp_Hypr2d;

//! Implementation of the analytical intersection between:
//! - two Lin2d,
//! - two Circ2d,
//! - a Lin2d and a Circ2d,
//! - an element of gp (Lin2d, Circ2d, Elips2d, Parab2d, Hypr2d)
//! and another conic.
//! No tolerance is given for all the intersections: the tolerance
//! will be the "precision machine".
class IntAna2d_AnaIntersection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor. IsDone returns False.
  Standard_EXPORT IntAna2d_AnaIntersection();

  //! Intersection between two lines.
  Standard_EXPORT IntAna2d_AnaIntersection(const gp_Lin2d& L1, const gp_Lin2d& L2);

  //! Intersection between two circles.
  Standard_EXPORT IntAna2d_AnaIntersection(const gp_Circ2d& C1, const gp_Circ2d& C2);

  //! Intersection between a line and a circle.
  Standard_EXPORT IntAna2d_AnaIntersection(const gp_Lin2d& L, const gp_Circ2d& C);

  //! Intersection between a line and a conic.
  Standard_EXPORT IntAna2d_AnaIntersection(const gp_Lin2d& L, const IntAna2d_Conic& C);

  //! Intersection between a circle and another conic.
  Standard_EXPORT IntAna2d_AnaIntersection(const gp_Circ2d& C, const IntAna2d_Conic& Co);

  //! Intersection between an ellipse and another conic.
  Standard_EXPORT IntAna2d_AnaIntersection(const gp_Elips2d& E, const IntAna2d_Conic& C);

  //! Intersection between a parabola and another conic.
  Standard_EXPORT IntAna2d_AnaIntersection(const gp_Parab2d& P, const IntAna2d_Conic& C);

  //! Intersection between an hyperbola and another conic.
  Standard_EXPORT IntAna2d_AnaIntersection(const gp_Hypr2d& H, const IntAna2d_Conic& C);

  //! Intersection between two lines.
  Standard_EXPORT void Perform(const gp_Lin2d& L1, const gp_Lin2d& L2);

  //! Intersection between two circles.
  Standard_EXPORT void Perform(const gp_Circ2d& C1, const gp_Circ2d& C2);

  //! Intersection between a line and a circle.
  Standard_EXPORT void Perform(const gp_Lin2d& L, const gp_Circ2d& C);

  //! Intersection between a line and a conic.
  Standard_EXPORT void Perform(const gp_Lin2d& L, const IntAna2d_Conic& C);

  //! Intersection between a circle and another conic.
  Standard_EXPORT void Perform(const gp_Circ2d& C, const IntAna2d_Conic& Co);

  //! Intersection between an ellipse and another conic.
  Standard_EXPORT void Perform(const gp_Elips2d& E, const IntAna2d_Conic& C);

  //! Intersection between a parabola and another conic.
  Standard_EXPORT void Perform(const gp_Parab2d& P, const IntAna2d_Conic& C);

  //! Intersection between an hyperbola and another conic.
  Standard_EXPORT void Perform(const gp_Hypr2d& H, const IntAna2d_Conic& C);

  //! Returns TRUE if the computation was successful.
  Standard_Boolean IsDone() const;

  //! Returns TRUE when there is no intersection, i-e
  //! - no intersection point
  //! - the elements are not identical.
  //! The element may be parallel in this case.
  Standard_Boolean IsEmpty() const;

  //! For the intersection between an element of gp and a conic
  //! known by an implicit equation, the result will be TRUE
  //! if the element of gp verifies the implicit equation.
  //! For the intersection between two Lin2d or two Circ2d, the
  //! result will be TRUE if the elements are identical.
  //! The function returns FALSE in all the other cases.
  Standard_Boolean IdenticalElements() const;

  //! For the intersection between two Lin2d or two Circ2d,
  //! the function returns TRUE if the elements are parallel.
  //! The function returns FALSE in all the other cases.
  Standard_Boolean ParallelElements() const;

  //! returns the number of IntPoint between the 2 curves.
  Standard_Integer NbPoints() const;

  //! returns the intersection point of range N;
  //! If (N<=0) or (N>NbPoints), an exception is raised.
  const IntAna2d_IntPoint& Point(const Standard_Integer N) const;

protected:
private:
  Standard_Boolean  done;
  Standard_Boolean  para;
  Standard_Boolean  iden;
  Standard_Boolean  empt;
  Standard_Integer  nbp;
  IntAna2d_IntPoint lpnt[4];
};

#include <IntAna2d_AnaIntersection.lxx>

#endif // _IntAna2d_AnaIntersection_HeaderFile
