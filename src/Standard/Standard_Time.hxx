// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _Standard_Time_HeaderFile
#define _Standard_Time_HeaderFile

#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

class Handle_Standard_Type;

__Standard_API const Handle_Standard_Type& Standard_Time_Type_();

// ===============================================
// Methods from Standard_Entity class which are redefined:  
//    - IsEqual
// ===============================================

// ------------------------------------------------------------------
// IsEqual : Returns Standard_True if two time values are equal
// ------------------------------------------------------------------
inline Standard_Boolean IsEqual (const Standard_Time theOne,
                                 const Standard_Time theTwo)
{
  return theOne == theTwo;
}

#endif
