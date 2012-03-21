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


#ifndef __TWO_NODES_STRUCT_HEADER__
#define __TWO_NODES_STRUCT_HEADER__

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

//! Structure containing two IDs (of nodes) for using as a key in a map
//! (as representation of a mesh link)
//!
struct MeshVS_TwoNodes
{
  Standard_Integer First, Second;

  MeshVS_TwoNodes (Standard_Integer aFirst=0, Standard_Integer aSecond=0) 
  : First(aFirst), Second(aSecond) {}
};

//================================================================
// Function : HashCode
// Purpose  :
//================================================================

inline Standard_Integer HashCode( const MeshVS_TwoNodes& obj,
                                  const Standard_Integer Upper )
{
  // symmetrical with respect to obj.First and obj.Second
  const Standard_Integer aKey = obj.First + obj.Second;
  return HashCode (aKey, Upper);
}

//================================================================
// Function : operator ==
// Purpose  :
//================================================================

inline Standard_Boolean operator==( const MeshVS_TwoNodes& obj1,
                                    const MeshVS_TwoNodes& obj2 )
{
  return ( ( obj1.First == obj2.First  ) && ( obj1.Second == obj2.Second ) ) ||
         ( ( obj1.First == obj2.Second ) && ( obj1.Second == obj2.First  ) );
}

#endif
