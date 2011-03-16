// File:      LDOM_Attr.hxx
// Created:   26.06.01 17:36:20
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001


#ifndef LDOM_Attr_HeaderFile
#define LDOM_Attr_HeaderFile

#include <LDOM_Node.hxx>

class LDOM_BasicAttribute;
class LDOM_Element;

//  Class LDOM_Attr
//

class LDOM_Attr : public LDOM_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  LDOM_Attr () {}
  //    Empty constructor

  LDOM_Attr (const LDOM_Attr& anOther) : LDOM_Node (anOther) {}
  //    Copy constructor

  LDOM_Attr&            operator =      (const LDOM_NullPtr * aNull)
                    { return (LDOM_Attr&) LDOM_Node::operator = (aNull); }
  //    Nullify

  LDOM_Attr&            operator =      (const LDOM_Attr& anOther)
                    { return (LDOM_Attr&) LDOM_Node::operator = (anOther); }
  //    Assignment

  LDOMString            getName         () const { return getNodeName (); }

  LDOMString            getValue        () const { return getNodeValue(); }

  Standard_EXPORT void  setValue        (const LDOMString& aValue);

 protected:
  friend class LDOM_Element;
  // ---------- PROTECTED METHODS ----------

  LDOM_Attr                             (const LDOM_BasicAttribute&     anAttr,
                                         const Handle(LDOM_MemManager)& aDoc);

 private:
  // ---------- PRIVATE FIELDS ----------

};

#endif
