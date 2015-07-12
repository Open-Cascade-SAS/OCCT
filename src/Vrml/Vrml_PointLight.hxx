// Created on: 1997-02-12
// Created by: Alexander BRIVIN
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

#ifndef _Vrml_PointLight_HeaderFile
#define _Vrml_PointLight_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Quantity_Color.hxx>
#include <gp_Vec.hxx>
#include <Standard_OStream.hxx>
class Quantity_Color;
class gp_Vec;


//! defines a point  light node of VRML specifying
//! properties of lights.
//! This  node  defines  a  point  light  source   at  a  fixed  3D  location
//! A  point  source  illuminates equally  in  all  directions;
//! that  is  omni-directional.
//! Color is  written  as  an  RGB  triple.
//! Light intensity must be in the range 0.0 to 1.0, inclusive.
class Vrml_PointLight 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Vrml_PointLight();
  
  Standard_EXPORT Vrml_PointLight(const Standard_Boolean aOnOff, const Standard_Real aIntensity, const Quantity_Color& aColor, const gp_Vec& aLocation);
  
  Standard_EXPORT void SetOnOff (const Standard_Boolean aOnOff);
  
  Standard_EXPORT Standard_Boolean OnOff() const;
  
  Standard_EXPORT void SetIntensity (const Standard_Real aIntensity);
  
  Standard_EXPORT Standard_Real Intensity() const;
  
  Standard_EXPORT void SetColor (const Quantity_Color& aColor);
  
  Standard_EXPORT Quantity_Color Color() const;
  
  Standard_EXPORT void SetLocation (const gp_Vec& aLocation);
  
  Standard_EXPORT gp_Vec Location() const;
  
  Standard_EXPORT Standard_OStream& Print (Standard_OStream& anOStream) const;




protected:





private:



  Standard_Boolean myOnOff;
  Standard_Real myIntensity;
  Quantity_Color myColor;
  gp_Vec myLocation;


};







#endif // _Vrml_PointLight_HeaderFile
