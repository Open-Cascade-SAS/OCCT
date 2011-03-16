// File:      LDOM_BasicText.hxx
// Created:   26.07.01 18:44:13
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001


#ifndef LDOM_BasicText_HeaderFile
#define LDOM_BasicText_HeaderFile

#include <LDOM_BasicNode.hxx>
#include <LDOMBasicString.hxx>

class LDOM_Node;
class LDOM_CharacterData;
class LDOMParser;
class LDOM_BasicElement;

#ifdef WNT
// Disable the warning: "operator new unmatched by delete"
#pragma warning (push)
#pragma warning (disable:4291)
#endif

//  Class LDOM_BasicText
//

class LDOM_BasicText : public LDOM_BasicNode
{
 public:
  void * operator new (size_t, void * anAddress) { return anAddress; }

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

#ifdef WNT
#pragma warning (pop)
#endif

#endif
