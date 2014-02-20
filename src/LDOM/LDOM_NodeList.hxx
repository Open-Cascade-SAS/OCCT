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

#ifndef LDOM_NodeList_HeaderFile
#define LDOM_NodeList_HeaderFile

#include <LDOM_Node.hxx>

class LDOM_BasicNode;
class LDOM_BasicNodeSequence;

//  Class LDOM_NodeList
//

class LDOM_NodeList 
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT LDOM_NodeList ();
  // Empty constructor

  Standard_EXPORT LDOM_NodeList (const LDOM_NodeList& theOther);
  // Copy constructor

  Standard_EXPORT LDOM_NodeList& operator =     (const LDOM_NodeList& theOther);
  // Copy constructor

  Standard_EXPORT ~LDOM_NodeList ();
  // Destructor

  Standard_EXPORT LDOM_NodeList& operator =     (const LDOM_NullPtr *);
  // Nullify

  Standard_EXPORT Standard_Boolean operator ==  (const LDOM_NullPtr *) const;
  
  Standard_EXPORT Standard_Boolean operator !=  (const LDOM_NullPtr *) const;
  
  Standard_EXPORT LDOM_Node item                (const Standard_Integer) const;

  Standard_EXPORT Standard_Integer getLength    () const;

 private:
  friend class LDOM_Document;
  friend class LDOM_Element;
  friend class LDOM_BasicElement;
  // ---------- PRIVATE FIELDS ----------

  Standard_EXPORT LDOM_NodeList (const Handle(LDOM_MemManager)& aDoc);

  Standard_EXPORT void Append (const LDOM_BasicNode& aNode) const;

  Handle(LDOM_MemManager)       myDoc;
  LDOM_BasicNodeSequence        * mySeq;
};

#endif
