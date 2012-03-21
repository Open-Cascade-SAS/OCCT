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



#ifndef LDOM_BasicNode_HeaderFile
#define LDOM_BasicNode_HeaderFile

#include <LDOM_Node.hxx>

class LDOM_BasicElement;
class LDOM_NullPtr;
class LDOMParser;

//  Block of comments describing class LDOM_BasicNode
//

class LDOM_BasicNode 
{
 public:
  DEFINE_STANDARD_ALLOC

 public:

  Standard_Boolean  isNull    () const {return myNodeType ==LDOM_Node::UNKNOWN;}

  LDOM_Node::NodeType getNodeType () const { return myNodeType; }

  Standard_EXPORT const LDOM_BasicNode * GetSibling () const;

 protected:
  // ---------- PROTECTED METHODS ----------

  LDOM_BasicNode () : myNodeType (LDOM_Node::UNKNOWN), mySibling (NULL) {}
  //    Empty constructor

  LDOM_BasicNode (LDOM_Node::NodeType aType)
    : myNodeType (aType), mySibling (NULL) {}
  //    Constructor

  LDOM_BasicNode (const LDOM_BasicNode& anOther)
    : myNodeType (anOther.getNodeType()), mySibling (anOther.GetSibling()) {}
  //    Copy constructor

  LDOM_BasicNode& operator =    (const LDOM_NullPtr * )
                              { myNodeType = LDOM_Node::UNKNOWN; return *this; }

  Standard_EXPORT LDOM_BasicNode& operator = (const LDOM_BasicNode& anOther);

  void SetSibling (const LDOM_BasicNode * anOther)      { mySibling = anOther; }

 protected:
  friend class LDOM_BasicElement;
  friend class LDOM_Node;
  friend class LDOMParser;
  // ---------- PROTECTED FIELDSS ----------

  LDOM_Node::NodeType   myNodeType;
  const LDOM_BasicNode  * mySibling;
};

#endif
