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

#ifndef _GeomFill_BoundWithSurf_HeaderFile
#define _GeomFill_BoundWithSurf_HeaderFile

#include <Standard.hxx>

#include <Adaptor3d_CurveOnSurface.hxx>
#include <GeomFill_Boundary.hxx>
#include <Standard_Real.hxx>
class Law_Function;
class gp_Pnt;
class gp_Vec;


class GeomFill_BoundWithSurf;
DEFINE_STANDARD_HANDLE(GeomFill_BoundWithSurf, GeomFill_Boundary)

//! Defines a 3d curve as a boundary for a
//! GeomFill_ConstrainedFilling algorithm.
//! This curve is attached to an existing surface.
//! Defines a  constrained boundary for  filling
//! the computations are done with a CurveOnSurf and a
//! normals field  defined by the normalized normal to
//! the surface along the PCurve.
//! Contains fields  to allow a reparametrization of curve
//! and normals field.
class GeomFill_BoundWithSurf : public GeomFill_Boundary
{

public:

  

  //! Constructs a boundary object defined by the 3d curve CurveOnSurf.
  //! The surface to be filled along this boundary will be in the
  //! tolerance range defined by Tol3d.
  //! What's more, at each point of CurveOnSurf, the angle
  //! between the normal to the surface to be filled along this
  //! boundary, and the normal to the surface on which
  //! CurveOnSurf lies, must not be greater than TolAng.
  //! This object is to be used as a boundary for a
  //! GeomFill_ConstrainedFilling framework.
  //! Warning
  //! CurveOnSurf is an adapted curve, that is, an object
  //! which is an interface between:
  //! -   the services provided by a curve lying on a surface from the package Geom
  //! -   and those required of the curve by the computation algorithm which uses it.
  //! The adapted curve is created in the following way:
  //! Handle(Geom_Surface) mySurface = ... ;
  //! Handle(Geom2d_Curve) myParamCurve = ... ;
  //! // where myParamCurve is a 2D curve in the parametric space of the surface mySurface
  //! Handle(GeomAdaptor_Surface)
  //! Surface = new
  //! GeomAdaptor_Surface(mySurface);
  //! Handle(Geom2dAdaptor_Curve)
  //! ParamCurve = new
  //! Geom2dAdaptor_Curve(myParamCurve);
  //! CurveOnSurf = Adaptor3d_CurveOnSurface(ParamCurve,Surface);
  //! The boundary is then constructed with the CurveOnSurf object:
  //! Standard_Real Tol = ... ;
  //! Standard_Real TolAng = ... ;
  //! myBoundary =  GeomFill_BoundWithSurf (
  //! CurveOnSurf, Tol, TolAng );
  Standard_EXPORT GeomFill_BoundWithSurf(const Adaptor3d_CurveOnSurface& CurveOnSurf, const Standard_Real Tol3d, const Standard_Real Tolang);
  
  Standard_EXPORT gp_Pnt Value (const Standard_Real U) const Standard_OVERRIDE;
  
  Standard_EXPORT void D1 (const Standard_Real U, gp_Pnt& P, gp_Vec& V) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean HasNormals() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual gp_Vec Norm (const Standard_Real U) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void D1Norm (const Standard_Real U, gp_Vec& N, gp_Vec& DN) const Standard_OVERRIDE;
  
  Standard_EXPORT void Reparametrize (const Standard_Real First, const Standard_Real Last, const Standard_Boolean HasDF, const Standard_Boolean HasDL, const Standard_Real DF, const Standard_Real DL, const Standard_Boolean Rev) Standard_OVERRIDE;
  
  Standard_EXPORT void Bounds (Standard_Real& First, Standard_Real& Last) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsDegenerated() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(GeomFill_BoundWithSurf,GeomFill_Boundary)

protected:




private:


  Adaptor3d_CurveOnSurface myConS;
  Handle(Law_Function) myPar;


};







#endif // _GeomFill_BoundWithSurf_HeaderFile
