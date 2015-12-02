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


#include <gp_Vec.hxx>
#include <Vrml_Scale.hxx>

Vrml_Scale::Vrml_Scale()
{
  gp_Vec tmpV(1,1,1);
  myScaleFactor = tmpV;
}

Vrml_Scale::Vrml_Scale(const gp_Vec& aScaleFactor)
{
  myScaleFactor = aScaleFactor;
}

 void Vrml_Scale::SetScaleFactor(const gp_Vec& aScaleFactor) 
{
  myScaleFactor = aScaleFactor;
}

 gp_Vec Vrml_Scale::ScaleFactor() const
{
  return  myScaleFactor;
}

 Standard_OStream& Vrml_Scale::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Scale {\n";

 if ( Abs(myScaleFactor.X() - 1) > 0.0001 || 
     Abs(myScaleFactor.Y() - 1) > 0.0001 || 
     Abs(myScaleFactor.Z() - 1) > 0.0001 ) 
   {
    anOStream  << "    scaleFactor\t";
    anOStream << myScaleFactor.X() << " " << myScaleFactor.Y() << " " << myScaleFactor.Z() << "\n";
   }

 anOStream  << "}\n";
 return anOStream;
}
