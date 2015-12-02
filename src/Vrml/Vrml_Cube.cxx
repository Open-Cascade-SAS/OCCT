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


#include <Vrml_Cube.hxx>

Vrml_Cube::Vrml_Cube(const Standard_Real aWidth,
		     const Standard_Real aHeight,
		     const Standard_Real aDepth)
{
    myWidth = aWidth;
    myHeight = aHeight;
    myDepth = aDepth;
}

 void Vrml_Cube::SetWidth(const Standard_Real aWidth) 
{
    myWidth = aWidth;
}

 Standard_Real Vrml_Cube::Width() const
{
  return myWidth;
}

 void Vrml_Cube::SetHeight(const Standard_Real aHeight) 
{
    myHeight = aHeight;
}

 Standard_Real Vrml_Cube::Height() const
{
  return myHeight;
}

 void Vrml_Cube::SetDepth(const Standard_Real aDepth) 
{
    myDepth = aDepth;
}

 Standard_Real Vrml_Cube::Depth() const
{
  return myDepth;
}

 Standard_OStream& Vrml_Cube::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Cube {\n";

 if ( Abs(myWidth - 2) > 0.0001 )
   {
    anOStream  << "    width\t";
    anOStream << myWidth << "\n";
   }

 if ( Abs(myHeight - 2) > 0.0001 )
   {
    anOStream  << "    height\t";
    anOStream << myHeight << "\n";
   }

 if ( Abs(myDepth - 2) > 0.0001 )
   {
    anOStream  << "    depth\t";
    anOStream << myDepth << "\n";
   }

 anOStream  << "}\n";
 return anOStream;

}
