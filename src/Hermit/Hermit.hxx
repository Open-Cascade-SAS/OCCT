// Created on: 1997-02-18
// Created by: Stagiaire Francois DUMONT
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _Hermit_HeaderFile
#define _Hermit_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class Geom2d_BSplineCurve;
class Geom_BSplineCurve;


//! This  is used to  reparameterize Rational  BSpline
//! Curves so that we can   concatenate them later to
//! build C1 Curves  It builds and 1D-reparameterizing
//! function starting from an Hermite interpolation and
//! adding knots and modifying poles of the 1D BSpline
//! obtained that way. The goal is to build a(u) so that
//! if we consider a BSpline curve
//! N(u)
//! f(u) =  -----
//! D(u)
//!
//! the function a(u)D(u) has value 1 at the umin and umax
//! and has 0.0e0 derivative value a umin and umax.
//! The details of the computation occuring in this package
//! can be found by reading :
//! " Etude sur la concatenation de NURBS en vue du
//! balayage de surfaces" PFE n S85 Ensam Lille
class Hermit 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! returns the correct spline a(u) which will
  //! be multiplicated with BS later.
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) Solution (const Handle(Geom_BSplineCurve)& BS, const Standard_Real TolPoles = 0.000001, const Standard_Real TolKnots = 0.000001);
  
  //! returns the correct spline a(u) which will
  //! be multiplicated with BS later.
  Standard_EXPORT static Handle(Geom2d_BSplineCurve) Solution (const Handle(Geom2d_BSplineCurve)& BS, const Standard_Real TolPoles = 0.000001, const Standard_Real TolKnots = 0.000001);
  
  //! returns the knots to insert to a(u) to
  //! stay with a constant sign and in the
  //! tolerances.
  Standard_EXPORT static void Solutionbis (const Handle(Geom_BSplineCurve)& BS, Standard_Real& Knotmin, Standard_Real& Knotmax, const Standard_Real TolPoles = 0.000001, const Standard_Real TolKnots = 0.000001);




protected:





private:





};







#endif // _Hermit_HeaderFile
