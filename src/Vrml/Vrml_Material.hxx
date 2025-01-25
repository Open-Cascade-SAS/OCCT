// Created on: 1997-02-10
// Created by: Alexander BRIVIN and Dmitry TARASOV
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

#ifndef _Vrml_Material_HeaderFile
#define _Vrml_Material_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Quantity_HArray1OfColor.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>

class Vrml_Material;
DEFINE_STANDARD_HANDLE(Vrml_Material, Standard_Transient)

//! defines a Material node of VRML specifying properties of geometry
//! and its appearance.
//! This node defines the current surface material properties for all subsequent shapes.
//! Material sets several components of the current material during traversal. Different shapes
//! interpret materials with multiple values differently. To bind materials to shapes, use a
//! MaterialBinding node.
class Vrml_Material : public Standard_Transient
{

public:
  Standard_EXPORT Vrml_Material(const Handle(Quantity_HArray1OfColor)& aAmbientColor,
                                const Handle(Quantity_HArray1OfColor)& aDiffuseColor,
                                const Handle(Quantity_HArray1OfColor)& aSpecularColor,
                                const Handle(Quantity_HArray1OfColor)& aEmissiveColor,
                                const Handle(TColStd_HArray1OfReal)&   aShininess,
                                const Handle(TColStd_HArray1OfReal)&   aTransparency);

  Standard_EXPORT Vrml_Material();

  Standard_EXPORT void SetAmbientColor(const Handle(Quantity_HArray1OfColor)& aAmbientColor);

  Standard_EXPORT Handle(Quantity_HArray1OfColor) AmbientColor() const;

  Standard_EXPORT void SetDiffuseColor(const Handle(Quantity_HArray1OfColor)& aDiffuseColor);

  Standard_EXPORT Handle(Quantity_HArray1OfColor) DiffuseColor() const;

  Standard_EXPORT void SetSpecularColor(const Handle(Quantity_HArray1OfColor)& aSpecularColor);

  Standard_EXPORT Handle(Quantity_HArray1OfColor) SpecularColor() const;

  Standard_EXPORT void SetEmissiveColor(const Handle(Quantity_HArray1OfColor)& aEmissiveColor);

  Standard_EXPORT Handle(Quantity_HArray1OfColor) EmissiveColor() const;

  Standard_EXPORT void SetShininess(const Handle(TColStd_HArray1OfReal)& aShininess);

  Standard_EXPORT Handle(TColStd_HArray1OfReal) Shininess() const;

  Standard_EXPORT void SetTransparency(const Handle(TColStd_HArray1OfReal)& aTransparency);

  Standard_EXPORT Handle(TColStd_HArray1OfReal) Transparency() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

  DEFINE_STANDARD_RTTIEXT(Vrml_Material, Standard_Transient)

protected:
private:
  Handle(Quantity_HArray1OfColor) myAmbientColor;
  Handle(Quantity_HArray1OfColor) myDiffuseColor;
  Handle(Quantity_HArray1OfColor) mySpecularColor;
  Handle(Quantity_HArray1OfColor) myEmissiveColor;
  Handle(TColStd_HArray1OfReal)   myShininess;
  Handle(TColStd_HArray1OfReal)   myTransparency;
};

#endif // _Vrml_Material_HeaderFile
