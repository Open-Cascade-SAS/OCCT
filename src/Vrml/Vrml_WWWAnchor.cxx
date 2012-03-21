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

#include <Vrml_WWWAnchor.ixx>

Vrml_WWWAnchor::Vrml_WWWAnchor(const TCollection_AsciiString& aName,
			       const TCollection_AsciiString& aDescription,
			       const Vrml_WWWAnchorMap aMap)
{
 myName = aName;
 myDescription = aDescription;
 myMap = aMap;
}

 void Vrml_WWWAnchor::SetName(const TCollection_AsciiString& aName) 
{
 myName = aName;
}

 TCollection_AsciiString Vrml_WWWAnchor::Name() const
{
  return myName;
}

 void Vrml_WWWAnchor::SetDescription(const TCollection_AsciiString& aDescription) 
{
 myDescription = aDescription;
}

 TCollection_AsciiString Vrml_WWWAnchor::Description() const
{
  return myDescription;
}

 void Vrml_WWWAnchor::SetMap(const Vrml_WWWAnchorMap aMap) 
{
 myMap = aMap;
}

 Vrml_WWWAnchorMap Vrml_WWWAnchor::Map() const
{
  return myMap;
}

 Standard_OStream& Vrml_WWWAnchor::Print(Standard_OStream& anOStream) const
{
 anOStream  << "WWWAnchor {" << endl;

 if ( !(myName.IsEqual( "" ) ) )
   {
    anOStream  << "    name" << '\t';
    anOStream << '"' << myName << '"' << endl;
   }

 if ( !(myDescription.IsEqual("") ) )
   {
    anOStream  << "    description" << '\t';
    anOStream << '"' << myDescription << '"' << endl;
   }

 switch ( myMap )
    {
     case Vrml_MAP_NONE: break; // anOStream  << "    map" << "\tNONE ";
     case Vrml_POINT: anOStream  << "    map" << "\t\tPOINT" << endl; break;
    }

 anOStream  << '}' << endl;
 return anOStream;

}
