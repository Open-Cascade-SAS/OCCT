// File: NCollection_DefaultHasher.hxx
// Created: 
// Author: Eugene Maltchikov
// 


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
