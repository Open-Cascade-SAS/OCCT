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



#ifndef PCDM_DOMHeaderParser_HeaderFile
#define PCDM_DOMHeaderParser_HeaderFile

#include <LDOMParser.hxx>

//  Block of comments describing class PCDM_DOMHeaderParser

class PCDM_DOMHeaderParser : public LDOMParser
{
 public:
  // ---------- PUBLIC METHODS ----------

  void SetStartElementName   (const TCollection_AsciiString& aStartElementName);
  //    set the name of the element which would stop parsing when detected

  void SetEndElementName     (const TCollection_AsciiString& anEndElementName);
  //    set the name of the element which would stop parsing when parsed

  Standard_Boolean startElement ();
  //    redefined method from LDOMParser
  //    stops parsing when the attributes of header element have been read

  Standard_Boolean endElement ();
  //    redefined method from LDOMParser
  //    stops parsing when the info element with all sub-elements has been read

  const LDOM_Element& GetElement () const { return myElement; }
  //    returns the LDOM_Element containing data about file format

 private:
  // ---------- PRIVATE FIELDS ----------

  LDOM_Element          myElement;
  LDOMString            myStartElementName;
  LDOMString            myEndElementName;
};

#endif
