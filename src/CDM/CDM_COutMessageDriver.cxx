// File:        CDM_COutMessageDriver.cxx
// Created:     Thu Aug 30 15:50:11 2001
// Author:      Alexander GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <CDM_COutMessageDriver.ixx>
#include <Standard_PCharacter.hxx>
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
  delete anAsciiString;
}
