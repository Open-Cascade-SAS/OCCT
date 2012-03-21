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


#include <Standard_Boolean.hxx>
#include <Standard_RangeError.hxx>
#ifndef _Standard_Stream_HeaderFile
#include <Standard_Stream.hxx>
#endif
#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

const Handle_Standard_Type& Standard_Boolean_Type_() 
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("Standard_Boolean",sizeof(Standard_Boolean),0,NULL);
  
  return _aType;
}

// ------------------------------------------------------------------
// Hascode : Computes a hascoding value for a given Integer
// ------------------------------------------------------------------
//Standard_Integer HashCode(const Standard_Boolean me
//			  ,const Standard_Integer Upper)
//{
//  if (Upper < 1){
//    Standard_RangeError::
//      Raise("Try to apply HashCode method with negative or null argument.");
//  }
//  return (me % Upper) + 1;
//}

// ----------------------------------------
// ShallowCopy : Makes a copy of a boolean
// ----------------------------------------
Standard_Boolean ShallowCopy (const Standard_Boolean me) 
{ return me; }

// ------------------------------------------------------------------
// ShallowDump : Writes a boolean value
// ------------------------------------------------------------------
Standard_EXPORT void ShallowDump (const Standard_Boolean Value, Standard_OStream& s)
{ s << Value << " Standard_Boolean" << "\n"; }
