// Created by: Eugene Maltchikov
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

#ifndef NCollection_DefaultHasher_HeaderFile
#define NCollection_DefaultHasher_HeaderFile

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>

//=======================================================================
//function : HashCode_Proxy
//purpose  : Function is required to call the global function HashCode.
//=======================================================================

template <class TheKeyType> 
inline Standard_Integer HashCode_Proxy (const TheKeyType& theKey, 
                                        const Standard_Integer Upper)
{
  return HashCode (theKey, Upper);
}

//=======================================================================
//function : IsEqual
//purpose  : Default implementation of IsEqual via operator ==
//=======================================================================

template <class TheKeyType> 
inline Standard_Boolean IsEqual (const TheKeyType& theKey1, 
                                 const TheKeyType& theKey2)
{
  return theKey1 == theKey2;
}

//=======================================================================
//function : IsEqual_Proxy
//purpose  : Function is required to call the global function IsEqual.
//=======================================================================

template <class TheKeyType> 
inline Standard_Boolean IsEqual_Proxy (const TheKeyType& theKey1, 
                                       const TheKeyType& theKey2)
{
  return IsEqual (theKey1, theKey2);
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
