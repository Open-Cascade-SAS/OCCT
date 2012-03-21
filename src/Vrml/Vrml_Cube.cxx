// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <Vrml_Cube.ixx>

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
 anOStream  << "Cube {" << endl;

 if ( Abs(myWidth - 2) > 0.0001 )
   {
    anOStream  << "    width" << '\t';
    anOStream << myWidth << endl;
   }

 if ( Abs(myHeight - 2) > 0.0001 )
   {
    anOStream  << "    height" << '\t';
    anOStream << myHeight << endl;
   }

 if ( Abs(myDepth - 2) > 0.0001 )
   {
    anOStream  << "    depth" << '\t';
    anOStream << myDepth << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;

}
