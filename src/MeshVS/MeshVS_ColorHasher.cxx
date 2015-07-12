// Created on: 2003-12-05
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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


#include <MeshVS_ColorHasher.hxx>
#include <Quantity_Color.hxx>

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
