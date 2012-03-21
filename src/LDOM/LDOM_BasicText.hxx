// Created on: 2001-07-26
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
