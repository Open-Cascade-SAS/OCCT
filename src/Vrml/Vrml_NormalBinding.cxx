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

#include <Vrml_NormalBinding.ixx>

Vrml_NormalBinding::Vrml_NormalBinding(const Vrml_MaterialBindingAndNormalBinding aValue)
{
 myValue = aValue;
}

 Vrml_NormalBinding::Vrml_NormalBinding()
{
 myValue = Vrml_DEFAULT;
}

void Vrml_NormalBinding::SetValue(const Vrml_MaterialBindingAndNormalBinding aValue)
{
 myValue = aValue;
}

Vrml_MaterialBindingAndNormalBinding Vrml_NormalBinding::Value() const 
{
 return myValue;
}

Standard_OStream& Vrml_NormalBinding::Print(Standard_OStream& anOStream) const 
{
 anOStream  << "NormalBinding {" << endl;
 switch ( myValue )
   {
   case Vrml_DEFAULT:  break; // anOStream  << "    value" << "\tDEFAULT";
   case Vrml_OVERALL:            anOStream  << "    value" << "\tOVERALL" << endl; break;
   case Vrml_PER_PART:           anOStream  << "    value" << "\tPER_PART" << endl; break;
   case Vrml_PER_PART_INDEXED:   anOStream  << "    value" << "\tPER_PART_INDEXED" << endl; break;
   case Vrml_PER_FACE:           anOStream  << "    value" << "\tPER_FACE" << endl; break;
   case Vrml_PER_FACE_INDEXED:   anOStream  << "    value" << "\tPER_FACE_INDEXED" << endl; break;
   case Vrml_PER_VERTEX:         anOStream  << "    value" << "\tPER_VERTEX" << endl; break;
   case Vrml_PER_VERTEX_INDEXED: anOStream  << "    value" << "\tPER_VERTEX_INDEXED" << endl; break;
   }
 anOStream  << '}' << endl;
 return anOStream;
}
