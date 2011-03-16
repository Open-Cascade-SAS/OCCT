// File:	MeshVS_ColorHasher.cxx
// Created:	Fri Dec 5 2003 
// Author:	Alexander SOLOVYOV
// Copyright:	Open CASCADE 2003

#include <MeshVS_ColorHasher.ixx>

//================================================================
// Function : HashCode
// Purpose  : 
//================================================================
Standard_Integer MeshVS_ColorHasher::HashCode ( const Quantity_Color& theCol, const Standard_Integer theUpper )
{
  Standard_Integer r = Standard_Integer( 255 * theCol.Red   () ),
                   g = Standard_Integer( 255 * theCol.Green () ),
                   b = Standard_Integer( 255 * theCol.Blue  () );

  #define MESHPRS_HASH_BYTE(val) { \
    aHash += (val);              \
    aHash += (aHash << 10);      \
    aHash ^= (aHash >> 6);       \
  }
  Standard_Integer aHash = 0;
  MESHPRS_HASH_BYTE ( r )
  MESHPRS_HASH_BYTE ( g )
  MESHPRS_HASH_BYTE ( b )
  aHash += (aHash << 3);
  aHash ^= (aHash >> 11);
  aHash += (aHash << 15);
  return (( aHash & 0x7fff ) % theUpper) + 1;
  #undef MESHPRS_HASH_BYTE
}

//================================================================
// Function : IsEqual
// Purpose  : 
//================================================================
Standard_Boolean MeshVS_ColorHasher::IsEqual( const Quantity_Color& K1, const Quantity_Color& K2 )
{
  return K1==K2;
}
