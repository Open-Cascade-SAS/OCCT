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
