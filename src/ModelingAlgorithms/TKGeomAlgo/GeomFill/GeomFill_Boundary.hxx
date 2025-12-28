// Created on: 1995-10-17
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _GeomFill_Boundary_HeaderFile
#define _GeomFill_Boundary_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
class gp_Pnt;
class gp_Vec;

//! Root class to define a boundary which will form part of a
//! contour around a gap requiring filling.
//! Any new type of constrained boundary must inherit this class.
//! The GeomFill package provides two classes to define constrained boundaries:
//! -   GeomFill_SimpleBound to define an unattached boundary
//! -   GeomFill_BoundWithSurf to define a boundary attached to a surface.
//! These objects are used to define the boundaries for a
//! GeomFill_ConstrainedFilling framework.
class GeomFill_Boundary : public Standard_Transient
{

public:
  Standard_EXPORT virtual gp_Pnt Value(const double U) const = 0;

  Standard_EXPORT virtual void D1(const double U, gp_Pnt& P, gp_Vec& V) const = 0;

  Standard_EXPORT virtual bool HasNormals() const;

  Standard_EXPORT virtual gp_Vec Norm(const double U) const;

  Standard_EXPORT virtual void D1Norm(const double U, gp_Vec& N, gp_Vec& DN) const;

  Standard_EXPORT virtual void Reparametrize(const double First,
                                             const double Last,
                                             const bool   HasDF,
                                             const bool   HasDL,
                                             const double DF,
                                             const double DL,
                                             const bool   Rev) = 0;

  Standard_EXPORT void Points(gp_Pnt& PFirst, gp_Pnt& PLast) const;

  Standard_EXPORT virtual void Bounds(double& First, double& Last) const = 0;

  Standard_EXPORT virtual bool IsDegenerated() const = 0;

  Standard_EXPORT double Tol3d() const;

  Standard_EXPORT void Tol3d(const double Tol);

  Standard_EXPORT double Tolang() const;

  Standard_EXPORT void Tolang(const double Tol);

  DEFINE_STANDARD_RTTIEXT(GeomFill_Boundary, Standard_Transient)

protected:
  Standard_EXPORT GeomFill_Boundary(const double Tol3d, const double Tolang);

private:
  double myT3d;
  double myTang;
};

#endif // _GeomFill_Boundary_HeaderFile
