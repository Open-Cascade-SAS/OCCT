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


#include <Vrml_NormalBinding.hxx>

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
