// Created on: 2001-06-25
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



#ifndef LDOM_Document_HeaderFile
#define LDOM_Document_HeaderFile

#include <LDOM_Element.hxx>
#include <Handle_LDOM_MemManager.hxx>
#include <LDOM_Text.hxx>
#include <LDOM_CDATASection.hxx>
#include <LDOM_Comment.hxx>

//  Class LDOM_Document

class LDOM_Document 
{
 public:
  // ---------- PUBLIC METHODS ----------
  Standard_EXPORT LDOM_Document ();
  // Empty constructor

  Standard_EXPORT LDOM_Document (const LDOM_MemManager& aMemManager);
  // Called by LDOM_MemManager::Doc()

//  Standard_EXPORT LDOM_Document (const LDOM_Document& theOther);
  // Copy constructor

  Standard_EXPORT ~LDOM_Document ();
  // Destructor

  // ---- CREATE ----

  static Standard_EXPORT LDOM_Document
                createDocument          (const LDOMString& theQualifiedName);
  // Create an empty document

  Standard_EXPORT LDOM_Element
                createElement           (const LDOMString& theTagName);

//  Standard_EXPORT LDOM_Element
//                createElementNS         (const LDOMString& theNSuri,
//                                         const LDOMString& theQualName);

  Standard_EXPORT LDOM_CDATASection
                createCDATASection      (const LDOMString& theData);

  Standard_EXPORT LDOM_Comment
                createComment           (const LDOMString& theData);

  Standard_EXPORT LDOM_Text
                createTextNode          (const LDOMString& theData);

  // ---- GET ----

  Standard_EXPORT LDOM_Element
                getDocumentElement      () const;

  Standard_EXPORT LDOM_NodeList
                getElementsByTagName    (const LDOMString& theTagName) const;

  // ---- COMPARE ----

  Standard_Boolean
                operator ==             (const LDOM_Document& anOther) const
                                { return myMemManager == anOther.myMemManager; }

  Standard_Boolean
                operator !=             (const LDOM_Document& anOther) const
                                { return myMemManager != anOther.myMemManager; }

  Standard_Boolean
                operator ==             (const LDOM_NullPtr *) const;
  Standard_Boolean
                operator !=             (const LDOM_NullPtr *) const;

  Standard_EXPORT Standard_Boolean
                isNull                  () const;

  // ---- UTIL ----

  Standard_EXPORT LDOM_Document&
                operator =              (const LDOM_NullPtr *);

 private:
  friend class LDOM_LDOMImplementation;
  friend class LDOMString;
  friend class LDOM_Node;
  // ---------- PRIVATE FIELDS ----------

  Handle(LDOM_MemManager)       myMemManager;
};

#endif
