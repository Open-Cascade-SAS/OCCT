// Created on: 2001-06-26
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

#ifndef LDOM_BasicNode_HeaderFile
#define LDOM_BasicNode_HeaderFile

#include <LDOM_Node.hxx>

class LDOM_NullPtr;

//  Block of comments describing class LDOM_BasicNode
//

class LDOM_BasicNode
{
public:
  DEFINE_STANDARD_ALLOC

public:
  Standard_Boolean isNull() const { return myNodeType == LDOM_Node::UNKNOWN; }

  LDOM_Node::NodeType getNodeType() const { return myNodeType; }

  Standard_EXPORT const LDOM_BasicNode* GetSibling() const;

protected:
  // ---------- PROTECTED METHODS ----------

  LDOM_BasicNode()
      : myNodeType(LDOM_Node::UNKNOWN),
        mySibling(NULL)
  {
  }

  //    Empty constructor

  LDOM_BasicNode(LDOM_Node::NodeType aType)
      : myNodeType(aType),
        mySibling(NULL)
  {
  }

  //    Constructor

  LDOM_BasicNode(const LDOM_BasicNode& anOther)
      : myNodeType(anOther.getNodeType()),
        mySibling(anOther.GetSibling())
  {
  }

  //    Copy constructor

  LDOM_BasicNode& operator=(const LDOM_NullPtr*)
  {
    myNodeType = LDOM_Node::UNKNOWN;
    return *this;
  }

  Standard_EXPORT LDOM_BasicNode& operator=(const LDOM_BasicNode& anOther);

  void SetSibling(const LDOM_BasicNode* anOther) { mySibling = anOther; }

protected:
  friend class LDOM_BasicElement;
  friend class LDOM_Node;
  friend class LDOMParser;
  // ---------- PROTECTED FIELDSS ----------

  LDOM_Node::NodeType   myNodeType;
  const LDOM_BasicNode* mySibling;
};

#endif
