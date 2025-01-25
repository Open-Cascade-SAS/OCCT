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

#include <Standard_OStream.hxx>
#include <Standard_TypeDef.hxx>

class LDOM_Document;
class LDOM_Node;
class LDOMBasicString;

class LDOM_XmlWriter
{
public:
  Standard_EXPORT LDOM_XmlWriter(const char* theEncoding = NULL);

  Standard_EXPORT ~LDOM_XmlWriter();

  // Set indentation for output (by default 0)
  void SetIndentation(const Standard_Integer theIndent) { myIndent = theIndent; }

  Standard_EXPORT void Write(Standard_OStream& theOStream, const LDOM_Document& theDoc);

  //  Stream out a DOM node, and, recursively, all of its children. This
  //  function is the heart of writing a DOM tree out as XML source. Give it
  //  a document node and it will do the whole thing.
  Standard_EXPORT void Write(Standard_OStream& theOStream, const LDOM_Node& theNode);

private:
  LDOM_XmlWriter(const LDOM_XmlWriter& anOther);

  LDOM_XmlWriter& operator=(const LDOM_XmlWriter& anOther);

  void Write(Standard_OStream& theOStream, const LDOMBasicString& theString);
  void Write(Standard_OStream& theOStream, const char* theString);
  void Write(Standard_OStream& theOStream, const char theChar);

  void WriteAttribute(Standard_OStream& theOStream, const LDOM_Node& theAtt);

private:
  char*            myEncodingName;
  Standard_Integer myIndent;
  Standard_Integer myCurIndent;
  char*            myABuffer;
  Standard_Integer myABufferLen;
};

#endif
