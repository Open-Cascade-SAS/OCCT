// File:      LDOM_Text.hxx
// Created:   26.07.01 19:29:54
// Author:    Alexander GRIGORIEV
// Copyright: OpenCascade 2001


#ifndef LDOM_Text_HeaderFile
#define LDOM_Text_HeaderFile

#include <LDOM_CharacterData.hxx>

//  Class LDOM_Text
//

class LDOM_Text : public LDOM_CharacterData
{
 public:
  // ---------- PUBLIC METHODS ----------

  LDOM_Text () {}
  // Empty constructor

  LDOM_Text (const LDOM_Text& anOther) : LDOM_CharacterData (anOther) {}
  // Copy constructor

  LDOM_Text&            operator =      (const LDOM_NullPtr * theNull)
                { return (LDOM_Text&) LDOM_CharacterData::operator= (theNull); }
  // Nullify

  LDOM_Text&            operator =      (const LDOM_Text& theOther)
                { return (LDOM_Text&) LDOM_CharacterData::operator= (theOther);}
  // Assignment

 protected:
  friend class LDOM_Document;
  // ---------- PROTECTED METHODS ----------

  LDOM_Text                             (const LDOM_BasicText&          theText,
                                         const Handle(LDOM_MemManager)& theDoc)
                                : LDOM_CharacterData (theText, theDoc) {}
};

#endif
