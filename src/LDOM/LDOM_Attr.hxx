// Created on: 2001-06-26
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
