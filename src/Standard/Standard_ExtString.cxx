// Copyright (c) 1998-1999 Matra Datavision
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Standard_ExtString.hxx>
#include <Standard_Type.hxx> 
#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

//============================================================================
//==== ShallowDump : Writes a ExtString value.
//============================================================================
Standard_EXPORT void ShallowDump (const Standard_ExtString Value, Standard_OStream& s)
{  
   s << hex << Value << " Standard_ExtString " << endl; 
}

const Handle_Standard_Type& Standard_ExtString_Type_() 
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("Standard_ExtString",sizeof(Standard_ExtString),0,NULL);
  
  return _aType;
}

Standard_Integer HashCode (const Standard_ExtString Value, 
			   const Standard_Integer Upper)
{
  // compute SDBM hash of an ext string
  Standard_Integer hash = 0;
  for (const Standard_ExtCharacter *c = Value; *c; c++)
  {
    /* hash = hash * 33 ^ c */
    hash = (*c) + (hash << 6) + (hash << 16) - hash;
  }

  return HashCode (hash, Upper);
}
