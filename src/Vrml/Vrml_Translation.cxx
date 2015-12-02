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
#include <Vrml_Translation.hxx>

Vrml_Translation::Vrml_Translation()
{
  gp_Vec tmpV(0,0,0);
  myTranslation = tmpV;
}

Vrml_Translation::Vrml_Translation(const gp_Vec& aTranslation)
{
  myTranslation = aTranslation;
}

 void Vrml_Translation::SetTranslation(const gp_Vec& aTranslation) 
{
  myTranslation = aTranslation;
}

 gp_Vec Vrml_Translation::Translation() const
{
  return myTranslation;
}

 Standard_OStream& Vrml_Translation::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Translation {\n";

 if ( Abs(myTranslation.X() - 0) > 0.0001 || 
     Abs(myTranslation.Y() - 0) > 0.0001 || 
     Abs(myTranslation.Z() - 0) > 0.0001 ) 
   {
    anOStream  << "    translation\t";
    anOStream << myTranslation.X() << " " << myTranslation.Y() << " " << myTranslation.Z() << "\n";
   }

 anOStream  << "}\n";
 return anOStream;
}
