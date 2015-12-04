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

#ifndef _GeomAdaptor_HSurface_HeaderFile
#define _GeomAdaptor_HSurface_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomAdaptor_GHSurface.hxx>
#include <Standard_Real.hxx>
class Standard_ConstructionError;
class GeomAdaptor_Surface;
class Geom_Surface;


class GeomAdaptor_HSurface;
DEFINE_STANDARD_HANDLE(GeomAdaptor_HSurface, GeomAdaptor_GHSurface)

//! An interface between the services provided by any
//! surface from the package Geom and those required
//! of the surface by algorithms which use it.
//! Provides a  surface handled by reference.
class GeomAdaptor_HSurface : public GeomAdaptor_GHSurface
{

public:

  
    GeomAdaptor_HSurface();
  
    GeomAdaptor_HSurface(const GeomAdaptor_Surface& AS);
  
    GeomAdaptor_HSurface(const Handle(Geom_Surface)& S);
  
  //! ConstructionError is raised if UFirst>ULast or VFirst>VLast
    GeomAdaptor_HSurface(const Handle(Geom_Surface)& S, const Standard_Real UFirst, const Standard_Real ULast, const Standard_Real VFirst, const Standard_Real VLast, const Standard_Real TolU = 0.0, const Standard_Real TolV = 0.0);




  DEFINE_STANDARD_RTTIEXT(GeomAdaptor_HSurface,GeomAdaptor_GHSurface)

protected:




private:




};


#include <GeomAdaptor_HSurface.lxx>





#endif // _GeomAdaptor_HSurface_HeaderFile
