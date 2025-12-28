// Created on: 1991-05-15
// Created by: Isabelle GRIGNON
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

#ifndef _IntAna_IntQuadQuad_HeaderFile
#define _IntAna_IntQuadQuad_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <IntAna_Curve.hxx>
#include <Standard_Integer.hxx>
#include <gp_Pnt.hxx>
class gp_Cylinder;
class IntAna_Quadric;
class gp_Cone;

//! This class provides the analytic intersection between a
//! cylinder or a cone from gp and another quadric, as defined
//! in the class Quadric from IntAna.
//! This algorithm is used when the geometric intersection
//! (class QuadQuadGeo from IntAna) returns no geometric
//! solution.
//! The result of the intersection may be
//! - Curves as defined in the class Curve from IntAna
//! - Points (Pnt from gp)
class IntAna_IntQuadQuad
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty Constructor
  Standard_EXPORT IntAna_IntQuadQuad();

  //! Creates the intersection between a cylinder and a quadric.
  //! Tol est a definir plus precisemment.
  Standard_EXPORT IntAna_IntQuadQuad(const gp_Cylinder&    C,
                                     const IntAna_Quadric& Q,
                                     const double          Tol);

  //! Creates the intersection between a cone and a quadric.
  //! Tol est a definir plus precisemment.
  Standard_EXPORT IntAna_IntQuadQuad(const gp_Cone& C, const IntAna_Quadric& Q, const double Tol);

  //! Intersects a cylinder and a quadric .
  //! Tol est a definir plus precisemment.
  Standard_EXPORT void Perform(const gp_Cylinder& C, const IntAna_Quadric& Q, const double Tol);

  //! Intersects a cone and a quadric.
  //! Tol est a definir plus precisemment.
  Standard_EXPORT void Perform(const gp_Cone& C, const IntAna_Quadric& Q, const double Tol);

  //! Returns True if the computation was successful.
  bool IsDone() const;

  //! Returns TRUE if the cylinder, the cone or the sphere
  //! is identical to the quadric.
  bool IdenticalElements() const;

  //! Returns the number of curves solution.
  int NbCurve() const;

  //! Returns the curve of range N.
  Standard_EXPORT const IntAna_Curve& Curve(const int N) const;

  //! Returns the number of contact point.
  int NbPnt() const;

  //! Returns the point of range N.
  Standard_EXPORT const gp_Pnt& Point(const int N) const;

  //! Returns the parameters on the "explicit quadric"
  //! (i.e. the cylinder or the cone, the first argument given to the constructor)
  //! of the point of range N.
  Standard_EXPORT void Parameters(const int N, double& U1, double& U2) const;

  //! Returns True if the Curve I shares its last bound
  //! with another curve.
  Standard_EXPORT bool HasNextCurve(const int I) const;

  //! If HasNextCurve(I) returns True, this function
  //! returns the Index J of the curve which has a
  //! common bound with the curve I. If theOpposite ==
  //! True, then the last parameter of the curve I, and
  //! the last parameter of the curve J give the same
  //! point. Else the last parameter of the curve I and
  //! the first parameter of the curve J are the same
  //! point.
  Standard_EXPORT int NextCurve(const int I, bool& theOpposite) const;

  //! Returns True if the Curve I shares its first bound
  //! with another curve.
  Standard_EXPORT bool HasPreviousCurve(const int I) const;

  //! if HasPreviousCurve(I) returns True, this function
  //! returns the Index J of the curve which has a common
  //! bound with the curve I. If theOpposite == True
  //! then the first parameter of the curve I, and the
  //! first parameter of the curve J give the same
  //! point. Else the first parameter of the curve I and
  //! the last parameter of the curve J are the same
  //! point.
  Standard_EXPORT int PreviousCurve(const int I, bool& theOpposite) const;

protected:
  //! Set the next and previous fields. Private method.
  Standard_EXPORT void InternalSetNextAndPrevious();

protected:
  bool         done;
  bool         identical;
  IntAna_Curve TheCurve[12];
  int          previouscurve[12];
  int          nextcurve[12];
  int          NbCurves;
  int          Nbpoints;
  gp_Pnt       Thepoints[2];
  int          myNbMaxCurves;
  double       myEpsilon;
  double       myEpsilonCoeffPolyNull;
};

#include <IntAna_IntQuadQuad.lxx>

#endif // _IntAna_IntQuadQuad_HeaderFile
