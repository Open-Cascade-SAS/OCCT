// Created on: 1995-08-25
// Created by: Remi LEQUETTE
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

#ifndef _GeomAdaptor_HCurve_HeaderFile
#define _GeomAdaptor_HCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomAdaptor_GHCurve.hxx>
#include <Standard_Real.hxx>
class Standard_ConstructionError;
class GeomAdaptor_Curve;
class Geom_Curve;


class GeomAdaptor_HCurve;
DEFINE_STANDARD_HANDLE(GeomAdaptor_HCurve, GeomAdaptor_GHCurve)

//! An interface between the services provided by any
//! curve from the package Geom and those required of
//! the curve by algorithms which use it.
class GeomAdaptor_HCurve : public GeomAdaptor_GHCurve
{

public:

  
    GeomAdaptor_HCurve();
  
    GeomAdaptor_HCurve(const GeomAdaptor_Curve& AS);
  
    GeomAdaptor_HCurve(const Handle(Geom_Curve)& S);
  
  //! ConstructionError is raised if UFirst>ULast or VFirst>VLast
    GeomAdaptor_HCurve(const Handle(Geom_Curve)& S, const Standard_Real UFirst, const Standard_Real ULast);




  DEFINE_STANDARD_RTTIEXT(GeomAdaptor_HCurve,GeomAdaptor_GHCurve)

protected:




private:




};


#include <GeomAdaptor_HCurve.lxx>





#endif // _GeomAdaptor_HCurve_HeaderFile
