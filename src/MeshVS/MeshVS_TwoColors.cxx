// Created on: 2003-10-08
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

#include <MeshVS_TwoColors.hxx>


//================================================================
// Function : HashCode
// Purpose  :
//================================================================
Standard_Integer HashCode ( const MeshVS_TwoColors&       theKey,
                            const Standard_Integer theUpper)
{
#define MESHPRS_HASH_BYTE(val) { \
    aHash += (val);              \
    aHash += (aHash << 10);      \
    aHash ^= (aHash >> 6);       \
  }
  Standard_Integer aHash = 0;
  MESHPRS_HASH_BYTE (theKey.r1)
  MESHPRS_HASH_BYTE (theKey.g1)
  MESHPRS_HASH_BYTE (theKey.b1)
  MESHPRS_HASH_BYTE (theKey.r2)
  MESHPRS_HASH_BYTE (theKey.g2)
  MESHPRS_HASH_BYTE (theKey.b2)
  aHash += (aHash << 3);
  aHash ^= (aHash >> 11);
  aHash += (aHash << 15);
  return (( aHash & 0x7fffffff ) % theUpper) + 1;
#undef MESHPRS_HASH_BYTE
}

//================================================================
// Function : IsEqual
// Purpose  :
//================================================================
Standard_Boolean IsEqual (const MeshVS_TwoColors& K1,
                          const MeshVS_TwoColors& K2)
{
  return (((K1.r1 * 256 + K1.g1) * 256 + K1.b1) ==
          ((K2.r1 * 256 + K2.g1) * 256 + K2.b1) &&
          ((K1.r2 * 256 + K1.g2) * 256 + K1.b2) ==
          ((K2.r2 * 256 + K2.g2) * 256 + K2.b2));
}

//================================================================
// Function : operator ==
// Purpose  :
//================================================================
Standard_Boolean operator== ( const MeshVS_TwoColors& K1,
                              const MeshVS_TwoColors& K2  )
{
  return IsEqual ( K1, K2 );
}

//================================================================
// Function : BindTwoColors
// Purpose  :
//================================================================
MeshVS_TwoColors BindTwoColors ( const Quantity_Color& theCol1, const Quantity_Color& theCol2 )
{
  MeshVS_TwoColors aRes;

  aRes.r1 = unsigned ( theCol1.Red()   * 255.0 );
  aRes.g1 = unsigned ( theCol1.Green() * 255.0 );
  aRes.b1 = unsigned ( theCol1.Blue()  * 255.0 );
  aRes.r2 = unsigned ( theCol2.Red()   * 255.0 );
  aRes.g2 = unsigned ( theCol2.Green() * 255.0 );
  aRes.b2 = unsigned ( theCol2.Blue()  * 255.0 );

  return aRes;
}

//================================================================
// Function : ExtractColor
// Purpose  :
//================================================================
Quantity_Color ExtractColor ( MeshVS_TwoColors& theTwoColors, const Standard_Integer Index )
{
  Quantity_Color aRes;
  Standard_Real max = 255.0;

  if ( Index == 1 )
    aRes.SetValues ( Standard_Real (theTwoColors.r1) / max,
                     Standard_Real (theTwoColors.g1) / max,
                     Standard_Real (theTwoColors.b1) / max, Quantity_TOC_RGB );
  else if (Index == 2)
    aRes.SetValues ( Standard_Real (theTwoColors.r2) / max,
                     Standard_Real (theTwoColors.g2) / max,
                     Standard_Real (theTwoColors.b2) / max, Quantity_TOC_RGB );

  return aRes;
}

//================================================================
// Function : ExtractColors
// Purpose  :
//================================================================
void ExtractColors ( MeshVS_TwoColors& theTwoColors, Quantity_Color& theCol1, Quantity_Color& theCol2 )
{
  Standard_Real max = 255.0;
  theCol1.SetValues ( Standard_Real (theTwoColors.r1) / max,
                      Standard_Real (theTwoColors.g1) / max,
                      Standard_Real (theTwoColors.b1) / max, Quantity_TOC_RGB );
  theCol2.SetValues ( Standard_Real (theTwoColors.r2) / max,
                      Standard_Real (theTwoColors.g2) / max,
                      Standard_Real (theTwoColors.b2) / max, Quantity_TOC_RGB );
}
