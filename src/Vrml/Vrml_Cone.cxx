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

#include <Vrml_Cone.ixx>

Vrml_Cone::Vrml_Cone(const Vrml_ConeParts aParts,
		     const Standard_Real aBottomRadius,
		     const Standard_Real aHeight)
{
    myParts = aParts;
    myBottomRadius = aBottomRadius;
    myHeight = aHeight;
}

 void Vrml_Cone::SetParts(const Vrml_ConeParts aParts) 
{
    myParts = aParts;
}

 Vrml_ConeParts Vrml_Cone::Parts() const
{
  return myParts;
}

 void Vrml_Cone::SetBottomRadius(const Standard_Real aBottomRadius) 
{
    myBottomRadius = aBottomRadius;
}

 Standard_Real Vrml_Cone::BottomRadius() const
{
  return myBottomRadius;
}

 void Vrml_Cone::SetHeight(const Standard_Real aHeight) 
{
    myHeight = aHeight;
}

 Standard_Real Vrml_Cone::Height() const
{
  return myHeight;
}

 Standard_OStream& Vrml_Cone::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Cone {" << endl;

  switch ( myParts )
    {
     case Vrml_ConeALL: break; // anOStream  << "    parts" << "\t\tALL ";
     case Vrml_ConeSIDES:  anOStream  << "    parts" << "\t\tSIDES" << endl; break;
     case Vrml_ConeBOTTOM: anOStream  << "    parts" << "\t\tBOTTOM" << endl; break; 
    }

 if ( Abs(myBottomRadius - 1 ) > 0.0001 )
   {
    anOStream  << "    bottomRadius" << '\t';
    anOStream << myBottomRadius << endl;
   }

 if ( Abs(myHeight - 2 ) > 0.0001 )
   {
    anOStream  << "    height" << "\t\t";
    anOStream << myHeight << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
