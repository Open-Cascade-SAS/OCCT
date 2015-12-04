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


#include <Standard_Type.hxx>
#include <Vrml_Material.hxx>
#include <VrmlConverter_IsoAspect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlConverter_IsoAspect,VrmlConverter_LineAspect)

VrmlConverter_IsoAspect::VrmlConverter_IsoAspect():VrmlConverter_LineAspect ()
{
 myNumber = 10; 
}

VrmlConverter_IsoAspect::VrmlConverter_IsoAspect (const Handle(Vrml_Material)& aMaterial,
                                                    const Standard_Boolean OnOff,
						    const Standard_Integer aNumber) 
 :VrmlConverter_LineAspect (aMaterial, OnOff)
{
   myNumber = aNumber;
}

void VrmlConverter_IsoAspect::SetNumber (const Standard_Integer aNumber) 
{
  myNumber = aNumber;
}


Standard_Integer VrmlConverter_IsoAspect::Number () const 
{
  return myNumber;
}
