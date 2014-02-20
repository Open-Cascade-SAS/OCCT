// Created on: 2001-07-26
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

#ifndef LDOM_BasicText_HeaderFile
#define LDOM_BasicText_HeaderFile

#include <LDOM_BasicNode.hxx>
#include <LDOMBasicString.hxx>

class LDOM_Node;
class LDOM_CharacterData;
class LDOMParser;
class LDOM_BasicElement;

//  Class LDOM_BasicText
//

class LDOM_BasicText : public LDOM_BasicNode
{
 public:

  // ---------- PUBLIC METHODS ----------

  LDOM_BasicText () : LDOM_BasicNode (LDOM_Node::UNKNOWN) {}
  //    Empty constructor

  LDOM_BasicText&       operator =      (const LDOM_NullPtr * aNull);
  //    Nullify

  const LDOMBasicString& GetData        () const
                                { return myValue; }

  void                  SetData         (const LDOMBasicString&         aValue,
                                         const Handle(LDOM_MemManager)& aDoc)
                                { myValue = LDOMString (aValue, aDoc); }

 private:
  // ---------- PRIVATE METHODS ----------
  friend class LDOM_Node;
  friend class LDOMParser;
  friend class LDOM_Document;
  friend class LDOM_BasicElement;

  LDOM_BasicText                        (const LDOM_Node::NodeType aType,
                                         const LDOMBasicString&    aData)
    : LDOM_BasicNode (aType), myValue (aData) {}
  // Constructor

  LDOM_BasicText                        (const LDOM_CharacterData& aText);

  static LDOM_BasicText& Create         (const LDOM_Node::NodeType      aType,
                                         const LDOMBasicString&         aData,
                                         const Handle(LDOM_MemManager)& aDoc);
  // Creation

 private:
  // ---------- PRIVATE FIELDS ----------

  LDOMBasicString       myValue;
};

#endif
