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

#include <Vrml_Cylinder.ixx>

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
  anOStream  << "Cylinder {" << endl;

  switch ( myParts )
    {
     case Vrml_CylinderALL: break; // anOStream  << "\tparts" << "\tALL ";
     case Vrml_CylinderSIDES:  anOStream  << "    parts" << "\tSIDES" << endl; break;
     case Vrml_CylinderTOP:    anOStream  << "    parts" << "\tTOP" << endl; break; 
     case Vrml_CylinderBOTTOM: anOStream  << "    parts" << "\tBOTTOM" << endl; break; 
    }

 if ( Abs(myRadius - 1) > 0.0001 )
   {
    anOStream  << "    radius" << '\t';
    anOStream << myRadius << endl;
   }

 if ( Abs(myHeight - 2) > 0.0001 )
   {
    anOStream  << "    height" << '\t';
    anOStream << myHeight << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;

}
