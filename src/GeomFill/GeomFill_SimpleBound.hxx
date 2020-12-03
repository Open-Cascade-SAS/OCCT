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
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>

class Law_Function;
class gp_Pnt;
class gp_Vec;


class GeomFill_SimpleBound;
DEFINE_STANDARD_HANDLE(GeomFill_SimpleBound, GeomFill_Boundary)

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
  //! Handle(Geom_Curve) myCurve = ... ;
  //! Handle(GeomAdaptor_Curve)
  //! Curve = new
  //! GeomAdaptor_Curve(myCurve);
  //! -   Second sequence:
  //! // Step 1
  //! Handle(Geom_Curve) myCurve = ... ;
  //! GeomAdaptor_Curve Crv (myCurve);
  //! // Step 2
  //! Handle(GeomAdaptor_Curve)
  //! Curve = new
  //! GeomAdaptor_Curve(Crv);
  //! You use the second part of this sequence if you already
  //! have the adapted curve Crv.
  //! The boundary is then constructed with the Curve object:
  //! Standard_Real Tol = ... ;
  //! Standard_Real dummy = 0. ;
  //! myBoundary = GeomFill_SimpleBound
  //! (Curve,Tol,dummy);
  Standard_EXPORT GeomFill_SimpleBound(const Handle(Adaptor3d_Curve)& Curve, const Standard_Real Tol3d, const Standard_Real Tolang);
  
  Standard_EXPORT gp_Pnt Value (const Standard_Real U) const Standard_OVERRIDE;
  
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V) const Standard_OVERRIDE;
  
  Standard_EXPORT void Reparametrize (const Standard_Real First, const Standard_Real Last, const Standard_Boolean HasDF, const Standard_Boolean HasDL, const Standard_Real DF, const Standard_Real DL, const Standard_Boolean Rev) Standard_OVERRIDE;
  
  Standard_EXPORT void Bounds (Standard_Real& First, Standard_Real& Last) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsDegenerated() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GeomFill_SimpleBound,GeomFill_Boundary)

protected:




private:


  Handle(Adaptor3d_Curve) myC3d;
  Handle(Law_Function) myPar;


};







#endif // _GeomFill_SimpleBound_HeaderFile
