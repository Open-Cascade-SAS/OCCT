// File:      LDOM_BasicAttribute.hxx
// Created:   26.06.01 16:16:07
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001


#ifndef LDOM_BasicAttribute_HeaderFile
#define LDOM_BasicAttribute_HeaderFile

#include <LDOM_BasicNode.hxx>
#include <LDOMBasicString.hxx>

class LDOM_XmlReader;
class LDOM_Element;
class LDOM_Attr;
class LDOM_Node;

#ifdef WNT
// Disable the warning: "operator new unmatched by delete"
#pragma warning (push)
#pragma warning (disable:4291)
#endif

//  Class LDOM_BasicAttribute
//

class LDOM_BasicAttribute : public LDOM_BasicNode
{
 public:
  void * operator new (size_t, void * anAddress) { return anAddress; }
 
  // ---------- PUBLIC METHODS ----------

  LDOM_BasicAttribute () : LDOM_BasicNode (LDOM_Node::UNKNOWN) {}
  //    Empty constructor

  LDOM_BasicAttribute&  operator =      (const LDOM_NullPtr * aNull);
  //    Nullify

  const char *           GetName        () const { return myName; }

  const LDOMBasicString& GetValue       () const { return myValue; }

  void                   SetValue       (const LDOMBasicString&         aValue,
                                         const Handle(LDOM_MemManager)& aDoc)
                                { myValue = LDOMString (aValue, aDoc); }

 private:
  friend class LDOM_Node;
  friend class LDOM_Attr;
  friend class LDOM_Element;
  friend class LDOM_BasicElement;
  friend class LDOM_XmlReader;
  
  // ---------- PRIVATE METHODS ----------

  LDOM_BasicAttribute (const LDOMBasicString& aName)
    : LDOM_BasicNode (LDOM_Node::ATTRIBUTE_NODE), myName (aName.GetString()) {}
  //    Constructor

  static LDOM_BasicAttribute& Create (const LDOMBasicString&           theName,
                                      const Handle(LDOM_MemManager)&   theDoc,
                                      Standard_Integer&           theHashIndex);

  LDOM_BasicAttribute (const LDOM_Attr& anAttr);

 private:
  // ---------- PRIVATE FIELDS ----------

//  LDOMBasicString       myName;
  const char            * myName;
  LDOMBasicString       myValue;
};

#ifdef WNT
#pragma warning (pop)
#endif

#endif
