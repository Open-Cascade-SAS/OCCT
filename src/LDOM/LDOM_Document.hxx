// File:      LDOM_Document.hxx
// Created:   25.06.01 12:41:02
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001


#ifndef LDOM_Document_HeaderFile
#define LDOM_Document_HeaderFile

#include <LDOM_Element.hxx>
#include <Handle_LDOM_MemManager.hxx>
#include <LDOM_Text.hxx>
#include <LDOM_CDATASection.hxx>
#include <LDOM_Comment.hxx>

//  Class LDOM_Document
//

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
