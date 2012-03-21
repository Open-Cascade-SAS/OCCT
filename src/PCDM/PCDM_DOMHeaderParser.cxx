// Created on: 2001-07-30
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



#include <PCDM_DOMHeaderParser.hxx>

//=======================================================================
//function : SetStartElementName
//purpose  : set the name of the element which would stop parsing when detected
//=======================================================================

void PCDM_DOMHeaderParser::SetStartElementName
                        (const TCollection_AsciiString& aStartElementName)
{
  myStartElementName = LDOMString (aStartElementName.ToCString());
}

//=======================================================================
//function : SetEndElementName
//purpose  : set the name of the element which would stop parsing when parsed
//=======================================================================

void PCDM_DOMHeaderParser::SetEndElementName
                        (const TCollection_AsciiString& anEndElementName)
{
  myEndElementName = LDOMString (anEndElementName.ToCString());
}

//=======================================================================
//function : PCDM_DOMHeaderParser()
//purpose  : Constructor
//=======================================================================

Standard_Boolean PCDM_DOMHeaderParser::startElement ()
{
  if (myStartElementName == NULL)
    return Standard_False;
  myElement = getCurrentElement ();
  return (myElement.getTagName().equals (myStartElementName));
}

//=======================================================================
//function : endElement
//purpose  : 
//=======================================================================

Standard_Boolean PCDM_DOMHeaderParser::endElement ()
{
  if (myEndElementName == NULL)
    return Standard_False;
  myElement = getCurrentElement ();
  return (myElement.getTagName().equals (myEndElementName));
}
