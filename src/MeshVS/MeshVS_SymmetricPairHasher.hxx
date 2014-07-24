#ifndef _MeshVS_SymmetricPairHasher_HeaderFile
#define _MeshVS_SymmetricPairHasher_HeaderFile

#include <Standard_Type.hxx>

typedef std::pair<Standard_Integer, Standard_Integer> MeshVS_NodePair;

//! Provides symmetric hash methods pair of integers.
struct MeshVS_SymmetricPairHasher
{
  static Standard_Integer HashCode (const MeshVS_NodePair& thePair, const Standard_Integer theMaxCode)
  {
    return ((thePair.first + thePair.second) & 0x7FFFFFFF) % theMaxCode + 1;
  }

  static Standard_Boolean IsEqual (const MeshVS_NodePair& thePair1, const MeshVS_NodePair& thePair2)
  {
    return (thePair1.first == thePair2.first && thePair1.second == thePair2.second)
        || (thePair1.first == thePair2.second && thePair1.second == thePair2.first);
  }
};

#endif // _MeshVS_SymmetricPairHasher_HeaderFile