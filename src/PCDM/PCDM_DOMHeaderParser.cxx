// File:      PCDM_DOMHeaderParser.cxx
// Created:   30.07.01 12:51:01
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001
// History:


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
