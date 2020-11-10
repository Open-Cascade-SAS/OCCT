// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _GeomConvert_Units_HeaderFile
#define _GeomConvert_Units_HeaderFile

#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>
class Geom2d_Curve;
class Geom_Surface;

//! Class contains conversion methods for 2d geom objects
class GeomConvert_Units
{
public:

  DEFINE_STANDARD_ALLOC

  //! Convert 2d curve for change angle unit from radian to degree 
  Standard_EXPORT static Handle(Geom2d_Curve) RadianToDegree(
    const Handle(Geom2d_Curve)& theCurve,
    const Handle(Geom_Surface)& theSurface,
    const Standard_Real theLengthFactor,
    const Standard_Real theFactorRadianDegree);
  
  //! Convert 2d curve for change angle unit from degree to radian
  Standard_EXPORT static Handle(Geom2d_Curve) DegreeToRadian(
    const Handle(Geom2d_Curve)& theCurve,
    const Handle(Geom_Surface)& theSurface,
    const Standard_Real theLengthFactor,
    const Standard_Real theFactorRadianDegree);
  
  //! return 2d curve as 'mirror' for given
  Standard_EXPORT static Handle(Geom2d_Curve) MirrorPCurve(const Handle(Geom2d_Curve)& theCurve);
  
};

#endif // _GeomConvert_Units_HeaderFile
