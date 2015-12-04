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

#ifndef _Geom2dAdaptor_HCurve_HeaderFile
#define _Geom2dAdaptor_HCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Geom2dAdaptor_GHCurve.hxx>
#include <Standard_Real.hxx>
class Standard_ConstructionError;
class Geom2dAdaptor_Curve;
class Geom2d_Curve;


class Geom2dAdaptor_HCurve;
DEFINE_STANDARD_HANDLE(Geom2dAdaptor_HCurve, Geom2dAdaptor_GHCurve)

//! Provides an interface between the services provided by any
//! curve from the package Geom2d and those required
//! of the curve by algorithms, which use it.
class Geom2dAdaptor_HCurve : public Geom2dAdaptor_GHCurve
{

public:

  
  Standard_EXPORT Geom2dAdaptor_HCurve();
  
  Standard_EXPORT Geom2dAdaptor_HCurve(const Geom2dAdaptor_Curve& AS);
  
  Standard_EXPORT Geom2dAdaptor_HCurve(const Handle(Geom2d_Curve)& S);
  
  //! ConstructionError is raised if UFirst>ULast or VFirst>VLast
  Standard_EXPORT Geom2dAdaptor_HCurve(const Handle(Geom2d_Curve)& S, const Standard_Real UFirst, const Standard_Real ULast);




  DEFINE_STANDARD_RTTIEXT(Geom2dAdaptor_HCurve,Geom2dAdaptor_GHCurve)

protected:




private:




};







#endif // _Geom2dAdaptor_HCurve_HeaderFile
