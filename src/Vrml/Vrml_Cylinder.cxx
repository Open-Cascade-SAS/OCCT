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


#include <Vrml_Cylinder.hxx>

Vrml_Cylinder::Vrml_Cylinder(const Vrml_CylinderParts aParts,
			     const Standard_Real aRadius,
			     const Standard_Real aHeight)
{
    myParts = aParts;   
    myRadius = aRadius;
    myHeight = aHeight;
}

 void Vrml_Cylinder::SetParts(const Vrml_CylinderParts aParts) 
{
    myParts = aParts;   
}

 Vrml_CylinderParts Vrml_Cylinder::Parts() const
{
  return myParts;
}

 void Vrml_Cylinder::SetRadius(const Standard_Real aRadius) 
{
    myRadius = aRadius;
}

 Standard_Real Vrml_Cylinder::Radius() const
{
  return myRadius;
}

 void Vrml_Cylinder::SetHeight(const Standard_Real aHeight) 
{
    myHeight = aHeight;
}

 Standard_Real Vrml_Cylinder::Height() const
{
  return myHeight;
}

 Standard_OStream& Vrml_Cylinder::Print(Standard_OStream& anOStream) const
{
  anOStream  << "Cylinder {\n";

  switch ( myParts )
    {
     case Vrml_CylinderALL: break; // anOStream  << "\tparts\tALL ";
     case Vrml_CylinderSIDES:  anOStream  << "    parts\tSIDES\n"; break;
     case Vrml_CylinderTOP:    anOStream  << "    parts\tTOP\n"; break; 
     case Vrml_CylinderBOTTOM: anOStream  << "    parts\tBOTTOM\n"; break; 
    }

 if ( Abs(myRadius - 1) > 0.0001 )
   {
    anOStream  << "    radius\t";
    anOStream << myRadius << "\n";
   }

 if ( Abs(myHeight - 2) > 0.0001 )
   {
    anOStream  << "    height\t";
    anOStream << myHeight << "\n";
   }

 anOStream  << "}\n";
 return anOStream;

}
