// Created on: 1994-09-29
// Created by: Dieter THIEMANN
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _UnitsMethods_HeaderFile
#define _UnitsMethods_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Geom2d_Curve;
class Geom_Surface;



class UnitsMethods 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes the 3 factors for the conversion of
  //! units
  Standard_EXPORT static void InitializeFactors (const Standard_Real LengthFactor, const Standard_Real PlaneAngleFactor, const Standard_Real SolidAngleFactor);
  
  Standard_EXPORT static Standard_Real LengthFactor();
  
  Standard_EXPORT static Standard_Real PlaneAngleFactor();
  
  Standard_EXPORT static Standard_Real SolidAngleFactor();
  
  Standard_EXPORT static void Set3dConversion (const Standard_Boolean B);
  
  Standard_EXPORT static Standard_Boolean Convert3d();
  
  Standard_EXPORT static Handle(Geom2d_Curve) RadianToDegree (const Handle(Geom2d_Curve)& C, const Handle(Geom_Surface)& S);
  
  Standard_EXPORT static Handle(Geom2d_Curve) DegreeToRadian (const Handle(Geom2d_Curve)& C, const Handle(Geom_Surface)& S);
  
  Standard_EXPORT static Handle(Geom2d_Curve) MirrorPCurve (const Handle(Geom2d_Curve)& C);
  
  //! Returns value of unit encoded by parameter param
  //! (integer value denoting unit, as described in IGES
  //! standard) in millimeters
  Standard_EXPORT static Standard_Real GetLengthFactorValue (const Standard_Integer param);
  
  //! Returns value of current internal unit for CASCADE
  //! in millemeters
  Standard_EXPORT static Standard_Real GetCasCadeLengthUnit();
  
  //! Sets value of current internal unit for CASCADE
  //! by parameter param (integer value denoting unit,
  //! as described in IGES standard)
  //! GetCasCadeLengthUnit() will then return value
  //! equal to GetLengthFactorValue(param)
  Standard_EXPORT static void SetCasCadeLengthUnit (const Standard_Integer param);




protected:





private:





};







#endif // _UnitsMethods_HeaderFile
