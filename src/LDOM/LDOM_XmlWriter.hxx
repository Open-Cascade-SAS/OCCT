// Created on: 2001-06-28
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

#ifndef LDOM_XmlWriter_HeaderFile
#define LDOM_XmlWriter_HeaderFile

#include <Standard_TypeDef.hxx>
#include <stdio.h>

typedef char LXMLCh;

class LDOM_Document;
class LDOM_Node;
class LDOMBasicString;

class LDOM_XmlWriter
{
 public:

  Standard_EXPORT LDOM_XmlWriter (FILE * aFile, const char * theEncoding= NULL);
  // Constructor

  Standard_EXPORT ~LDOM_XmlWriter ();
  // Destructor

  void SetIndentation (const Standard_Integer theIndent) { myIndent=theIndent; }
  // Set indentation for output (by default 0)

  Standard_EXPORT LDOM_XmlWriter& operator<<    (const LDOM_Document& aDoc);

  Standard_EXPORT LDOM_XmlWriter& operator<<    (const LDOM_Node& toWrite);
  //  ostream << DOM_Node   
  //  Stream out a DOM node, and, recursively, all of its children. This
  //  function is the heart of writing a DOM tree out as XML source. Give it
  //  a document node and it will do the whole thing.

 private:

  void  WriteAttribute (const LDOM_Node& theAtt);

  LDOM_XmlWriter& operator<< (const LDOMBasicString&);
  //  Stream out LDOM String

  inline LDOM_XmlWriter& operator<< (const LXMLCh * toWrite);
  //  Stream out a character string. Doing this requires that we first transcode
  //  to char * form in the default code page for the system

  inline LDOM_XmlWriter& operator <<    (const LXMLCh aChar);

  LDOM_XmlWriter (const LDOM_XmlWriter& anOther);
  //    Copy constructor - prohibited

  LDOM_XmlWriter& operator = (const LDOM_XmlWriter& anOther);
  //    Assignment operator - prohibited

 private:

  FILE                          * myFile;
  LXMLCh                        * myEncodingName;
  Standard_Integer              myIndent;
  Standard_Integer              myCurIndent;
  char *                        myABuffer;      // for WriteAttribute()
  Standard_Integer              myABufferLen;   // for WriteAttribute()
};

#endif
