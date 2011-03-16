// File:      LDOM_NodeList.hxx
// Created:   28.06.01 14:59:31
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001


#ifndef LDOM_NodeList_HeaderFile
#define LDOM_NodeList_HeaderFile

#include <LDOM_Node.hxx>

class LDOM_BasicNode;
class LDOM_BasicNodeSequence;

//  Class LDOM_NodeList
//

class LDOM_NodeList 
{
 public:
  // ---------- PUBLIC METHODS ----------

  Standard_EXPORT LDOM_NodeList ();
  // Empty constructor

  Standard_EXPORT LDOM_NodeList (const LDOM_NodeList& theOther);
  // Copy constructor

  Standard_EXPORT LDOM_NodeList& operator =     (const LDOM_NodeList& theOther);
  // Copy constructor

  Standard_EXPORT ~LDOM_NodeList ();
  // Destructor

  Standard_EXPORT LDOM_NodeList& operator =     (const LDOM_NullPtr *);
  // Nullify

  Standard_EXPORT Standard_Boolean operator ==  (const LDOM_NullPtr *) const;
  
  Standard_EXPORT Standard_Boolean operator !=  (const LDOM_NullPtr *) const;
  
  Standard_EXPORT LDOM_Node item                (const Standard_Integer) const;

  Standard_EXPORT Standard_Integer getLength    () const;

 private:
  friend class LDOM_Document;
  friend class LDOM_Element;
  friend class LDOM_BasicElement;
  // ---------- PRIVATE FIELDS ----------

  Standard_EXPORT LDOM_NodeList (const Handle(LDOM_MemManager)& aDoc);

  Standard_EXPORT void Append (const LDOM_BasicNode& aNode) const;

  Handle(LDOM_MemManager)       myDoc;
  LDOM_BasicNodeSequence        * mySeq;
};

#endif
