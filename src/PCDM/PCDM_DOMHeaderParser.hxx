// File:      PCDM_DOMHeaderParser.hxx
// Created:   30.07.01 12:45:51
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001


#ifndef PCDM_DOMHeaderParser_HeaderFile
#define PCDM_DOMHeaderParser_HeaderFile

#include <LDOMParser.hxx>

//  Block of comments describing class PCDM_DOMHeaderParser
//

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
