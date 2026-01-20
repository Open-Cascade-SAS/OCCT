// Created on: 1995-12-05
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

#ifndef _GeomFill_DegeneratedBound_HeaderFile
#define _GeomFill_DegeneratedBound_HeaderFile

#include <Standard.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <GeomFill_Boundary.hxx>
class gp_Vec;

//! Description of a degenerated boundary (a point).
//! Class defining a degenerated boundary for a
//! constrained filling with a point and no other
//! constraint. Only used to simulate an ordinary bound,
//! may not be useful and desapear soon.
class GeomFill_DegeneratedBound : public GeomFill_Boundary
{

public:
  Standard_EXPORT GeomFill_DegeneratedBound(const gp_Pnt&       Point,
                                            const double First,
                                            const double Last,
                                            const double Tol3d,
                                            const double Tolang);

  Standard_EXPORT gp_Pnt Value(const double U) const override;

  Standard_EXPORT void D1(const double U, gp_Pnt& P, gp_Vec& V) const override;

  Standard_EXPORT void Reparametrize(const double    First,
                                     const double    Last,
                                     const bool HasDF,
                                     const bool HasDL,
                                     const double    DF,
                                     const double    DL,
                                     const bool Rev) override;

  Standard_EXPORT void Bounds(double& First, double& Last) const override;

  Standard_EXPORT bool IsDegenerated() const override;

  DEFINE_STANDARD_RTTIEXT(GeomFill_DegeneratedBound, GeomFill_Boundary)

private:
  gp_Pnt        myPoint;
  double myFirst;
  double myLast;
};

#endif // _GeomFill_DegeneratedBound_HeaderFile
