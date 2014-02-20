// Created on: 2001-07-30
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
