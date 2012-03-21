// Created by: Eugene Maltchikov
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



#ifndef NCollection_DefaultHasher_HeaderFile
#define NCollection_DefaultHasher_HeaderFile

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>

//=======================================================================
//function : HashCode_Proxy
//purpose  : Function is required to call the global function HashCode.
//=======================================================================
template <class TheKeyType> 
  static Standard_Integer HashCode_Proxy(const TheKeyType& theKey, 
                                         const Standard_Integer Upper) {
  return HashCode(theKey, Upper);
}

//=======================================================================
//function : IsEqual_Proxy
//purpose  : Function is required to call the global function IsEqual.
//=======================================================================
template <class TheKeyType> 
  static Standard_Boolean IsEqual_Proxy(const TheKeyType& theKey1, 
                                        const TheKeyType& theKey2) {
  return IsEqual(theKey1, theKey2);
}


/**
 * Purpose:     The  DefaultHasher  is a  Hasher  that is used by
 *              default in NCollection maps. 
 *              To compute the  hash code of the key  is used the
 *              global function HashCode.
 *              To compare two keys is used  the  global function 
 *              IsEqual.
*/
template <class TheKeyType> class NCollection_DefaultHasher {
public:
  //
  static Standard_Integer HashCode(const TheKeyType& theKey, 
                                   const Standard_Integer Upper) {
    return HashCode_Proxy(theKey, Upper);
  }
  //
  static Standard_Boolean IsEqual(const TheKeyType& theKey1, 
                                  const TheKeyType& theKey2) {
    return IsEqual_Proxy(theKey1, theKey2);
  }
};

#endif
