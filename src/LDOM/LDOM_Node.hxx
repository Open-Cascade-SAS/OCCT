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

//AGV 120202: Replace myDocument for myPtrDocument for better
//                          consistency of data

#ifndef LDOM_Node_HeaderFile
#define LDOM_Node_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_TypeDef.hxx>
#include <Handle_LDOM_MemManager.hxx>
#include <LDOMString.hxx>

class LDOM_BasicNode;

//  LDOM_Node : base class for LDOM interface objects
//              references LDOM_BasicNode - the real data stored in Document
//  This type can be safely cast to any derived type, e.g. :
//         LDOM_Node aNode          = MyGetElementNode();
//         LDOM_Element& anElemNode = (LDOM_Element&) aNode;
//         LDOMString anXcoord      = anElemNode.getAtttribute("x");

class LDOM_Node
{
 public:
  enum NodeType {
    UNKNOWN             = 0,
    ELEMENT_NODE        = 1,
    ATTRIBUTE_NODE      = 2,
    TEXT_NODE           = 3,
    CDATA_SECTION_NODE  = 4,
    COMMENT_NODE        = 8
    };

  // ---------- PUBLIC METHODS ----------

  LDOM_Node () : myOrigin        (NULL),
                 myLastChild     (NULL) {}
  //    Empty constructor

  LDOM_Node (const LDOM_Node& anOther)
    : myDocument        (anOther.myDocument),
      myOrigin          (anOther.myOrigin),
      myLastChild       (anOther.myLastChild) {}
  //    Copy constructor

  Standard_EXPORT const LDOM_MemManager& getOwnerDocument () const;

  Standard_EXPORT LDOM_Node&
                        operator =      (const LDOM_Node& anOther);

  Standard_EXPORT LDOM_Node&
                        operator =      (const LDOM_NullPtr * aNull); 

  Standard_Boolean      operator ==     (const LDOM_NullPtr *) const
                                        { return isNull(); }

  Standard_Boolean      operator !=     (const LDOM_NullPtr *) const
                                        { return ! isNull(); }

  Standard_EXPORT Standard_Boolean
                        operator ==     (const LDOM_Node& anOther) const;

  Standard_EXPORT Standard_Boolean
                        operator !=     (const LDOM_Node& anOther) const;

  Standard_EXPORT Standard_Boolean
                        isNull          () const;

  Standard_EXPORT NodeType
                        getNodeType     () const;

  Standard_EXPORT LDOMString
                        getNodeName     () const;

  Standard_EXPORT LDOMString
                        getNodeValue     () const;

  Standard_EXPORT LDOM_Node
                        getFirstChild   () const;

  Standard_EXPORT LDOM_Node
                        getLastChild   () const;

  Standard_EXPORT LDOM_Node
                        getNextSibling  () const;

  Standard_EXPORT void  removeChild     (const LDOM_Node& aChild);

  Standard_EXPORT void  appendChild     (const LDOM_Node& aChild);

  Standard_EXPORT Standard_Boolean
                        hasChildNodes   () const;

  Standard_EXPORT void  SetValueClear   () const;
  //    Avoids checking for '<', '&', '\'', '\"', etc. on storage (saves time)
  //    You MUST be pretty sure that the node value string is OK in this sense
  //    NEVER call this method if you are not stuck on the performance 

 protected:
friend class LDOM_BasicAttribute;
friend class LDOM_BasicElement;
friend class LDOM_BasicText;
friend class LDOM_NodeList;
  // ---------- PROTECTED METHODS ----------

  const LDOM_BasicNode& Origin          () const;

  LDOM_Node (const LDOM_BasicNode& anOrig, const Handle(LDOM_MemManager)& aDoc)
    : myDocument        (aDoc),
      myOrigin          ((LDOM_BasicNode *) &anOrig),
      myLastChild       (NULL) {}
  //    Copy constructor with assignment to Document

 protected:
  // ---------- PROTECTED FIELDS ----------

        // smart pointer to document owner of the node
  Handle(LDOM_MemManager)       myDocument;

        // pointer to (non-transient) node data in the document-managed memory
  LDOM_BasicNode                * myOrigin;

        // Transient data (only applicable to LDOM_Element type):
        // the last child; myLastChild->mySibling points to the first attribute
  const LDOM_BasicNode          * myLastChild;

  friend char * db_pretty_print (const LDOM_Node *, int, char *);
};

#endif
