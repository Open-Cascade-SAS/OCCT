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

#include <Quantity_Color.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Transient.hxx>
#include <Standard_OStream.hxx>

//! defines a Material node of VRML specifying properties of geometry
//! and its appearance.
//! This node defines the current surface material properties for all subsequent shapes.
//! Material sets several components of the current material during traversal. Different shapes
//! interpret materials with multiple values differently. To bind materials to shapes, use a
//! MaterialBinding node.
class Vrml_Material : public Standard_Transient
{

public:
  Standard_EXPORT Vrml_Material(const occ::handle<NCollection_HArray1<Quantity_Color>>& aAmbientColor,
                                const occ::handle<NCollection_HArray1<Quantity_Color>>& aDiffuseColor,
                                const occ::handle<NCollection_HArray1<Quantity_Color>>& aSpecularColor,
                                const occ::handle<NCollection_HArray1<Quantity_Color>>& aEmissiveColor,
                                const occ::handle<NCollection_HArray1<double>>&   aShininess,
                                const occ::handle<NCollection_HArray1<double>>&   aTransparency);

  Standard_EXPORT Vrml_Material();

  Standard_EXPORT void SetAmbientColor(const occ::handle<NCollection_HArray1<Quantity_Color>>& aAmbientColor);

  Standard_EXPORT occ::handle<NCollection_HArray1<Quantity_Color>> AmbientColor() const;

  Standard_EXPORT void SetDiffuseColor(const occ::handle<NCollection_HArray1<Quantity_Color>>& aDiffuseColor);

  Standard_EXPORT occ::handle<NCollection_HArray1<Quantity_Color>> DiffuseColor() const;

  Standard_EXPORT void SetSpecularColor(const occ::handle<NCollection_HArray1<Quantity_Color>>& aSpecularColor);

  Standard_EXPORT occ::handle<NCollection_HArray1<Quantity_Color>> SpecularColor() const;

  Standard_EXPORT void SetEmissiveColor(const occ::handle<NCollection_HArray1<Quantity_Color>>& aEmissiveColor);

  Standard_EXPORT occ::handle<NCollection_HArray1<Quantity_Color>> EmissiveColor() const;

  Standard_EXPORT void SetShininess(const occ::handle<NCollection_HArray1<double>>& aShininess);

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> Shininess() const;

  Standard_EXPORT void SetTransparency(const occ::handle<NCollection_HArray1<double>>& aTransparency);

  Standard_EXPORT occ::handle<NCollection_HArray1<double>> Transparency() const;

  Standard_EXPORT Standard_OStream& Print(Standard_OStream& anOStream) const;

  DEFINE_STANDARD_RTTIEXT(Vrml_Material, Standard_Transient)

private:
  occ::handle<NCollection_HArray1<Quantity_Color>> myAmbientColor;
  occ::handle<NCollection_HArray1<Quantity_Color>> myDiffuseColor;
  occ::handle<NCollection_HArray1<Quantity_Color>> mySpecularColor;
  occ::handle<NCollection_HArray1<Quantity_Color>> myEmissiveColor;
  occ::handle<NCollection_HArray1<double>>   myShininess;
  occ::handle<NCollection_HArray1<double>>   myTransparency;
};

#endif // _Vrml_Material_HeaderFile
