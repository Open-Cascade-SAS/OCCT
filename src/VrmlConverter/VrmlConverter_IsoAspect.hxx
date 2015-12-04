// Created on: 1997-05-13
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

#ifndef _VrmlConverter_IsoAspect_HeaderFile
#define _VrmlConverter_IsoAspect_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <VrmlConverter_LineAspect.hxx>
#include <Standard_Boolean.hxx>
class Vrml_Material;


class VrmlConverter_IsoAspect;
DEFINE_STANDARD_HANDLE(VrmlConverter_IsoAspect, VrmlConverter_LineAspect)

//! qualifies the aspect properties for
//! the VRML conversation of iso curves .
class VrmlConverter_IsoAspect : public VrmlConverter_LineAspect
{

public:

  
  //! create a default IsoAspect.
  //! Default value: myNumber  - 10.
  Standard_EXPORT VrmlConverter_IsoAspect();
  
  Standard_EXPORT VrmlConverter_IsoAspect(const Handle(Vrml_Material)& aMaterial, const Standard_Boolean OnOff, const Standard_Integer aNumber);
  
  Standard_EXPORT void SetNumber (const Standard_Integer aNumber);
  
  //! returns the number of U or V isoparametric curves drawn for a
  //! single face.
  Standard_EXPORT Standard_Integer Number() const;




  DEFINE_STANDARD_RTTIEXT(VrmlConverter_IsoAspect,VrmlConverter_LineAspect)

protected:




private:


  Standard_Integer myNumber;


};







#endif // _VrmlConverter_IsoAspect_HeaderFile
