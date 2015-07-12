// Copyright (c) 1995-1999 Matra Datavision
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


#include <Prs3d_IsoAspect.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>

Prs3d_IsoAspect::Prs3d_IsoAspect(const Quantity_Color &aColor,
			     const Aspect_TypeOfLine aType,
			     const Standard_Real aWidth,
			     const Standard_Integer aNumber) 
 :Prs3d_LineAspect (aColor,aType,aWidth) {
   myNumber = aNumber;
}

Prs3d_IsoAspect::Prs3d_IsoAspect(const Quantity_NameOfColor aColor,
			     const Aspect_TypeOfLine aType,
			     const Standard_Real aWidth,
			     const Standard_Integer aNumber) 
 :Prs3d_LineAspect (aColor,aType,aWidth) {
   myNumber = aNumber;
}
void Prs3d_IsoAspect::SetNumber (const Standard_Integer aNumber) {
  myNumber = aNumber;
}

Standard_Integer Prs3d_IsoAspect::Number () const {return myNumber;}
