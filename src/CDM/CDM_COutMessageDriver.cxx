// Created on: 2001-08-30
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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
