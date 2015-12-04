// Created on: 2001-08-30
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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


#include <CDM_COutMessageDriver.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(CDM_COutMessageDriver,CDM_MessageDriver)

//=======================================================================
//function : Write
//purpose  : 
//=======================================================================
void CDM_COutMessageDriver::Write (const Standard_ExtString aString)
{
  //    Find the length of the input string
  Standard_ExtCharacter aChar = aString[0];
  Standard_Integer aLength = 0;
  while (aChar) {
    if (IsAnAscii(aChar) == Standard_False)
      return;
    aChar = aString [++aLength];
  }

  //    Copy the input string to ASCII buffer
  Standard_PCharacter anAsciiString = new Standard_Character [aLength+2];
  Standard_Integer anI = 0;
  while (anI < aLength) {
    anAsciiString[anI] = Standard_Character (aString[anI]);
    ++ anI;
  }
  anAsciiString[anI+0] = '\n';
  anAsciiString[anI+1] = '\0';

  //    Output
  cout << anAsciiString << flush;
  delete [] anAsciiString;
}
