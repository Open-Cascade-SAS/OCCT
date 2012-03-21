// Created on: 2001-07-20
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

//AGV 060302: Input from istream

#ifndef LDOMParser_HeaderFile
#define LDOMParser_HeaderFile

#include <LDOM_Document.hxx>
#include <LDOM_OSStream.hxx>

class LDOM_XmlReader;
//class istream;

//  Class LDOMParser
//

class LDOMParser
{
 public:
  // ---------- PUBLIC METHODS ----------

  LDOMParser () : myReader (NULL), myCurrentData (16384) {}
  // Empty constructor

  virtual Standard_EXPORT ~LDOMParser  ();
  // Destructor

  Standard_EXPORT LDOM_Document
                        getDocument    ();
  // Get the LDOM_Document

  Standard_EXPORT Standard_Boolean
                        parse           (const char * const aFileName);
  // Parse a file
  // Returns True if error occurred, then GetError() can be called

  Standard_EXPORT Standard_Boolean
                        parse           (istream& anInput);
  // Parse a C++ stream
  // Returns True if error occurred, then GetError() can be called

  Standard_EXPORT const TCollection_AsciiString&
                        GetError        (TCollection_AsciiString& aData) const;
  // Return text describing a parsing error, or Empty if no error occurred

 protected:
  // ---------- PROTECTED METHODS ----------

  Standard_EXPORT virtual Standard_Boolean
                        startElement    ();
  // virtual hook on 'StartElement' event for descendant classes

  Standard_EXPORT virtual Standard_Boolean
                        endElement      ();
  // virtual hook on 'EndElement' event for descendant classes

  Standard_EXPORT LDOM_Element
                        getCurrentElement () const;
  // to be called from startElement() and endElement()

 private:
  // ---------- PRIVATE METHODS ----------
  Standard_Boolean      ParseDocument   ();

  Standard_Boolean      ParseElement    ();

  // ---------- PRIVATE (PROHIBITED) METHODS ----------

  LDOMParser (const LDOMParser& theOther);
  // Copy constructor

  LDOMParser& operator = (const LDOMParser& theOther);
  // Assignment

 private:
  // ---------- PRIVATE FIELDS ----------

  LDOM_XmlReader                * myReader;
  Handle(LDOM_MemManager)       myDocument;
  LDOM_OSStream                 myCurrentData;
  TCollection_AsciiString       myError;
};

#endif
