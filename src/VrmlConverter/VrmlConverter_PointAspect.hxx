// Created on: 1997-04-23
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

#ifndef _VrmlConverter_PointAspect_HeaderFile
#define _VrmlConverter_PointAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>
class Vrml_Material;


class VrmlConverter_PointAspect;
DEFINE_STANDARD_HANDLE(VrmlConverter_PointAspect, Standard_Transient)

//! qualifies the aspect properties for
//! the VRML conversation of a Point Set.
class VrmlConverter_PointAspect : public Standard_Transient
{

public:

  
  //! create a default PointAspect.
  //! Default value: HasMaterial  =  False  - a  line  hasn't  own  material (color)
  Standard_EXPORT VrmlConverter_PointAspect();
  
  Standard_EXPORT VrmlConverter_PointAspect(const Handle(Vrml_Material)& aMaterial, const Standard_Boolean OnOff);
  
  Standard_EXPORT void SetMaterial (const Handle(Vrml_Material)& aMaterial);
  
  Standard_EXPORT Handle(Vrml_Material) Material() const;
  
  //! defines the necessary of writing  own  Material from Vrml into  output  OStream.
  //! By default False  -  the material is not writing into OStream,
  //! True  -  the material is writing.
  Standard_EXPORT void SetHasMaterial (const Standard_Boolean OnOff);
  
  //! returns True if the  materials is  writing into OStream.
  Standard_EXPORT Standard_Boolean HasMaterial() const;




  DEFINE_STANDARD_RTTIEXT(VrmlConverter_PointAspect,Standard_Transient)

protected:




private:


  Handle(Vrml_Material) myMaterial;
  Standard_Boolean myHasMaterial;


};







#endif // _VrmlConverter_PointAspect_HeaderFile
