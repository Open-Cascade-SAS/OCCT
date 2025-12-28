// Created on: 1995-11-03
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

#ifndef _GeomFill_SimpleBound_HeaderFile
#define _GeomFill_SimpleBound_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomFill_Boundary.hxx>

class Law_Function;
class gp_Pnt;
class gp_Vec;

//! Defines a 3d curve as a boundary for a
//! GeomFill_ConstrainedFilling algorithm.
//! This curve is unattached to an existing surface.D
//! Contains fields to allow a reparametrization of curve.
class GeomFill_SimpleBound : public GeomFill_Boundary
{

public:
  //! Constructs the boundary object defined by the 3d curve.
  //! The surface to be built along this boundary will be in the
  //! tolerance range defined by Tol3d.
  //! This object is to be used as a boundary for a
  //! GeomFill_ConstrainedFilling framework.
  //! Dummy is initialized but has no function in this class.
  //! Warning
  //! Curve is an adapted curve, that is, an object which is an interface between:
  //! -   the services provided by a 3D curve from the package Geom
  //! -   and those required of the curve by the computation
  //! algorithm which uses it.
  //! The adapted curve is created in one of the following ways:
  //! -   First sequence:
  //! occ::handle<Geom_Curve> myCurve = ... ;
  //! occ::handle<GeomAdaptor_Curve>
  //! Curve = new
  //! GeomAdaptor_Curve(myCurve);
  //! -   Second sequence:
  //! // Step 1
  //! occ::handle<Geom_Curve> myCurve = ... ;
  //! GeomAdaptor_Curve Crv (myCurve);
  //! // Step 2
  //! occ::handle<GeomAdaptor_Curve>
  //! Curve = new
  //! GeomAdaptor_Curve(Crv);
  //! You use the second part of this sequence if you already
  //! have the adapted curve Crv.
  //! The boundary is then constructed with the Curve object:
  //! double Tol = ... ;
  //! double dummy = 0. ;
  //! myBoundary = GeomFill_SimpleBound
  //! (Curve,Tol,dummy);
  Standard_EXPORT GeomFill_SimpleBound(const occ::handle<Adaptor3d_Curve>& Curve,
                                       const double            Tol3d,
                                       const double            Tolang);

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

  DEFINE_STANDARD_RTTIEXT(GeomFill_SimpleBound, GeomFill_Boundary)

private:
  occ::handle<Adaptor3d_Curve> myC3d;
  occ::handle<Law_Function>    myPar;
};

#endif // _GeomFill_SimpleBound_HeaderFile
