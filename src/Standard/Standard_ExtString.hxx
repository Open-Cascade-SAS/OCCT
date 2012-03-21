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

//============================================================================
//==== Titre: Standard_ExtString.hxx
//==== 
//==== Implementation:  This is a primitive type implementadef with typedef
//====        typedef short* Standard_ExtString;
//============================================================================

#ifndef _Standard_ExtString_HeaderFile
#define _Standard_ExtString_HeaderFile


#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

class Handle_Standard_Type;

__Standard_API const Handle_Standard_Type& Standard_ExtString_Type_();
//class Standard_OStream;
//void ShallowDump (const Standard_ExtString, Standard_OStream& );
//============================================================================
//==== ShallowCopy: Returns a CString
//============================================================================
inline Standard_ExtString ShallowCopy (const Standard_ExtString Value)
{
  return Value;
}

//============================================================================
//==== HashCode : Returns a HashCode ExtString
//============================================================================
__Standard_API Standard_Integer HashCode (const Standard_ExtString, const Standard_Integer);

#endif
