// Created on: 1992-10-08
// Created by: Isabelle GRIGNON
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _GeomAdaptor_HeaderFile
#define _GeomAdaptor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Geom_Curve;
class Adaptor3d_Curve;
class Geom_Surface;
class Adaptor3d_Surface;

//! this package contains the  geometric definition of
//! curve and surface necessary to use algorithms.
class GeomAdaptor
{
public:
  DEFINE_STANDARD_ALLOC

  //! Inherited  from    GHCurve.   Provides a  curve
  //! handled by reference.
  //! Build a Geom_Curve using the information from the
  //! Curve from Adaptor3d
  Standard_EXPORT static Handle(Geom_Curve) MakeCurve(const Adaptor3d_Curve& C);

  //! Build a Geom_Surface using the information from the Surface from Adaptor3d
  //! @param theS - Surface adaptor to convert.
  //! @param theTrimFlag - True if perform trim surface values by adaptor and false otherwise.
  Standard_EXPORT static Handle(Geom_Surface) MakeSurface(
    const Adaptor3d_Surface& theS,
    const Standard_Boolean   theTrimFlag = Standard_True);
};

#endif // _GeomAdaptor_HeaderFile
