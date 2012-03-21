// Created on: 2001-09-12
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



#ifndef LDOM_CharacterData_HeaderFile
#define LDOM_CharacterData_HeaderFile

#include <LDOM_Node.hxx>

class LDOM_BasicText;

//  Class LDOM_CharacterData
//

class LDOM_CharacterData : public LDOM_Node
{
 public:
  // ---------- PUBLIC METHODS ----------

  LDOM_CharacterData () : myLength (-1) {}
  // Empty constructor

  LDOM_CharacterData (const LDOM_CharacterData& theOther)
    : LDOM_Node (theOther), myLength (-1) {}
  // Copy constructor

  Standard_EXPORT LDOM_CharacterData&
                        operator =      (const LDOM_NullPtr * aNull);
  // Nullify

  Standard_EXPORT LDOM_CharacterData&
                        operator =      (const LDOM_CharacterData& anOther);
  // Assignment

  LDOMString            getData         () const { return getNodeValue(); }
  // Query data

  Standard_EXPORT void  setData         (const LDOMString& aValue);
  // Assign to data

  Standard_EXPORT Standard_Integer
                        getLength       () const;
  // Length of the string

 protected:
  // ---------- PROTECTED METHODS ----------

  LDOM_CharacterData                    (const LDOM_BasicText&          aText,
                                         const Handle(LDOM_MemManager)& aDoc);
 private:
  // ------------ PRIVATE FIELDS -----------
  Standard_Integer      myLength;

};

#endif
