// Created on: 1997-03-12
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

#ifndef _VrmlConverter_ShadingAspect_HeaderFile
#define _VrmlConverter_ShadingAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Vrml_ShapeHints.hxx>
#include <Standard_Transient.hxx>
class Vrml_Material;

class VrmlConverter_ShadingAspect;
DEFINE_STANDARD_HANDLE(VrmlConverter_ShadingAspect, Standard_Transient)

//! qualifies the aspect properties for
//! the VRML conversation of ShadedShape .
class VrmlConverter_ShadingAspect : public Standard_Transient
{

public:
  //! create a default ShadingAspect.
  Standard_EXPORT VrmlConverter_ShadingAspect();

  Standard_EXPORT void SetFrontMaterial(const Handle(Vrml_Material)& aMaterial);

  Standard_EXPORT Handle(Vrml_Material) FrontMaterial() const;

  Standard_EXPORT void SetShapeHints(const Vrml_ShapeHints& aShapeHints);

  Standard_EXPORT Vrml_ShapeHints ShapeHints() const;

  //! defines necessary of  a  calculation  of  normals for  ShadedShape  to  more
  //! accurately  display  curved  surfaces,  pacticularly  when  smoooth  or  phong
  //! shading  is  used  in  VRML  viewer.
  //! By default False  -  the normals are not calculated,
  //! True  -  the normals are calculated.
  //! Warning: If  normals  are  calculated  the  resulting  VRML  file  will
  //! be  substantially  lager.
  Standard_EXPORT void SetHasNormals(const Standard_Boolean OnOff);

  //! returns True if the normals are calculating
  Standard_EXPORT Standard_Boolean HasNormals() const;

  //! defines necessary of writing  Material from Vrml into  output  OStream.
  //! By default False  -  the material is not writing into OStream,
  //! True  -  the material is writing.
  Standard_EXPORT void SetHasMaterial(const Standard_Boolean OnOff);

  //! returns True if the  materials is  writing into OStream.
  Standard_EXPORT Standard_Boolean HasMaterial() const;

  DEFINE_STANDARD_RTTIEXT(VrmlConverter_ShadingAspect, Standard_Transient)

protected:
private:
  Handle(Vrml_Material) myFrontMaterial;
  Vrml_ShapeHints       myShapeHints;
  Standard_Boolean      myHasNormals;
  Standard_Boolean      myHasMaterial;
};

#endif // _VrmlConverter_ShadingAspect_HeaderFile
