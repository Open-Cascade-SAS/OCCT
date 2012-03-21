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

#include <Vrml_FontStyle.ixx>

Vrml_FontStyle::Vrml_FontStyle(const Standard_Real aSize,
			       const Vrml_FontStyleFamily aFamily,
			       const Vrml_FontStyleStyle aStyle)
{
  mySize = aSize;
  myFamily = aFamily;
  myStyle = aStyle;
}

 void Vrml_FontStyle::SetSize(const Standard_Real aSize) 
{
  mySize = aSize;
}

 Standard_Real Vrml_FontStyle::Size() const
{
  return mySize;
}

 void Vrml_FontStyle::SetFamily(const Vrml_FontStyleFamily aFamily) 
{
  myFamily = aFamily;
}

 Vrml_FontStyleFamily Vrml_FontStyle::Family() const
{
  return myFamily;
}

 void Vrml_FontStyle::SetStyle(const Vrml_FontStyleStyle aStyle) 
{
  myStyle = aStyle;
}

 Vrml_FontStyleStyle Vrml_FontStyle::Style() const
{
  return myStyle;
}

 Standard_OStream& Vrml_FontStyle::Print(Standard_OStream& anOStream) const
{
 anOStream  << "FontStyle {" << endl;

 if ( Abs(mySize - 10) > 0.0001 )
   {
    anOStream  << "    size" << '\t';
    anOStream <<  mySize  << endl;
   }

  switch ( myFamily )
    {
     case Vrml_SERIF:      break; // anOStream  << "    family" << "\tSERIF ";
     case Vrml_SANS:       anOStream  << "    family" << "\tSANS" << endl; break;
     case Vrml_TYPEWRITER: anOStream  << "    family" << "\tTYPEWRITER" << endl; break; 
    }

  switch ( myStyle )
    {
     case Vrml_NONE:   break; // anOStream  << "    style" << "\tSERIF ";
     case Vrml_BOLD:   anOStream  << "    style" << "\tBOLD" << endl; break;
     case Vrml_ITALIC: anOStream  << "    style" << "\tITALIC" << endl; break; 
    }

 anOStream  << '}' << endl;
 return anOStream;

}
