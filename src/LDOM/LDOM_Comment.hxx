// File:      LDOM_Comment.hxx
// Created:   12.09.01 17:13:32
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2001

#ifndef LDOM_Comment_HeaderFile
#define LDOM_Comment_HeaderFile

#include <LDOM_CharacterData.hxx>

//  Class LDOM_Comment
//

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
