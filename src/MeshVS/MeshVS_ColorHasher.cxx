// Created on: 2003-12-05
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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
