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

#ifndef _Vrml_DirectionalLight_HeaderFile
#define _Vrml_DirectionalLight_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Quantity_Color.hxx>
#include <gp_Vec.hxx>
#include <Standard_OStream.hxx>

//! defines a directional  light node of VRML specifying
//! properties of lights.
//! This  node  defines  a  directional  light  source  that  illuminates
//! along  rays  parallel  to  a  given  3-dimensional  vector
//! Color is  written  as  an  RGB  triple.
//! Light intensity must be in the range 0.0 to 1.0, inclusive.
class Vrml_DirectionalLight
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Vrml_DirectionalLight();

  Standard_EXPORT Vrml_DirectionalLight(const Standard_Boolean aOnOff,
                                        const Standard_Real    aIntensity,
                                        const Quantity_Color&  aColor,
                                        const gp_Vec&          aDirection);

  Standard_EXPORT void SetOnOff(const Standard_Boolean aOnOff);

  Standard_EXPORT Standard_Boolean OnOff() const;

  Standard_EXPORT void SetIntensity(const Standard_Real aIntensity);

  Standard_EXPORT Standard_Real Intensity() const;

  Standard_EXPORT void SetColor(const Quantity_Color& aColor);

  Standard_EXPORT Quantity_Color Color() const;

  Standard_EXPORT void SetDirection(const gp_Vec& aDirection);

  Standard_EXPORT gp_Vec Direction() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

protected:
private:
  Standard_Boolean myOnOff;
  Standard_Real    myIntensity;
  Quantity_Color   myColor;
  gp_Vec           myDirection;
};

#endif // _Vrml_DirectionalLight_HeaderFile
