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


#ifndef LDOM_Comment_HeaderFile
#define LDOM_Comment_HeaderFile

#include <LDOM_CharacterData.hxx>

//  Class LDOM_Comment

class LDOM_Comment : public LDOM_CharacterData
{
 public:
  // ---------- PUBLIC METHODS ----------

  LDOM_Comment () {}
  // Empty constructor

  LDOM_Comment (const LDOM_Comment& theOther) : LDOM_CharacterData (theOther) {}
  // Copy constructor

  LDOM_Comment&         operator =      (const LDOM_NullPtr * theNull)
           { return (LDOM_Comment&) LDOM_CharacterData::operator = (theNull); }
  // Nullify

  LDOM_Comment&         operator =      (const LDOM_Comment& theOther)
           { return (LDOM_Comment&) LDOM_CharacterData::operator = (theOther); }
  // Assignment

 protected:
  friend class LDOM_Document;
  // ---------- PROTECTED METHODS ----------

  LDOM_Comment                          (const LDOM_BasicText&          theText,
                                         const Handle(LDOM_MemManager)& theDoc)
    : LDOM_CharacterData (theText, theDoc) {}
};

#endif
