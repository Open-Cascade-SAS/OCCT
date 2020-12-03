// Created on: 1993-06-03
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Geom2dAdaptor_HeaderFile
#define _Geom2dAdaptor_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class Geom2d_Curve;
class Adaptor2d_Curve2d;

//! this package  contains the geometric definition of
//! 2d  curves compatible  with  the  Adaptor  package
//! templates.
class Geom2dAdaptor 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Inherited  from    GHCurve.   Provides a  curve
  //! handled by reference.
  //! Creates  a 2d  curve  from  a  HCurve2d.  This
  //! cannot process the OtherCurves.
  Standard_EXPORT static Handle(Geom2d_Curve) MakeCurve (const Adaptor2d_Curve2d& HC);

};

#endif // _Geom2dAdaptor_HeaderFile
