// Created on: 2001-06-28
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
